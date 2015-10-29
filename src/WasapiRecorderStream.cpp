#include "stdafx.h"
#include <basswasapi.h>
#include <Audiopolicy.h> //for IAudioSessionControl
#include "WasapiRecorderStream.h"
#include "WasapiHelpers.h" //for GetActiveDevice
//#include <Wmcodecdsp.h> //supports auto gain control, echo cancellation, noise filter

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MFTIMES_PER_MILLISEC  10000

/////////////////////////////////////////////////////////////////////////////

CWasapiRecorderStream::CWasapiRecorderStream(int device)
	:m_deviceID(device)
	,m_hStream(0)
	,m_wfx(NULL)
	,m_packetOffsetBytes(0) //need to reset it on object construction (static variable not ok)
{
	HRESULT hr;

	// Taking device, based on driver name, not ID (BASS enumeration doesn't
	// equal with WASAPI's). Again, real device IDs starts from 1 in BASS.
	BASS_WASAPI_DEVICEINFO info;
	BOOL result = BASS_WASAPI_GetDeviceInfo(device, &info);
	ASSERT(result);
	EIF(WasapiHelpers::GetActiveDevice(CString(info.id), &m_audio_client));

	const DWORD streamFlags = (info.flags & BASS_DEVICE_LOOPBACK) ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0;
	const REFERENCE_TIME bufferDuration = 500 * MFTIMES_PER_MILLISEC;

	EIF(m_audio_client->GetMixFormat(&m_wfx));
	EIF(m_audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, bufferDuration, 0, m_wfx, NULL));
	EIF(m_audio_client->GetService(__uuidof(IAudioCaptureClient), (void**)&m_capture_client));
	EIF(m_audio_client->Start());

Exit:
	return;
}
//---------------------------------------------------------------------------

CWasapiRecorderStream::~CWasapiRecorderStream()
{
	if (m_audio_client)
	{
		HRESULT hr = m_audio_client->Stop();
		ASSERT(!FAILED(hr));

		BASS_StreamFree(m_hStream);
		CoTaskMemFree(m_wfx);
	}
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderStream::GetActualFrequency() const
{
	return (m_wfx != NULL) ? m_wfx->nSamplesPerSec : 0;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderStream::GetActualChannelCount() const
{
	return (m_wfx != NULL) ? m_wfx->nChannels : 0;
}
//---------------------------------------------------------------------------

HSTREAM CWasapiRecorderStream::GetStreamHandle() const
{
	if (m_hStream == 0)
	{
		CComPtr<IAudioSessionControl> sessionControl;
		AudioSessionState sessionState;

		HRESULT hr;
		EIF(m_audio_client->GetService(__uuidof(IAudioSessionControl), (void**)&sessionControl));
		EIF(sessionControl->GetState(&sessionState));
		if (sessionState != AudioSessionStateActive)
			return 0;

		m_hStream = BASS_StreamCreate(
			GetActualFrequency(),
			GetActualChannelCount(),
			BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE,
			StreamProc,
			(void*)this);
	}

Exit:
	return m_hStream;
}
//---------------------------------------------------------------------------

float CWasapiRecorderStream::GetVolume() const
{
	CComPtr<IChannelAudioVolume> channelVolume;
	float resultVolume = 0;

	HRESULT hr;
	EIF(m_audio_client->GetService(__uuidof(IChannelAudioVolume), (void**)&channelVolume));
	EIF(channelVolume->GetChannelVolume(0, &resultVolume));

Exit:
	return resultVolume;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderStream::SetVolume(float volume)
{
	CComPtr<IChannelAudioVolume> channelVolume;
	UINT32 channelCount = 0;
	float* newVolumes = NULL;

	HRESULT hr;
	EIF(m_audio_client->GetService(__uuidof(IChannelAudioVolume), (void**)&channelVolume));
	EIF(channelVolume->GetChannelCount(&channelCount));

	ASSERT(channelCount > 0);
	newVolumes = new float[channelCount];
	for (unsigned i = 0; i < channelCount; i++)
		newVolumes[i] = volume;

	EIF(channelVolume->SetAllVolumes(channelCount, newVolumes, NULL));

Exit:
	SAFE_DELETE_ARRAY(newVolumes);
	return hr == S_OK;
}
//---------------------------------------------------------------------------

float CWasapiRecorderStream::GetPeakLevel(int channel) const
{
	//TODO: rewrite without using BASS wasapi (see Get/SetVolume).
	const bool isMono = (GetActualChannelCount() == 1);
	const float level = BASS_WASAPI_GetDeviceLevel(m_deviceID, isMono ? 0 : channel);
	//ASSERT(level >= 0);
	return level;
}
//---------------------------------------------------------------------------

DWORD CALLBACK CWasapiRecorderStream::StreamProc(HSTREAM,
	void* dstBuffer, DWORD dstBufferLengthBytes, void* userData)
{
	CWasapiRecorderStream* rs = static_cast<CWasapiRecorderStream *>(userData);
	HRESULT hr = S_OK;

//WriteDbg() << " ::1  , dstBufferLengthBytes=" << dstBufferLengthBytes << ", packetOffsetBytes=" << rs->m_packetOffsetBytes;

	//ENSURE WE HAVE ENOUGTH DATA TO FILL OUTPUT BUFFER

	UINT32 paddingFramesCount = 0;
	EIF(rs->m_audio_client->GetCurrentPadding(&paddingFramesCount));
	//if ((paddingFramesCount * rs->m_wfx->nBlockAlign - rs->m_packetOffsetBytes) < dstBufferLengthBytes)
	//	return 0;

//WriteDbg() << " ::1  , paddingFramesCount=" << paddingFramesCount << ", bytes=" << paddingFramesCount * rs->m_wfx->nBlockAlign;
	int attemptsCount = 100;
	int zeroCheckCount = 10;
	while ((paddingFramesCount * rs->m_wfx->nBlockAlign - rs->m_packetOffsetBytes) < dstBufferLengthBytes)
	{
		::Sleep(10);
		if (--zeroCheckCount == 0 && paddingFramesCount == 0)
			return 0;
		EIF(rs->m_audio_client->GetCurrentPadding(&paddingFramesCount));
		if (--attemptsCount == 0)
			return BASS_STREAMPROC_END;
//WriteDbg() << " ::1  , paddingFramesCount=" << paddingFramesCount << ", bytes=" << paddingFramesCount * rs->m_wfx->nBlockAlign;
	}

//WriteDbg() << " ::2  , (endpoint buffer ready)";


	//FILLING BUFFER

	UINT32 bufferOffsetBytes = 0;
	while (bufferOffsetBytes < dstBufferLengthBytes)
	{

//WriteDbg() << " ::3.1, bufferOffsetBytes=" << bufferOffsetBytes << ", packetOffsetBytes=" << rs->m_packetOffsetBytes;

		BYTE* srcBuffer = NULL;
		DWORD srcBufferFlags = 0;
		UINT32 numFramesAvailable = 0;
		EIF(rs->m_capture_client->GetBuffer(&srcBuffer, &numFramesAvailable, &srcBufferFlags, NULL, NULL));

		const UINT32 numBytesAvailable = numFramesAvailable * rs->m_wfx->nBlockAlign;
		const UINT32 copyBytesAvailable = numBytesAvailable - rs->m_packetOffsetBytes;
		const UINT32 bytesToWrite = min(copyBytesAvailable, dstBufferLengthBytes - bufferOffsetBytes);
		ASSERT(numBytesAvailable >= rs->m_packetOffsetBytes); //copyBytesAvailable overflow check

//WriteDbg() << " ::3.2, numBytesAvailable=" << numBytesAvailable << ", copyBytesAvailable=" << copyBytesAvailable << ", bytesToWrite=" << bytesToWrite;

		if (srcBufferFlags & AUDCLNT_BUFFERFLAGS_SILENT)
			ZeroMemory((BYTE*)dstBuffer + bufferOffsetBytes, bytesToWrite);
		else
			memcpy((BYTE*)dstBuffer + bufferOffsetBytes, srcBuffer + rs->m_packetOffsetBytes, bytesToWrite);

		bufferOffsetBytes += bytesToWrite;
		rs->m_packetOffsetBytes += bytesToWrite;

		//Check full packet has been read.
		ASSERT(rs->m_packetOffsetBytes <= numBytesAvailable);
		const bool fullPacketRead = (rs->m_packetOffsetBytes == numBytesAvailable);
		if (fullPacketRead)
		  rs->m_packetOffsetBytes = 0;

//WriteDbg() << " ::3.3, fullPacketRead=" << fullPacketRead;

		EIF(rs->m_capture_client->ReleaseBuffer(fullPacketRead ? numFramesAvailable : 0));
	}
//WriteDbg() << " ::4  , (DST BUFFER FILLED)";
	return bufferOffsetBytes;

Exit:
//WriteDbg() << " ::5  , (DEVICE ERROR)";
	return BASS_STREAMPROC_END; //case with invalid 'hr' variable
}
//---------------------------------------------------------------------------

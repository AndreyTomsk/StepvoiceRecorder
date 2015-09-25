#include "stdafx.h"
#include <basswasapi.h>
#include "WasapiAudioClient.h"
#include "WasapiHelpers.h" //for GetActiveDevice
#include "Debug.h"
#include "common.h" //for EIF
#include "SampleConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MFTIMES_PER_MILLISEC  10000

/////////////////////////////////////////////////////////////////////////////

CWasapiAudioClient::CWasapiAudioClient(int device, DWORD freq, DWORD chans)
	:m_deviceID(device)
	,m_wfx(NULL)
	,m_audioState(eStopped)
	,m_captureBuffer(NULL)
	,m_resampleFreq(0)
	,m_resampleChans(0)
{
	HRESULT hr;

	// Taking device, based on device ID string (BASS enumeration doesn't
	// equal with WASAPI's). Again, real device IDs starts from 1 in BASS.
	BASS_WASAPI_DEVICEINFO info;
	BOOL result = BASS_WASAPI_GetDeviceInfo(device, &info);
	ASSERT(result);
	EIF(WasapiHelpers::GetActiveDevice(CString(info.id), &m_audioClient));

	//WriteDbg() << __FUNCTION__ << " ::device=" << device << ", name=" << info.name;

	const DWORD streamFlags = (info.flags & BASS_DEVICE_LOOPBACK) ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0;
	const REFERENCE_TIME bufferDuration = 500 * MFTIMES_PER_MILLISEC;

	EIF(InitMixFormat(m_audioClient, freq, chans, &m_wfx));
	EIF(m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, bufferDuration, 0, m_wfx, NULL));
	EIF(m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_captureClient));

	m_captureBuffer = new CWasapiCaptureBuffer(m_audioClient);
	m_resampleFreq = m_wfx->nSamplesPerSec;
	m_resampleChans = m_wfx->nChannels;

Exit:
	return;
}
//---------------------------------------------------------------------------

CWasapiAudioClient::~CWasapiAudioClient()
{
	Stop();
	SAFE_DELETE(m_captureBuffer);
	CoTaskMemFree(m_wfx);
}
//---------------------------------------------------------------------------

int CWasapiAudioClient::GetDeviceID() const
{
	return m_deviceID;
}
//---------------------------------------------------------------------------

DWORD CWasapiAudioClient::GetActualFrequency() const
{
	return m_wfx != NULL ? m_wfx->nSamplesPerSec : 0;
}
//---------------------------------------------------------------------------

DWORD CWasapiAudioClient::GetActualChannelCount() const
{
	return m_wfx != NULL ? m_wfx->nChannels : 0;
}
//---------------------------------------------------------------------------

BOOL CWasapiAudioClient::Start()
{
	CMyLock lock(m_sync_object);

	if (m_audioClient && m_audioClient->Start() == S_OK)
		m_audioState = eStarted;

	return m_audioState == eStarted;
}
//---------------------------------------------------------------------------

BOOL CWasapiAudioClient::Pause()
{
	CMyLock lock(m_sync_object);

	if (m_audioClient && m_audioClient->Stop() == S_OK)
		m_audioState = ePaused;

	return m_audioState == ePaused;
}
//---------------------------------------------------------------------------

BOOL CWasapiAudioClient::Stop()
{
	CMyLock lock(m_sync_object);

	if (Pause() && m_audioClient->Reset() == S_OK)
		m_audioState = eStopped;

	return m_audioState == eStopped;
}
//---------------------------------------------------------------------------

BOOL CWasapiAudioClient::IsStarted() const
{
	return m_audioState == eStarted;
}
//---------------------------------------------------------------------------

BOOL CWasapiAudioClient::IsPaused() const
{
	return m_audioState == ePaused;
}
//---------------------------------------------------------------------------

BOOL CWasapiAudioClient::IsStopped() const
{
	return m_audioState == eStopped;
}
//---------------------------------------------------------------------------

float CWasapiAudioClient::GetVolume() const
{
	float resultVolume = 0;
	if (m_audioClient)
	{
		HRESULT hr;
		CComPtr<IChannelAudioVolume> channelVolume;
		EIF(m_audioClient->GetService(__uuidof(IChannelAudioVolume), (void**)&channelVolume));
		EIF(channelVolume->GetChannelVolume(0, &resultVolume));
	}
Exit:
	return resultVolume;
}
//---------------------------------------------------------------------------

BOOL CWasapiAudioClient::SetVolume(float volume)
{
	if (!m_audioClient)
		return FALSE;

	CComPtr<IChannelAudioVolume> channelVolume;
	UINT32 channelCount = 0;
	float* newVolumes = NULL;

	HRESULT hr;
	EIF(m_audioClient->GetService(__uuidof(IChannelAudioVolume), (void**)&channelVolume));
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

float CWasapiAudioClient::GetPeakLevel(int channel) const //0 = first channel, -1 = all channels
{
	//TODO: rewrite without using BASS wasapi (see Get/SetVolume).
	const bool isMono = (GetActualChannelCount() == 1);
	const float level = BASS_WASAPI_GetDeviceLevel(m_deviceID, isMono ? 0 : channel);
	return (level >= 0) ? level : 0;
}
//---------------------------------------------------------------------------

void CWasapiAudioClient::SetResampleParams(unsigned destFreq, unsigned destChannels)
{
	ASSERT(m_wfx != NULL);
	ASSERT(destFreq <= m_wfx->nSamplesPerSec);
	m_resampleFreq  = destFreq;
	m_resampleChans = destChannels;

	//WriteDbg() << "streamFreq=" << m_wfx->nSamplesPerSec
	//		   << ", streamChans=" << m_wfx->nChannels
	//		   << ", m_resampleFreq=" << m_resampleFreq
	//		   << ", m_resampleChans=" << m_resampleChans;
}
//---------------------------------------------------------------------------

bool CWasapiAudioClient::GetData(BYTE* destBuffer,
	const UINT32& bufferSize, bool& streamError) const
{
	//Check resampling not needed.
	if (m_wfx->nSamplesPerSec == m_resampleFreq && m_wfx->nChannels == m_resampleChans)
		return m_captureBuffer->FillBuffer(destBuffer, bufferSize, streamError);

	//Calculating a required sample count, to get from wasapi stream.
	using namespace SampleConverter;

	const int srcFreq = m_wfx->nSamplesPerSec;
	const int dstFreq = m_resampleFreq;
	const int dstSampleCount = bufferSize / (m_resampleChans * sizeof(float));
	const int srcSampleCount = GetSrcSampleCount(srcFreq, dstFreq, dstSampleCount);
	ASSERT(VerifyDstSampleCount(srcFreq, srcSampleCount, dstFreq, dstSampleCount));

	const DWORD TEMP_BUFFER_SIZE = 20480;
	static float srcBuffer[TEMP_BUFFER_SIZE];
	const int srcBufferSize = srcSampleCount * (m_wfx->nChannels * sizeof(float));
	ASSERT(srcBufferSize <= TEMP_BUFFER_SIZE);

	//Receiving samples.

	if (!m_captureBuffer->FillBuffer((BYTE*)srcBuffer, srcBufferSize, streamError))
	{
		//if (streamError)
		//{
		//WriteDbg() << "OOPS! unable to fill buffer." << destSampleCount
		//		   << "destSamples=" << dstSampleCount
		//		   << ", reqSamples=" << srcSampleCount
		//		   << ", reqByteSize=" << srcBufferSize;
		//}
		return false;
	}

	//Converting samples.

	SampleBuffer requestSB(srcFreq, m_wfx->nChannels, srcBuffer);
	requestSB.curSamples = srcSampleCount;

	SampleBuffer destSB(dstFreq, m_resampleChans, (float*)destBuffer);
	memset(destBuffer, 0, bufferSize);

	ConvertSamples(requestSB, destSB);

	//WriteDbg() << "destSamples=" << dstSampleCount
	//		   << ", reqSamples=" << srcSampleCount
	//		   << ", reqByteSize=" << srcBufferSize
	//		   << ", samplesWritten=" << destSB.curSamples;

	return true;
}
//---------------------------------------------------------------------------

HRESULT CWasapiAudioClient::InitMixFormat(IAudioClient* ac,
	DWORD freq, DWORD chans, WAVEFORMATEX** deviceFormat)
{
	WAVEFORMATEX wfx = {0};
	wfx.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	wfx.nChannels = WORD(chans);
	wfx.nSamplesPerSec = freq;
	wfx.wBitsPerSample = 32; //float
	wfx.nBlockAlign = wfx.wBitsPerSample/8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;

	WAVEFORMATEX* closestMatchWfx = NULL;

	const HRESULT hr = ac->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wfx, &closestMatchWfx);
	if (hr == S_OK)
	{
		//Using CoTaskMemAlloc - pointer should be freed by CoTaskMemFree
		//(from docs). So all these 3 branches return similar allocated memory.
		*deviceFormat = static_cast<WAVEFORMATEX*>(CoTaskMemAlloc(sizeof(wfx)));
		memcpy(*deviceFormat, &wfx, sizeof(wfx));
		return S_OK;
	}
	else if (hr == S_FALSE)
	{
		*deviceFormat = closestMatchWfx;
		return S_OK;
	}
	else
	{
		return ac->GetMixFormat(deviceFormat);
	}
}
//---------------------------------------------------------------------------

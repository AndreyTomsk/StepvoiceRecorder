#include "stdafx.h"
#include <basswasapi.h>
#include "WasapiRecorderStream.h"
#include "WasapiHelpers.h" //for GetActiveDevice
#include "Debug.h"
#include "common.h" //for EIF

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
	,m_src_offset(0)
	,m_buffer_delay(false)
	,m_buffer_size(0)
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
	EIF(m_audio_client->GetBufferSize(&m_buffer_size)); //in frames

	EIF(m_audio_client->GetService(__uuidof(IAudioCaptureClient),
		(void**)&m_capture_client));
	EIF(m_audio_client->Start());

	m_hStream = BASS_StreamCreate(
		GetActualFrequency(),
		GetActualChannelCount(),
		BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE,
		StreamProc,
		this);

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
	void* a_buffer, DWORD a_length, void* a_user)
{
	//TODO: refactor, looks overcomplicated.

	CWasapiRecorderStream* l_this = static_cast<CWasapiRecorderStream *>(a_user);
	ASSERT(l_this);

	HRESULT hr;

	// Applying sound delay for 0.1 sec (currently).
	{
		UINT32 paddingFrameCount;
		hr = l_this->m_audio_client->GetCurrentPadding(&paddingFrameCount);
		if (!paddingFrameCount || (paddingFrameCount < l_this->m_buffer_size/4 && !l_this->m_buffer_delay))
		{
			//if (addingFrameCount)
			//	::OutputDebugString(__FUNCTION__ " ::Delaying (wait 1/4 buffer)");
			//else
			//	::OutputDebugString(__FUNCTION__ " ::Delaying (no frames in buffer");

			l_this->m_buffer_delay = false;
			ZeroMemory(a_buffer, a_length);
			return a_length;
		}
		else
			l_this->m_buffer_delay = true;
	}

	BYTE*  l_dst_buffer = (BYTE*)a_buffer;
	UINT32 l_dst_length = a_length;

	// NOTE: we fill the required buffer completely
	while (l_dst_length)
	{
		// 1. Getting new chunk
		// 2. If we don't have any sounds, then just making the remaining part
		//      of the buffer silent and exit
		// 3. Counting data length for copying
		// 4. Filling destination buffer
		// 5. Updating variables
		// 6. If l_dst_length, then repeat

		BYTE*   l_src_buffer = NULL;
		DWORD   l_src_flags = 0;
		UINT32  l_src_frames_available = 0;
		UINT32& l_src_offset = l_this->m_src_offset;

		hr = l_this->m_capture_client->GetBuffer(&l_src_buffer,
			&l_src_frames_available, &l_src_flags, NULL, NULL);

		if (!l_src_frames_available)
		{
			if (l_dst_length == a_length)
				::OutputDebugString(__FUNCTION__ " ::Empty buffer");
			else
				::OutputDebugString(__FUNCTION__ " ::Empty buffer (partially filled)");

			ZeroMemory(l_dst_buffer, l_dst_length);
			hr = l_this->m_capture_client->ReleaseBuffer(l_src_frames_available);
			// Return filled length if no more frames.
			return (l_dst_length == a_length) ? a_length : a_length - l_dst_length;
		}

		UINT32 l_src_bytes_available = l_src_frames_available * l_this->m_wfx->nBlockAlign;

		if (l_src_bytes_available > a_length)
			::OutputDebugString(__FUNCTION__ " :: TOO MANY FRAMES!");

		UINT32 l_bytes_4copy = min(l_dst_length, l_src_bytes_available - l_src_offset);

		memcpy(l_dst_buffer, l_src_buffer + l_src_offset, l_bytes_4copy);

		l_dst_length -= l_bytes_4copy;
		l_dst_buffer += l_bytes_4copy;
		l_src_offset += l_bytes_4copy;

		ASSERT(l_src_offset <= l_src_bytes_available);
		if (l_src_offset == l_src_bytes_available)
			l_src_offset = 0;	// clearing offset if all copied

		// Asking for same chunk (0) if it wasn't fully copied
		hr = l_this->m_capture_client->ReleaseBuffer(
			(l_src_offset) ? 0 : l_src_frames_available);
	}
	return a_length;
}
//---------------------------------------------------------------------------

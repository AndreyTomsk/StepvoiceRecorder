#include "stdafx.h"
#include <basswasapi.h>
#include "WasapiAudioClient.h"
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

CWasapiAudioClient::CWasapiAudioClient(int device)
	:m_wfx(NULL)
	,m_audioState(eStopped)
{
	HRESULT hr;

	// Taking device, based on device ID string (BASS enumeration doesn't
	// equal with WASAPI's). Again, real device IDs starts from 1 in BASS.
	BASS_WASAPI_DEVICEINFO info;
	BOOL result = BASS_WASAPI_GetDeviceInfo(device, &info);
	ASSERT(result);
	EIF(WasapiHelpers::GetActiveDevice(CString(info.id), &m_audioClient));

	const DWORD streamFlags = (info.flags & BASS_DEVICE_LOOPBACK) ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0;
	const REFERENCE_TIME bufferDuration = 500 * MFTIMES_PER_MILLISEC;

	EIF(m_audioClient->GetMixFormat(&m_wfx));
	EIF(m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, bufferDuration, 0, m_wfx, NULL));
	EIF(m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_captureClient));

Exit:
	return;
}
//---------------------------------------------------------------------------

CWasapiAudioClient::~CWasapiAudioClient()
{
	Stop();
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

std::auto_ptr<CWasapiCaptureBuffer> CWasapiAudioClient::GetCaptureBuffer() const
{
	//This method is likely called from another thread.
	CMyLock lock(m_sync_object);
	if (m_audioState == eStarted)
		return new CWasapiCaptureBuffer(m_captureClient, m_wfx->nBlockAlign);
	else
		return new CWasapiCaptureBuffer(NULL, 0);
}
//---------------------------------------------------------------------------

#include "stdafx.h"
#include <bassmix.h>
#include <math.h> //for abs
#include "WasapiRecorderMulti.h"
#include "WasapiRecorderStream.h"
#include "WasapiHelpers.h"
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWasapiRecorderMulti::CWasapiRecorderMulti(
	WasapiHelpers::DevicesArray devices, DWORD freq, DWORD chans)
	:m_actualFreq(0)
	,m_actualChans(0)
	,m_hTimer(INVALID_HANDLE_VALUE)
{
	for (unsigned i = 0; i < devices.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair& p = devices[i];
		const int deviceID = p.first;

		CWasapiRecorderStream* rs = new CWasapiRecorderStream(deviceID, freq, chans);
		m_recorderStreams.push_back(rs);

		m_actualFreq = max(m_actualFreq, rs->GetActualFrequency());
		m_actualChans = max(m_actualChans, rs->GetActualChannelCount());
	}

	const DWORD flags = (m_actualChans == 1) ? BASS_DEVICE_MONO : 0;
	const HWND mainWindowHandle = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	BOOL result = BASS_Init(0, m_actualFreq, flags, mainWindowHandle, NULL);

	m_mixerStream = BASS_Mixer_StreamCreate(m_actualFreq, m_actualChans,
		BASS_SAMPLE_FLOAT|BASS_STREAM_DECODE);
	ASSERT(m_mixerStream != 0);

	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
	{
		CWasapiRecorderStream* rs = m_recorderStreams[i];
		result = BASS_Mixer_StreamAddChannel(m_mixerStream, rs->GetStreamHandle(), BASS_MIXER_DOWNMIX);
		ASSERT(result);
	}
}
//---------------------------------------------------------------------------

CWasapiRecorderMulti::~CWasapiRecorderMulti()
{
	Stop();
	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
		delete m_recorderStreams[i];
	m_recorderStreams.clear();

	BASS_StreamFree(m_mixerStream);
	BASS_Free();
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetActualFrequency() const
{
	return m_actualFreq;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetActualChannelCount() const
{
	return m_actualChans;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Start()
{
	CMyLock lock(m_sync_object);

	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
		m_recorderStreams[i]->SetStarted(true);

	BOOL result = BASS_ChannelPlay(m_mixerStream, false); //no-clear internal buffer
	ASSERT(result);

	result = CreateTimerQueueTimer(&m_hTimer, NULL, TimerCallback, this, 0, 50, WT_EXECUTEDEFAULT);
	ASSERT(m_hTimer != INVALID_HANDLE_VALUE);
	ASSERT(result);

	return result;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Pause()
{
	CMyLock lock(m_sync_object);

	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
		m_recorderStreams[i]->SetStarted(false);

	if (m_hTimer !=	INVALID_HANDLE_VALUE)
	{
		DeleteTimerQueueTimer(NULL, m_hTimer, 0);
		m_hTimer = INVALID_HANDLE_VALUE;
	}

	BOOL result = BASS_ChannelPause(m_mixerStream);
	ASSERT(result);
	return result;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Stop()
{
	CMyLock lock(m_sync_object);

	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
		m_recorderStreams[i]->SetStarted(false);

	if (m_hTimer !=	INVALID_HANDLE_VALUE)
	{
		DeleteTimerQueueTimer(NULL, m_hTimer, 0);
		m_hTimer = INVALID_HANDLE_VALUE;
	}

	BOOL result = BASS_ChannelStop(m_mixerStream);
	ASSERT(result);
	return result;
}
//---------------------------------------------------------------------------

float CWasapiRecorderMulti::GetVolume() const
{
	if (!m_recorderStreams.empty())
		return m_recorderStreams[0]->GetVolume();
	else
		return 1.0;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::SetVolume(float volume)
{
	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
		m_recorderStreams[i]->SetVolume(volume);
	return TRUE;
}
//---------------------------------------------------------------------------

float CWasapiRecorderMulti::GetPeakLevel(int channel) const
{
	float maxLevel = 0;
	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
	{
		const float streamLevel = abs(m_recorderStreams[i]->GetPeakLevel(channel));
		maxLevel = max(maxLevel, streamLevel);
	}
	return maxLevel;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetData(void* buffer, DWORD lengthBytes) const
{
	return 0;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderMulti::GetChannelData(int channel, float* buffer, int bufferSize)
{
	return 0;
}
//---------------------------------------------------------------------------

void CALLBACK CWasapiRecorderMulti::TimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	CWasapiRecorderMulti* multiRecorder = static_cast<CWasapiRecorderMulti*>(lpParameter);
	CMyLock lock(multiRecorder->m_sync_object);

	const DWORD TEMP_BUFFER_SIZE = 8192;
	static float tempBuffer[TEMP_BUFFER_SIZE] = {0.0};

	while (true)
	{
		const DWORD bytesReceived = BASS_ChannelGetData(
				multiRecorder->m_mixerStream, tempBuffer, TEMP_BUFFER_SIZE);
		if (bytesReceived == -1)
			break;
		multiRecorder->ProcessData(tempBuffer, bytesReceived);
	}
}
//------------------------------------------------------------------------------

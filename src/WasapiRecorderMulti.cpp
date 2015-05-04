#include "stdafx.h"
#include <bassmix.h>
#include <math.h> //for abs
#include "WasapiRecorderMulti.h"
#include "WasapiRecorderStream.h"
#include "WasapiHelpers.h"
#include "BASS_Functions.h" //for Bass::ShowErrorFrom
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
	,m_exitThread(false)
	,m_recorderState(eStopped)
{
	// Creating WASAPI recorders for each device. Calculating result freq and channels.

	for (unsigned i = 0; i < devices.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair& p = devices[i];
		const int deviceID = p.first;

		CWasapiRecorderStream* rs = new CWasapiRecorderStream(deviceID);
		m_recorderStreams.push_back(rs);

		m_actualFreq = max(m_actualFreq, rs->GetActualFrequency());
		m_actualChans = max(m_actualChans, rs->GetActualChannelCount());
	}

	// Creating mixer stream.

	const DWORD flags = (m_actualChans == 1) ? BASS_DEVICE_MONO : 0;
	const HWND mainWindowHandle = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	BOOL result = BASS_Init(-1, m_actualFreq, flags, mainWindowHandle, NULL);

	m_mixerStream = BASS_Mixer_StreamCreate(m_actualFreq, m_actualChans,
		BASS_SAMPLE_FLOAT);//|BASS_STREAM_DECODE);
	ASSERT(m_mixerStream != 0);

	// Adding channels to stream.

	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
	{
		CWasapiRecorderStream* rs = m_recorderStreams[i];
		result = BASS_Mixer_StreamAddChannel(m_mixerStream, rs->GetStreamHandle(), BASS_MIXER_DOWNMIX);
		if (!result)
			Bass::ShowErrorFrom(_T("BASS_Mixer_StreamAddChannel"));
		ASSERT(result);
	}


	// Creating thread for reading data from stream.

	//m_streamEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL); //manual, not signaled
	//m_streamThread = ::CreateThread(NULL, 0, ReadDataFromStreamProc, this, 0, 0);
	//ASSERT(m_streamThread != NULL);
}
//---------------------------------------------------------------------------

CWasapiRecorderMulti::~CWasapiRecorderMulti()
{
	/*
	//Closing working thread.
	m_exitThread = true;
	::SetEvent(m_streamEvent);
	::WaitForSingleObject(m_streamThread, INFINITE);

	::CloseHandle(m_streamEvent);
	::CloseHandle(m_streamThread);
	*/

	for (unsigned i = 0; i < m_recorderStreams.size(); i++)
		delete m_recorderStreams[i];
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
	/*
	BOOL result = ::SetEvent(m_streamEvent);
	m_recorderState = eStarted;
	return result;
	*/
	return BASS_ChannelPlay(m_mixerStream, FALSE);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Pause()
{
	/*
	CMyLock lock(m_sync_object);

	BOOL result = ::ResetEvent(m_streamEvent);
	m_recorderState = ePaused;
	return result;
	*/
	return BASS_ChannelPause(m_mixerStream);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Stop()
{
	/*
	//Aqquire lock to ensure ProcessData in worker thread is finished. This is
	//needed to let other filters close their resources (we have an example in
	//MainFrm.cpp - ForceClose is called to file just after stopping recorder).
	CMyLock lock(m_sync_object);

	BOOL result = ::ResetEvent(m_streamEvent);
	m_recorderState = eStopped;

	return result;
	*/
	return BASS_ChannelStop(m_mixerStream);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::IsStarted() const
{
	return (m_recorderState == eStarted);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::IsPaused() const
{
	return (m_recorderState == ePaused);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::IsStopped() const
{
	return (m_recorderState == eStopped);
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

DWORD WINAPI CWasapiRecorderMulti::ReadDataFromStreamProc(LPVOID lpParam)
{
	const DWORD TEMP_BUFFER_SIZE = 8192;
	static float tempBuffer[TEMP_BUFFER_SIZE];

	CWasapiRecorderMulti* recorder = static_cast<CWasapiRecorderMulti*>(lpParam);
	while (true)
	{
		::WaitForSingleObject(recorder->m_streamEvent, INFINITE);
		if (recorder->m_exitThread)
			break;

		::Sleep(20); //Letting data appear in mixer buffer.
		while (true)
		{
			CMyLock lock(recorder->m_sync_object);
			if (recorder->m_recorderState != eStarted)
				break;

			const DWORD bytesReceived = BASS_ChannelGetData(
				recorder->m_mixerStream, tempBuffer, TEMP_BUFFER_SIZE);

			if (bytesReceived == 0 || bytesReceived == -1)
				break;

			recorder->ProcessData(tempBuffer, bytesReceived);
		}
	}

	return 0;
}
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <bassmix.h>
#include <math.h> //for abs
#include <memory> //for auto_ptr
#include "SampleConverter.h"
#include "WasapiRecorderMulti.h"
#include "WasapiAudioClient.h"
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
	:m_actualFreq(INT_MAX)
	,m_actualChans(0)
	,m_exitThread(false)
	,m_recorderState(eStopped)
{
	// Creating WASAPI recorders for each device. Calculating result freq and channels.
	// For easier mixing, we get minimal frequency among all channels as result.

	for (unsigned i = 0; i < devices.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair& p = devices[i];
		const int deviceID = p.first;

		CWasapiAudioClient* ac = new CWasapiAudioClient(deviceID);
		m_audioClients.push_back(ac);

		//m_actualFreq = min(m_actualFreq, ac->GetActualFrequency());
		//m_actualChans = max(m_actualChans, ac->GetActualChannelCount());
		m_actualFreq = 44100;
		m_actualChans = 2;
	}

	// Creating thread for reading data from stream.

	m_streamEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL); //manual, not signaled
	m_streamThread = ::CreateThread(NULL, 0, ReadDataFromStreamProc, this, 0, 0);
	ASSERT(m_streamThread != NULL);
}
//---------------------------------------------------------------------------

CWasapiRecorderMulti::~CWasapiRecorderMulti()
{
	//Close working thread.
	m_exitThread = true;
	::SetEvent(m_streamEvent);
	::WaitForSingleObject(m_streamThread, INFINITE);

	::CloseHandle(m_streamEvent);
	::CloseHandle(m_streamThread);

	for (unsigned i = 0; i < m_audioClients.size(); i++)
		delete m_audioClients[i];
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

	BOOL atLeastOneStarted = FALSE;
	for (unsigned i = 0; i < m_audioClients.size(); i++)
		atLeastOneStarted |= m_audioClients[i]->Start();

	if (atLeastOneStarted)
	{
		m_recorderState = eStarted;
		::SetEvent(m_streamEvent);
	}
	return atLeastOneStarted;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Pause()
{
	CMyLock lock(m_sync_object);

	BOOL atLeastOnePaused = FALSE;
	for (unsigned i = 0; i < m_audioClients.size(); i++)
		atLeastOnePaused |= m_audioClients[i]->Pause();

	if (atLeastOnePaused)
	{
		m_recorderState = ePaused;
		::ResetEvent(m_streamEvent);
	}
	return atLeastOnePaused;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::Stop()
{
	//Aqquire lock to ensure ProcessData in worker thread is finished. This is
	//needed to let other filters close their resources (we have an example in
	//MainFrm.cpp - ForceClose is called to file just after stopping recorder).
	CMyLock lock(m_sync_object);

	for (unsigned i = 0; i < m_audioClients.size(); i++)
		m_audioClients[i]->Stop();

	BOOL result = ::ResetEvent(m_streamEvent);
	m_recorderState = eStopped;
	return result;
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

BOOL CWasapiRecorderMulti::VolumeControlAvailable() const
{
	return TRUE;
}
//---------------------------------------------------------------------------

float CWasapiRecorderMulti::GetVolume() const
{
	if (!m_audioClients.empty())
		return m_audioClients[0]->GetVolume();
	else
		return 1.0;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::SetVolume(float volume)
{
	for (unsigned i = 0; i < m_audioClients.size(); i++)
		m_audioClients[i]->SetVolume(volume);
	return TRUE;
}
//---------------------------------------------------------------------------

float CWasapiRecorderMulti::GetPeakLevel(int channel) const
{
	float maxLevel = 0;
	for (unsigned i = 0; i < m_audioClients.size(); i++)
	{
		const float streamLevel = abs(m_audioClients[i]->GetPeakLevel(channel));
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
	//New algorithm:
	//1. Making large temprorary buffer.
	//2. Putting all data from a channel to buffer.
	//3. Getting all data from another channel, re-sampling
	//   and mixing it with existing data in buffer.
	//4. If all channels silent, then determining amount of empty samples.
	//5. Passing buffer data to next filter.

	CWasapiRecorderMulti* recorder = static_cast<CWasapiRecorderMulti*>(lpParam);
	
	const DWORD TEMP_BUFFER_SIZE = 81920;
	static float tempBufferInternal[TEMP_BUFFER_SIZE];

	const int actualFreq = recorder->GetActualFrequency();
	const int actualChans = recorder->GetActualChannelCount();
	const int sampleSizeBytes = sizeof(float)*actualChans;
	SampleBuffer tempBuffer(actualFreq, actualChans, tempBufferInternal);
	tempBuffer.maxSamples = TEMP_BUFFER_SIZE/actualChans;

	while (true)
	{
		::WaitForSingleObject(recorder->m_streamEvent, INFINITE);
		if (recorder->m_exitThread)
			break;

		::Sleep(100); //Letting data appear in recorders.
		while (true)
		{
			CMyLock lock(recorder->m_sync_object);
			if (recorder->m_recorderState != eStarted)
				break;

			bool newDataAvailable = false;
			for (unsigned i = 0; i < recorder->m_audioClients.size(); i++)
			{
				std::auto_ptr<CWasapiCaptureBuffer> buffer(
					recorder->m_audioClients[i]->GetCaptureBuffer());

				if (buffer->GetBytesAvailable())
				{
	//WriteDbg() << __FUNCTION__ << " ::2, stream" << i << ", bytes available=" << buffer->GetBytesAvailable();

					newDataAvailable = true;
					//recorder->ProcessData(buffer->GetBuffer(), buffer->GetBytesAvailable());

					const int actualFreq = recorder->m_audioClients[i]->GetActualFrequency();
					const int actualChans2 = recorder->m_audioClients[i]->GetActualChannelCount();
					SampleBuffer sourceBuffer(actualFreq, actualChans2, (float*)buffer->GetBuffer());
					sourceBuffer.curSamples = buffer->GetBytesAvailable()/(sizeof(float)*actualChans2);
					sourceBuffer.maxSamples = sourceBuffer.curSamples;

					ConvertSamples(sourceBuffer, tempBuffer);
					recorder->ProcessData(tempBuffer.data, tempBuffer.curSamples*sampleSizeBytes);
				}
			}

			if (!newDataAvailable)
				break;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------

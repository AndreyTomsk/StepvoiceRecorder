/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include <cmath> //for float abs
#include <vector>
#include <algorithm>
#include "SampleConverter.h"
#include "WasapiRecorderMulti.h"
#include "WasapiAudioClient.h"
#include "WasapiHelpers.h"
#include "BASS_Functions.h" //for Bass::ShowErrorFrom

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
	LOG_DEBUG();
	LOG_DEBUG() << __FUNCTION__<< ", frequency: " << freq << ", channels: " << chans << ", devices:";
	for (size_t i = 0; i < devices.size(); i++)
		LOG_DEBUG() << "  " << devices[i].first << " - " << devices[i].second;

	// Creating WASAPI recorders for each device. Calculating result freq and channels.
	// For easier mixing, we get minimal frequency among all channels as result.

	ASSERT(devices.size() > 0);
	for (unsigned i = 0; i < devices.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair& p = devices[i];
		const int deviceID = p.first;

		CWasapiAudioClient* ac = new CWasapiAudioClient(deviceID, freq, chans);
		m_audioClients.push_back(ac);

		m_actualFreq = min(m_actualFreq, ac->GetActualFrequency());
		m_actualChans = max(m_actualChans, ac->GetActualChannelCount());
	}

	LOG_DEBUG() << "actual frequency: " << m_actualFreq << ", actual channels: " << m_actualChans;

	//Not forget resampling. Getting fixed data count from each stream - 
	//should mix all sounds at one sample rate and stereo mode.

	for (size_t i = 0; i < m_audioClients.size(); i++)
		m_audioClients[i]->SetResampleParams(m_actualFreq, m_actualChans);

	// Creating thread for reading data from stream.

	m_streamEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL); //manual, not signaled
	m_streamThread = ::CreateThread(NULL, 0, ReadDataFromStreamProc, this, 0, 0);
	ASSERT(m_streamThread != NULL);

	LOG_DEBUG() << "Ok";
}
//---------------------------------------------------------------------------

CWasapiRecorderMulti::~CWasapiRecorderMulti()
{
	LOG_DEBUG() << __FUNCTION__;

	//Close working thread.
	m_exitThread = true;
	::SetEvent(m_streamEvent);
	::WaitForSingleObject(m_streamThread, INFINITE);

	::CloseHandle(m_streamEvent);
	::CloseHandle(m_streamThread);

	for (unsigned i = 0; i < m_audioClients.size(); i++)
		delete m_audioClients[i];

	LOG_DEBUG() << "Ok";
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
	LOG_DEBUG() << __FUNCTION__;

	CMyLock lock(m_sync_object);

	BOOL atLeastOneStarted = FALSE;
	for (unsigned i = 0; i < m_audioClients.size(); i++)
		atLeastOneStarted |= m_audioClients[i]->Start();

	if (atLeastOneStarted)
	{
		m_recorderState = eStarted;
		::SetEvent(m_streamEvent);
	}

	LOG_DEBUG() << "Ok, atLeastOneStarted=" << atLeastOneStarted;
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
	LOG_DEBUG() << __FUNCTION__;

	//Aqquire lock to ensure ProcessData in worker thread is finished. This is
	//needed to let other filters close their resources (we have an example in
	//MainFrm.cpp - ForceClose is called to file just after stopping recorder).
	CMyLock lock(m_sync_object);

	for (unsigned i = 0; i < m_audioClients.size(); i++)
		m_audioClients[i]->Stop();

	BOOL result = ::ResetEvent(m_streamEvent);
	m_recorderState = eStopped;

	LOG_DEBUG() << "Ok, result=" << result;
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
	//Get minimal volume of all devices. Devices that doesn't have volume
	//control (loopback, etc.) return 1.0f.

	float curVolume = 1.0f;
	for (unsigned i = 0; i < m_audioClients.size(); i++)
		curVolume = min(curVolume, m_audioClients[i]->GetVolume());

	return curVolume;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderMulti::SetVolume(float newVolume)
{
	if (newVolume != GetVolume())
	{
		//LOG_DEBUG() << __FUNCTION__ << ", volume=" << newVolume;
		for (unsigned i = 0; i < m_audioClients.size(); i++)
			m_audioClients[i]->SetVolume(newVolume);
	}
	return TRUE;
}
//---------------------------------------------------------------------------

float CWasapiRecorderMulti::GetPeakLevel(int channel) const
{
	float maxLevel = 0;
	for (unsigned i = 0; i < m_audioClients.size(); i++)
	{
		const float streamLevel = std::abs(m_audioClients[i]->GetPeakLevel(channel));
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

static float MixChannels(float a, float b)
{
	return (float)max(-1.0, min(a + b, 1.0));
}
//---------------------------------------------------------------------------

DWORD WINAPI CWasapiRecorderMulti::ReadDataFromStreamProc(LPVOID lpParam)
{
	CWasapiRecorderMulti* recorder = static_cast<CWasapiRecorderMulti*>(lpParam);

	//NOTE: problematic thing. If too large, devices could not fill it.
	const DWORD BUFFER_SIZE = 1024;
	const DWORD BUFFER_SIZE_BYTES = BUFFER_SIZE*sizeof(float);

	std::vector<float> vDst;
	std::vector<float> vTmp;
	vDst.resize(BUFFER_SIZE);
	vTmp.resize(BUFFER_SIZE);

	while (true)
	{
		::WaitForSingleObject(recorder->m_streamEvent, INFINITE);
		if (recorder->m_exitThread)
		{
			LOG_DEBUG() << __FUNCTION__ <<", exiting thread.";
			break;
		}

		::Sleep(200); //Letting data appear in recorders.
		while (true)
		{
			CMyLock lock(recorder->m_sync_object);
			if (recorder->m_recorderState != eStarted)
			{
				LOG_DEBUG() << __FUNCTION__ <<", not started.";
				break;
			}

			// 1. Clear result buffer;
			// 2. Get stream buffer;
			// 3. Mix with result.
			// 4. Goto 2, until all buffers mixed.

			std::fill(vDst.begin(), vDst.end(), float(0));

			bool vDstFilled = false;
			for (size_t i = 0; i < recorder->m_audioClients.size(); i++)
			{
				bool streamError = false;
				CWasapiAudioClient* ac = recorder->m_audioClients[i];
				if (!ac->GetData((BYTE*)&vTmp.front(), BUFFER_SIZE_BYTES, streamError))
				{
					//LOG_DEBUG() << __FUNCTION__ << ", no data in device " << ac->GetDeviceID();
					break;
				}

				std::transform(vDst.begin(), vDst.end(), vTmp.begin(), vDst.begin(), MixChannels);
				vDstFilled = true;

				//All buffers must be non-empty, or distortions possible.
				//if (ac->GetData((BYTE*)&vTmp.front(), BUFFER_SIZE_BYTES, streamError))
				//{
				//	std::transform(vDst.begin(), vDst.end(), vTmp.begin(), vDst.begin(), MixChannels);
				//	vDstFilled = true;
				//}
			}
			if (!vDstFilled)
				break;

			recorder->ProcessData(&vDst.front(), BUFFER_SIZE_BYTES);
		}
	}

	return 0;
}
//------------------------------------------------------------------------------

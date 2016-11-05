/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include "DeviceLevels.h"
#include <basswasapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDeviceInitializer
{
public:
	CDeviceInitializer(int deviceID) : m_deviceID(deviceID)
	{
		BASS_WASAPI_Init(m_deviceID, 44100, 2, BASS_WASAPI_AUTOFORMAT, 0.5, 0, EmptyProc, NULL);
		BASS_WASAPI_Start();
	}
	~CDeviceInitializer()
	{
		BASS_WASAPI_SetDevice(m_deviceID);
		BASS_WASAPI_Free(); //frees device, currently set for this thread
	}
private:
	static DWORD CALLBACK EmptyProc(void*, DWORD, void*) { return 1; }//0 stops recording
	int m_deviceID;
};

/////////////////////////////////////////////////////////////////////////////

class CDeviceLevel
{
public:
	CDeviceLevel(int deviceID) : m_deviceID(deviceID)
	{
		m_exitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL); //manual, not signaled
		m_thread = ::CreateThread(NULL, 0, DeviceInitializerProc, this, 0, 0);
		ASSERT(m_thread != NULL);
	}
	~CDeviceLevel()
	{
		::SetEvent(m_exitEvent);
		::WaitForSingleObject(m_thread, INFINITE);
		::CloseHandle(m_thread);
		::CloseHandle(m_exitEvent);
	}
	float GetLevel() const
	{
		return BASS_WASAPI_GetDeviceLevel(m_deviceID, -1); //all channels
	}

	static DWORD WINAPI DeviceInitializerProc(LPVOID lpParam)
	{
		CDeviceLevel* dl = static_cast<CDeviceLevel*>(lpParam);
		CDeviceInitializer device(dl->m_deviceID);
		::WaitForSingleObject(dl->m_exitEvent, INFINITE);
		return 0;
	}
	const int m_deviceID;
	HANDLE m_thread;
	HANDLE m_exitEvent;
};

/////////////////////////////////////////////////////////////////////////////

CDeviceLevels::CDeviceLevels(const WasapiHelpers::DevicesArray& devices)
{
	for (size_t i = 0; i < devices.size(); i++)
		m_deviceLevels.push_back(new CDeviceLevel(devices[i].first));
}
//---------------------------------------------------------------------------

CDeviceLevels::~CDeviceLevels()
{
	for (size_t i = 0; i < m_deviceLevels.size(); i++)
		delete m_deviceLevels[i];
}
//---------------------------------------------------------------------------

float CDeviceLevels::GetLevel(int deviceID) const
{
	for (size_t i = 0; i < m_deviceLevels.size(); i++)
	{
		CDeviceLevel* curDevice = m_deviceLevels[i];
		if (curDevice->m_deviceID == deviceID) return curDevice->GetLevel();
	}
	return 0.0;
}
//---------------------------------------------------------------------------

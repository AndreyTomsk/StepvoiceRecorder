// WasapiRecorder.cpp : implementation file
//

#include "stdafx.h"
#include "WasapiRecorder.h"
#include <basswasapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CurrentThreadDevice
{
public:
	CurrentThreadDevice(int device)
	{
		m_oldDevice = BASS_WASAPI_GetDevice();
		BASS_WASAPI_SetDevice(device);
	}
	~CurrentThreadDevice()
	{
		BASS_WASAPI_SetDevice(m_oldDevice);
	}
private:
	int m_oldDevice;
};

/////////////////////////////////////////////////////////////////////////////

static DWORD CALLBACK EmptyProc(void* , DWORD , void* ) { return 1; }

//---------------------------------------------------------------------------

static BOOL GetDeviceActualData(int device, DWORD& frequency, DWORD& channels)
//all reference params are in/out
{
	BASS_WASAPI_Init(device, frequency, channels, BASS_WASAPI_AUTOFORMAT, 0.5, 0, EmptyProc, NULL);

	BASS_WASAPI_INFO info;
	BASS_WASAPI_GetInfo(&info);
	frequency = info.freq;
	channels = info.chans;

	return BASS_WASAPI_Free();
}

/////////////////////////////////////////////////////////////////////////////

CWasapiRecorder::CWasapiRecorder(int device, DWORD freq, DWORD chans, OUTPUTPROC* outputProc, void* user)
	:m_deviceID(device)
	,m_isMono(false)
{
	BOOL result = GetDeviceActualData(device, freq, chans);
	ASSERT(result);
	m_isMono = (chans == 1);

	result = BASS_Init(0, freq, m_isMono ? BASS_DEVICE_MONO : 0, 0, NULL);
	//0=no sound device.
	ASSERT(result);

	if (outputProc == NULL)
		outputProc = EmptyProc; //could not initialize if output procedure empty

	result = BASS_WASAPI_Init(device, freq, chans,
		BASS_WASAPI_AUTOFORMAT | BASS_WASAPI_BUFFER,
		0.5,	//length of the device's buffer in seconds
		0,		//interval (in seconds) between callback function calls, 0=use default.
		outputProc,
		user);
	ASSERT(result);
}
//---------------------------------------------------------------------------

CWasapiRecorder::~CWasapiRecorder()
{
	CurrentThreadDevice temp(m_deviceID);
	BASS_WASAPI_Free();

	BASS_SetDevice(0); //no sound device
	BASS_Free();
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorder::GetActualFrequency() const
{
	CurrentThreadDevice temp(m_deviceID);
	BASS_WASAPI_INFO info;
	BASS_WASAPI_GetInfo(&info);
	return info.freq;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorder::GetActualChannelCount() const
{
	CurrentThreadDevice temp(m_deviceID);
	BASS_WASAPI_INFO info;
	BASS_WASAPI_GetInfo(&info);
	return info.chans;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::Start()
{
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_Start();
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::Pause()
{
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_Stop(FALSE /*keep device buffer*/);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::Stop()
{
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_Stop(TRUE /*reset device buffer*/);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::IsStarted() const
{
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_IsStarted();
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::IsPaused() const
{
	const DWORD bytesAvailable = GetData(NULL, BASS_DATA_AVAILABLE);
	return !IsStarted() && (bytesAvailable != 0);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::IsStopped() const
{
	const DWORD bytesAvailable = GetData(NULL, BASS_DATA_AVAILABLE);
	return !IsStarted() && (bytesAvailable == 0);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::VolumeControlAvailable() const
{
	return GetVolume() != -1;
}
//---------------------------------------------------------------------------

float CWasapiRecorder::GetVolume() const
{
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_GetVolume(BASS_WASAPI_CURVE_WINDOWS);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::SetVolume(float volume)
{
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_SetVolume(BASS_WASAPI_CURVE_WINDOWS, volume);
}
//---------------------------------------------------------------------------

float CWasapiRecorder::GetPeakLevel(int channel) const
{
	const float level = BASS_WASAPI_GetDeviceLevel(m_deviceID, m_isMono ? 0 : channel);
	ASSERT(level != -1);
	return level;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorder::GetData(void* buffer, DWORD lengthBytes) const
{
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_GetData(buffer, lengthBytes);
}
//---------------------------------------------------------------------------

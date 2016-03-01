#include "stdafx.h"
#include <basswasapi.h>
#include "WasapiRecorder.h"
#include "WasapiHelpers.h"
#include <algorithm>

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

//static DWORD CALLBACK EmptyProc(void* , DWORD , void* ) { return 1; }

//CWasapiRecorder::CWasapiRecorder(int device, DWORD freq, DWORD chans, OUTPUTPROC* outputProc, void* user)
CWasapiRecorder::CWasapiRecorder(int device, DWORD freq, DWORD chans)
	:m_deviceID(device)
{
	LOG_DEBUG();
	LOG_DEBUG() << __FUNCTION__ << ", device=" << device << ", frequency=" << freq << ", channels=" << chans;

	if (!WasapiHelpers::GetDeviceActualData(device, freq, chans, m_actualFreq, m_actualChans))
		return;

	LOG_DEBUG() << "  actual data received. actualFreq=" << m_actualFreq << ", actualChans=" << m_actualChans;

	const bool isMono = (m_actualChans == 1);
	if (!BASS_Init(0 /*no sound device*/, m_actualFreq, isMono ? BASS_DEVICE_MONO : 0, 0, NULL))
		return;

	LOG_DEBUG() << "  bass initialized";

	//if (outputProc == NULL)
	//	outputProc = EmptyProc; //could not initialize if output procedure empty

	BOOL result = BASS_WASAPI_Init(device, m_actualFreq, m_actualChans,
		BASS_WASAPI_BUFFER, //with actual freq/chans should work without BASS_WASAPI_AUTOFORMAT!
		0.5,	//length of the device's buffer in seconds
		0,		//interval (in seconds) between callback function calls, 0=use default.
		OutputProc,
		this);

	ASSERT(result);
	LOG_DEBUG() << "Ok, bass wasapi init result=" << result;
}
//---------------------------------------------------------------------------

CWasapiRecorder::~CWasapiRecorder()
{
	LOG_DEBUG() << __FUNCTION__;

	CurrentThreadDevice temp(m_deviceID);
	BASS_WASAPI_Free();

	LOG_DEBUG() << "  bass wasapi free.";

	BASS_SetDevice(0); //no sound device
	BASS_Free();
	LOG_DEBUG() << "  Ok.";
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorder::GetActualFrequency() const
{
	return m_actualFreq;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorder::GetActualChannelCount() const
{
	return m_actualChans;
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
	CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_GetVolume(BASS_WASAPI_CURVE_WINDOWS) != -1;
}
//---------------------------------------------------------------------------

float CWasapiRecorder::GetVolume() const
{
	CurrentThreadDevice temp(m_deviceID);
	const float resultVolume = BASS_WASAPI_GetVolume(BASS_WASAPI_CURVE_WINDOWS);
	return (resultVolume != -1) ? resultVolume : 1.0f;
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorder::SetVolume(float newVolume)
{
	CurrentThreadDevice temp(m_deviceID);
	if (newVolume == GetVolume())
		return TRUE;

	//LOG_DEBUG() << __FUNCTION__ << ", newVolume=" << newVolume;
	
	BOOL result = BASS_WASAPI_SetVolume(BASS_WASAPI_CURVE_WINDOWS, newVolume);
	CallVolumeChangedEvents(newVolume);
	return result;
}
//---------------------------------------------------------------------------

float CWasapiRecorder::GetPeakLevel(int channel) const
{
	const bool isMono = (m_actualChans == 1);
	const float level = BASS_WASAPI_GetDeviceLevel(m_deviceID, isMono ? 0 : channel);
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

DWORD CWasapiRecorder::GetChannelData(int channel, float* buffer, int bufferSize)
{
	const bool isMono = (m_actualChans == 1);
	if (isMono)
	{
		const DWORD bytesWritten = GetData(buffer, bufferSize*sizeof(float));
		return (bytesWritten == -1) ? 0 : bytesWritten/sizeof(float);
	}

	static const int LOCAL_SIZE = 2048*2; //hack, used parameters from GraphWnd.cpp
	static float localBuffer[LOCAL_SIZE];

	const DWORD bytesRequested = min(LOCAL_SIZE, bufferSize)*sizeof(float);
	const DWORD bytesWritten = GetData(localBuffer, bytesRequested);
	if (bytesWritten == -1)
		return 0;

	for (int i=0, count=bytesWritten/sizeof(float); i < count; i += 2)
		buffer[i] = localBuffer[i+channel];
	return bytesWritten/sizeof(float);
}
//---------------------------------------------------------------------------

DWORD CALLBACK CWasapiRecorder::OutputProc(void* buffer, DWORD lengthBytes, void* user)
{
	CWasapiRecorder* recorder = static_cast<CWasapiRecorder*>(user);
	if (recorder->ProcessData(buffer, lengthBytes))
		return 1;

	recorder->SendNotification(Parameter(_T("Recorder.Stopped"), CString()));
	return 0;
}
//---------------------------------------------------------------------------

bool CWasapiRecorder::ProcessData(void* buffer, DWORD lengthBytes)
{
	//Just using base implementation: it sends data to next filter in chain.
	return Filter::ProcessData(buffer, lengthBytes);
}
//---------------------------------------------------------------------------

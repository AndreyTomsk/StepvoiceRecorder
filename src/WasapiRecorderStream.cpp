#include "stdafx.h"
#include <bass.h>
#include <basswasapi.h>
#include "WasapiRecorderStream.h"
#include "WasapiHelpers.h"
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWasapiRecorderStream::CWasapiRecorderStream(int device, DWORD freq, DWORD chans)
	:m_deviceID(device)
	,m_isStarted(false)
	,m_hStream(0)
{
	//BASS_Init for no-sound device and BASS_Free must be called from the
	//multi-recorder.
	
	if (!WasapiHelpers::GetDeviceActualData(device, freq, chans, m_actualFreq, m_actualChans))
		return;

	//m_hStream = BASS_StreamCreate(m_actualFreq, m_actualChans,
	//	BASS_SAMPLE_FLOAT|BASS_STREAM_DECODE, STREAMPROC_PUSH, NULL);
	//ASSERT(m_hStream != 0);

	BOOL result = BASS_WASAPI_Init(device, m_actualFreq, m_actualChans,
		BASS_WASAPI_AUTOFORMAT, 0.5, 0,	OutputProc, this);
	ASSERT(result);
	result = BASS_WASAPI_Start();
	ASSERT(result);
}
//---------------------------------------------------------------------------

CWasapiRecorderStream::~CWasapiRecorderStream()
{
	CMyLock lock(m_sync_object);

	WasapiHelpers::CurrentThreadDevice temp(m_deviceID);
	BASS_WASAPI_Stop(TRUE /*reset device buffer*/);
	BASS_WASAPI_Free();

	BASS_StreamFree(m_hStream);
	m_hStream = 0;
}
//---------------------------------------------------------------------------

int CWasapiRecorderStream::GetDeviceID() const
{
	return m_deviceID;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderStream::GetActualFrequency() const
{
	return m_actualFreq;
}
//---------------------------------------------------------------------------

DWORD CWasapiRecorderStream::GetActualChannelCount() const
{
	return m_actualChans;
}
//---------------------------------------------------------------------------

HSTREAM CWasapiRecorderStream::GetStreamHandle() const
{
	if (!m_hStream)
	{
		m_hStream = BASS_StreamCreate(m_actualFreq, m_actualChans,
			BASS_SAMPLE_FLOAT|BASS_STREAM_DECODE, STREAMPROC_PUSH, NULL);
		ASSERT(m_hStream != 0);
	}
	return m_hStream;
}
//---------------------------------------------------------------------------

void CWasapiRecorderStream::SetStarted(bool isStarted)
{
	CMyLock lock(m_sync_object);
	m_isStarted = isStarted;
}
//---------------------------------------------------------------------------

float CWasapiRecorderStream::GetVolume() const
{
	WasapiHelpers::CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_GetVolume(BASS_WASAPI_CURVE_WINDOWS);
}
//---------------------------------------------------------------------------

BOOL CWasapiRecorderStream::SetVolume(float volume)
{
	WasapiHelpers::CurrentThreadDevice temp(m_deviceID);
	return BASS_WASAPI_SetVolume(BASS_WASAPI_CURVE_WINDOWS, volume);
}
//---------------------------------------------------------------------------

float CWasapiRecorderStream::GetPeakLevel(int channel) const
{
	const bool isMono = (m_actualChans == 1);
	const float level = BASS_WASAPI_GetDeviceLevel(m_deviceID, isMono ? 0 : channel);
	ASSERT(level != -1);
	return level;
}
//---------------------------------------------------------------------------

DWORD CALLBACK CWasapiRecorderStream::OutputProc(void* buffer, DWORD lengthBytes, void* user)
{
	CWasapiRecorderStream* recorder = static_cast<CWasapiRecorderStream*>(user);

	CMyLock lock(recorder->m_sync_object);
	if (recorder->m_isStarted && recorder->m_hStream != 0)
		BASS_StreamPutData(recorder->m_hStream, buffer, lengthBytes);

	return 1;
}
//---------------------------------------------------------------------------

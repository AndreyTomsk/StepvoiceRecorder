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
{
	//BASS_Init for no-sound device and BASS_Free must be called from the
	//multi-recorder.
	
	if (!WasapiHelpers::GetDeviceActualData(device, freq, chans, m_actualFreq, m_actualChans))
		return;

	m_hStream = BASS_StreamCreate(m_actualFreq, m_actualChans,
		BASS_SAMPLE_FLOAT|BASS_STREAM_DECODE, STREAMPROC_PUSH, NULL);
	ASSERT(m_hStream != 0);

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

DWORD CALLBACK CWasapiRecorderStream::OutputProc(void* buffer, DWORD lengthBytes, void* user)
{
	CWasapiRecorderStream* recorder = static_cast<CWasapiRecorderStream*>(user);

	CMyLock lock(recorder->m_sync_object);
	if (recorder->m_isStarted)
		BASS_StreamPutData(recorder->m_hStream, buffer, lengthBytes);

	return 1;
}
//---------------------------------------------------------------------------

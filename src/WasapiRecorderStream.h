#ifndef WASAPI_RECORDER_STREAM_H
#define WASAPI_RECORDER_STREAM_H
#pragma once

#include "SyncObjects.h"

/////////////////////////////////////////////////////////////////////////////
//Ok, idea is currently to provide a HSTREAM handle for multi-recorder.
//WASAPI recorder starts recording immediately, playback/pause should be
//controlled using the HSTREAM.

//BASS_Init for a no-sound device (+BASS_Free) must be called from the
//multi-recorder, before creating the CWasapiRecorderStream objects.

class CWasapiRecorderStream
{
	friend class CWasapiRecorderMulti;

private:
	CWasapiRecorderStream(int device, DWORD freq, DWORD chans);
	virtual ~CWasapiRecorderStream();

	//Notify CWasapiRecorderStream about started state, to begin
	//push data to HSTREAM.
	void SetStarted(bool isStarted);

	float GetVolume() const;
	BOOL  SetVolume(float volume); //0..1

private:
	static DWORD CALLBACK OutputProc(void* buffer, DWORD lengthBytes, void* user);

	typedef DWORD HSTREAM; //Sample stream handle (from bass.h).
	HSTREAM m_hStream;     //Main thing, used by multi-recorder.

	int m_deviceID;
	DWORD m_actualFreq;
	DWORD m_actualChans;
	bool m_isStarted;
	CMyCriticalSection m_sync_object;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_RECORDER_STREAM_H

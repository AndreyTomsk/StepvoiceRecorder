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

//Action sequence:
//  1. Create RecorderStream object;
//  2. Retrieve actual frequency and channels;
//  3. Get stream handle;
//  4. Set started flag to put a recording data to stream;

class CWasapiRecorderStream
{
public:
	typedef DWORD HSTREAM; //Sample stream handle (from bass.h).

	CWasapiRecorderStream(int device, DWORD freq, DWORD chans);
	~CWasapiRecorderStream();

	int GetDeviceID() const;
	DWORD GetActualFrequency() const;
	DWORD GetActualChannelCount() const;

	HSTREAM GetStreamHandle() const;
	void SetStarted(bool isStarted);

	float GetVolume() const;
	BOOL  SetVolume(float volume); //0..1

	float GetPeakLevel(int channel) const; //0 = first channel, -1 = all channels

private:
	static DWORD CALLBACK OutputProc(void* buffer, DWORD lengthBytes, void* user);

	mutable HSTREAM m_hStream;

	int m_deviceID;
	DWORD m_actualFreq;
	DWORD m_actualChans;
	bool m_isStarted;
	CMyCriticalSection m_sync_object;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_RECORDER_STREAM_H

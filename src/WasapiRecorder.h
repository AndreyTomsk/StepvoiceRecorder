#ifndef _WASAPI_RECORDER_H
#define _WASAPI_RECORDER_H
#pragma once

#include "Filter.h"

/////////////////////////////////////////////////////////////////////////////

class CWasapiRecorder : public Filter
{
public:
	//typedef DWORD (CALLBACK OUTPUTPROC)(void* buffer, DWORD lengthBytes, void* user);
	/*Copied definition from WASAPIPROC (to avoid additional include).
	buffer : Buffer containing a sample data
	length : Number of bytes
	user   : The 'user' parameter given when calling BASS_WASAPI_Init
	RETURN : The number of bytes written (output devices), 0/1 = stop/continue (input devices)*/

	//typedef DWORD (CALLBACK STATEPROC)(CWasapiRecorder* obj);

	//CWasapiRecorder(int device, DWORD freq, DWORD chans, OUTPUTPROC* outputProc, void* user);
	CWasapiRecorder(int device, DWORD freq, DWORD chans);
	~CWasapiRecorder();

	DWORD GetActualFrequency() const;
	DWORD GetActualChannelCount() const;

	BOOL Start();
	BOOL Pause();
	BOOL Stop();
	BOOL IsStarted() const;
	BOOL IsPaused() const;
	BOOL IsStopped() const;

	BOOL  VolumeControlAvailable() const;
	float GetVolume() const;
	BOOL  SetVolume(float volume); //0..1

	float GetPeakLevel(int channel) const; //0 = first channel, -1 = all channels
	DWORD GetData(void* buffer, DWORD lengthBytes) const; //returns -1 if error

	//For compatibility with GraphWnd callback
	DWORD GetChannelData(int channel, float* buffer, int bufferSize);

private:
	static DWORD CALLBACK OutputProc(void* buffer, DWORD lengthBytes, void* user);
	virtual bool ProcessData(void* buffer, DWORD lengthBytes); //overridden

	int m_deviceID;
	DWORD m_actualFreq;
	DWORD m_actualChans;
};

/////////////////////////////////////////////////////////////////////////////

#endif // _WASAPI_RECORDER_H

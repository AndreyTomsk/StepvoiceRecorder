#ifndef _WASAPI_RECORDER_H
#define _WASAPI_RECORDER_H
#pragma once

/////////////////////////////////////////////////////////////////////////////

class CWasapiRecorder
{
public:
	typedef DWORD (CALLBACK OUTPUTPROC)(void* buffer, DWORD lengthBytes, void* user);
	/*Copied definition from WASAPIPROC (to avoid additional include).
	buffer : Buffer containing the sample data
	length : Number of bytes
	user   : The 'user' parameter given when calling BASS_WASAPI_Init
	RETURN : The number of bytes written (output devices), 0/1 = stop/continue (input devices)*/

	CWasapiRecorder(int device, DWORD freq, DWORD chans, OUTPUTPROC* outputProc, void* user);
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
	int m_deviceID;
	DWORD m_actualFreq;
	DWORD m_actualChans;
};

/////////////////////////////////////////////////////////////////////////////

#endif // _WASAPI_RECORDER_H

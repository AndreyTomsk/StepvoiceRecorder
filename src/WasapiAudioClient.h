#ifndef WASAPI_AUDIO_CLIENT_H
#define WASAPI_AUDIO_CLIENT_H
#pragma once

#include <Audioclient.h>    //for IAudioClient, IAudioCaptureClient
#include "WasapiCaptureBuffer.h"
#include "WasapiCaptureBuffer2.h"
#include "SyncObjects.h"

/////////////////////////////////////////////////////////////////////////////

class CWasapiAudioClient
{
public:
	CWasapiAudioClient(int device);
	~CWasapiAudioClient();

	int GetDeviceID() const;
	DWORD GetActualFrequency() const;
	DWORD GetActualChannelCount() const;

	BOOL Start();
	BOOL Pause();
	BOOL Stop();
	BOOL IsStarted() const;
	BOOL IsPaused() const;
	BOOL IsStopped() const;

	float GetVolume() const;
	BOOL  SetVolume(float volume); //0..1

	CWasapiCaptureBuffer* GetCaptureBuffer() const; //Must be deleted by caller.
	float GetPeakLevel(int channel) const; //0 = first channel, -1 = all channels

	CWasapiCaptureBuffer2* m_captureBuffer2;

private:
	int m_deviceID;
	CComPtr<IAudioClient> m_audioClient;
	CComPtr<IAudioCaptureClient> m_captureClient;
	WAVEFORMATEX* m_wfx;

	mutable CMyCriticalSection m_sync_object;
	enum RecorderState { eStopped, ePaused, eStarted } m_audioState;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_AUDIO_CLIENT_H

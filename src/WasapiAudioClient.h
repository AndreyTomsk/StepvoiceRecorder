#ifndef WASAPI_AUDIO_CLIENT_H
#define WASAPI_AUDIO_CLIENT_H
#pragma once

#include <Audioclient.h> //for IAudioClient, IAudioCaptureClient
#include <memory>        //for auto_ptr
#include "WasapiCaptureBuffer.h"
#include "SyncObjects.h"

/////////////////////////////////////////////////////////////////////////////

class CWasapiAudioClient
{
public:
	CWasapiAudioClient(int device);
	~CWasapiAudioClient();

	DWORD GetActualFrequency() const;
	DWORD GetActualChannelCount() const;

	BOOL Start();
	BOOL Pause();
	BOOL Stop();

	float GetVolume() const;
	BOOL  SetVolume(float volume); //0..1

	std::auto_ptr<CWasapiCaptureBuffer> GetCaptureBuffer() const;

private:
	CComPtr<IAudioClient> m_audioClient;
	CComPtr<IAudioCaptureClient> m_captureClient;
	WAVEFORMATEX* m_wfx;

	mutable CMyCriticalSection m_sync_object;
	enum RecorderState { eStopped, ePaused, eStarted } m_audioState;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_AUDIO_CLIENT_H

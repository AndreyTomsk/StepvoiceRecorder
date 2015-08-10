#ifndef WASAPI_AUDIO_CLIENT_H
#define WASAPI_AUDIO_CLIENT_H
#pragma once

#include <Audioclient.h>    //for IAudioClient, IAudioCaptureClient
#include "WasapiCaptureBuffer.h"
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

	float GetPeakLevel(int channel) const; //0 = first channel, -1 = all channels

	//Destination freq MUST be <= m_wfx freq (downsampling).
	void SetResampleParams(unsigned int destFreq, unsigned int destChannels);
	bool GetData(BYTE* destBuffer, const UINT32& bufferSize, bool& streamError) const;

private:
	int m_deviceID;
	CComPtr<IAudioClient> m_audioClient;
	CComPtr<IAudioCaptureClient> m_captureClient;
	WAVEFORMATEX* m_wfx;

	mutable CMyCriticalSection m_sync_object;
	enum RecorderState { eStopped, ePaused, eStarted } m_audioState;

	CWasapiCaptureBuffer* m_captureBuffer;
	unsigned int m_resampleFreq;
	unsigned int m_resampleChans;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_AUDIO_CLIENT_H

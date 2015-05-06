#ifndef WASAPI_RECORDER_STREAM_H
#define WASAPI_RECORDER_STREAM_H
#pragma once

#include <Audioclient.h> //for IAudioClient, IAudioCaptureClient

/////////////////////////////////////////////////////////////////////////////
//Ok, idea is currently to provide a HSTREAM handle for multi-recorder.
//It is a decoding stream (requirement from BASSMix), so no playback/pause
//control needed. Mixer just reads data from stream.

//Action sequence:
//  1. Create RecorderStream object;
//  2. Retrieve actual frequency and channels;
//  3. Get stream handle;

class CWasapiRecorderStream
{
public:
	typedef DWORD HSTREAM; //Sample stream handle (from bass.h).

	CWasapiRecorderStream(int device);
	~CWasapiRecorderStream();

	DWORD GetActualFrequency() const;
	DWORD GetActualChannelCount() const;
	HSTREAM GetStreamHandle() const;

	float GetVolume() const;
	BOOL  SetVolume(float volume); //0..1

	float GetPeakLevel(int channel) const; //0 = first channel, -1 = all channels

private:
	static DWORD CALLBACK StreamProc(HSTREAM handle,
		void* dstBuffer, DWORD dstLengthBytes, void* user);

private:
	CComPtr<IAudioClient> m_audio_client;
	CComPtr<IAudioCaptureClient> m_capture_client;

	int m_deviceID;
	mutable HSTREAM m_hStream;
	WAVEFORMATEX* m_wfx;
	
	UINT32 m_src_offset;
	bool m_buffer_delay;
	UINT32 m_buffer_size;
	UINT32 m_packetOffsetBytes;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_RECORDER_STREAM_H

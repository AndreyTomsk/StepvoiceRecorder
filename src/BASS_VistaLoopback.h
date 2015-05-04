// BASS stream for loopback recording in Windows Vista.
// Copyright (c) 2008 Andrew Firsov.

#ifndef _BASS_VISTA_LOOPBACK_H
#define _BASS_VISTA_LOOPBACK_H

#include <MMDeviceApi.h>
#include <Audioclient.h> //for IAudioClient, IAudioCaptureClient

class BassVistaLoopback
{
	typedef DWORD HSTREAM; //Sample stream handle (from bass.h).

public:
	BassVistaLoopback(int a_device = 0);
	virtual ~BassVistaLoopback();

	DWORD GetActualFrequency() const;
	DWORD GetActualChannelCount() const;
	
	static HRESULT GetPlaybackDevicesNames(CStringArray& arr);
	HSTREAM GetLoopbackStream() const;

private:
	HRESULT GetDefaultDevice(EDataFlow a_flow, IAudioClient **a_client);
	HRESULT GetDevice(int a_device_id, EDataFlow a_flow, IAudioClient **a_client);
	HRESULT GetDevice(LPCWSTR a_device_driver, EDataFlow a_flow, IAudioClient **a_client);

	static DWORD CALLBACK LoopbackStreamProc(HSTREAM a_handle,
		void* a_buffer, DWORD a_length, void* a_user);

private:
	CComPtr<IAudioClient> m_audio_client;
	CComPtr<IAudioCaptureClient> m_capture_client;
	WAVEFORMATEX* m_wfx;
	HSTREAM m_loopback_stream;
	UINT32 m_src_offset;
	bool m_buffer_delay;
	UINT32 m_buffer_size;
};

#endif //_BASS_VISTA_LOOPBACK_H

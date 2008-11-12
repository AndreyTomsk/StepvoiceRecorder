// BASS stream for loopback recording in Windows Vista.
// Copyright (c) 2008 Andrew Firsov.

#ifndef _BASS_VISTA_LOOPBACK_H
#define _BASS_VISTA_LOOPBACK_H

#include <bass.h>
#include <MMDeviceApi.h>
#include <Audioclient.h>
#include <stdio.h>

class BassVistaLoopback
{
public:
	BassVistaLoopback();
	virtual ~BassVistaLoopback();

	HSTREAM GetLoopbackStream() const;

private:
	HRESULT GetDefaultDevice(EDataFlow a_flow, IAudioClient **a_client);

	static DWORD CALLBACK LoopbackStreamProc(HSTREAM a_handle,
		void* a_buffer, DWORD a_length, void* a_user);

private:
	CComPtr<IAudioClient> m_audio_client;
	CComPtr<IAudioCaptureClient> m_capture_client;
	WAVEFORMATEX* m_wfx;
	HSTREAM m_loopback_stream;
	UINT32 m_src_offset;
};

#endif //_BASS_VISTA_LOOPBACK_H

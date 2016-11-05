/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#ifndef WASAPI_CAPTURE_BUFFER_SIMPLE_H
#define WASAPI_CAPTURE_BUFFER_SIMPLE_H
#pragma once

#include <Audioclient.h> //for IAudioCaptureClient

/////////////////////////////////////////////////////////////////////////////
//IAudioCaptureClient wrapper. The object locks a WASAPI stream during
//construction, you should destroy it after use as quickly as possible.

//Check IsError and IsSilent after creation to test everything ok. Set packet
//full read = false if same packet needed on next call (see IAudioCaptureClient docs).

class CWasapiCaptureBufferSimple
{
	NON_COPYABLE(CWasapiCaptureBufferSimple);
public:
	CWasapiCaptureBufferSimple(CComPtr<IAudioCaptureClient> cc);
	~CWasapiCaptureBufferSimple();

	bool IsError() const;
	bool IsSilent() const;
	UINT32 GetFramesAvailable() const;
	BYTE* GetBuffer() const;
	void SetPacketFullRead(bool fullRead);

private:
	CComPtr<IAudioCaptureClient> m_captureClient;
	UINT32 m_framesInNextPacket;
	BYTE* m_buffer;
	bool m_isError;
	bool m_isSilent;
	bool m_fullRead;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_CAPTURE_BUFFER_SIMPLE_H

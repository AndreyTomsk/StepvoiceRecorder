#ifndef WASAPI_CAPTURE_BUFFER_H
#define WASAPI_CAPTURE_BUFFER_H
#pragma once

#include <Audioclient.h> //for IAudioCaptureClient
#include "common.h"

/////////////////////////////////////////////////////////////////////////////
//Destroy this object as quickly as possible - it locks a WASAPI stream buffer.

class CWasapiCaptureBuffer
{
	NON_COPYABLE(CWasapiCaptureBuffer);

public:
	CWasapiCaptureBuffer(IAudioCaptureClient* cc, UINT32 frameSize);
	~CWasapiCaptureBuffer();

	UINT32 GetBytesAvailable() const;
	BYTE* GetBuffer() const;

private:
	IAudioCaptureClient* m_captureClient;
	UINT32 m_frameSize;
	UINT32 m_framesInNextPacket;
	BYTE* m_buffer;
};

//---------------------------------------------------------------------------

class CWasapiCaptureBufferSimple
{
	NON_COPYABLE(CWasapiCaptureBufferSimple);
public:
	CWasapiCaptureBufferSimple(IAudioCaptureClient* cc);
	~CWasapiCaptureBufferSimple();

	bool IsError() const;
	bool IsSilent() const;
	UINT32 GetFramesAvailable() const;
	BYTE* GetBuffer() const;
	void SetPacketFullRead(bool fullRead);

private:
	IAudioCaptureClient* m_captureClient;
	UINT32 m_framesInNextPacket;
	BYTE* m_buffer;
	bool m_isError;
	bool m_isSilent;
	bool m_fullRead;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_CAPTURE_BUFFER_H

#ifndef WASAPI_CAPTURE_BUFFER_H
#define WASAPI_CAPTURE_BUFFER_H
#pragma once

#include <Audioclient.h> //for IAudioCaptureClient

/////////////////////////////////////////////////////////////////////////////
//Destroy the objects as quickly as possible - it performs a capture buffer lock.

class CWasapiCaptureBuffer
{
	//Non copyable declarations.
	CWasapiCaptureBuffer(const CWasapiCaptureBuffer&);
	CWasapiCaptureBuffer& operator=(const CWasapiCaptureBuffer&);

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

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_CAPTURE_BUFFER_H

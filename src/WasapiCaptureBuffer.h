#ifndef WASAPI_CAPTURE_BUFFER2_H
#define WASAPI_CAPTURE_BUFFER2_H
#pragma once

#include <Audioclient.h> //for IAudioCaptureClient

/////////////////////////////////////////////////////////////////////////////
//This class is used for requesting a custom amount of frames from WASAPI
//stream. The problem is that a stream gives a fixed frames count (packet).
//So, if a requested frames count != packet frames count - we should keep a
//frame offset between sequential FillBuffer calls.

//Note: frames == samples.
//Note: using IAudioClient to determine all frames available.

class CWasapiCaptureBuffer
{
	NON_COPYABLE(CWasapiCaptureBuffer);

public:
	CWasapiCaptureBuffer(CComPtr<IAudioClient> ac);
	~CWasapiCaptureBuffer();

	bool FillBuffer(BYTE* destBuffer, const UINT32& bufferSize, bool& streamError) const;
	//bool FillBuffer2(BYTE* destBuffer, const UINT32& bufferSize, bool& streamError) const;

private:
	CComPtr<IAudioClient> m_audioClient;
	CComPtr<IAudioCaptureClient> m_captureClient;
	int m_frameSize;
	mutable int m_captureBufferOffset;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_CAPTURE_BUFFER2_H

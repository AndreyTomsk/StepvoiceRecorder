#ifndef WASAPI_CAPTURE_BUFFER2_H
#define WASAPI_CAPTURE_BUFFER2_H
#pragma once

#include <Audioclient.h> //for IAudioCaptureClient
#include "common.h"

/////////////////////////////////////////////////////////////////////////////
//This class is used for requesting a certain amount of frames from WASAPI
//stream. The problem is that a stream can give fixed frames count (packet).
//So, if a requested frames count != packet frames count - we should keep a
//frame offset between sequential FillBuffer calls.

//Note: frames == samples.
//Note: using IAudioClient to determine all frames available.

class CWasapiCaptureBuffer2
{
	NON_COPYABLE(CWasapiCaptureBuffer2);

public:
	CWasapiCaptureBuffer2(IAudioClient* ac);
	~CWasapiCaptureBuffer2();

	bool FillBuffer(BYTE* destBuffer, const UINT32& bufferSize, bool& streamError);

private:
	IAudioClient* m_audioClient;
	IAudioCaptureClient* m_captureClient;
	int m_frameSize;
	int m_captureBufferOffset;
};

/////////////////////////////////////////////////////////////////////////////
#endif // WASAPI_CAPTURE_BUFFER2_H

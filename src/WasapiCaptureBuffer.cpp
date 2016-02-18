#include "stdafx.h"
#include "WasapiCaptureBuffer.h"
#include "WasapiCaptureBufferSimple.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWasapiCaptureBuffer::CWasapiCaptureBuffer(CComPtr<IAudioClient> ac)
	:m_audioClient(ac)
	,m_captureClient(NULL)
	,m_frameSize(0)
	,m_captureBufferOffset(0)
{
	ASSERT(m_audioClient != NULL);
	m_audioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_captureClient);
	ASSERT(m_captureClient != NULL);

	WAVEFORMATEX* wfx = NULL;
	m_audioClient->GetMixFormat(&wfx);
	m_frameSize = wfx->nBlockAlign;
	CoTaskMemFree(wfx);
}
//---------------------------------------------------------------------------

CWasapiCaptureBuffer::~CWasapiCaptureBuffer()
{
}
//---------------------------------------------------------------------------

bool CWasapiCaptureBuffer::FillBuffer(BYTE* destBuffer,
	const UINT32& destBufferSize, bool& streamError) const
{
	//0. ensure we have enougth data to fill the buffer.
	//1. calculate offsets
	//2. read frames from packet
	//3. if need more, goto 1.
	//4. If packet not fully read, save its offset and re-read next time.

	UINT32 framesAvailable = 0;
	streamError = m_audioClient->GetCurrentPadding(&framesAvailable) != S_OK;
	if (streamError || (framesAvailable*m_frameSize-m_captureBufferOffset) < destBufferSize)
	{
		if (streamError)
			LOG_ERROR() << __FUNCTION__ << ", streamError.";
		else
			LOG_WARNING() << "FillBuffer"
						<< ", insufficient frames(1)=" << framesAvailable
						<< ", available bytes=" << framesAvailable*m_frameSize-m_captureBufferOffset
						<< ", destBufferSize=" << destBufferSize;
		return false;
	}

	UINT32 destBufferOffset = 0;
	while (destBufferOffset < destBufferSize)
	{
		CWasapiCaptureBufferSimple captureBuffer(m_captureClient);
		streamError = captureBuffer.IsError();
		if (streamError || !captureBuffer.GetFramesAvailable())
		{
			UINT32 realframesAvailable = 0;
			m_audioClient->GetCurrentPadding(&realframesAvailable);
			LOG_WARNING() << "FillBuffer"
						<< ", streamError = " << streamError
						<< ", insufficient frames(2)=" << framesAvailable
						<< ", realframesAvailable=" << realframesAvailable
						<< ", captureOffset=" << m_captureBufferOffset
						<< ", destBufferSize=" << destBufferSize;
			return false;
		}

		const size_t bytesAvailable = captureBuffer.GetFramesAvailable()*m_frameSize - m_captureBufferOffset;
		const size_t bytesNeeded = destBufferSize - destBufferOffset;
		const size_t bytesToFill = min(bytesAvailable, bytesNeeded);

		void* writePos = destBuffer + destBufferOffset;
		if (captureBuffer.IsSilent())
			memset(writePos, 0, bytesToFill);
		else
		{
			const void* readPos = captureBuffer.GetBuffer() + m_captureBufferOffset;
			memcpy(writePos, readPos, bytesToFill);
		}

		const bool fullPacketRead = (bytesAvailable == bytesToFill);
		if (fullPacketRead)
			m_captureBufferOffset = 0;
		else
			m_captureBufferOffset += bytesToFill;
		captureBuffer.SetPacketFullRead(fullPacketRead);
		destBufferOffset += bytesToFill;
	}
	return true;
}
//---------------------------------------------------------------------------

//bool CWasapiCaptureBuffer::FillBuffer2(BYTE* destBuffer,
//	const UINT32& destBufferSize, bool& streamError) const
//{
	//Version 2 of the function. Motivation: in v1 we need to specify a small
	//dest buffer so GetCurrentPadding can report that all current packets fit
	//into dest. This data hardcoding is not good. Besides, sometimes i see the
	//"streamError=0, or no frames available" error. Produces glitch.

	//0. Until dest buffer is filled:
	//1. Get next packet size from capture client.
	//2. If no error, but size is empty - sleep for some time. Goto 1.
	//3. Fill buffer from current packet data.
	//return false;
//}
//---------------------------------------------------------------------------

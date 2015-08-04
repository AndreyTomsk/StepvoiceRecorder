#include "stdafx.h"
#include "WasapiCaptureBuffer2.h"
#include "WasapiCaptureBuffer.h" //CWasapiCaptureBufferSimple
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWasapiCaptureBuffer2::CWasapiCaptureBuffer2(IAudioClient* ac)
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

CWasapiCaptureBuffer2::~CWasapiCaptureBuffer2()
{
}
//---------------------------------------------------------------------------

bool CWasapiCaptureBuffer2::FillBuffer(BYTE* destBuffer, const UINT32& destBufferSize, bool& streamError)
{
	//0. ensure we have enougth data to fill the buffer.
	//1. calculate offsets
	//2. read frames from packet
	//3. if need more, goto 1.
	//4. If packet not fully read, save its offset and re-read next time.

	UINT32 framesAvailable = 0;
	streamError = m_audioClient->GetCurrentPadding(&framesAvailable) != S_OK;
	if (streamError || framesAvailable < UINT32(destBufferSize/m_frameSize))
		return false;

	UINT32 destBufferOffset = 0;
	while (destBufferOffset < destBufferSize)
	{
		CWasapiCaptureBufferSimple captureBuffer(m_captureClient);
		streamError = captureBuffer.IsError();
		if (streamError || !captureBuffer.GetFramesAvailable())
			return false;

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
		m_captureBufferOffset = bytesAvailable - bytesToFill;
		captureBuffer.SetPacketFullRead(m_captureBufferOffset == 0);
		destBufferOffset += bytesToFill;
	}
	return true;
}
//---------------------------------------------------------------------------


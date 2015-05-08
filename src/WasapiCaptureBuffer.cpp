#include "stdafx.h"
#include "WasapiCaptureBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWasapiCaptureBuffer::CWasapiCaptureBuffer(IAudioCaptureClient* cc, UINT32 frameSize)
	:m_captureClient(cc)
	,m_frameSize(frameSize)
	,m_framesInNextPacket(0)
	,m_buffer(NULL)
{
	if (m_captureClient != NULL &&
		m_captureClient->GetNextPacketSize(&m_framesInNextPacket) == S_OK &&
		m_framesInNextPacket > 0)
	{
		DWORD bufferFlags;
		if (m_captureClient->GetBuffer(&m_buffer, &m_framesInNextPacket, &bufferFlags, NULL, NULL) == S_OK)
		{
			if (bufferFlags & AUDCLNT_BUFFERFLAGS_SILENT)
				m_captureClient->ReleaseBuffer(m_framesInNextPacket);
			else
				return;	//Everighing ok!
		}
		m_buffer = NULL;
		m_framesInNextPacket = 0;
	}
}
//---------------------------------------------------------------------------

CWasapiCaptureBuffer::~CWasapiCaptureBuffer()
{
 	if (m_framesInNextPacket > 0)
	{
		m_captureClient->ReleaseBuffer(m_framesInNextPacket);
	}
}
//---------------------------------------------------------------------------

UINT32 CWasapiCaptureBuffer::GetBytesAvailable() const
{
	return m_framesInNextPacket * m_frameSize;
}
//---------------------------------------------------------------------------

BYTE* CWasapiCaptureBuffer::GetBuffer() const
{
	return m_buffer;
}
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "WasapiCaptureBuffer.h"
#include "Debug.h"

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

/////////////////////////////////////////////////////////////////////////////

CWasapiCaptureBufferSimple::CWasapiCaptureBufferSimple(IAudioCaptureClient* cc)
	:m_captureClient(cc)
	,m_framesInNextPacket(0)
	,m_buffer(NULL)
	,m_isError(false)
	,m_isSilent(false)
	,m_fullRead(true)
{
	ASSERT(m_captureClient != NULL);
	if (m_captureClient->GetNextPacketSize(&m_framesInNextPacket) == S_OK)
	{
		if (m_framesInNextPacket == 0)
			return;

		DWORD bufferFlags;
		if (m_captureClient->GetBuffer(&m_buffer, &m_framesInNextPacket, &bufferFlags, NULL, NULL) == S_OK)
		{
			if (bufferFlags & AUDCLNT_BUFFERFLAGS_SILENT)
				m_isSilent = true;
			return;
		}
	}
	m_isError = true;
}
//---------------------------------------------------------------------------

CWasapiCaptureBufferSimple::~CWasapiCaptureBufferSimple()
{
 	if (!m_isError && m_framesInNextPacket > 0)
		m_captureClient->ReleaseBuffer(m_fullRead ? m_framesInNextPacket : 0);
}
//---------------------------------------------------------------------------

bool CWasapiCaptureBufferSimple::IsError() const
{
	return m_isError;
}
//---------------------------------------------------------------------------

bool CWasapiCaptureBufferSimple::IsSilent() const
{
	return m_isSilent;
}
//---------------------------------------------------------------------------

UINT32 CWasapiCaptureBufferSimple::GetFramesAvailable() const
{
	return m_framesInNextPacket;
}
//---------------------------------------------------------------------------

BYTE* CWasapiCaptureBufferSimple::GetBuffer() const
{
	return m_buffer;
}
//---------------------------------------------------------------------------

void CWasapiCaptureBufferSimple::SetPacketFullRead(bool fullRead)
{
	m_fullRead = fullRead;
}
//---------------------------------------------------------------------------

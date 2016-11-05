/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include "WasapiCaptureBufferSimple.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWasapiCaptureBufferSimple::CWasapiCaptureBufferSimple(CComPtr<IAudioCaptureClient> cc)
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

/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CFile CLog::m_logFile;

/*static*/ void CLog::Open(CString logFilePath)
{
	if (m_logFile != CFile::hFileNull)
		m_logFile.Close();

	CFileException ex;
	UINT flags = CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite;
	if (!m_logFile.Open(logFilePath, flags, &ex))
	{
		TCHAR szCause[255];
		ex.GetErrorMessage(szCause, sizeof(szCause)/sizeof(TCHAR));

		CString strFormatted;
		strFormatted = _T("The log file could not be opened because of this error: ");
		strFormatted += szCause;

		AfxMessageBox(strFormatted);
	}
}
//---------------------------------------------------------------------------

CLog::CLog()
{
	SYSTEMTIME t;
	::GetLocalTime(&t);

	CString curTime;
	curTime.Format(_T("%02d:%02d:%02d.%03d"), t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	//Using 'this' just to write CString correctly. See operator overload in header.
	*this << curTime << ' ';
}
//---------------------------------------------------------------------------

CLog::~CLog()
{
	m_stream << L"\r\n";
	if (m_logFile != CFile::hFileNull)
	{
		//CW2A ansiText(m_stream.str().c_str());
		//m_logFile.Write(ansiText, strlen(ansiText));

		std::wstring msg = m_stream.str();
		m_logFile.Write(msg.c_str(), msg.length() * sizeof(wchar_t));
	}
}

/////////////////////////////////////////////////////////////////////////////

CLog& operator <<(CLog& logger, CString msg)
{
	logger << msg.GetBuffer(msg.GetLength());
	return logger;
}
//---------------------------------------------------------------------------

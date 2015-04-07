#include "stdafx.h"
#include "FilterFileWriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

FileWriter::FileWriter(const CString& fileName)
	:m_outputFile(fileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite)
{
}
//---------------------------------------------------------------------------

FileWriter::~FileWriter()
{
	ForceClose();
}
//---------------------------------------------------------------------------

void FileWriter::ForceClose()
{
	if (m_outputFile.m_hFile != CFile::hFileNull)
	{
		m_outputFile.Flush();
		m_outputFile.Close();
	}
}
//---------------------------------------------------------------------------

ULONGLONG FileWriter::GetFileLength() const
{
	return m_outputFile.GetLength();
}
//---------------------------------------------------------------------------

bool FileWriter::ProcessData(void* buffer, DWORD lengthBytes)
{
	//static const DWORD msecInitial = ::GetTickCount();
	//static int counter = 0;
	try
	{
		m_outputFile.Write(buffer, lengthBytes);
		//if (++counter%10 == 0)
		//{
		//	const int fileLength = (int)m_outputFile.GetLength();
		//	const int secondsFromStart = (::GetTickCount()-msecInitial)/1000;
		//	SendNotification(Parameter(_T("FileWriter.BytesWritten"), fileLength));
		//	SendNotification(Parameter(_T("FileWriter.SecondsWritten"), secondsFromStart));
		//}
	}
	catch (CException *e)
	{
		// Possible, the disk is full, etc.
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

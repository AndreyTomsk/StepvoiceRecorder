#include "stdafx.h"
#include "FilterFileWriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

FileWriter::FileWriter(const CString& fileName)
	:m_outputFile(fileName, CFile::modeCreate | CFile::modeWrite)
{
}
//---------------------------------------------------------------------------

FileWriter::~FileWriter()
{
	m_outputFile.Flush();
	m_outputFile.Close();
}
//---------------------------------------------------------------------------

bool FileWriter::ProcessData(void* buffer, DWORD lengthBytes)
{
	try
	{
		m_outputFile.Write(buffer, lengthBytes);
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

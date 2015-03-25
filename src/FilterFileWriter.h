#ifndef FILTER_FILE_WRITER_H
#define FILTER_FILE_WRITER_H
#pragma once

#include "Filter.h"

/////////////////////////////////////////////////////////////////////////////
// Opens file for writing, clears it if not empty.
// Throws? exception from constructor if file cannot be opened for writing.

class FileWriter : public Filter
{
public:
	FileWriter(const CString& fileName);
	virtual ~FileWriter();

private:
	virtual bool ProcessData(void* buffer, DWORD lengthBytes);
	CFile m_outputFile;
};

/////////////////////////////////////////////////////////////////////////////
#endif

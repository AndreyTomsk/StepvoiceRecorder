/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once
#include <sstream>

/////////////////////////////////////////////////////////////////////////////

class CLog
{
public:
	static void Open(CString logFilePath);

	CLog();
	~CLog();

	template<typename T>
	CLog& operator << (const T& t)
	{
		m_stream << t;
		return *this;
	}

private:
	static CFile m_logFile;
	std::wstringstream m_stream;
};

//Support of writing CString object.
CLog& operator <<(CLog& writer, CString msg);

/////////////////////////////////////////////////////////////////////////////

#define LOG_DEBUG()   CLog()<<"DEBUG   "
#define LOG_INFO()    CLog()<<"INFO    "
#define LOG_WARNING() CLog()<<"WARNING "
#define LOG_ERROR()   CLog()<<"ERROR   "

#ifndef MY_DEBUG_H
#define MY_DEBUG_H
#pragma once

#include <sstream>

/////////////////////////////////////////////////////////////////////////////
//Helper class to simplify debugging - variable-to-string conversion, unicode
//or not strings mixing is done automatically by wstringstream.
//Example:  WriteDbg() << L"pi=" << 3.14 << ", result=" << true;

class WriteDbg
{
public:
	WriteDbg();
	~WriteDbg();

	template<typename T>
	WriteDbg& operator << (const T& t)
	{
		m_stream << t;
		return *this;
	}

private:
	std::wstringstream m_stream;
};

/////////////////////////////////////////////////////////////////////////////
#endif

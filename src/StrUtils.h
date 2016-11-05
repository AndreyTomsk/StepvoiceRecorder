/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once
#include <vector>

namespace StrUtils
{
	std::vector<CString> Split(const CString& source, TCHAR delimeter);
	CString Join(const std::vector<CString>& source, TCHAR delimeter);

	CString ToString(int value);
	int FromString(CString str);
}

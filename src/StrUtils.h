#pragma once
#include <vector>

namespace StrUtils
{
	std::vector<CString> Split(const CString& source, TCHAR delimeter);
	CString Join(const std::vector<CString>& source, TCHAR delimeter);
}

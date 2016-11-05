/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include "StrUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace StrUtils
{
//---------------------------------------------------------------------------

std::vector<CString> Split(const CString& source, TCHAR delimeter)
{
	const CString delimSymbols(delimeter);
	std::vector<CString> result;
	CString resToken;
	int curPos = 0;

	//NOTE: Tokenize treats string as array of delim symbols.
	resToken = source.Tokenize(delimSymbols, curPos);
	while (!resToken.IsEmpty())
	{
		result.push_back(resToken);
		resToken = source.Tokenize(delimSymbols, curPos);
	}
	return result;
}
//---------------------------------------------------------------------------

CString Join(const std::vector<CString>& tokens, TCHAR delimeter)
{
	CString result;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (i > 0) result += delimeter;
		result += tokens[i];
	}
	return result;
}
//---------------------------------------------------------------------------

CString ToString(int value)
{
	CString result;
	result.Format(_T("%d"), value);
	return result;
}
//---------------------------------------------------------------------------

int FromString(CString str)
{
	return _ttoi(str);
}
//---------------------------------------------------------------------------

} //namespace StrUtils

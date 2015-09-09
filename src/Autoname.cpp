#include "stdafx.h"
#include "Autoname.h"
//#include <atlrx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

CString FilterTemplate(CString a_template)
{
	const CString ALLOWED_CHARS(_T("BbdHjMmSYy%"));
	CString l_template = a_template;
	CString l_char_str;

	l_template.TrimRight('%'); // important! the algorithm below using this

	int l_pos = 0;
	while (l_pos < l_template.GetLength())
	{
		if (l_template.Mid(l_pos, 1) != _T("%"))
		{
			l_pos++;	// ordinary char, jump to next symbol
			continue;
		}
		
		l_char_str = l_template.Mid(l_pos + 1, 1);
		if (ALLOWED_CHARS.Find(l_char_str) == -1)
		{
			l_template.Delete(l_pos, 2);
			continue;	// checking the updated string from same position
		}

		l_pos += 2;		// char is correct, jump over 2 symbols
	}

	return l_template;
}
//---------------------------------------------------------------------------

CString GetAutoName(CString strTemplate, CString fileExtension)
{
	strTemplate = FilterTemplate(strTemplate);

	// Removing all '%' symbols from the end of string
	if (strTemplate.Right(1) == "%")
		strTemplate.TrimRight('%');

	CTime t = CTime::GetCurrentTime();	
	CString resultName = t.Format(strTemplate);
	if (resultName.IsEmpty())
		resultName = _T("Empty");

	return resultName + _T('.') + fileExtension;
}
//---------------------------------------------------------------------------

CString GetAutonameRegEx(CString strTemplate, CString fileExtension)
{
	/*
	CAtlRegExp<> reUrl;
	REParseError status = reUrl.Parse("^%b%d_%H%M_{(%N)+}.mp3$");
    if (REPARSE_ERROR_OK != status)
    {
        // Unexpected error.
        return CString();
    }

    CAtlREMatchContext<> mc;
    if (!reUrl.Match("%b%d_%H%M_%N%N%N.mp3", &mc))
    {
        // Unexpected error.
        return CString();
    }

    for (UINT groupIndex = 0; groupIndex < mc.m_uNumGroups; ++groupIndex)
    {
        const CAtlREMatchContext<>::RECHAR* szStart = 0;
        const CAtlREMatchContext<>::RECHAR* szEnd = 0;
        mc.GetMatch(groupIndex, &szStart, &szEnd);

        ptrdiff_t nLength = szEnd - szStart;
		CString strGroup(szStart, nLength);
		AfxMessageBox(strGroup);
    }
	*/
    return CString();
}
//---------------------------------------------------------------------------

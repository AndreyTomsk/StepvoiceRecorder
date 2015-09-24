#include "stdafx.h"
#include "MainFrm_Helpers.h"
#include "FileUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Helpers
{
//------------------------------------------------------------------------------

CString ToString_HMMSS(unsigned int seconds)
{
	const unsigned int iHour= seconds/3600;
	const unsigned int iMin =(seconds - iHour*3600)/60;
	const unsigned int iSec = seconds - iHour*3600 - iMin*60;

	CString str;
	str.Format(_T("%d:%02d:%02d"), iHour, iMin, iSec);
	return str;
}
//------------------------------------------------------------------------------

CString ToString_HMMSS(double seconds)
{
	return ToString_HMMSS((unsigned int)seconds);
}
//------------------------------------------------------------------------------

CString GetNewRecordingFilePath()
{
	using namespace FileUtils;
	const CString autonameNoExt = GetFileNameNoExt(GetMp3AutonameFromConfig());
	const CString outputFolder = GetOutputFolder();

	CString outputFile = CombinePath(outputFolder, autonameNoExt + _T(".mp3"));
	int index = 2;

	while (FileExists(outputFile))
	{
		CString newAutoname;
		newAutoname.Format(_T("%s_%02d.mp3"), autonameNoExt, index++);
		outputFile = CombinePath(outputFolder, newAutoname);
	}
	return outputFile;
}
//------------------------------------------------------------------------------

bool IsSuitableForRecording(const CString& filePath, DWORD* outErrorCode)
{
	HANDLE fileHandle = CreateFile(filePath, GENERIC_READ, FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	const DWORD errorCode = ::GetLastError();
	if (outErrorCode != NULL)
		*outErrorCode = errorCode;

	if (fileHandle == INVALID_HANDLE_VALUE)
		return (errorCode == ERROR_FILE_NOT_FOUND) ? true : false;

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(fileHandle, &fileSize))
		fileSize.QuadPart = -1; //for not suitable check later

	CloseHandle(fileHandle);
	return (fileSize.QuadPart == 0);
}
//------------------------------------------------------------------------------

CString GetOutputFolder()
{
	const CString lastFileName = RegistryConfig::GetOption(_T("General\\LastFile"), CString());
	const CString lastFileFolder = FileUtils::GetFolderOnly(lastFileName);

	const CString optionFolder = RegistryConfig::GetOption(_T("General\\OutputFolder"), CString());
	const bool storeInOptionFolder = RegistryConfig::GetOption(_T("General\\StoreInOutputFolder"), 0) ? true : false;

	if (optionFolder.IsEmpty() || lastFileFolder.IsEmpty())
		return FileUtils::GetSpecialFolder(CSIDL_DESKTOP);
	else
		return storeInOptionFolder ? optionFolder : lastFileFolder;
}
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

CString GetAutoname(CString strTemplate, CString fileExtension)
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

CString GetMp3AutonameFromConfig()
{
	const CString defaultTemplate = _T("%b%d_%H%M");
	const CString fileTemplate = RegistryConfig::GetOption(_T("Autoname\\Current template"), defaultTemplate);
	return GetAutoname(fileTemplate, _T("mp3"));
}
//---------------------------------------------------------------------------

/*
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
	return CString();
}
//---------------------------------------------------------------------------
*/
////////////////////////////////////////////////////////////////////////////////

} // namespace Helpers

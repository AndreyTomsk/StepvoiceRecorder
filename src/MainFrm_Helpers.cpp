////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "MainFrm_Helpers.h"

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

////////////////////////////////////////////////////////////////////////////////

} // namespace Helpers

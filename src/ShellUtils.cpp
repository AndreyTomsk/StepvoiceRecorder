#include "stdafx.h"
#include "ShellUtils.h"
#include "common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ShellUtils
{
//---------------------------------------------------------------------------

CString GetSpecialFolder(int csidl)
{
	TCHAR szBuffer[MAX_PATH*2] = {0};
	SHGetSpecialFolderPath(NULL, szBuffer, csidl, FALSE);
	return CString(szBuffer);
}
//---------------------------------------------------------------------------

CString GetProgramFolder()
{
	TCHAR exeName[MAX_PATH*2] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), exeName, MAX_PATH*2);

	CString programPath(exeName);
	return programPath.Left(programPath.ReverseFind(_T('\\')));
}
//---------------------------------------------------------------------------

bool DialogPickupFolder(CString& inOutFolderPath)
{
	HRESULT hr = S_OK;
	CComPtr<IFileOpenDialog> pFileOpen;
	CComPtr<IShellItem> pItem;
	PWSTR pszFilePath = NULL;

	CComPtr<IShellItem> siFolder;
	SHCreateItemFromParsingName(CStringW(inOutFolderPath), NULL, IID_IShellItem, (void**)&siFolder);

	EIF(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
		IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen)));
	EIF(pFileOpen->SetFolder(siFolder));
	EIF(pFileOpen->SetOptions(FOS_PICKFOLDERS));
	EIF(pFileOpen->Show(NULL));
	EIF(pFileOpen->GetResult(&pItem));
	EIF(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath));

	inOutFolderPath = pszFilePath;
	CoTaskMemFree(pszFilePath);

Exit:
	return hr == S_OK;
}
//---------------------------------------------------------------------------

bool FolderExists(const CString& path)
{
  const DWORD dwAttrib = GetFileAttributes(path);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

/////////////////////////////////////////////////////////////////////////////

} //namespace ShellUtils

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

CString GetSpecialFolderPath(int csidl)
{
	TCHAR szBuffer[MAX_PATH*2] = {0};
	SHGetSpecialFolderPath(NULL, szBuffer, csidl, FALSE);
	return CString(szBuffer);
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

/////////////////////////////////////////////////////////////////////////////

} //namespace ShellUtils

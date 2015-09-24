#include "stdafx.h"
#include "FileUtils.h"
#include "common.h"
#include <Shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace FileUtils
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
//---------------------------------------------------------------------------

bool FileExists(const CString& path)
{
	return ::PathFileExists(path) ? true : false;
}
//---------------------------------------------------------------------------

CString GetFileName(LPCTSTR path)
{
//	Strip off the path and return just the filename part
	CString temp = (LPCTSTR) path; // Force CString to make a copy
	::PathStripPath(temp.GetBuffer(0));
	temp.ReleaseBuffer(-1);
	return temp;
}
//---------------------------------------------------------------------------

CString GetFileNameNoExt(LPCTSTR path)
{
	CString csFile = GetFileName(path);
	if(!GetFileExt(csFile).IsEmpty()) { // Is there an extension
		::PathRemoveExtension(csFile.GetBuffer(_MAX_PATH));
		csFile.ReleaseBuffer();
	}
	return csFile;
}
//---------------------------------------------------------------------------

CString GetFolderOnly(LPCTSTR path)
{
//	Strip off the file name so we can direct the file scanning dialog to go
//	back to the same directory as before.
	CString temp = (LPCTSTR) path; // Force CString to make a copy
	::PathRemoveFileSpec(temp.GetBuffer(0));
	temp.ReleaseBuffer(-1);
	return temp;
}
//---------------------------------------------------------------------------

CString AddBackslash(LPCTSTR path)
{
	CString cs = path;
	::PathAddBackslash(cs.GetBuffer(_MAX_PATH));
	cs.ReleaseBuffer(-1);
	//if(cs.IsEmpty())
	//	cs = BACK_SLASH;
	return cs;
}
//---------------------------------------------------------------------------

CString RemoveBackslash(LPCTSTR path)
{
	CString cs = path;
	::PathRemoveBackslash(cs.GetBuffer(_MAX_PATH));
	cs.ReleaseBuffer(-1);
	return cs;
}
//---------------------------------------------------------------------------

CString CombinePath(LPCTSTR Folder, LPCTSTR File)
{
	CString cs = Folder;
	::PathAddBackslash(cs.GetBuffer(_MAX_PATH));
	::PathAppend(cs.GetBuffer(_MAX_PATH),File);
	cs.ReleaseBuffer(-1);
	return cs;
}
//---------------------------------------------------------------------------

CString AddFileExt(LPCTSTR path, LPCTSTR ext)
{
	CString cs = path;
	::PathAddExtension(cs.GetBuffer(_MAX_PATH),ext);
	cs.ReleaseBuffer(-1);
	return cs;
}
//---------------------------------------------------------------------------

CString GetFileExt(LPCTSTR path)
{
	CString cs;
	cs = ::PathFindExtension(path);
	return cs;
}
//---------------------------------------------------------------------------

CString RenameFileExt(LPCTSTR path, LPCTSTR newExt)
{
	CString cs = path;
	::PathRenameExtension(cs.GetBuffer(_MAX_PATH), newExt);
	return cs;
}
//---------------------------------------------------------------------------

} //namespace FileUtils

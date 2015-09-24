#pragma once

namespace FileUtils
{
	//Returns a path to known windows folders. Check:
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb762494%28v=vs.85%29.aspx
	//for a list of known folder ids (CSIDL_MYDOCUMENTS, etc.).
	CString GetSpecialFolder(int csidl);

	//Displays a "select folder" dialog.
	bool DialogPickupFolder(CString& folderPath); //[in/out]

	CString GetProgramFolder();

	bool FolderExists(const CString& path);
	bool FileExists(const CString& path);

	//code taken from:
	//http://www.camaswood.com/tech/some-handy-file-functions-for-use-with-mfc/
	CString GetFileName(const CString& path);
	CString GetFileNameNoExt(const CString& path);
	CString GetFolderOnly(const CString& path);
	CString AddBackslash(const CString& path);
	CString RemoveBackslash(const CString& path);
	CString CombinePath(const CString& folder, const CString& file);
	CString AddFileExt(const CString& path, const CString& ext); //fails if extension present, if ext empty ".exe" is used
	CString GetFileExt(const CString& path);
	CString ChangeFileExt(const CString& path, const CString& newExt);
}

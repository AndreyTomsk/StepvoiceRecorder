#pragma once

namespace FileUtils
{
	//Returns a path to known windows folders. Check:
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb762494%28v=vs.85%29.aspx
	//for a list of known folder ids (CSIDL_MYDOCUMENTS, etc.).
	CString GetSpecialFolder(int csidl);

	CString GetProgramFolder();
	CString AddProgramFolderAndFile(LPCTSTR fileName);

	//Displays a "select folder" dialog.
	bool DialogPickupFolder(CString& folderPath); //[in/out]

	bool FolderExists(const CString& path);
	bool FileExists(const CString& path);

	//code taken from:
	//http://www.camaswood.com/tech/some-handy-file-functions-for-use-with-mfc/
	CString GetFileName(LPCTSTR path);
	CString GetFileNameNoExt(LPCTSTR path);
	CString GetFolderOnly(LPCTSTR path);
	CString AddBackslash(LPCTSTR path);
	CString RemoveBackslash(LPCTSTR path);
	CString AddFolderAndFile(LPCTSTR folder, LPCTSTR file);
	CString AddExtension(LPCTSTR path, LPCTSTR ext);
	CString GetFileExtension(LPCTSTR path);
	CString ChangeFileExtension(LPCTSTR path, LPCTSTR newExt);
}

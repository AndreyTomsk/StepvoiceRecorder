#ifndef _SHELL_UTILS_H
#define _SHELL_UTILS_H
#pragma once

namespace ShellUtils
{
	//Returns a path to known windows folders. Check:
	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb762494%28v=vs.85%29.aspx
	//for a list of known folder ids (CSIDL_MYDOCUMENTS, etc.).
	CString GetSpecialFolderPath(int csidl);

	//Displays a "select folder" dialog.
	bool DialogPickupFolder(CString& folderPath); //[in/out]
}

#endif // _SHELL_UTILS_H

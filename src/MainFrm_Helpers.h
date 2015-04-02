#ifndef _MAINFRM_HELPERS_H
#define _MAINFRM_HELPERS_H
#pragma once


////////////////////////////////////////////////////////////////////////////////

namespace Helpers
{
	CString ToString_HMMSS(unsigned int seconds);
	CString ToString_HMMSS(double seconds);

	//Check if a file is suitable for recording (not exist or length = 0).
	bool IsSuitableForRecording(const CString& filePath, DWORD* outErrorCode = NULL);
};

////////////////////////////////////////////////////////////////////////////////

#endif // _MAINFRM_HELPERS_H

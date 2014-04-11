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

/*
	const char* szPattern[] = {"%d", "0%d"};
	char szMin[3] = "", szSec[3] = "";

	int iHour= nCurSec/3600;
	int iMin =(nCurSec - iHour*3600)/60;
	int iSec = nCurSec - iHour*3600 - iMin*60;

	sprintf_s(szMin, sizeof(szMin), szPattern[iMin<10], iMin);
	sprintf_s(szSec, sizeof(szSec), szPattern[iSec<10], iSec);
	sprintf_s(pszTime, nStrSize, "%d:%s:%s", iHour, szMin, szSec);
*/
}

////////////////////////////////////////////////////////////////////////////////

} // namespace Helpers

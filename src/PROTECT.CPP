#include "stdafx.h"
#include "PROTECT.H"

static int g_DaysTotal = -1;
static int g_DaysLeft = -1;
static LPCSTR g_pszRegUser = NULL;

//Called by ASProtect
/////////////////////////////////////////////////////////////////////////////

void __declspec(dllexport) __stdcall GetRegistrationInformation (char* pszRegUserName)
{
	g_pszRegUser = pszRegUserName;
}
//---------------------------------------------------------------------------

void __declspec(dllexport) __stdcall GetTrialDays (int Total, int Left)
{
	g_DaysTotal = Total;
	g_DaysLeft = Left;
}

/////////////////////////////////////////////////////////////////////////////

CString fsProtect_GetUserName ()
{
	CA2T str(g_pszRegUser);
	return str;
}
//---------------------------------------------------------------------------

int fsProtect_GetDaysLeft() // число оставшихся дней
{
	return g_DaysLeft;
}
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// Protect.cpp - program protection
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <string.h>

/////////////////////////////////////////////////////////////////////////////
namespace protect
{
	// _xxSomeXX -- "in case"
	int _iSome3 = -1;
	int _iSome2 = 0;
	int _DaysTotal = -1;
	int _DaysLeft = -1;
	LPCSTR _pszSome1 = NULL;
	LPCSTR _pszRegUser = NULL;
	LPCSTR _pszSome2 = NULL;
	LPCSTR _pszHID = NULL;
	LPCSTR _pszSome3 = NULL;
	typedef void (*fntVoidProcedure) ();
	fntVoidProcedure _pfnASProtectRunApplication = NULL;
	int _iSome1 = -1;
};

using namespace protect;

/////////////////////////////////////////////////////////////////////////////
void __declspec(dllexport) __stdcall GetRegistrationInformation (char* pszRegUserName)
{
	_pszRegUser = pszRegUserName;
}

/////////////////////////////////////////////////////////////////////////////
void __declspec(dllexport) __stdcall GetTrialDays (int Total, int Left)
{
	_DaysTotal = Total;
	_DaysLeft = Left;
}

/////////////////////////////////////////////////////////////////////////////
void __declspec(dllexport) __stdcall GetHardwareID (char* HardwareID)
{
    _pszHID = HardwareID;
}

/////////////////////////////////////////////////////////////////////////////
void __declspec(dllexport) ExecuteApplication ()
//------------------------------------------------
//  This procedure will be called by ASProtect if:
//  1. Trial doesn't expire
//  2. Application was registered
//------------------------------------------------
{
	_pfnASProtectRunApplication = ExecuteApplication;
}

/////////////////////////////////////////////////////////////////////////////
void __declspec(dllexport) ExecuteTrial()
//---------------------------------------------------------------------
//  This procedure will be called by ASProtect if:
//  1. Evaluation period of application has expired
//---------------------------------------------------------------------
{
	_pfnASProtectRunApplication = ExecuteTrial;
}

/////////////////////////////////////////////////////////////////////////////
LPCSTR fsProtect_GetUserName ()
{
	return _pszRegUser;
}

/////////////////////////////////////////////////////////////////////////////
BOOL fsProtect_IsRegistered ()
{
	try {
		return strlen (_pszRegUser) > 0;
	}
	catch (...)
	{
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
bool fsProtect_IsSingleUserLicense()
{
	const CString userName(_pszRegUser);
	const char* singleSubstrs[] = {"single", "one", "personal", NULL};
	
	int i = 0;
	while (singleSubstrs[i] != NULL)
	{
		if (userName.Find(singleSubstrs[i]) != -1)
			return true;
		i++;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
int fsProtect_GetDaysTotal ()
{
	return _DaysTotal;
}

/////////////////////////////////////////////////////////////////////////////
int fsProtect_GetDaysLeft () // число оставшихся дней
{
	return _DaysLeft;
}

/////////////////////////////////////////////////////////////////////////////
int fsProtect_GetDaysPassed ()
{
	return _DaysTotal - _DaysLeft;
}

/////////////////////////////////////////////////////////////////////////////
LPCSTR fsProtect_GetHardwareID ()
{
	return _pszHID;
}

/////////////////////////////////////////////////////////////////////////////

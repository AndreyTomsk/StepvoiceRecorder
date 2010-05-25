/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#ifndef __PROTECT_H_
#define __PROTECT_H_

//#include "system.h"

/////////////////////////////////////////////////////////////////////////////
extern LPCSTR fsProtect_GetUserName ();
extern BOOL fsProtect_IsRegistered ();
extern int fsProtect_GetDaysTotal ();
extern int fsProtect_GetDaysLeft ();
extern int fsProtect_GetDaysPassed ();
extern LPCSTR fsProtect_GetHardwareID ();
extern bool fsProtect_IsSingleUserLicense();

/////////////////////////////////////////////////////////////////////////////
inline void fsProtect_FillCrackedStr1 (LPSTR pszMsg)
{
	int a = 1;
	a--;
	pszMsg [0] = a + 'c';
	pszMsg [1] = a + 'r';
	pszMsg [2] = a + 'a';
	pszMsg [3] = a + 'c';
	pszMsg [4] = a + 'k';
	pszMsg [5] = a + 'e';
	pszMsg [6] = a + 'd';
	pszMsg [7] = a + '!';
	pszMsg [8] = a + 0;
}

/////////////////////////////////////////////////////////////////////////////
inline void fsProtect_CrackedMsg ()
{
	char szMsg[100];
	fsProtect_FillCrackedStr1 (szMsg);
	AfxMessageBox(szMsg);
	//fsOpenUrlInBrowser(szMsg);
}

/////////////////////////////////////////////////////////////////////////////
inline void __stdcall TestUSER32orKERNEL32API(void* Addr)
{
	__asm 
	{
		mov  eax,  Addr
		push dword ptr [eax]
		pop  dword ptr [eax]	
    }    
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL fsProtect_CheckASP1()
{
#ifdef _DEBUG
	return TRUE;
#endif

	__try
	{
        TestUSER32orKERNEL32API (CreateThread);
        return TRUE;

    }
	__except(EXCEPTION_EXECUTE_HANDLER) 
	{
		return FALSE;
    }
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL fsProtect_CheckASP2()
{
#ifdef _DEBUG
	return TRUE;
#endif

	return fsProtect_GetDaysTotal () != -1 && fsProtect_GetDaysLeft () != -1;
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL fsProtect_CheckASP3()
{
#ifdef _DEBUG
	return TRUE;
#endif

	return fsProtect_GetUserName () != NULL && fsProtect_GetHardwareID () != NULL;
}

/////////////////////////////////////////////////////////////////////////////
#endif
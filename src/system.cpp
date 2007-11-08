/////////////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "MP3_Recorder.h"

/////////////////////////////////////////////////////////////////////////////
//
#define KILLAPP_TERMINATE    -1
#define KILLAPP_WAIT          0
#define KILLAPP_CANCEL        1

typedef int (CALLBACK * PFNWAITCALLBACK)(LPARAM);

BOOL KillApplication(
    IN HWND hWnd,                       // ������������� ����
    IN PFNWAITCALLBACK pfnWaitCallback, // ���������������� �������
    IN LPARAM lParam                    // �������� ��� ���������������� �������
    );

static
BOOL CALLBACK KillAppEnumWindows(
    IN HWND hWnd,
    IN LPARAM lParam
    );

int CALLBACK WaitCallback(
    IN LPARAM lParam
    );

/////////////////////////////////////////////////////////////////////////////
//
typedef BOOL (CALLBACK * PFNENUMAPP)(
    IN HWND hWnd,             // ������������� �������� ���� ����������
    IN LPCTSTR pszName,	      // �������� ����������
    IN HICON hIcon,           // ������ ����������
    IN LPARAM lParam          // ���������������� ��������
    );

typedef struct _ENUMAPPDATA {
    LPARAM        lParam;
    PFNENUMAPP    pfnEnumApp;
} ENUMAPPDATA, * PENUMAPPDATA;

static
BOOL CALLBACK EnumWindowsCallback(
    IN HWND hWnd,
    IN LPARAM lParam
    );

BOOL EnumApplications(
    IN PFNENUMAPP pfnEnumApp,
    IN LPARAM lParam
    );

BOOL CALLBACK EnumKillAppCallback (
    IN HWND hWnd,    
    IN LPCTSTR pszName,
    IN HICON hIcon, 
    IN LPARAM lParam
    );

/////////////////////////////////////////////////////////////////////////////
// ���������� ����������
//
void ShutDownComputer()
{
	HANDLE hToken;   
	TOKEN_PRIVILEGES tkp, prevst;
	DWORD dwRL;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, "SeShutdownPrivilege", &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(prevst), &prevst, &dwRL);
	ExitWindowsEx(EWX_POWEROFF, 0);
}

/////////////////////////////////////////////////////////////////////////////
// ��������� ��� ���������
void CloseAllPrograms()
{
	// ��������� ��� ������� ��������� (����� �����)
	//EnumApplications(EnumKillAppCallback, NULL);

	// ��������� ���� ���������
	CWnd* pMainWnd = AfxGetApp()->GetMainWnd();
	pMainWnd->PostMessage(WM_COMMAND, MAKEWPARAM(IDM_TRAY_EXIT, 0), 0);
}

/////////////////////////////////////////////////////////////////////////////
// ������� ���������� ����������
/////////////////////////////////////////////////////////////////////////////
BOOL KillApplication(
    IN HWND hWnd,
    IN PFNWAITCALLBACK pfnWaitCallback,
    IN LPARAM lParam
    )
{
    _ASSERTE(pfnWaitCallback != NULL);

    if (!IsWindow(hWnd))
        return SetLastError(ERROR_INVALID_PARAMETER), FALSE;

    // ���������� ������������� �������� � ������, �������
    // ����������� ��������� ����
    DWORD dwProcessId;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);

	// ��������� ��������� ��������
	HANDLE hProcess = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,
								  FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		DWORD dwError = GetLastError();

		return SetLastError(dwError), FALSE;
	}

	// �������� ��������� WM_CLOSE ���� ������� �����, �������������
	// ��������
	EnumWindows(KillAppEnumWindows, dwProcessId);

	// �������� ���� ��������
	int nRet = KILLAPP_WAIT;
	while (WaitForSingleObject(hProcess, 100) == WAIT_TIMEOUT)
	{
		nRet = pfnWaitCallback(lParam);
		if (nRet != KILLAPP_WAIT)
			break;
	}

	// ��������� �������, ���� ���������������� �������
	// ������ ��������� ����� �������
	if (nRet == KILLAPP_TERMINATE)
		TerminateProcess(hProcess, (UINT)-1);

	CloseHandle(hProcess);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
static
BOOL CALLBACK KillAppEnumWindows(
    IN HWND hWnd,
    IN LPARAM lParam
    )
{
    _ASSERTE(lParam != 0);

    DWORD dwProcessId;
    GetWindowThreadProcessId(hWnd, &dwProcessId);

    if (IsWindowVisible(hWnd) &&
        dwProcessId == (DWORD)lParam)
        PostMessage(hWnd, WM_CLOSE, 0, 0);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CALLBACK WaitCallback(
    IN LPARAM lParam
    )
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (GetTickCount() < (DWORD)lParam) ? KILLAPP_WAIT : KILLAPP_TERMINATE;
}

/////////////////////////////////////////////////////////////////////////////
// ������� ������������ ������� ����
/////////////////////////////////////////////////////////////////////////////
static
BOOL CALLBACK EnumWindowsCallback(
    IN HWND hWnd,
    IN LPARAM lParam
    )
{
    PENUMAPPDATA pEnumData = (PENUMAPPDATA)lParam;
    _ASSERTE(_CrtIsValidPointer(pEnumData, sizeof(ENUMAPPDATA), 1));

    if (!IsWindowVisible(hWnd) || GetWindow(hWnd, GW_OWNER) != NULL)
        return TRUE;

    TCHAR szClassName[80];
    GetClassName(hWnd, szClassName, 80);

    if (lstrcmpi(szClassName, _T("Progman")) == 0)
        return TRUE;

    if (lstrcmpi(szClassName, _T("SvRec")) == 0)
        return TRUE;

    // �������� ��������� ���� 
    TCHAR szText[256];
    DWORD cchText = GetWindowText(hWnd, szText, 256);

    if (cchText == 0)
        return TRUE;

	// (�������) ������ ��� ��������� �������������� ������ ����������

    // �������� ���������������� �������
    return pEnumData->pfnEnumApp(hWnd, szText, NULL, pEnumData->lParam);
}

/////////////////////////////////////////////////////////////////////////////
BOOL EnumApplications(
    IN PFNENUMAPP pfnEnumApp,
    IN LPARAM lParam
    )
{
    _ASSERTE(pfnEnumApp != NULL);

    ENUMAPPDATA EnumData;
    EnumData.pfnEnumApp = pfnEnumApp;
    EnumData.lParam = lParam;

    return EnumWindows(EnumWindowsCallback, (LPARAM)&EnumData);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EnumKillAppCallback (
    IN HWND hWnd,    
    IN LPCTSTR pszName,
    IN HICON hIcon, 
    IN LPARAM lParam
    )
{
	return KillApplication(hWnd, WaitCallback, GetTickCount() + 5000);
}
/////////////////////////////////////////////////////////////////////////////

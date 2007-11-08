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
    IN HWND hWnd,                       // идентификатор окна
    IN PFNWAITCALLBACK pfnWaitCallback, // пользовательская функция
    IN LPARAM lParam                    // параметр для пользовательской функции
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
    IN HWND hWnd,             // идентификатор главного окна приложения
    IN LPCTSTR pszName,	      // название приложения
    IN HICON hIcon,           // иконка приложения
    IN LPARAM lParam          // пользовательский параметр
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
// Выключение компьютера
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
// Закрываем все программы
void CloseAllPrograms()
{
	// закрываем все видимые программы (кроме нашей)
	//EnumApplications(EnumKillAppCallback, NULL);

	// закрываем нашу программу
	CWnd* pMainWnd = AfxGetApp()->GetMainWnd();
	pMainWnd->PostMessage(WM_COMMAND, MAKEWPARAM(IDM_TRAY_EXIT, 0), 0);
}

/////////////////////////////////////////////////////////////////////////////
// ФУНКЦИИ ЗАВЕРШЕНИЯ ПРИЛОЖЕНИЙ
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

    // определяем идентификатор процесса и потока, которым
    // принадлежит указанное окно
    DWORD dwProcessId;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);

	// открываем описатель процесса
	HANDLE hProcess = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE,
								  FALSE, dwProcessId);
	if (hProcess == NULL)
	{
		DWORD dwError = GetLastError();

		return SetLastError(dwError), FALSE;
	}

	// посылаем сообщение WM_CLOSE всем видимым окнам, принадлежащим
	// процессу
	EnumWindows(KillAppEnumWindows, dwProcessId);

	// основной цикл ожидания
	int nRet = KILLAPP_WAIT;
	while (WaitForSingleObject(hProcess, 100) == WAIT_TIMEOUT)
	{
		nRet = pfnWaitCallback(lParam);
		if (nRet != KILLAPP_WAIT)
			break;
	}

	// завершаем процесс, если пользовательская функция
	// решила поступить таким образом
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
// ФУНКЦИИ ПЕРЕЧИСЛЕНИЯ ВИДИМЫХ ОКОН
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

    // получаем заголовок окна 
    TCHAR szText[256];
    DWORD cchText = GetWindowText(hWnd, szText, 256);

    if (cchText == 0)
        return TRUE;

	// (удалено) строки для получения идентификатора иконки приложения

    // вызываем пользовательскую функцию
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

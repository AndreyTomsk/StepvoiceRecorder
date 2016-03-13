////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <htmlhelp.h>

#include "MP3_Recorder.h"

#include "MainFrm.h"
#include "NagScreenDlg.h"
#include "EnterCodeDlg.h"

#include "UrlWnd.h"
#include "version.h"
#include "FileUtils.h"
#include <versionhelpers.h>
//#include <vld.h> //Header file from "Visual Leak Detector" - detecting memory leaks.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define SPECIAL_VERSION 1
#ifdef SPECIAL_VERSION
const char* SPECIAL_REGKEY="0IuDod5iSkTIHhlC2++aUJF30QIYhSySsWM920dr8YLPwfLpsrHD/uehHgTUlsXc9hwI+C7RgQiqDw9XFneqyXLquU1N1mNn09n94sBJLEJiciHOwUBSDPaJd61tStzH1PkjyX1Tt9fGif9BcGjId1IrQzOJ2QiqiBx61Xc8wUjE=";
#endif

////////////////////////////////////////////////////////////////////////////////
extern TCHAR g_command_line[MAX_PATH];

////////////////////////////////////////////////////////////////////////////////
// About dialog
////////////////////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	CUrlWnd	m_wndOrder;
	CFont	m_FontBold;
	static bool m_bRecIcon;

	//{{AFX_MSG(CAboutDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPlayerIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PLAYERICON, OnPlayerIcon)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
bool CAboutDlg::m_bRecIcon = false;

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	m_FontBold.CreateFont (-8, 0, 0, 0, FW_BOLD,  0, 0, 0, 0, 0, 0, 0, 0,
		_T("MS Sans Serif"));
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT

	m_bRecIcon = true;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

void CAboutDlg::OnPlayerIcon() 
{
	CStatic* wndRecIcon = (CStatic *)GetDlgItem(IDC_PLAYERICON);
	if(m_bRecIcon)
	{	SetTimer(1, 900, NULL);
	wndRecIcon->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME2));
	}
	else
	{	KillTimer(1);
	wndRecIcon->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME));
	}

	m_bRecIcon = !m_bRecIcon;
}

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	static bool bRecIcon = false;
	if(nIDEvent == 1)
	{
		CStatic* wndRecIcon = (CStatic *)GetDlgItem(IDC_PLAYERICON);
		if(bRecIcon)
			wndRecIcon->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME2));
		else
			wndRecIcon->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME));
		bRecIcon = !bRecIcon;
	}
	CDialog::OnTimer(nIDEvent);
}

#pragma optimize ("", off)
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString l_thanks_to[] = {
		_T("MPEG Layer-3 Audio Codec Lame 3.98.2\r\n(http://www.mp3dev.org/mp3)"),
		_T("BASS library (bass.dll) by Ian Luck\r\n(http://www.un4seen.com)"),
		_T("")
	};
	CString l_wnd_text;

	int i = 0;
	while (l_thanks_to[i] != _T(""))
	{
		l_wnd_text += l_thanks_to[i++];
		l_wnd_text += _T("\r\n\r\n");
	}

	CEdit* pThanksEdit = (CEdit *)GetDlgItem(IDC_THANKSTO);
	pThanksEdit->SetWindowText(l_wnd_text);

	m_wndOrder.SubclassDlgItem(IDC_GETNOW, this);
	m_wndOrder.SetUrl("http://stepvoice.com/order.shtml");


	REG_CRYPT_BEGIN;
#ifndef _DEBUG
	SetDlgItemText(IDC_REGNAME, fsProtect_GetUserName());
	m_wndOrder.ShowWindow(SW_HIDE);
#endif
	REG_CRYPT_END;

	// Preparing version string.

	CString l_format_string;
	GetDlgItemText(IDC_STATIC_VERSION, l_format_string);
	
	CString l_version_string;
	l_version_string.Format(l_format_string, STRFILEVER);
	SetDlgItemText(IDC_STATIC_VERSION, l_version_string);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	REG_CRYPT_BEGIN;
#ifndef _DEBUG
	if (pWnd->m_hWnd == GetDlgItem (IDC_REGNAME)->m_hWnd)
		pDC->SelectObject (&m_FontBold);
#endif
	REG_CRYPT_END;

	return hbr;
}
#pragma optimize ("", on)

////////////////////////////////////////////////////////////////////////////////
// Recorder application class
////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMP3_RecorderApp, CWinApp)
	//{{AFX_MSG_MAP(CMP3_RecorderApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//ON_COMMAND(IDM_HELP_WWW, OnHelpWww)
	ON_COMMAND(ID_HELP_OPENLOGFOLDER, OnHelpOpenLogFolder)
	ON_COMMAND(IDM_HELP_EMAIL, OnHelpEmail)
	ON_COMMAND(IDM_HELP_DOC, OnHelpDoc)
	ON_COMMAND(ID_HELP_ENTERCODE, OnHelpEntercode)
	//ON_COMMAND(ID_HELP_HOWTO, OnHelpHowto)
	ON_COMMAND(IDA_HELP_DOC, OnHelpDoc)
	//ON_COMMAND(ID_HELP_ORDERONLINE, OnHelpOrderOnline)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_CHECKFORUPDATES, &CMP3_RecorderApp::OnHelpCheckforupdates)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CMP3_RecorderApp theApp;

////////////////////////////////////////////////////////////////////////////////
CMP3_RecorderApp::CMP3_RecorderApp()
	:m_is_vista(::IsWindowsVistaOrGreater())
{
}

////////////////////////////////////////////////////////////////////////////////
static const UINT UWM_ARE_YOU_ME = ::RegisterWindowMessage(
	_T("UWM_ARE_YOU_ME-{B87861B4-8BE0-4dc7-A952-E8FFEEF48FD3}"));

static const UINT UWM_PARSE_LINE = ::RegisterWindowMessage(
	_T("UWM_PARSE_LINE-{FE0907E6-B77E-46da-8D2B-15F41F32F440}"));

////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CMP3_RecorderApp::searcher(HWND hWnd, LPARAM lParam)
{
    DWORD result;
    LRESULT ok = ::SendMessageTimeout(hWnd, UWM_ARE_YOU_ME, 0, 0,
                                      SMTO_BLOCK | SMTO_ABORTIFHUNG, 200,
                                      &result);
    if(ok == 0)
    return TRUE; // игнорируем это окно и продолжаем

    if(result == UWM_ARE_YOU_ME)
    { /* нашли */
        HWND * target = (HWND *)lParam;
        *target = hWnd;
        return FALSE; // заканчиваем поиск
    } /* нашли */

    return TRUE; // продолжаем поиск
} // CMyApp::searcher


bool CMP3_RecorderApp::IsNeedOneInstance()
{
	return !RegistryConfig::GetOption(_T("General\\Multiple instances"), false);
}

bool CMP3_RecorderApp::IsAlreadyRunning()
{
    HANDLE hMutexOneInstance = ::CreateMutex( NULL, FALSE,
        _T("SVREC-169A0B91-77B7-4533-9C25-59FCB08FCD614"));

	bool l_already_running = (::GetLastError() == ERROR_ALREADY_EXISTS ||
							  ::GetLastError() == ERROR_ACCESS_DENIED);
    // вызов возвращает ERROR_ACCESS_DENIED, если мьютекс был создан
    // в другой пользовательской сессии, т.к. в качестве параметра
    // SECURITY_ATTRIBUTES при создании мьютекса передается NULL

	return l_already_running;
}

////////////////////////////////////////////////////////////////////////////////
BOOL CMP3_RecorderApp::InitInstance()
{
	//New SetRegistryKey commented - must be called once.
	SetRegistryKey(_T("StepVoice Software"));
	//RegistryConfig::SetRegistryKey(_T("StepVoice Software"));

	if (m_lpCmdLine[0] != _T('\0') && CString(m_lpCmdLine) == _T("/register"))
	{
#ifdef SPECIAL_VERSION
			// Adding a registration key into the system registry
			CEnterCodeDlg ecDlg;
			ecDlg.RegisterKey(SPECIAL_REGKEY, true);
#endif
			return true;
	}

	// Variables for command line
	CString l_cmd_line(m_lpCmdLine);
	const int CMD_LENGTH = l_cmd_line.GetLength();

	if (this->IsAlreadyRunning() && this->IsNeedOneInstance())
    {
        HWND hOther = NULL;
        EnumWindows(searcher, (LPARAM)&hOther);
        if ( hOther != NULL )
        {
			if (CMD_LENGTH > 0)
			{
				_tcsncpy_s(g_command_line, MAX_PATH,
					l_cmd_line.GetBuffer(CMD_LENGTH), CMD_LENGTH);
				::PostMessage(hOther, UWM_PARSE_LINE, 0, 0);
			}
			else
			{
				::SetForegroundWindow( hOther );
				::ShowWindow( hOther, SW_RESTORE );
			}
			return FALSE; // window exist, exiting this instance
        }
		//return FALSE; // exiting
    }

	//Initializing logger after the running instance check (avoid log creation error).
	InitLogger();
	LOG_INFO() << "Program started.";
	LOG_INFO() << "Operation system: " << GetWindowsVersionString() << '.';

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	pFrame->DragAcceptFiles(TRUE);
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	if (CMD_LENGTH > 0)
	{
		_tcsncpy_s(g_command_line, MAX_PATH,
			l_cmd_line.GetBuffer(CMD_LENGTH), CMD_LENGTH);
		pFrame->PostMessage(UWM_PARSE_LINE, 0, 0);
	}

	return TRUE;
}
//---------------------------------------------------------------------------

int CMP3_RecorderApp::ExitInstance()
{
	LOG_INFO() << "Program closed.";
	return 0;
}
//---------------------------------------------------------------------------

void CMP3_RecorderApp::InitLogger()
{
	//Checking stepvoice data folder.

	using namespace FileUtils;
	const CString appDataFolder = GetSpecialFolder(CSIDL_COMMON_APPDATA);
	const CString svrecDataFolder = CombinePath(appDataFolder, _T("Stepvoice"));

	if (!FolderExists(svrecDataFolder) && !::CreateDirectory(svrecDataFolder, NULL))
	{
		AfxMessageBox(_T("Unable to create log folder: ") + svrecDataFolder);
		return;
	}

	//Keeping only last 3 log files in folder (in case of some problem,
	//user reopen svrec and send report with current-normal and previous-
	//problematic logs). Log names are in this format:
	//	SvRec_1.log.txt <-- oldest
	//	SvRec_2.log.txt
	//	SvRec_3.log.txt <-- newest

	const CString log1Path = CombinePath(svrecDataFolder, _T("SvRec_1.log.txt"));
	const CString log2Path = CombinePath(svrecDataFolder, _T("SvRec_2.log.txt"));
	const CString log3Path = CombinePath(svrecDataFolder, _T("SvRec_3.log.txt"));

	const bool haveLog1 = FileExists(log1Path);
	const bool haveLog2 = FileExists(log2Path);
	const bool haveLog3 = FileExists(log3Path);

	//When all files present - remove the oldest log and shift names.
	if (haveLog3)
	{
		if (haveLog1)
			::DeleteFile(log1Path);
		if (haveLog2)
			::MoveFile(log2Path, log1Path);
		::MoveFile(log3Path, log2Path);
	}

	//Ok, previous logs processed, creating logger with appropriate path.

	if (haveLog3 || haveLog2)
		CLog::Open(log3Path);
	else
	if (haveLog1)
		CLog::Open(log2Path);
	else
		CLog::Open(log1Path);
}
//---------------------------------------------------------------------------

CString CMP3_RecorderApp::GetWindowsVersionString() const
{
	CString version = _T("?");

	//if (::IsWindows10OrGreater())
	//	version = _T("10");
	if (::IsWindows8Point1OrGreater())
		version = _T("8.1");
	else
	if (::IsWindows8OrGreater())
		version = _T("8.0");
	else
	if (::IsWindows7SP1OrGreater())
		version = _T("7 (SP1)");
	else
	if (::IsWindows7OrGreater())
		version = _T("7");
	else
	if (::IsWindowsVistaSP2OrGreater())
		version = _T("Vista (SP2)");
	else
	if (::IsWindowsVistaSP1OrGreater())
		version = _T("Vista (SP1)");
	else
	if (::IsWindowsVistaOrGreater())
		version = _T("Vista");

	CString resultString;
	resultString.Format(_T("Windows %s (or greater) %s"), version, IsWow64() ? _T("x64") : _T("x32"));
	return resultString;
}
//---------------------------------------------------------------------------

bool CMP3_RecorderApp::IsWow64() const
{
	BOOL bIsWow64 = FALSE;

	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function
	//and GetProcAddress to get a pointer to the function if available.

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			//handle error
		}
	}
	return bIsWow64 ? true : false;
}

////////////////////////////////////////////////////////////////////////////////
void CMP3_RecorderApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

//void CMP3_RecorderApp::OnHelpWww() 
//{
//	ShellExecute(0, NULL, _T("http://www.stepvoice.com"), NULL, NULL,
//		SW_SHOWNORMAL);
//}

//void CMP3_RecorderApp::OnHelpOrderOnline() 
//{
//	ShellExecute(0, NULL, _T("http://www.stepvoice.com/order.shtml"), NULL, NULL,
//		SW_SHOWNORMAL);
//}

void CMP3_RecorderApp::OnHelpOpenLogFolder()
{
	const CString appDataFolder = FileUtils::GetSpecialFolder(CSIDL_COMMON_APPDATA);
	const CString svrecDataFolder = FileUtils::CombinePath(appDataFolder, _T("Stepvoice"));
	ShellExecute(::GetDesktopWindow(), _T("open"), svrecDataFolder, NULL, NULL, SW_SHOW);
}

void CMP3_RecorderApp::OnHelpEmail() 
{
	CString mailString;
	mailString.Format(_T("mailto:support@stepvoice.com?subject=[svrec %s]&body=(Please attach log files, accessible via program menu: Help | Open log folder)"),
		STRFILEVER);

	mailString.Replace(_T(" "), _T("%20"));
	ShellExecute(0, NULL, mailString, NULL, NULL, SW_SHOWNORMAL);
}

void CMP3_RecorderApp::OnHelpDoc() 
{
	CMainFrame* pFrame = (CMainFrame *)m_pMainWnd;

	using namespace FileUtils;
	CString helpFile = CombinePath(GetProgramFolder(), _T("svrec.chm::/stepvoice_recorder/overview.html"));
	::HtmlHelp(GetDesktopWindow(), helpFile, HH_DISPLAY_TOPIC, NULL);
}

void CMP3_RecorderApp::OnHelpCheckforupdates()
{
	AfxMessageBox(_T("TODO: implement check for updates."));
	// TODO: Add your command handler code here
}

//void CMP3_RecorderApp::OnHelpHowto() 
//{
//	CMainFrame* pFrame = (CMainFrame *)m_pMainWnd;
//
//	using namespace FileUtils;
//	CString helpFile = CombinePath(GetProgramFolder(), _T("svrec.chm::/stepvoice_recorder/how_to_register.html"));
//	::HtmlHelp(GetDesktopWindow(), helpFile, HH_DISPLAY_TOPIC, NULL);
//}

void CMP3_RecorderApp::OnHelpEntercode() 
{
	CEnterCodeDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		//Restarting program to check the registration key.
		AfxGetMainWnd()->PostMessageW(WM_CLOSE);
		const CString programPath = FileUtils::GetProgramPath();
		ShellExecute(::GetDesktopWindow(), _T("open"), programPath, NULL, NULL, SW_SHOW);
	}
}

////////////////////////////////////////////////////////////////////////////////

#pragma optimize ("", off)
BOOL CMP3_RecorderApp::OnIdle(LONG lCount)
{
	// Letting the base class make all needed work
	if (CWinApp::OnIdle(lCount))
	{
		return TRUE;
	}

	static bool nagScreenDisplayed = false;
	static DWORD initialMS = ::GetTickCount();

	REG_CRYPT_BEGIN;
	nagScreenDisplayed = true;
	REG_CRYPT_END;

	if (!nagScreenDisplayed && ::GetTickCount()-initialMS > 500)
	{
		try
		{
			CNagScreenDlg nagDlg(AfxGetMainWnd());
			UINT resultID = nagDlg.DoModal();
			resultID = resultID / (resultID - IDOK); //Just some obfuscation: close button is IDCANCEL
			nagScreenDisplayed = true;
		}
		catch (...)
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
		}
	}

	// Updating the main window interface
	CMainFrame* l_main_wnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(l_main_wnd);
	l_main_wnd->UpdateInterface();

	return FALSE;	// No more idle time needed
}
#pragma optimize ("", on)

////////////////////////////////////////////////////////////////////////////////

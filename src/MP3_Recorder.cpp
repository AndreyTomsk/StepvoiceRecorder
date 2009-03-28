////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <htmlhelp.h>

#include "MP3_Recorder.h"

#include "MainFrm.h"
#include "EnterCodeDlg.h"
#include "NagScreenDlg.h"

#include "UrlWnd.h"
#include "version.h"

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
	//ON_NOTIFY(NM_CLICK, IDC_PLAYERICON, OnPlayerIcon)
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

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if(pWnd->m_hWnd == GetDlgItem(IDC_GETNOW)->m_hWnd)
		pDC->SetTextColor(RGB(0, 0, 230));

	REG_CRYPT_BEGIN;
#ifndef _DEBUG
	if (pWnd->m_hWnd == GetDlgItem (IDC_REGNAME)->m_hWnd)
		pDC->SelectObject (&m_FontBold);
#endif
	REG_CRYPT_END;

	return hbr;
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
	//m_wndOrder.Init();
	m_wndOrder.SetUrl("http://stepvoice.com/order.html");


	REG_CRYPT_BEGIN;
#ifndef _DEBUG
	SetDlgItemText(IDC_REGNAME, fsProtect_GetUserName());
	m_wndOrder.ShowWindow(SW_HIDE);
#endif
	REG_CRYPT_END;

	// Preparing the version string.
	int n[4] = {0, 0, 0, 0};
	sscanf_s(STRFILEVER, _T("%d, %d, %d, %d\0"), &n[0], &n[1], &n[2], &n[3]);

	CString l_format_string;
	GetDlgItemText(IDC_STATIC_VERSION, l_format_string);
	
	CString l_version_string;
	l_version_string.Format(l_format_string, n[0], n[1], n[2], n[3]);
	SetDlgItemText(IDC_STATIC_VERSION, l_version_string);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
#pragma optimize ("", on)

////////////////////////////////////////////////////////////////////////////////
// Recorder application class
////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMP3_RecorderApp, CWinApp)
	//{{AFX_MSG_MAP(CMP3_RecorderApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(IDM_HELP_WWW, OnHelpWww)
	ON_COMMAND(IDM_HELP_EMAIL, OnHelpEmail)
	ON_COMMAND(IDM_HELP_DOC, OnHelpDoc)
	ON_COMMAND(ID_HELP_ENTERCODE, OnHelpEntercode)
	ON_COMMAND(ID_HELP_HOWTO, OnHelpHowto)
	ON_UPDATE_COMMAND_UI(ID_HELP_ENTERCODE, OnUpdateHelpEntercode)
	ON_UPDATE_COMMAND_UI(ID_HELP_HOWTO, OnUpdateHelpHowto)
	ON_COMMAND(IDA_HELP_DOC, OnHelpDoc)
	ON_COMMAND(ID_HELP_ORDERONLINE, OnHelpOrderOnline)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CMP3_RecorderApp theApp;

////////////////////////////////////////////////////////////////////////////////
CMP3_RecorderApp::CMP3_RecorderApp()
{
	// Checking if running in Vista.
	OSVERSIONINFO OSVersion;
	OSVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&OSVersion);
	m_is_vista = OSVersion.dwMajorVersion > 5;
}

////////////////////////////////////////////////////////////////////////////////
CString CMP3_RecorderApp::GetProgramDir()
{
	CString l_exe_name;

	GetModuleFileName(AfxGetInstanceHandle(),
		l_exe_name.GetBuffer(MAX_PATH), MAX_PATH);
	l_exe_name.ReleaseBuffer();

	return l_exe_name.Left(l_exe_name.ReverseFind(_T('\\')));
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
	return (AfxGetApp()->GetProfileInt(
		_T("General"), _T("Multiple instances"), 0) == 0);
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

#pragma optimize ("", off)
////////////////////////////////////////////////////////////////////////////////
BOOL CMP3_RecorderApp::InitInstance()
{
	// метка в реестре
	SetRegistryKey(_T("StepVoice Software"));

	if (m_lpCmdLine[0] != _T('\0') && strcmp(m_lpCmdLine, _T("/register")) == 0)
	{
#ifdef SPECIAL_VERSION
			// Adding a registration key into the system registry
			CEnterCodeDlg ecDlg;
			ecDlg.RegisterKey( SPECIAL_REGKEY );
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
        }

		return FALSE; // exiting this instance
    }

	// показываем наг-скрин
	REG_CRYPT_BEGIN;
	//#ifndef _DEBUG
		goto _lNoNag;
	//#endif
	REG_CRYPT_END;

	try
	{
		CNagScreenDlg NagDlg;
		UINT nIDRet = NagDlg.DoModal();
		nIDRet = nIDRet / (nIDRet - IDOK);
	}
	catch (...)
	{
		return FALSE;
	}

_lNoNag:
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	pFrame->DragAcceptFiles(TRUE);
	pFrame->ShowWindow();
	pFrame->UpdateWindow();
	if (CMD_LENGTH > 0)
	{
		_tcsncpy_s(g_command_line, MAX_PATH,
			l_cmd_line.GetBuffer(CMD_LENGTH), CMD_LENGTH);
		pFrame->PostMessage(UWM_PARSE_LINE, 0, 0);
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
void CMP3_RecorderApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CMP3_RecorderApp::OnHelpWww() 
{
	ShellExecute(0, NULL, _T("http://www.stepvoice.com"), NULL, NULL,
		SW_SHOWNORMAL);
}

void CMP3_RecorderApp::OnHelpOrderOnline() 
{
	ShellExecute(0, NULL, _T("http://www.stepvoice.com/order.html"), NULL, NULL,
		SW_SHOWNORMAL);
}

void CMP3_RecorderApp::OnHelpEmail() 
{
	// Preparing the version string.
	int n[4] = {0, 0, 0, 0};
	CString l_version_string(_T("%d.%d.%d.%d"));

	sscanf_s(STRFILEVER, _T("%d, %d, %d, %d\0"), &n[0], &n[1], &n[2], &n[3]);
	l_version_string.Format(l_version_string, n[0], n[1], n[2], n[3]);

	CString l_mail_string(
		_T("mailto:support@stepvoice.com?subject=SV%20Recorder,%20version%20"));

	l_mail_string = l_mail_string + l_version_string;

	///@bug Add body message regarding common recording problems.
	///see "mailto" in Windows SDK.

	ShellExecute(0, NULL, l_mail_string, NULL, NULL, SW_SHOWNORMAL);
}

void CMP3_RecorderApp::OnHelpDoc() 
{
	CMainFrame* pFrame = (CMainFrame *)m_pMainWnd;

	CString l_help_file = GetProgramDir() + _T("\\svrec.chm::/stepvoice_recorder/overview.html");
	::HtmlHelp(GetDesktopWindow(), l_help_file, HH_DISPLAY_TOPIC, NULL);
}

void CMP3_RecorderApp::OnHelpHowto() 
{
	CMainFrame* pFrame = (CMainFrame *)m_pMainWnd;

	CString l_help_file = GetProgramDir() + _T("\\svrec.chm::/stepvoice_recorder/how_to_register.html");
	::HtmlHelp(GetDesktopWindow(), l_help_file, HH_DISPLAY_TOPIC, NULL);
}

void CMP3_RecorderApp::OnHelpEntercode() 
{
	CEnterCodeDlg dlg;
	dlg.DoModal();
}

////////////////////////////////////////////////////////////////////////////////
void CMP3_RecorderApp::OnUpdateHelpHowto(CCmdUI* pCmdUI) 
{
	/*
	REG_CRYPT_BEGIN;
	pCmdUI->Enable(false);
	REG_CRYPT_END;
	*/
}

void CMP3_RecorderApp::OnUpdateHelpEntercode(CCmdUI* pCmdUI) 
{
	/*
	REG_CRYPT_BEGIN;
	pCmdUI->Enable(false);
	REG_CRYPT_END;
	*/
}

////////////////////////////////////////////////////////////////////////////////
BOOL CMP3_RecorderApp::OnIdle(LONG lCount)
{
	// Letting the base class make all needed work
	if (CWinApp::OnIdle(lCount))
	{
		return TRUE;
	}

	// Updating the main window interface
	CMainFrame* l_main_wnd = (CMainFrame*)AfxGetMainWnd();
	ASSERT(l_main_wnd);
	l_main_wnd->UpdateInterface();

	return FALSE;	// No more idle time needed
}

////////////////////////////////////////////////////////////////////////////////
#pragma optimize ("", on)

/////////////////////////////////////////////////////////////////////////////
// NagScreenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NagScreenDlg.h"
#include "EnterCodeDlg.h"
#include "HtmlHelp.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ORDER_URL "http://stepvoice.com/order.shtml"

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CNagScreenDlg, CDialog)
	//{{AFX_MSG_MAP(CNagScreenDlg)
	ON_BN_CLICKED(IDC_ENTERCODE, OnEntercode)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, OnOrder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CNagScreenDlg::CNagScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNagScreenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNagScreenDlg)
	//}}AFX_DATA_INIT
	m_BoldFont.CreateFont (-8, 0, 0, 0, FW_BOLD,  0, 0, 0, 0, 0, 0, 0, 0,
		"MS Sans Serif");
}


void CNagScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNagScreenDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CNagScreenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE); 

	m_wndOrder.SubclassDlgItem(IDC_ONLINEREGISTER, this);
	m_wndOrder.SetUrl(ORDER_URL);

	CString unregText;
	GetDlgItemText(IDC_UNREGVERSION, unregText);
	unregText.Format(unregText, fsProtect_GetDaysLeft());
	SetDlgItemText(IDC_UNREGVERSION, unregText);

	CButton* pBtnOK = (CButton *)GetDlgItem(IDCANCEL);
	pBtnOK->EnableWindow(false);

	SetTimer(1, 1000, NULL);

	{
		// Setting a version number in window caption
		int n[4] = {0, 0, 0, 0};
		sscanf_s(STRFILEVER, _T("%d, %d, %d, %d\0"), &n[0], &n[1], &n[2], &n[3]);

		CString l_format_string;
		GetWindowText(l_format_string);

		CString l_version_string;
		l_version_string.Format(l_format_string, n[0], n[1], n[2], n[3]);
		SetWindowText(l_version_string);
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CNagScreenDlg::OnTimer(UINT nIDEvent) 
{
	static int nSeconds = 2;
	CButton* pBtnOK = (CButton *)GetDlgItem(IDCANCEL);

	if (nSeconds == 0) {	
		pBtnOK->EnableWindow(true);
		KillTimer(1);
	}
	else
		nSeconds--;

	CDialog::OnTimer(nIDEvent);
}


/////////////////////////////////////////////////////////////////////////////
HBRUSH CNagScreenDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == GetDlgItem(IDC_THANKYOU)->m_hWnd)
		pDC->SelectObject(&m_BoldFont);

	else if (pWnd->m_hWnd == GetDlgItem(IDC_UNREGVERSION)->m_hWnd) {
		pDC->SelectObject(&m_BoldFont);
		pDC->SetTextColor(RGB(150, 0, 0));
	}

	return hbr;
}

/////////////////////////////////////////////////////////////////////////////
void CNagScreenDlg::OnEntercode() 
{
	CEnterCodeDlg dlg;
	if (dlg.DoModal() == IDOK)
	{	
		// Exit code as IDOK - to make an error and quit current instance.
		PostMessage(WM_COMMAND, IDOK, (LPARAM)GetDlgItem(IDCANCEL)->m_hWnd);

		char FileName[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(), FileName, MAX_PATH);
		ShellExecute(::GetDesktopWindow(), "open", FileName, NULL, NULL, SW_SHOW);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CNagScreenDlg::OnOrder() 
{
	ShellExecute(::GetDesktopWindow(), "open", ORDER_URL, NULL, NULL, SW_SHOW);
}

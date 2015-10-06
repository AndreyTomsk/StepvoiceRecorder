// EnterCodeDlg.cpp : implementation file
//
#include "stdafx.h"
#include "EnterCodeDlg.h"
#include <htmlhelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnterCodeDlg dialog
CEnterCodeDlg::CEnterCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterCodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnterCodeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEnterCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterCodeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnterCodeDlg, CDialog)
	//{{AFX_MSG_MAP(CEnterCodeDlg)
	ON_BN_CLICKED(IDC_PASTE, OnPaste)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_EN_CHANGE(IDC_EDITCODE, OnChangeEditcode)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CEnterCodeDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	/*if(LOWORD(wParam) == IDHELP)
	{	// ���������� ������ �� �����
		CMP3_RecorderApp* App = (CMP3_RecorderApp *)AfxGetApp();
		CString strFile = App->GetProgramDir();
		strFile += "\\svrec.chm::/license_key.html";
		HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
	}*/
	
	return CDialog::OnCommand(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
bool CEnterCodeDlg::RegisterKey(CString strKey, bool multiUserKey)
{
	// ������� ��� �������� �� ������

	strKey.Remove(_T(' '));	// ������� ��� �������
	const TCHAR BEGIN_MARK[] = _T("KEYBEGINKEY");// ����� "KEY BEGIN KEY" ��� ��������
	const TCHAR END_MARK[]   = _T("KEYENDKEY");  // ����� "KEY END KEY" ��� ��������

	int nLength = strKey.Find(BEGIN_MARK);
	if(nLength != -1)
	{	// ���� ����� ������ ����� �������, �� ��������� ������ ����� ����� ���
		nLength = strKey.GetLength() - nLength - (sizeof(BEGIN_MARK) - 1);
		strKey = strKey.Right(nLength);
		strKey.TrimLeft(_T('-'));	// ������� ������ �����
		strKey.TrimLeft();		// ������� "������� ������" ���� ���� (�����)
	}

	nLength = strKey.Find(END_MARK);
	if(nLength != -1)
	{	// ���� ����� ����� ����� �������, �� ��������� ����� ����� �� ���
		strKey = strKey.Left(nLength);
		strKey.TrimRight(_T('-'));	// ������� ������ ������
		strKey.TrimRight();		// ������� "������� ������" ���� ���� (������)
	}

	// Writing key to registry

	const DWORD keySizeBytes = (strKey.GetLength()+1)*sizeof(TCHAR);
	const LONG result = RegSetKeyValue(
		multiUserKey ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER,
		_T("Software\\Stepvoice Software\\svrec"), _T("Key"),
		REG_SZ, strKey,	keySizeBytes);

	return result == ERROR_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEnterCodeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rectParent;
	this->GetParent()->GetWindowRect(&rectParent);
	RECT rectDialog;
	this->GetWindowRect(&rectDialog);

	int newX = rectParent.right + 30 - (rectDialog.right - rectDialog.left);
	int newY = rectParent.bottom + 42 - (rectDialog.bottom - rectDialog.top);
	this->SetWindowPos(&wndTop, newX, newY, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	
	//SetIcon(LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CEnterCodeDlg::OnOK() 
{
	CString strCode;
	GetDlgItemText(IDC_EDITCODE, strCode);
	if (!strCode.IsEmpty())
	{
		if (RegisterKey(strCode))
			AfxMessageBox(IDS_REGISTRATION_RESTART, MB_OK|MB_ICONINFORMATION);
		else
			AfxMessageBox(IDS_ERROR_REGISTER_WRITE, MB_OK|MB_ICONWARNING);
		CDialog::OnOK();
	}
	else
		CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
void CEnterCodeDlg::OnPaste() 
{
	CEdit* pEditKey = (CEdit *)GetDlgItem(IDC_EDITCODE);

	OnClear();
	pEditKey->Paste();
}

/////////////////////////////////////////////////////////////////////////////
void CEnterCodeDlg::OnClear() 
{
	CEdit* pEditKey = (CEdit *)GetDlgItem(IDC_EDITCODE);
	pEditKey->SetSel(0, -1, true);
	pEditKey->Clear();
	pEditKey->SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
void CEnterCodeDlg::OnChangeEditcode() 
{
	/*CEdit* pEditKey = (CEdit *)GetDlgItem(IDC_EDITCODE);
	int nLines	= pEditKey->GetLineCount();
	int nLength	= pEditKey->LineLength(-1);

	CButton* pBtnOK	= (CButton *)GetDlgItem(IDOK);
	CButton* pBtnCl	= (CButton *)GetDlgItem(IDC_CLEAR);
	if(nLines == 1 && nLength == 0)
	{	pBtnOK->ModifyStyle(0, WS_DISABLED);
		pBtnCl->ModifyStyle(0, WS_DISABLED);
	}
	else
	{	pBtnOK->ModifyStyle(WS_DISABLED, 0);
		pBtnCl->ModifyStyle(WS_DISABLED, 0);
	}
	pBtnOK->Invalidate(false);
	pBtnCl->Invalidate(false);*/
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEnterCodeDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	this->SendMessage(WM_COMMAND, IDHELP,
		(LPARAM)GetDlgItem(IDHELP)->GetSafeHwnd());

	return CDialog::OnHelpInfo(pHelpInfo);
}

/////////////////////////////////////////////////////////////////////////////

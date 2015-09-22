// EnterCodeDlg.cpp : implementation file
//
#include "stdafx.h"
#include "EnterCodeDlg.h"
#include "HtmlHelp.h"

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
	{	// отображаем помощь по ключу
		CMP3_RecorderApp* App = (CMP3_RecorderApp *)AfxGetApp();
		CString strFile = App->GetProgramDir();
		strFile += "\\svrec.chm::/license_key.html";
		HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
	}*/
	
	return CDialog::OnCommand(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
bool CEnterCodeDlg::RegisterKey(LPCSTR pszKey, bool multiUserKey)
{
	HKEY hKey;
    DWORD dwDisposition;

	// удаляем все ненужное из строки
	CString strKey = pszKey;

	strKey.Remove(' ');	// удаляем все пробелы
	const char BEGIN_MARK[] = "KEYBEGINKEY";// метка "KEY BEGIN KEY" без пробелов
	const char END_MARK[]   = "KEYENDKEY";	// метка "KEY END KEY" без пробелов

	int nLength = strKey.Find(BEGIN_MARK);
	if(nLength != -1)
	{	// если метка начала ключа найдена, то оставляем правую часть после нее
		nLength = strKey.GetLength() - nLength - (sizeof(BEGIN_MARK) - 1);
		strKey = strKey.Right(nLength);
		strKey.TrimLeft('-');	// убираем минусы слева
		strKey.TrimLeft();		// убираем "переход строки" если есть (слева)
	}

	nLength = strKey.Find(END_MARK);
	if(nLength != -1)
	{	// если метка конца ключа найдена, то оставляем левую часть до нее
		strKey = strKey.Left(nLength);
		strKey.TrimRight('-');	// убираем минусы справа
		strKey.TrimRight();		// убираем "переход строки" если есть (справа)
	}

	// открываем ветку реестра
	HKEY destHKEY = multiUserKey ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
	if (ERROR_SUCCESS != RegCreateKeyEx(destHKEY,
		"Software\\StepVoice Software\\svrec", 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
		&dwDisposition)) 
    {
        return FALSE;
    }

	// записываем в реестр ключ
	if (RegSetValueEx(hKey, "Key", 0, REG_SZ, (BYTE*)strKey.GetBuffer(strKey.GetLength()),
		strKey.GetLength()) != ERROR_SUCCESS)
    { 
        RegCloseKey(hKey);
        return FALSE;
    }

    RegCloseKey(hKey);
    return TRUE;
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

	if (strCode.IsEmpty()) {
		CDialog::OnOK();
		return;
	}

	if (FALSE == RegisterKey(strCode)) {
		MessageBox("Error writing to the registry", "Error", MB_ICONERROR);
		CDialog::OnOK();
		return;
	}

	CString strText((LPCSTR)IDS_REG_TEXT_OK);
	CString strCaption((LPCSTR)IDS_REG_CAPTION);
	MessageBox(strText, strCaption, MB_OK);

	CDialog::OnOK();
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

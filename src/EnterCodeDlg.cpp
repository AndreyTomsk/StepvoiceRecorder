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
	ON_EN_CHANGE(IDC_EDITCODE, OnChangeEditcode)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_GETNOW, &CEnterCodeDlg::OnBnClickedGetnow)
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
bool CEnterCodeDlg::RegisterKey(CString strKey, bool multiUserKey)
{
	// удаляем все ненужное из строки

	strKey.Remove(_T(' '));	// удаляем все пробелы
	const TCHAR BEGIN_MARK[] = _T("KEYBEGINKEY");// метка "KEY BEGIN KEY" без пробелов
	const TCHAR END_MARK[]   = _T("KEYENDKEY");  // метка "KEY END KEY" без пробелов

	int nLength = strKey.Find(BEGIN_MARK);
	if(nLength != -1)
	{	// если метка начала ключа найдена, то оставляем правую часть после нее
		nLength = strKey.GetLength() - nLength - (sizeof(BEGIN_MARK) - 1);
		strKey = strKey.Right(nLength);
		strKey.TrimLeft(_T('-'));	// убираем минусы слева
		strKey.TrimLeft();		// убираем "переход строки" если есть (слева)
	}

	nLength = strKey.Find(END_MARK);
	if(nLength != -1)
	{	// если метка конца ключа найдена, то оставляем левую часть до нее
		strKey = strKey.Left(nLength);
		strKey.TrimRight(_T('-'));	// убираем минусы справа
		strKey.TrimRight();		// убираем "переход строки" если есть (справа)
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

	m_wndLostKey.SubclassDlgItem(IDC_SUPPORT, this);
	m_wndLostKey.SetUrl("http://stepvoice.com/support.shtml");
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
void CEnterCodeDlg::OnChangeEditcode() 
{
	CButton* regButton = static_cast<CButton*>(GetDlgItem(IDOK));
	CEdit* keyEdit = static_cast<CEdit*>(GetDlgItem(IDC_EDITCODE));

	CString key;
	keyEdit->GetWindowTextW(key);

	//bool simpleValidCheck = key.Left(1) == _T('0') && key.Right(1) == _T('=');
	regButton->EnableWindow(key.GetLength() > 0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEnterCodeDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	this->SendMessage(WM_COMMAND, IDHELP,
		(LPARAM)GetDlgItem(IDHELP)->GetSafeHwnd());

	return CDialog::OnHelpInfo(pHelpInfo);
}

/////////////////////////////////////////////////////////////////////////////

void CEnterCodeDlg::OnBnClickedGetnow()
{
	const CString orderLink = _T("http://www.regnow.com/softsell/nph-softsell.cgi?item=9959-1&quantity_9959-1=1&ss_short_order=true");
	ShellExecute(0, NULL, orderLink, NULL, NULL, SW_SHOWNORMAL);
}

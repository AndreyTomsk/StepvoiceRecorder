#include "stdafx.h"
#include "MP3_Recorder.h"
#include "PageCom.h"
#include "Config.h"
#include "ShellUtils.h"
#include "Autoname.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPageCom, CPropertyPage)

BEGIN_MESSAGE_MAP(CPageCom, CPropertyPage)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDB_OUTPUT_FOLDER, &CPageCom::OnBnClickedOutputFolder)
	ON_STN_CLICKED(IDC_DEFAULT_TEMPLATE, &CPageCom::OnStnClickedDefaultTemplate)
	ON_EN_CHANGE(IDE_OUTPUT_FILE_TEMPLATE, &CPageCom::OnEnChangeOutputFileTemplate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CPageCom::CPageCom()
	:CPropertyPage(CPageCom::IDD)
	,m_TrayIcon(FALSE)
	,m_TrayMin(FALSE)
{
}
//---------------------------------------------------------------------------
 
CPageCom::~CPageCom()
{
}
//---------------------------------------------------------------------------

void CPageCom::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_TRAYICON, m_TrayIcon);
	DDX_Check(pDX, IDC_TRAYMIN, m_TrayMin);
	DDX_Text(pDX, IDE_OUTPUT_FOLDER, m_outputFolder);
	DDX_Text(pDX, IDE_OUTPUT_FILE_TEMPLATE, m_outputFileTemplate);

	DDV_MaxChars(pDX, m_outputFileTemplate, 64);
}
//---------------------------------------------------------------------------

BOOL CPageCom::OnKillActive()
{
	UpdateData();
	if (!ShellUtils::FolderExists(m_outputFolder))
	{
		CString msg;
		msg.Format(_T("Folder '%s' does not exist or not accessible."), m_outputFolder);
		AfxMessageBox(msg);
		return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------

void CPageCom::OnOK()
{
	RegistryConfig::SetOption(_T("General\\OutputFolder"), m_outputFolder);
	RegistryConfig::SetOption(_T("General\\OutputFileTemplate"), m_outputFileTemplate);
	RegistryConfig::SetOption(_T("General\\Show icon in tray"), m_TrayIcon);
	RegistryConfig::SetOption(_T("General\\Minimize to tray"), m_TrayMin);

	CPropertyPage::OnOK();
}
//---------------------------------------------------------------------------

BOOL CPageCom::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTEXTHELP;
	return CPropertyPage::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------

BOOL CPageCom::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
//---------------------------------------------------------------------------

BOOL CPageCom::OnInitDialog()
{
	//Changing cursor style to arror (on the default name template label).
	CStatic* pStatic = static_cast<CStatic *>(GetDlgItem(IDC_DEFAULT_TEMPLATE));
	SetClassLong(pStatic->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_HAND2)));

	const CString defaultOutputFolder = ShellUtils::GetSpecialFolderPath(CSIDL_MYDOCUMENTS);
	const CString defaultFileTemplate = _T("%b%d_%H%M");

	m_outputFolder = RegistryConfig::GetOption(_T("General\\OutputFolder"), defaultOutputFolder);
	m_outputFileTemplate = RegistryConfig::GetOption(_T("General\\OutputFileTemplate"), defaultFileTemplate);
	m_TrayIcon = RegistryConfig::GetOption(_T("General\\Show icon in tray"), FALSE);
	m_TrayMin  = RegistryConfig::GetOption(_T("General\\Minimize to tray"), FALSE);

	CPropertyPage::OnInitDialog(); //internally calls DoDataExchange
	OnEnChangeOutputFileTemplate();
	return TRUE;
}
//---------------------------------------------------------------------------

void CPageCom::OnBnClickedOutputFolder()
{
	if (ShellUtils::DialogPickupFolder(m_outputFolder))
		GetDlgItem(IDE_OUTPUT_FOLDER)->SetWindowTextA(m_outputFolder);
}
//---------------------------------------------------------------------------

void CPageCom::OnStnClickedDefaultTemplate()
{
	GetDlgItem(IDE_OUTPUT_FILE_TEMPLATE)->SetWindowTextA(_T("%b%d_%H%M"));
}
//---------------------------------------------------------------------------

void CPageCom::OnEnChangeOutputFileTemplate()
{
	CWnd* wndTemplate = (CWnd *)GetDlgItem(IDE_OUTPUT_FILE_TEMPLATE);
	CWnd* wndResult = (CWnd *)GetDlgItem(IDC_RESULT_NAME);

	CString strTemplate, strResult;
	wndTemplate->GetWindowText(strTemplate);
	strResult = GetAutoName(strTemplate, _T("mp3"));

	wndResult->SetWindowText(strResult);
}
//---------------------------------------------------------------------------

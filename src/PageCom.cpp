#include "stdafx.h"
#include "PageCom.h"
#include "ShellUtils.h"

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
	ON_BN_CLICKED(IDC_STORE_IN_OUTPUT_FOLDER, &CPageCom::OnBnClickedStoreInOutputFolder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CPageCom::CPageCom()
	:CPropertyPage(CPageCom::IDD)
	,m_TrayIcon(FALSE)
	,m_TrayMin(FALSE)
	,m_multipleInstances(FALSE)
	,m_storeInOutputFolder(FALSE)
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
	DDX_Check(pDX, IDC_GEN_MINSTANCES, m_multipleInstances);
	DDX_Check(pDX, IDC_STORE_IN_OUTPUT_FOLDER, m_storeInOutputFolder);
	DDX_Text(pDX, IDE_OUTPUT_FOLDER, m_outputFolder);
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
	RegistryConfig::SetOption(_T("General\\StoreInOutputFolder"), m_storeInOutputFolder);
	RegistryConfig::SetOption(_T("General\\Show icon in tray"), m_TrayIcon);
	RegistryConfig::SetOption(_T("General\\Minimize to tray"), m_TrayMin);
	RegistryConfig::SetOption(_T("General\\Multiple instances"), m_multipleInstances);

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
	const CString defaultOutputFolder = ShellUtils::GetSpecialFolderPath(CSIDL_DESKTOP);

	m_outputFolder = RegistryConfig::GetOption(_T("General\\OutputFolder"), defaultOutputFolder);
	m_storeInOutputFolder = RegistryConfig::GetOption(_T("General\\StoreInOutputFolder"), FALSE);
	m_TrayIcon = RegistryConfig::GetOption(_T("General\\Show icon in tray"), FALSE);
	m_TrayMin  = RegistryConfig::GetOption(_T("General\\Minimize to tray"), FALSE);
	m_multipleInstances = RegistryConfig::GetOption(_T("General\\Multiple instances"), FALSE);

	CPropertyPage::OnInitDialog(); //internally calls DoDataExchange
	OnBnClickedStoreInOutputFolder();
	return TRUE;
}
//---------------------------------------------------------------------------

void CPageCom::OnBnClickedOutputFolder()
{
	if (ShellUtils::DialogPickupFolder(m_outputFolder))
		SetDlgItemText(IDE_OUTPUT_FOLDER, m_outputFolder);
}
//---------------------------------------------------------------------------

void CPageCom::OnBnClickedStoreInOutputFolder()
{
	CButton* cbStore = static_cast<CButton*>(GetDlgItem(IDC_STORE_IN_OUTPUT_FOLDER));
	const bool doStore = cbStore->GetCheck() == BST_CHECKED;

	GetDlgItem(IDE_OUTPUT_FOLDER)->EnableWindow(doStore);
	GetDlgItem(IDB_OUTPUT_FOLDER)->EnableWindow(doStore);
}
//---------------------------------------------------------------------------

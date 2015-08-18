#include "stdafx.h"
#include "MP3_Recorder.h"
#include "PageCom.h"
#include "Config.h"
#include "ShellUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPageCom, CPropertyPage)

BEGIN_MESSAGE_MAP(CPageCom, CPropertyPage)
	//{{AFX_MSG_MAP(CPageCom)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDB_OUTPUT_FOLDER, &CPageCom::OnBnClickedOutputFolder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CPageCom::CPageCom()
	:CPropertyPage(CPageCom::IDD)
{
	//{{AFX_DATA_INIT(CPageCom)
	m_Loader	= FALSE;
	m_AutoRun	= FALSE;
	m_Minimized	= FALSE;
	m_ToolTipEnable = TRUE;
	m_TrayIcon = FALSE;
	m_TrayMin = FALSE;
	//m_MInstances = FALSE;
	//}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
 
CPageCom::~CPageCom()
{
}
//---------------------------------------------------------------------------

void CPageCom::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageCom)
	//DDX_Radio(pDX, IDR_OPT_BLANK,	m_Loader);
	//DDX_Check(pDX, IDC_AUTORUN,		m_AutoRun);
	//DDX_Check(pDX, IDC_MINIMIZED,	m_Minimized);
	//DDX_Check(pDX, IDC_TOOLTIP_ENABLE, m_ToolTipEnable);
	DDX_Check(pDX, IDC_TRAYICON, m_TrayIcon);
	DDX_Check(pDX, IDC_TRAYMIN, m_TrayMin);
	//DDX_Check(pDX, IDC_GEN_MINSTANCES, m_MInstances);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDE_OUTPUT_FOLDER, m_outputFolder);
	DDX_Text(pDX, IDE_OUTPUT_FILE_TEMPLATE, m_outputFileTemplate);
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

void CPageCom::SetConfig(CONF_DIAL_GEN* pconf_dial_gen)
{
	/*
	pconfig			= pconf_dial_gen;

	m_Loader		= pconfig->nLoader;
	m_AutoRun		= pconfig->bAutoRun;
	m_Minimized		= pconfig->bMinimized;
	m_ToolTipEnable	= pconfig->bToolTips;
	m_TrayIcon		= pconfig->bTrayIcon;
	m_TrayMin		= pconfig->bTrayMin;
	m_MInstances	= pconfig->bMInstances;
	*/
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
	const CString defaultOutputFolder = ShellUtils::GetSpecialFolderPath(CSIDL_MYDOCUMENTS);
	const CString defaultFileTemplate = _T("%b%d_%H%M");

	m_outputFolder = RegistryConfig::GetOption(_T("General\\OutputFolder"), defaultOutputFolder);
	m_outputFileTemplate = RegistryConfig::GetOption(_T("General\\OutputFileTemplate"), defaultFileTemplate);
	m_TrayIcon = RegistryConfig::GetOption(_T("General\\Show icon in tray"), FALSE);
	m_TrayMin  = RegistryConfig::GetOption(_T("General\\Minimize to tray"), FALSE);

	CPropertyPage::OnInitDialog(); //internally calls DoDataExchange
	return TRUE;
}
//---------------------------------------------------------------------------

void CPageCom::OnBnClickedOutputFolder()
{
	if (ShellUtils::DialogPickupFolder(m_outputFolder))
		GetDlgItem(IDE_OUTPUT_FOLDER)->SetWindowTextA(m_outputFolder);
}
//---------------------------------------------------------------------------

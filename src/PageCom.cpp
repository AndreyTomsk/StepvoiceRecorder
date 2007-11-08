/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "PageCom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CPageCom, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
CPageCom::CPageCom() : CPropertyPage(CPageCom::IDD)
{
	//{{AFX_DATA_INIT(CPageCom)
	m_Loader	= FALSE;
	m_AutoRun	= FALSE;
	m_Minimized	= FALSE;
	m_ToolTipEnable = FALSE;
	m_TrayIcon = FALSE;
	m_TrayMin = FALSE;
	m_MInstances = FALSE;
	//}}AFX_DATA_INIT

	pconfig = NULL;
}
 
CPageCom::~CPageCom()
{
}

/////////////////////////////////////////////////////////////////////////////
void CPageCom::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageCom)
	DDX_Radio(pDX, IDR_OPT_BLANK,	m_Loader);
	DDX_Check(pDX, IDC_AUTORUN,		m_AutoRun);
	DDX_Check(pDX, IDC_MINIMIZED,	m_Minimized);
	DDX_Check(pDX, IDC_TOOLTIP_ENABLE, m_ToolTipEnable);
	DDX_Check(pDX, IDC_TRAYICON, m_TrayIcon);
	DDX_Check(pDX, IDC_TRAYMIN, m_TrayMin);
	DDX_Check(pDX, IDC_GEN_MINSTANCES, m_MInstances);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
void CPageCom::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pconfig) {
		pconfig->nLoader	= m_Loader;
		pconfig->bAutoRun	= m_AutoRun;
		pconfig->bMinimized	= m_Minimized;
		pconfig->bToolTips	= m_ToolTipEnable;
		//pconfig->nShowIn	= m_ShowIn;
		pconfig->bTrayIcon	= m_TrayIcon;
		pconfig->bTrayMin	= m_TrayMin;
		pconfig->bMInstances = m_MInstances;
	}
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPageCom, CPropertyPage)
	//{{AFX_MSG_MAP(CPageCom)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CPageCom::SetConfig(CONF_DIAL_GEN* pconf_dial_gen)
{
	pconfig			= pconf_dial_gen;

	m_Loader		= pconfig->nLoader;
	m_AutoRun		= pconfig->bAutoRun;
	m_Minimized		= pconfig->bMinimized;
	m_ToolTipEnable	= pconfig->bToolTips;
	//m_ShowIn		= pconfig->nShowIn;
	m_TrayIcon		= pconfig->bTrayIcon;
	m_TrayMin		= pconfig->bTrayMin;
	m_MInstances	= pconfig->bMInstances;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPageCom::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTEXTHELP;
	return CPropertyPage::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPageCom::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

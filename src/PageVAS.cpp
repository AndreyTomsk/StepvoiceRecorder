//
#include "stdafx.h"
#include "mp3_recorder.h"
#include "MainFrm.h"
#include "PageVAS.h"
#include <algorithm>
#include <htmlhelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
const int VAS_THRESHOLD[] = {-54, -48, -42, -36, -30, -24, -18, -12, -6};
//const int VAS_WAITTIME[]  = {500, 750, 1000, 1500, 2000, 3000, 4000, 6000, 8000};
//const int VAS_THRESHOLD[] = {-54, -42, -30, -18, -9};
const int VAS_WAITTIME[]  = {500, 1000, 2000, 4000, 8000};

/////////////////////////////////////////////////////////////////////////////
// CPageVAS property page
IMPLEMENT_DYNCREATE(CPageVAS, CPropertyPage)

CPageVAS::CPageVAS() : CPropertyPage(CPageVAS::IDD)
{
	m_nThreshold = 0;
	m_nWaitTime	 = 0;

	m_FontWarn.CreateFont(12, 0, 0, 0, FW_LIGHT,  0, 0, 0, 0, 0, 0, 0, 0,
		"Arial");

	//{{AFX_DATA_INIT(CPageVAS)
	m_bEnable = FALSE;
	m_nAction = -1;
	//}}AFX_DATA_INIT
}

CPageVAS::~CPageVAS()
{
}

void CPageVAS::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageVAS)
	DDX_Check(pDX, IDC_VAS_ENABLE, m_bEnable);
	DDX_Radio(pDX, IDC_VAS_SILENTPAUSE, m_nAction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageVAS, CPropertyPage)
	//{{AFX_MSG_MAP(CPageVAS)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
#pragma optimize ("", off)
BOOL CPageVAS::OnInitDialog() 
{
	CSliderCtrl* pSlider;
	int nSizeT = sizeof(VAS_THRESHOLD)/sizeof(int);
	int nSizeW = sizeof(VAS_WAITTIME)/sizeof(int);

	CPropertyPage::OnInitDialog();
	
	// настраиваем параметры слайдера уровня
	pSlider = (CSliderCtrl *)GetDlgItem(IDC_VAS_THRESHOLD);
	pSlider->SetRange(0, nSizeT-1);
	pSlider->SetLineSize(1);
	pSlider->SetPageSize(1);
	pSlider->SetPos(m_nThreshold);

	// настраиваем параметры слайдера времени
	pSlider = (CSliderCtrl *)GetDlgItem(IDC_VAS_TIME);
	pSlider->SetRange(0, nSizeW-1);
	pSlider->SetLineSize(1);
	pSlider->SetPageSize(1);
	pSlider->SetPos(m_nWaitTime);

#ifndef _DEBUG
	CStatic* pTrialNote = (CStatic *)GetDlgItem(IDC_DLG_TRIALNOTE);

	// дизаблим VAS после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0) {	
		CButton *pBtnEnable = (CButton *)GetDlgItem(IDC_VAS_ENABLE);
		pBtnEnable->EnableWindow(false);
		m_bEnable = false;
		pTrialNote->ModifyStyle(0, WS_VISIBLE);
		pTrialNote->Invalidate(false);
	}

	// убираем триальную надпись в зарегистрированной версии
	REG_CRYPT_BEGIN;
	pTrialNote->ModifyStyle(WS_VISIBLE, 0);
	pTrialNote->Invalidate(false);
	REG_CRYPT_END;
#endif
	// меняем курсор
	CStatic* pStatic = (CStatic *)GetDlgItem(IDC_DLG_TRIALNOTE);
	SetClassLong(pStatic->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_HAND2))); //вместо IDC_HAND

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#pragma optimize ("", on)
/////////////////////////////////////////////////////////////////////////////
void CPageVAS::OnOK() 
{
	CSliderCtrl *pSlider1, *pSlider2;

	pSlider1 = (CSliderCtrl *)GetDlgItem(IDC_VAS_THRESHOLD);
	pSlider2 = (CSliderCtrl *)GetDlgItem(IDC_VAS_TIME);

#ifndef _DEBUG
	// дизаблим VAS после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		m_bEnable = false;
#endif

	m_pConfig->bEnable	  = m_bEnable;
	m_pConfig->nAction	  = m_nAction;
	m_pConfig->nThreshold = VAS_THRESHOLD[pSlider1->GetPos()];
	m_pConfig->nWaitTime  = VAS_WAITTIME[pSlider2->GetPos()];

	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
void CPageVAS::SetConfig(CONF_DIAL_VAS *pconf_dial_vas)
{
	int nSizeT = sizeof(VAS_THRESHOLD)/sizeof(int);
	int nSizeW = sizeof(VAS_WAITTIME)/sizeof(int);

	m_pConfig = pconf_dial_vas;
	m_bEnable = pconf_dial_vas->bEnable;
	m_nAction = pconf_dial_vas->nAction;

	m_nThreshold = int(std::find(VAS_THRESHOLD, VAS_THRESHOLD+nSizeT, m_pConfig->nThreshold) - VAS_THRESHOLD);
	m_nThreshold = (m_nThreshold < nSizeT) ? m_nThreshold : 0;

	m_nWaitTime = int(std::find(VAS_WAITTIME, VAS_WAITTIME+nSizeW, m_pConfig->nWaitTime) - VAS_WAITTIME);
	m_nWaitTime	= (m_nWaitTime < nSizeW) ? m_nWaitTime : 0;

#ifndef _DEBUG
	// дизаблим VAS после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		m_bEnable = false;
#endif
}

/////////////////////////////////////////////////////////////////////////////

HBRUSH CPageVAS::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (pWnd->m_hWnd == GetDlgItem(IDC_DLG_TRIALNOTE)->m_hWnd)
	{
		pDC->SelectObject(&m_FontWarn);
		pDC->SetTextColor(RGB(128, 0, 0));
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CPageVAS::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// меняем курсор на стандартный
	CStatic* pStatic = (CStatic *)GetDlgItem(IDC_DLG_TRIALNOTE);
	SetClassLong(pStatic->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ARROW))); //вместо IDC_HAND
}

BOOL CPageVAS::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());
	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

BOOL CPageVAS::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	int nCode = HIWORD(wParam);

	if(nID == IDC_DLG_TRIALNOTE && (nCode == BN_CLICKED || nCode == BN_DOUBLECLICKED))
	{
		// отображаем "How To Order" страницу
		CMainFrame* pFrame = (CMainFrame *)AfxGetMainWnd();
		CString strFile = pFrame->GetProgramDir();
		strFile += "\\svrec.chm::/order.html";
		::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
	}
	
	return CPropertyPage::OnCommand(wParam, lParam);
}

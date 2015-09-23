#include "stdafx.h"
#include "PageVAS.h"
#include "FileUtils.h"
#include <algorithm>
#include <htmlhelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CPageVAS, CPropertyPage)

BEGIN_MESSAGE_MAP(CPageVAS, CPropertyPage)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

const int VAS_THRESHOLD[] = {-54, -48, -42, -36, -30, -24, -18, -12, -6};
const int VAS_WAITTIME[]  = {500, 1000, 2000, 4000, 8000};

/////////////////////////////////////////////////////////////////////////////

CPageVAS::CPageVAS()
	:CPropertyPage(CPageVAS::IDD)
	,m_thresholdIndex(0)
	,m_delayIndex(0)
	,m_enable(FALSE)
	,m_action(0)
{
	m_FontWarn.CreateFont(12, 0, 0, 0, FW_LIGHT,  0, 0, 0, 0, 0, 0, 0, 0,
		"Arial");
}
//---------------------------------------------------------------------------

CPageVAS::~CPageVAS()
{
}
//---------------------------------------------------------------------------

void CPageVAS::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_VAS_ENABLE, m_enable);
	DDX_Radio(pDX, IDC_VAS_SILENTPAUSE, m_action);
}
//---------------------------------------------------------------------------

#pragma optimize ("", off)
BOOL CPageVAS::OnInitDialog() 
{
	m_enable = RegistryConfig::GetOption(_T("Tools\\VAS\\Enable"), 0);
	m_action = RegistryConfig::GetOption(_T("Tools\\VAS\\Action"), 0);
	const int configTreshold = RegistryConfig::GetOption(_T("Tools\\VAS\\Threshold"), -30);
	const int configWaitTime = RegistryConfig::GetOption(_T("Tools\\VAS\\WaitTime"), 2000);

	const int SIZE_T = sizeof(VAS_THRESHOLD)/sizeof(int);
	const int SIZE_W = sizeof(VAS_WAITTIME)/sizeof(int);

	m_thresholdIndex = int(std::find(VAS_THRESHOLD, VAS_THRESHOLD+SIZE_T, configTreshold) - VAS_THRESHOLD);
	m_thresholdIndex = (m_thresholdIndex >= 0 && m_thresholdIndex < SIZE_T) ? m_thresholdIndex : 0;

	m_delayIndex = int(std::find(VAS_WAITTIME, VAS_WAITTIME+SIZE_W, configWaitTime) - VAS_WAITTIME);
	m_delayIndex = (m_delayIndex >= 0 && m_delayIndex < SIZE_W) ? m_delayIndex : 0;

	CPropertyPage::OnInitDialog(); //internally calls DoDataExchange


	// настраиваем параметры слайдера уровня
	CSliderCtrl* pSlider = (CSliderCtrl *)GetDlgItem(IDC_VAS_THRESHOLD);
	pSlider->SetRange(0, SIZE_T-1);
	pSlider->SetLineSize(1);
	pSlider->SetPageSize(1);
	pSlider->SetPos(m_thresholdIndex);

	// настраиваем параметры слайдера времени
	pSlider = (CSliderCtrl *)GetDlgItem(IDC_VAS_TIME);
	pSlider->SetRange(0, SIZE_W-1);
	pSlider->SetLineSize(1);
	pSlider->SetPageSize(1);
	pSlider->SetPos(m_delayIndex);

#ifndef _DEBUG
	CStatic* pTrialNote = (CStatic *)GetDlgItem(IDC_DLG_TRIALNOTE);

	// дизаблим VAS после завершения триального периода
	if (fsProtect_GetDaysLeft() <= 0)
	{	
		CButton *pBtnEnable = (CButton *)GetDlgItem(IDC_VAS_ENABLE);
		pBtnEnable->EnableWindow(false);
		m_enable = false;
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
	SetClassLong(GetDlgItem(IDC_DLG_TRIALNOTE)->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_HAND2)));

	return TRUE;
}
#pragma optimize ("", on)
//---------------------------------------------------------------------------

void CPageVAS::OnOK() 
{
#ifndef _DEBUG
	// дизаблим VAS после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		m_enable = false;
#endif

	CSliderCtrl* pSlider1 = static_cast<CSliderCtrl*>(GetDlgItem(IDC_VAS_THRESHOLD));
	CSliderCtrl* pSlider2 = static_cast<CSliderCtrl*>(GetDlgItem(IDC_VAS_TIME));

	RegistryConfig::SetOption(_T("Tools\\VAS\\Enable"),   m_enable);
	RegistryConfig::SetOption(_T("Tools\\VAS\\Action"),   m_action);
	RegistryConfig::SetOption(_T("Tools\\VAS\\Threshold"),VAS_THRESHOLD[pSlider1->GetPos()]);
	RegistryConfig::SetOption(_T("Tools\\VAS\\WaitTime"), VAS_WAITTIME[pSlider2->GetPos()]);

	CPropertyPage::OnOK();
}
//---------------------------------------------------------------------------

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
//---------------------------------------------------------------------------

void CPageVAS::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	// меняем курсор на стандартный
	SetClassLong(GetDlgItem(IDC_DLG_TRIALNOTE)->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ARROW)));
}
//---------------------------------------------------------------------------

BOOL CPageVAS::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
//---------------------------------------------------------------------------

BOOL CPageVAS::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	int nCode = HIWORD(wParam);

	if (nID == IDC_DLG_TRIALNOTE && (nCode == BN_CLICKED || nCode == BN_DOUBLECLICKED))
	{
		// отображаем "How To Order" страницу
		CString strFile = FileUtils::GetProgramFolder();
		strFile += _T("\\svrec.chm::/stepvoice_recorder/how_to_register.html");
		::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
	}
	
	return CPropertyPage::OnCommand(wParam, lParam);
}
//---------------------------------------------------------------------------

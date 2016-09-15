#include "stdafx.h"
#include "PageVAS.h"
#include "FileUtils.h"
#include "Utility.h"
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
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

const int VAS_THRESHOLD[] = {-54, -48, -42, -36, -30, -24, -18, -12, -6};
const int VAS_WAITTIME[]  = {500, 1000, 2000, 4000, 8000};

/////////////////////////////////////////////////////////////////////////////

CPageVAS::CPageVAS()
	:CPropertyPage(CPageVAS::IDD)
	,m_thresholdIndex(0)
	,m_delayIndex(0)
	,m_action(0)
{
}
//---------------------------------------------------------------------------

CPageVAS::~CPageVAS()
{
}
//---------------------------------------------------------------------------

void CPageVAS::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_VAS_SILENTPAUSE, m_action);
}
//---------------------------------------------------------------------------

BOOL CPageVAS::OnInitDialog() 
{
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

	return TRUE;
}
//---------------------------------------------------------------------------

void CPageVAS::OnOK() 
{
	CSliderCtrl* pSlider1 = static_cast<CSliderCtrl*>(GetDlgItem(IDC_VAS_THRESHOLD));
	CSliderCtrl* pSlider2 = static_cast<CSliderCtrl*>(GetDlgItem(IDC_VAS_TIME));

	RegistryConfig::SetOption(_T("Tools\\VAS\\Action"),   m_action);
	RegistryConfig::SetOption(_T("Tools\\VAS\\Threshold"),VAS_THRESHOLD[pSlider1->GetPos()]);
	RegistryConfig::SetOption(_T("Tools\\VAS\\WaitTime"), VAS_WAITTIME[pSlider2->GetPos()]);

	CPropertyPage::OnOK();
}
//---------------------------------------------------------------------------

BOOL CPageVAS::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
//---------------------------------------------------------------------------

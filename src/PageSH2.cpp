// PageSH2.cpp : implementation file
//

#include "stdafx.h"
#include "PageSH2.h"
#include "MainFrm.h"
#include "ShellUtils.h"
#include <htmlhelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageSH2 property page

IMPLEMENT_DYNCREATE(CPageSH2, CPropertyPage)

CPageSH2::CPageSH2() : CPropertyPage(CPageSH2::IDD)
{
	m_ctStart = m_ctStop = m_ctRecTime = CTime(2004, 1, 1, 0, 0, 0);
	m_pConfig = NULL;
	m_strFullName = "";

	m_FontWarn.CreateFont(12, 0, 0, 0, FW_LIGHT,  0, 0, 0, 0, 0, 0, 0, 0,
		"Arial");

	//{{AFX_DATA_INIT(CPageSH2)
	m_bSchedStart = FALSE;
	m_nSchedStopID = 0;
	m_bRunExternal = FALSE;
	m_bActShutDown = FALSE;
	m_bActCloseAll = FALSE;
	m_bEnableSHR = FALSE;
	//}}AFX_DATA_INIT
}

CPageSH2::~CPageSH2()
{
}

void CPageSH2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSH2)
	DDX_Check(pDX, IDC_SH2_ENABLESTART, m_bSchedStart);
	DDX_Radio(pDX, IDC_SH2_BTN_STOPTIME, m_nSchedStopID);
	DDX_Check(pDX, IDC_SH2_RUNEXTERNAL, m_bRunExternal);
	DDX_Check(pDX, IDC_SH2_BTN_SHUTDOWN, m_bActShutDown);
	DDX_Check(pDX, IDC_SH2_BTN_CLOSEALL, m_bActCloseAll);
	DDX_Check(pDX, IDC_SH2_ENABLESHR, m_bEnableSHR);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageSH2, CPropertyPage)
	//{{AFX_MSG_MAP(CPageSH2)
	ON_BN_CLICKED(IDC_SH2_ENABLESTART, OnSh2EnableStart)
	ON_BN_CLICKED(IDC_SH2_RUNEXTERNAL, OnSh2RunExternal)
	ON_BN_CLICKED(IDC_SH2_SELEXTERNAL, OnSh2SelectExternal)
	ON_BN_CLICKED(IDC_SH2_BTN_STOPTIME, OnSh2BtnStopTime)
	ON_BN_CLICKED(IDC_SH2_BTN_RECTIME, OnSh2BtnRecTime)
	ON_EN_CHANGE(IDC_SH2_EDITFILENAME,   OnChangeSh2EditFileName)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_EN_SETFOCUS(IDC_SH2_EDITFILENAME, OnChangeSh2EditFileName)
	ON_EN_UPDATE(IDC_SH2_EDITFILENAME, OnChangeSh2EditFileName)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
#pragma optimize ("", off)
BOOL CPageSH2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	CButton* pBtnSelExternal= (CButton *)GetDlgItem(IDC_SH2_SELEXTERNAL);
	pBtnSelExternal->SetIcon( LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_OPEN2)) );
	// хитрая техника для кнопки выбора файла
	//CRgn cRgn;
	//CRect cRect;
	//pBtnSelExternal->GetWindowRect(&cRect);
	//cRgn.CreateRectRgn(2, 0, cRect.right, cRect.bottom-cRect.top);
	//pBtnSelExternal->SetWindowRgn(cRgn, true);

	// задаем формат вывода времени в элементах
	CDateTimeCtrl* pRecStartCtrl = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTART);
	CDateTimeCtrl* pRecStopCtrl  = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTOP);
	CDateTimeCtrl* pRecTimeCtrl  = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPRECTIME);
	DateTime_SetFormat(pRecStartCtrl->GetSafeHwnd(), "HH':'mm':'ss");
	DateTime_SetFormat(pRecTimeCtrl->GetSafeHwnd(),  "HH':'mm':'ss");
	DateTime_SetFormat(pRecStopCtrl->GetSafeHwnd(),  "HH':'mm':'ss");

	pRecStartCtrl->SetTime(&m_ctStart);
	pRecStopCtrl->SetTime(&m_ctStop);
	pRecTimeCtrl->SetTime(&m_ctRecTime);

	// обновляем вид окон (дизаблим ненужные)
	UpdateStartSection();
	UpdateStopSection();
	UpdateFileNameText();

#ifndef _DEBUG
	CStatic* pTrialNote = (CStatic *)GetDlgItem(IDC_DLG_TRIALNOTE);

	// дизаблим шедулер после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0) {	
		CButton *pBtnEnable = (CButton *)GetDlgItem(IDC_SH2_ENABLESHR);
		pBtnEnable->EnableWindow(false);
		m_bEnableSHR = false;
		pTrialNote->ModifyStyle(0, WS_VISIBLE);
		pTrialNote->Invalidate(false);
	}

	// убираем триальную надпись в зарегистрированной версии
	REG_CRYPT_BEGIN;
	pTrialNote->ModifyStyle(WS_VISIBLE, 0);
	pTrialNote->Invalidate(false);
	REG_CRYPT_END;
#endif

	// изменяем курсор в группе template codes
	CStatic* pStatic = (CStatic *)GetDlgItem(IDC_DLG_TRIALNOTE);
	SetClassLong(pStatic->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_HAND2))); //вместо IDC_HAND

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#pragma optimize ("", on)
/////////////////////////////////////////////////////////////////////////////
void CPageSH2::OnOK() 
{
	ASSERT(m_pConfig!=NULL);

	CDateTimeCtrl* pRecStartCtrl = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTART);
	CDateTimeCtrl* pRecStopCtrl  = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTOP);
	CDateTimeCtrl* pRecTimeCtrl  = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPRECTIME);
	CEdit* pFileNameText  = (CEdit *)GetDlgItem(IDC_SH2_EDITFILENAME);

	pRecStartCtrl->GetTime(m_ctStart);
	pRecStopCtrl->GetTime(m_ctStop);
	pRecTimeCtrl->GetTime(m_ctRecTime);
	pFileNameText->GetWindowText(m_strFullName);

	m_pConfig->t_start.h = m_ctStart.GetHour();
	m_pConfig->t_start.m = m_ctStart.GetMinute();
	m_pConfig->t_start.s = m_ctStart.GetSecond();
	m_pConfig->t_stop.h = m_ctStop.GetHour();
	m_pConfig->t_stop.m = m_ctStop.GetMinute();
	m_pConfig->t_stop.s = m_ctStop.GetSecond();
	m_pConfig->t_rec.h = m_ctStop.GetHour();
	m_pConfig->t_rec.m = m_ctStop.GetMinute();
	m_pConfig->t_rec.s = m_ctStop.GetSecond();
	// !!!!!
	//m_pConfig->t_rec.h = m_ctRecTime.GetHour();
	//m_pConfig->t_rec.m = m_ctRecTime.GetMinute();
	//m_pConfig->t_rec.s = m_ctRecTime.GetSecond();

#ifndef _DEBUG
	// дизаблим шедулер после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		m_bEnableSHR = false;
#endif

	m_pConfig->bIsEnabled		= m_bEnableSHR;
	m_pConfig->bSchedStart		= m_bSchedStart;
	m_pConfig->bRunExternal		= m_bRunExternal;
	m_pConfig->strFileName		= m_strFullName;
	m_pConfig->nStopByID		= m_nSchedStopID;
	m_pConfig->action.shutDown	= m_bActShutDown;
	m_pConfig->action.closeAll	= m_bActCloseAll;
	
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPageSH2::OnKillActive() 
{
	this->UpdateData(true);

	CDateTimeCtrl* pRecStartCtrl = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTART);
	CDateTimeCtrl* pRecStopCtrl  = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTOP);
	CDateTimeCtrl* pRecTimeCtrl  = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPRECTIME);
	CString strNotSame("Start and Stop time must be different.");
	CString strNotZero("Please change the Recording time.");

	pRecStartCtrl->GetTime(m_ctStart);
	pRecStopCtrl->GetTime(m_ctStop);	
	// !!!!!
	//pRecTimeCtrl->GetTime(m_ctRecTime);

	// обр. ошибки - время начала и конца записи не м.б. одинаковым
	if(m_bSchedStart && (m_nSchedStopID == 0) && (m_ctStart == m_ctStop))
	{
		AfxMessageBox(strNotSame);
		pRecStopCtrl->SetFocus();
		return 0;
	}

	// !!!!! обр. ошибки - длительность записи не м.б. равна 0
	//if((m_nSchedStopID == 1) && (m_ctRecTime.GetHour() == 0) &&
	//	(m_ctRecTime.GetMinute() == 0) && (m_ctRecTime.GetSecond() == 0))
	if((m_nSchedStopID == 1) && (m_ctStop.GetHour() == 0) &&
		(m_ctStop.GetMinute() == 0) && (m_ctStop.GetSecond() == 0))
	{
		AfxMessageBox(strNotZero);
		//pRecTimeCtrl->SetFocus();
		pRecStopCtrl->SetFocus();
		return 0;
	}

	return CPropertyPage::OnKillActive();
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::OnSh2EnableStart() 
{
	UpdateStartSection();
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::OnSh2RunExternal() 
{
	UpdateRunSection();
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::OnSh2SelectExternal() 
{
	this->SetFocus();

	// получаем директорию запускаемого файла (если есть)
	CString strFileDir = m_strFullName;
	if(!strFileDir.IsEmpty()) {
		strFileDir = strFileDir.Left(strFileDir.ReverseFind('\\'));
	}

	// вызываем стандартный диалог для выбора имени файла
	CFileDialog SelectDlg(
		true,
		NULL,
		m_strFullName,
		OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		"All files (*.*)|*.*||");
	SelectDlg.m_ofn.lpstrInitialDir = strFileDir;
	if(SelectDlg.DoModal() == IDOK) {
		m_strFullName = SelectDlg.GetPathName();
	}
	
	// обновляем тест имени вызываемого файла
	UpdateFileNameText();
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::OnSh2BtnStopTime() 
{
	UpdateStopSection();
}

void CPageSH2::OnSh2BtnRecTime() 
{
	UpdateStopSection();
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::UpdateStartSection()
{
	bool bIsEnabled;
	CStatic* pStartText = (CStatic *)GetDlgItem(IDC_SH2_STARTTEXT);
	CDateTimeCtrl* pStartCtrl = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTART);
	CButton* pRunExternal = (CButton *)GetDlgItem(IDC_SH2_RUNEXTERNAL);
	//CButton* pBtnSelExternal= (CButton *)GetDlgItem(IDC_SH2_SELEXTERNAL);

	bIsEnabled = IsDlgButtonChecked(IDC_SH2_ENABLESTART) != 0;

	pStartText->EnableWindow(bIsEnabled);
	pStartCtrl->EnableWindow(bIsEnabled);
	pRunExternal->EnableWindow(bIsEnabled);
	//pBtnSelExternal->EnableWindow(bIsEnabled);

	UpdateRunSection();
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::UpdateRunSection()
{
	bool bIsEnabled;
	CButton* pBtnSelExternal = (CButton *)GetDlgItem(IDC_SH2_SELEXTERNAL);
	CEdit* pFileNameText  = (CEdit *)GetDlgItem(IDC_SH2_EDITFILENAME);

	bIsEnabled = IsDlgButtonChecked(IDC_SH2_ENABLESTART)&&
		IsDlgButtonChecked(IDC_SH2_RUNEXTERNAL);

	pFileNameText->EnableWindow(bIsEnabled);
	pBtnSelExternal->EnableWindow(bIsEnabled);
	pBtnSelExternal->InvalidateRect(NULL); // перерисовываем кнопку (лев. край)
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::UpdateStopSection()
{
/*	// !!!!!
	bool bIsEnabled;
	CDateTimeCtrl* pStopCtrl = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPSTOP);
	CDateTimeCtrl* pRecTCtrl = (CDateTimeCtrl *)GetDlgItem(IDC_SH2_TPRECTIME);

	bIsEnabled = IsDlgButtonChecked(IDC_SH2_BTN_STOPTIME) != 0;

	pStopCtrl->EnableWindow(bIsEnabled);
	pRecTCtrl->EnableWindow(!bIsEnabled);
*/
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::UpdateFileNameText()
{
	CEdit* pFileNameText  = (CEdit *)GetDlgItem(IDC_SH2_EDITFILENAME);

	// выводим текст
	pFileNameText->SetWindowText(m_strFullName);
	pFileNameText->LineScroll(0, pFileNameText->LineLength());
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::OnChangeSh2EditFileName() 
{
	// TODO: Add your control notification handler code here
	CButton* pBtnSelExternal = (CButton *)GetDlgItem(IDC_SH2_SELEXTERNAL);
	pBtnSelExternal->InvalidateRect(NULL);
}

/////////////////////////////////////////////////////////////////////////////
void CPageSH2::SetConfig(CONF_DIAL_SH2 *pConfig)
{
	struct tm tmStart, tmStop, tmRecTime;
	
	m_ctStart.GetLocalTm(&tmStart);
	m_ctStop.GetLocalTm(&tmStop);
	m_ctRecTime.GetLocalTm(&tmRecTime);

	tmStart.tm_hour	= pConfig->t_start.h;
	tmStart.tm_min	= pConfig->t_start.m;
	tmStart.tm_sec	= pConfig->t_start.s;

	tmStop.tm_hour	= pConfig->t_stop.h;
	tmStop.tm_min	= pConfig->t_stop.m;
	tmStop.tm_sec	= pConfig->t_stop.s;

	tmRecTime.tm_hour = pConfig->t_stop.h;
	tmRecTime.tm_min  = pConfig->t_stop.m;
	tmRecTime.tm_sec  = pConfig->t_stop.s;

	// !!!!!
	//tmRecTime.tm_hour = pConfig->t_rec.h;
	//tmRecTime.tm_min  = pConfig->t_rec.m;
	//tmRecTime.tm_sec  = pConfig->t_rec.s;

	m_ctStart	= mktime(&tmStart);
	m_ctStop	= mktime(&tmStop);
	m_ctRecTime	= mktime(&tmRecTime);

	m_bEnableSHR  = pConfig->bIsEnabled;
	m_bSchedStart = pConfig->bSchedStart;
	m_bRunExternal= pConfig->bRunExternal;
	m_strFullName = pConfig->strFileName;
	m_nSchedStopID= pConfig->nStopByID;
	m_bActShutDown= pConfig->action.shutDown;
	m_bActCloseAll= pConfig->action.closeAll;

	m_pConfig = pConfig; // сохр. указатель для возможности изм. настроек

#ifndef _DEBUG
	// дизаблим шедулер после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		m_bEnableSHR = false;
#endif
}

/////////////////////////////////////////////////////////////////////////////


HBRUSH CPageSH2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

void CPageSH2::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
	CStatic* pStatic = (CStatic *)GetDlgItem(IDC_DLG_TRIALNOTE);
	SetClassLong(pStatic->GetSafeHwnd(), GCL_HCURSOR,
		(LONG)LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ARROW))); //вместо IDC_HAND	
}

BOOL CPageSH2::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());

	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

BOOL CPageSH2::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	UINT nID = LOWORD(wParam);
	int nCode = HIWORD(wParam);

	if(nID == IDC_DLG_TRIALNOTE && (nCode == BN_CLICKED || nCode == BN_DOUBLECLICKED))
	{
		// отображаем "How To Order" страницу
		CString strFile = ShellUtils::GetProgramFolder();
		strFile += _T("\\svrec.chm::/stepvoice_recorder/how_to_register.html");
		::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
	}
	
	return CPropertyPage::OnCommand(wParam, lParam);
}

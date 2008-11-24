// TimerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mp3_recorder.h"
#include "TimerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
#define DLG_TIMER_ID 998

/////////////////////////////////////////////////////////////////////////////
// CTimerDlg dialog
CTimerDlg::CTimerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_tdi.strCaption = "Stepvoice Recorder";
	m_tdi.strDlgText = "";
	m_tdi.strBtnText = "&OK";
	m_tdi.nCloseTime = 30;
	m_nSeconds = 0;
}

BEGIN_MESSAGE_MAP(CTimerDlg, CDialog)
	//{{AFX_MSG_MAP(CTimerDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerDlg message handlers
BOOL CTimerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString  strBtnText;
	CStatic* pDlgText  = (CStatic *)GetDlgItem(IDC_TDLG_TEXT);
	CStatic* pDlgIcon  = (CStatic *)GetDlgItem(IDC_TDLG_ICON);
	CButton* pBtnClose = (CButton *)GetDlgItem(IDOK);

	// устанавливаем вид диалога
	this->SetWindowText(m_tdi.strCaption);
	pDlgText->SetWindowText(m_tdi.strDlgText);
	strBtnText.Format("%s (%d)", m_tdi.strBtnText, m_tdi.nCloseTime);
	pBtnClose->SetWindowText(strBtnText);
	pDlgIcon->SetIcon(LoadIcon(NULL, IDI_INFORMATION));

	// делаем окно поверх всех
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);

	// устанавливаем таймер
	m_nSeconds = m_tdi.nCloseTime;
	SetTimer(DLG_TIMER_ID, 1000, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CTimerDlg::OnTimer(UINT nIDEvent) 
{
	CButton* pBtnClose = (CButton *)GetDlgItem(IDOK);
	CString  strBtnText;

	// уменьшаем счетчик, если время кончилось - закрываем диалог
	m_nSeconds -= 1;
	if(m_nSeconds == 0)
		this->PostMessage(WM_COMMAND, IDOK, LONG(pBtnClose->m_hWnd));

	// изменяем текст на кнопке вида: OK(29)
	strBtnText.Format("%s (%d)", m_tdi.strBtnText, m_nSeconds);
	pBtnClose->SetWindowText(strBtnText);
	
	CDialog::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
void CTimerDlg::OnDestroy() 
{
	CDialog::OnDestroy();	
	KillTimer(DLG_TIMER_ID);	
}

/////////////////////////////////////////////////////////////////////////////
//

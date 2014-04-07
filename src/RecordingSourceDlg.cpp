/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "mp3_recorder.h"
#include "RecordingSourceDlg.h"
#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRecordingSourceDlg, CDialog)
  //{{AFX_MSG_MAP(CRecordingSourceDlg)
  ON_WM_CTLCOLOR()
  //}}AFX_MSG_MAP
  ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CRecordingSourceDlg* g_dialogInstance = NULL;

/////////////////////////////////////////////////////////////////////////////

CRecordingSourceDlg::CRecordingSourceDlg(CWnd* pParent /*=NULL*/)
  :CDialog(CRecordingSourceDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CRecordingSourceDlg)
  //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------

/*static*/ void CRecordingSourceDlg::Execute(CWnd* parent)
{
	if (!g_dialogInstance)
	{
		g_dialogInstance = new CRecordingSourceDlg(parent);
		g_dialogInstance->Create(CRecordingSourceDlg::IDD);
	}
	g_dialogInstance->ShowWindow(SW_SHOW);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::PostNcDestroy()
{
	g_dialogInstance = NULL;
	CDialog::PostNcDestroy();
	delete this;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CRecordingSourceDlg)
  //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceDlg::OnInitDialog() 
{
  CDialog::OnInitDialog();
  return TRUE;
}
//---------------------------------------------------------------------------

HBRUSH CRecordingSourceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
/*
  std::map<DWORD, DWORD> controlsMap;
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_1, COLOR_MENU));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_2, COLOR_MENUHILIGHT));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_3, COLOR_MENUBAR));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_4, COLOR_DESKTOP));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_5, COLOR_MENUTEXT));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_6, COLOR_GRAYTEXT));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_7, COLOR_HIGHLIGHTTEXT));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_8, COLOR_HIGHLIGHT));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_9, COLOR_HOTLIGHT));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_10,COLOR_INACTIVEBORDER));
  controlsMap.insert(std::pair<DWORD,DWORD>(IDC_STATIC_11,COLOR_INFOBK));
*/
/*
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_1)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_MENU));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_2)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_MENUHILIGHT));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_3)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_MENUBAR));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_4)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_DESKTOP));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_5)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_MENUTEXT));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_6)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_GRAYTEXT));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_7)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_8)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_9)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_HOTLIGHT));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_10)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_INACTIVEBORDER));
	else
	if (pWnd->m_hWnd == GetDlgItem(IDC_STATIC_11)->m_hWnd)
		pDC->SetBkColor(GetSysColor(COLOR_INFOBK));
*/

	/*
	if (nCtlColor == CTLCOLOR_DLG)
	{
		pDC->MoveTo(0, 0);
		pDC->LineTo(200, 200);

		for (int i = 0; i < 30; i++)
		  pDC->FillSolidRect(0, i*20, 100, 20, GetSysColor(i));
	}
	*/
	return hbr;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	for (int i = 0; i < 30; i++)
	  dc.FillSolidRect(0, i*15, 100, 15, GetSysColor(i));
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//Hiding dialog when changing focus to other windows (similar to menus)
	if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE)
	{
		this->ShowWindow(SW_HIDE);
		this->GetParent()->PostMessage(WM_RECSOURCE_DLGCLOSED);
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}
//---------------------------------------------------------------------------

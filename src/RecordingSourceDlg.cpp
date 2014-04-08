/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include <vssym32.h>
#include <Uxtheme.h>
#include "mp3_recorder.h"
#include "RecordingSourceDlg.h"
#include <map>

#pragma comment(lib, "UxTheme.lib")

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
  ON_BN_CLICKED(IDC_BUTTON1, &CRecordingSourceDlg::OnBnClickedButton1)
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

	//HTHEME hTheme = GetWindowTheme(this->GetSafeHwnd());
	//for (int i = 0; i < 30; i++)
	//{
	//	dc.FillSolidRect(120, i*15, 100, 15, GetThemeSysColor(hTheme, i));
	//}
	//CloseThemeData(hTheme);
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

void CRecordingSourceDlg::OnBnClickedButton1()
{
	CDC* dc = GetWindowDC();

	HTHEME hTheme = OpenThemeData(this->GetSafeHwnd(), VSCLASS_MENU);
	{
		RECT rect;
		this->GetClientRect(&rect);

        int part = MENU_POPUPBORDERS;
        int state = 0;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);
	}
	{
		RECT rect = {120, 0, 122, 100};
        int part = MENU_POPUPGUTTER;
        int state = 0;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);
	}
	{
		RECT rect = {130, 50, 230, 55};
        int part = MENU_POPUPSEPARATOR;
        int state = 0;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);
	}
	{
		RECT rect = {120, 100, 220, 125};
        int part = MENU_POPUPITEM;
        int state = MPI_NORMAL;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);
	}
	{
		RECT rect = {120, 126, 220, 151};
        int part = MENU_POPUPITEM;
        int state = MPI_HOT;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);
	}
	{
		RECT rect = {120, 152, 220, 177};
        int part = MENU_POPUPITEM;
        int state = MPI_DISABLED;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);
	}
	{
		RECT rect = {120, 178, 220, 203};
        int part = MENU_POPUPITEM;
        int state = MPI_DISABLEDHOT;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);
	}
	{
		RECT rect = {125, 209, 141, 225};
        int part = MENU_POPUPCHECK;
        int state = MC_CHECKMARKNORMAL;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);

		RECT rect2 = {120, 204, 145, 229};
        part = MENU_BARITEM;
        state = MBI_PUSHED;
		//if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
		//	DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect2, &rect2);
	}


	{
		RECT rect = {125, 235, 141, 251};
        int part = MENU_POPUPCHECK;
        int state = MC_CHECKMARKNORMAL;

		if (IsThemeBackgroundPartiallyTransparent(hTheme, part, state))
			DrawThemeParentBackground(this->GetSafeHwnd(), dc->GetSafeHdc(), &rect);
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect, &rect);

		RECT rect2 = {120, 230, 145, 255};
        part = MENU_BARITEM;
        state = MBI_PUSHED;
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect2, &rect2);

		RECT rect3 = {120, 230, 220, 255};
        part = MENU_POPUPITEM;
        state = MPI_HOT;
		DrawThemeBackground(hTheme, dc->GetSafeHdc(), part, state, &rect3, &rect3);
	}

	CloseThemeData(hTheme);
}

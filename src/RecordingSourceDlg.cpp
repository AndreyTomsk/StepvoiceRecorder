/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
//#include <vssym32.h>
//#include <Uxtheme.h>
#include "mp3_recorder.h"
#include "RecordingSourceDlg.h"
#include <map>

//#pragma comment(lib, "UxTheme.lib")

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
	/*
	CDC* dc = GetWindowDC();

	HTHEME hTheme = OpenThemeData(this->GetSafeHwnd(), VSCLASS_MENU);

	COLORREF color;
	HRESULT  result;
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_ACTIVEBORDER, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_BACKGROUND, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_BORDERCOLOR, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_FILLCOLOR, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_FROMCOLOR1, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_FROMCOLOR2, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_FROMCOLOR3, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_FROMCOLOR4, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_FROMCOLOR5, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_MENU, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_WINDOW, &color);
	result = GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_EDGEFILLCOLOR, &color);
	*/
	//dc->FillSolidRect(120, 0*15, 100, 15, color);

	/*
	GetThemeColor(hTheme, MENU_BARBACKGROUND, MB_INACTIVE, TMT_COLOR, &color);
	dc.FillSolidRect(120, 1*15, 100, 15, color);
	
	GetThemeColor(hTheme, MENU_BARITEM, MBI_DISABLED, TMT_COLOR, &color);
	dc.FillSolidRect(120, 2*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_BARITEM, MBI_DISABLEDHOT, TMT_COLOR, &color);
	dc.FillSolidRect(120, 3*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_BARITEM, MBI_DISABLEDPUSHED, TMT_COLOR, &color);
	dc.FillSolidRect(120, 4*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_BARITEM, MBI_HOT, TMT_COLOR, &color);
	dc.FillSolidRect(120, 5*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_BARITEM, MBI_NORMAL, TMT_COLOR, &color);
	dc.FillSolidRect(120, 6*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_BARITEM, MBI_PUSHED, TMT_COLOR, &color);
	dc.FillSolidRect(120, 7*15, 100, 15, color);

	GetThemeColor(hTheme, MENU_CHEVRON_TMSCHEMA, 0, TMT_COLOR, &color);
	dc.FillSolidRect(120, 8*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_MENUBARDROPDOWN_TMSCHEMA, 0, TMT_COLOR, &color);
	dc.FillSolidRect(120, 9*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_MENUBARITEM_TMSCHEMA, 0, TMT_COLOR, &color);
	dc.FillSolidRect(120,10*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_MENUDROPDOWN_TMSCHEMA, 0, TMT_COLOR, &color);
	dc.FillSolidRect(120,11*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_MENUITEM_TMSCHEMA, 0, TMT_COLOR, &color);
	dc.FillSolidRect(120,12*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_POPUPBACKGROUND, 0, TMT_COLOR, &color);
	dc.FillSolidRect(120,13*15, 100, 15, color);
	GetThemeColor(hTheme, MENU_POPUPBORDERS, 0, TMT_COLOR, &color);
	dc.FillSolidRect(120,14*15, 100, 15, color);
	*/

	//CloseThemeData(hTheme);
}

/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "MySheet.h"

#include "MainFrm.h"
#include <htmlhelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMySheet, CPropertySheet)

/////////////////////////////////////////////////////////////////////////////
CMySheet::CMySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	//m_pc.m_psp.dwFlags |= PSP_HASHELP;
	//m_pr.m_psp.dwFlags |= PSP_HASHELP;
	AddPage(&m_pc);
	AddPage(&m_pr);
	//AddPage(&m_ps2);
	AddPage(&m_pv);
	AddPage(&m_pa);
}

CMySheet::CMySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	//m_pc.m_psp.dwFlags |= PSP_HASHELP;
	//m_pr.m_psp.dwFlags |= PSP_HASHELP;
	AddPage(&m_pc);
	AddPage(&m_pr);
	//AddPage(&m_ps2);
	AddPage(&m_pv);
	AddPage(&m_pa);	
}

CMySheet::~CMySheet()
{
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMySheet)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dialogs configure
/////////////////////////////////////////////////////////////////////////////

//void CMySheet::SetConfig(CONF_DIAL_GEN* pconfig)
//{
//	m_pc.SetConfig( pconfig );
//}

void CMySheet::SetConfig(CONF_DIAL_MP3* pconfig)
{
	m_pr.SetConfig( pconfig );
}

//void CMySheet::SetConfig(CONF_DIAL_SH *pconfig)
//{
//	m_ps.SetConfig( pconfig );
//}

//void CMySheet::SetConfig(CONF_DIAL_SH2 *pconfig)
//{
//	m_ps2.SetConfig(pconfig);
//}


void CMySheet::SetConfig(CONF_DIAL_VAS* pconfig)
{
	m_pv.SetConfig( pconfig );
}

void CMySheet::SetConfig(CONF_DIAL_AN *pconfig)
{
	m_pa.SetConfig( pconfig );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMySheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		m_nPageIndex = GetActiveIndex();
	if(LOWORD(wParam == IDHELP))
	{	// !!!!! 
		CMainFrame* pFrame = (CMainFrame *)GetParent();
		CString strFile = pFrame->GetProgramDir();
		strFile += "\\svrec.chm::/how_to_use/preferences.html";
		::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
	}

	return CPropertySheet::OnCommand(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
int CMySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	//ModifyStyleEx(0, WS_EX_CONTEXTHELP);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CMySheet::OnDestroy() 
{
	//this->SendMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
	//EndDialog(IDCANCEL);
	CPropertySheet::OnDestroy();
}

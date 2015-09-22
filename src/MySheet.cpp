#include "stdafx.h"
#include "MySheet.h"

#include "MainFrm.h" //for GetProgramDir. TODO: refactor.
#include <htmlhelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMySheet, CPropertySheet)

/////////////////////////////////////////////////////////////////////////////

CMySheet::CMySheet()
	:CPropertySheet(GetWindowTitle(), AfxGetApp()->GetMainWnd(),
		RegistryConfig::GetOption(_T("General\\DialogIndex"), 0))
{
	this->m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage(&m_pc);
	AddPage(&m_pr);
	AddPage(&m_pv);
	AddPage(&m_pa);	
}
//---------------------------------------------------------------------------

CString CMySheet::GetWindowTitle()
{
	return _T("Preferences");
}
//---------------------------------------------------------------------------

BOOL CMySheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		RegistryConfig::SetOption(_T("General\\DialogIndex"), GetActiveIndex());

	if (LOWORD(wParam == IDHELP))
	{
		CMainFrame* pFrame = (CMainFrame *)GetParent();
		CString strFile = pFrame->GetProgramDir();
		strFile += _T("\\svrec.chm::/how_to_use/preferences.html");
		::HtmlHelp(::GetDesktopWindow(), strFile, HH_DISPLAY_TOPIC, NULL);
	}

	return CPropertySheet::OnCommand(wParam, lParam);
}
//---------------------------------------------------------------------------

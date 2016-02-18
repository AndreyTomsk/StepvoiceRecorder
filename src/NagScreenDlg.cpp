#include "stdafx.h"
#include "NagScreenDlg.h"
#include "EnterCodeDlg.h"
#include "FileUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CNagScreenDlg, CDialog)
	ON_BN_CLICKED(IDC_ENTERCODE, OnEntercode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CNagScreenDlg::CNagScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNagScreenDlg::IDD, pParent)
{
}
//---------------------------------------------------------------------------

BOOL CNagScreenDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE); 

	BOOL result = m_wndOrder.SubclassDlgItem(IDC_ORDER, this);
	m_wndOrder.SetUrl("http://stepvoice.com/order.shtml");

	return TRUE;
}
//---------------------------------------------------------------------------

void CNagScreenDlg::OnEntercode() 
{
	CEnterCodeDlg dlg;
	if (dlg.DoModal() == IDOK)
	{	
		// Exit code as IDOK - to make an error and quit current instance.
		PostMessage(WM_COMMAND, IDOK, (LPARAM)GetDlgItem(IDCANCEL)->m_hWnd);

		const CString programPath = FileUtils::GetProgramPath();
		ShellExecute(::GetDesktopWindow(), _T("open"), programPath, NULL, NULL, SW_SHOW);
	}
}
//---------------------------------------------------------------------------

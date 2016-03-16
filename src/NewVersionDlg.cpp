#include "stdafx.h"
#include "NewVersionDlg.h"

IMPLEMENT_DYNAMIC(CNewVersionDlg, CDialog)

BEGIN_MESSAGE_MAP(CNewVersionDlg, CDialog)
END_MESSAGE_MAP()
//---------------------------------------------------------------------------

CNewVersionDlg::CNewVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewVersionDlg::IDD, pParent)
{
}
//---------------------------------------------------------------------------

CNewVersionDlg::~CNewVersionDlg()
{
}
//---------------------------------------------------------------------------

void CNewVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
//---------------------------------------------------------------------------

INT_PTR CNewVersionDlg::DoModal(CString lastVersion, CString downloadLink)
{
	m_lastVersion = lastVersion;
	m_downloadLink = downloadLink;
	return CDialog::DoModal();
}
//---------------------------------------------------------------------------

BOOL CNewVersionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CStatic* infoIcon = static_cast<CStatic*>(GetDlgItem(IDC_STATIC0_0));
	infoIcon->SetIcon(LoadIcon(NULL, MAKEINTRESOURCE(IDI_INFORMATION)));

	CStatic* downloadLink = static_cast<CStatic*>(GetDlgItem(IDC_DOWNLOAD_LINK));
	downloadLink->SetWindowTextW(m_downloadLink);
	m_downloadWnd.SubclassDlgItem(IDC_DOWNLOAD_LINK, this);
	m_downloadWnd.SetUrl(m_downloadLink);

	CString versionText;
	GetDlgItemText(IDC_STATIC_VERSION, versionText);
	versionText.Replace(L"<version>", m_lastVersion);
	SetDlgItemText(IDC_STATIC_VERSION, versionText);

	return TRUE;
}
//---------------------------------------------------------------------------


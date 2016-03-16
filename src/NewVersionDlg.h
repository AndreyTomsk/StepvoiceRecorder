#pragma once
#include "UrlWnd.h"

class CNewVersionDlg : public CDialog
{
public:
	CNewVersionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewVersionDlg();

	INT_PTR DoModal(CString lastVersion, CString downloadLink);

// Dialog Data
	enum { IDD = IDD_NEWVERSION };

private:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_DYNAMIC(CNewVersionDlg)
	DECLARE_MESSAGE_MAP()

	CString m_lastVersion;
	CString m_downloadLink;
	CUrlWnd	m_downloadWnd;
};

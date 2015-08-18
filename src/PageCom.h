/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PAGECOM_H__189B5FC7_2E15_4E20_BA3C_82EADCA86D62__INCLUDED_)
#define AFX_PAGECOM_H__189B5FC7_2E15_4E20_BA3C_82EADCA86D62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
class CPageCom : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageCom)
	enum { IDD = IDD_COMMON };

	CString m_outputFolder;
	CString m_outputFileTemplate;
	BOOL	m_TrayIcon;
	BOOL	m_TrayMin;

public:
	CPageCom();
	virtual ~CPageCom();
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnOK();

	afx_msg void OnBnClickedOutputFolder();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedDefaultTemplate();
};

#endif

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
	CONF_DIAL_GEN* pconfig;
	CString m_outputFolder;
	CString m_outputFileTemplate;

public:
	CPageCom();
	virtual ~CPageCom();

	virtual BOOL OnInitDialog();
	void SetConfig(CONF_DIAL_GEN* pconf_dial_gen);

// Dialog Data
	//{{AFX_DATA(CPageCom)
	enum { IDD = IDD_COMMON };
	BOOL	m_Loader;
	BOOL	m_AutoRun;
	BOOL	m_Minimized;
	BOOL	m_ToolTipEnable;
	BOOL	m_TrayIcon;
	BOOL	m_TrayMin;
	BOOL	m_MInstances;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageCom)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CPageCom)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOutputFolder();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PAGECOM_H__189B5FC7_2E15_4E20_BA3C_82EADCA86D62__INCLUDED_)

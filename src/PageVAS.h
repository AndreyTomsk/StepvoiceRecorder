#if !defined(AFX_PAGEVAS_H__47251355_3516_4D39_AC4E_FCE43C04C65A__INCLUDED_)
#define AFX_PAGEVAS_H__47251355_3516_4D39_AC4E_FCE43C04C65A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
// CPageVAS dialog
class CPageVAS : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageVAS)

private:
	CONF_DIAL_VAS* m_pConfig;
	CFont	m_FontWarn;

public:
	void SetConfig(CONF_DIAL_VAS* pconf_dial_vas);
	CPageVAS();
	~CPageVAS();

// Dialog Data
	//{{AFX_DATA(CPageVAS)
	enum { IDD = IDD_VAS };
	BOOL	m_bEnable;
	int		m_nAction;
	//}}AFX_DATA
	int		m_nWaitTime;
	int		m_nThreshold;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageVAS)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageVAS)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEVAS_H__47251355_3516_4D39_AC4E_FCE43C04C65A__INCLUDED_)

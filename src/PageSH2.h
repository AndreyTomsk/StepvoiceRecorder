#if !defined(AFX_PAGESH2_H__BA37F26D_8592_4E1B_A3A8_0EA393ABB145__INCLUDED_)
#define AFX_PAGESH2_H__BA37F26D_8592_4E1B_A3A8_0EA393ABB145__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSH2.h : header file
//
#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
// CPageSH2 dialog

class CPageSH2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageSH2)

private:
	CONF_DIAL_SH2*	m_pConfig;
	CFont	m_FontWarn;

// Construction
public:
	void SetConfig(CONF_DIAL_SH2* pConfig);
	CPageSH2();
	~CPageSH2();

// Dialog Data
	CTime	m_ctStart;
	CTime	m_ctStop;
	CTime	m_ctRecTime;
	CString m_strFullName;
	//{{AFX_DATA(CPageSH2)
	enum { IDD = IDD_SCHEDULER2 };
	BOOL	m_bSchedStart;
	int		m_nSchedStopID;
	BOOL	m_bRunExternal;
	BOOL	m_bActShutDown;
	BOOL	m_bActCloseAll;
	BOOL	m_bEnableSHR;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageSH2)
	public:
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageSH2)
	afx_msg void OnSh2EnableStart();
	afx_msg void OnSh2RunExternal();
	afx_msg void OnSh2SelectExternal();
	virtual BOOL OnInitDialog();
	afx_msg void OnSh2BtnStopTime();
	afx_msg void OnSh2BtnRecTime();
	afx_msg void OnChangeSh2EditFileName();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void UpdateStopSection();
	void UpdateFileNameText();
	void UpdateRunSection();
	void UpdateStartSection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PAGESH2_H__BA37F26D_8592_4E1B_A3A8_0EA393ABB145__INCLUDED_)

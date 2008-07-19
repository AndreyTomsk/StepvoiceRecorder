#if !defined(AFX_NAGSCREENDLG_H__CCC0127F_CF80_467F_9189_3A75251653E5__INCLUDED_)
#define AFX_NAGSCREENDLG_H__CCC0127F_CF80_467F_9189_3A75251653E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UrlWnd.h"

/////////////////////////////////////////////////////////////////////////////
class CNagScreenDlg : public CDialog
{
public:
	CNagScreenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNagScreenDlg)
	enum { IDD = IDD_NAGSCREEN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNagScreenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CUrlWnd	m_wndOrder;
	CFont	m_BoldFont;

	// Generated message map functions
	//{{AFX_MSG(CNagScreenDlg)
	afx_msg void OnEntercode();
	afx_msg void OnOrder();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_NAGSCREENDLG_H__CCC0127F_CF80_467F_9189_3A75251653E5__INCLUDED_)

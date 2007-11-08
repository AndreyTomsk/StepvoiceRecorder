#if !defined(AFX_TIMERDLG_H__2E2E1BC9_6354_4860_BF94_F2027090EB8C__INCLUDED_)
#define AFX_TIMERDLG_H__2E2E1BC9_6354_4860_BF94_F2027090EB8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//
typedef struct {
	CString strCaption;
	CString strDlgText;
	CString strBtnText;
	int	nCloseTime;
} TIMERDLG_INFO;

/////////////////////////////////////////////////////////////////////////////
// CTimerDlg dialog
class CTimerDlg : public CDialog
{
	int m_nSeconds;

// Construction
public:
	TIMERDLG_INFO m_tdi;
	CTimerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimerDlg)
	enum { IDD = IDD_TIMERDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTimerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_TIMERDLG_H__2E2E1BC9_6354_4860_BF94_F2027090EB8C__INCLUDED_)

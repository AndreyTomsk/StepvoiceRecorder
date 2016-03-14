#pragma once
#include "UrlWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CEnterCodeDlg dialog

class CEnterCodeDlg : public CDialog
{
// Construction
public:
	CEnterCodeDlg(CWnd* pParent = NULL);   // standard constructor
	bool RegisterKey(CString key, bool multiUserKey = false);

// Dialog Data
	//{{AFX_DATA(CEnterCodeDlg)
	enum { IDD = IDD_ENTERCODE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterCodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEnterCodeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEditcode();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGetnow();
private:
	CUrlWnd	m_wndLostKey;
};

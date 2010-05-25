#if !defined(AFX_ENTERCODEDLG_H__756A9FDB_987B_47EF_B7E8_FC3DB026DAE4__INCLUDED_)
#define AFX_ENTERCODEDLG_H__756A9FDB_987B_47EF_B7E8_FC3DB026DAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnterCodeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnterCodeDlg dialog

class CEnterCodeDlg : public CDialog
{
// Construction
public:
	CEnterCodeDlg(CWnd* pParent = NULL);   // standard constructor
	bool RegisterKey(LPCSTR pszKey, bool multiUserKey = false);

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
	afx_msg void OnPaste();
	afx_msg void OnClear();
	afx_msg void OnChangeEditcode();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTERCODEDLG_H__756A9FDB_987B_47EF_B7E8_FC3DB026DAE4__INCLUDED_)

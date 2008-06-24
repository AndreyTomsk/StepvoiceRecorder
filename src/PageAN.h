#if !defined(AFX_PAGEAN_H__11FCE574_39A3_4791_9B7E_68E8DC7FD6D3__INCLUDED_)
#define AFX_PAGEAN_H__11FCE574_39A3_4791_9B7E_68E8DC7FD6D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageAN.h : header file
//

#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
// CPageAN dialog

class CPageAN : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageAN)

private:
	CFont	m_BoldFont;
	CONF_DIAL_AN* m_pconfig;

// Construction
public:
	void SetConfig(CONF_DIAL_AN* pconfig);
	CPageAN();
	~CPageAN();

// Dialog Data
	//{{AFX_DATA(CPageAN)
	enum { IDD = IDD_AUTONAME };
	CString	m_strTemplate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageAN)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	/// This function removes invalid characters from a template
	CString FilterTemplate(CString a_template);

	// Generated message map functions
	//{{AFX_MSG(CPageAN)
	afx_msg void OnChangeNametemplate();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEAN_H__11FCE574_39A3_4791_9B7E_68E8DC7FD6D3__INCLUDED_)

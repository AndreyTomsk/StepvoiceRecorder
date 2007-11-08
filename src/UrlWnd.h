#if !defined(AFX_URLWND_H__5A0A2462_64CF_4FF8_B7C6_A24BBC3EFF42__INCLUDED_)
#define AFX_URLWND_H__5A0A2462_64CF_4FF8_B7C6_A24BBC3EFF42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UrlWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUrlWnd window

class CUrlWnd : public CStatic
{
// Construction
public:
	CUrlWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUrlWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Init();
	void SetUrl(LPCSTR pszUrl);
	virtual ~CUrlWnd();

	// Generated message map functions
protected:
	CString m_strUrl;
	//{{AFX_MSG(CUrlWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URLWND_H__5A0A2462_64CF_4FF8_B7C6_A24BBC3EFF42__INCLUDED_)

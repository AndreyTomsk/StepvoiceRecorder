#ifndef _URLWND_H
#define _URLWND_H
#pragma once

/////////////////////////////////////////////////////////////////////////////

class CUrlWnd : public CStatic
{
public:
	CUrlWnd();
	virtual ~CUrlWnd();

	void SetUrl(CString pszUrl);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUrlWnd)
	//}}AFX_VIRTUAL
	// Generated message map functions
protected:
	//{{AFX_MSG(CUrlWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_strUrl;
};

/////////////////////////////////////////////////////////////////////////////

#endif // _URLWND_H

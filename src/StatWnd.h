/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_STATWND_H__4F11F7E5_3092_4A02_BAAB_5974438BFDF0__INCLUDED_)
#define AFX_STATWND_H__4F11F7E5_3092_4A02_BAAB_5974438BFDF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "ImageBtn.h"
#include "Interface\Buttons\BitmapBtn.h"

/////////////////////////////////////////////////////////////////////////////
class CStatWnd : public CWnd
{
	CSize	m_wndsize;
	CDC		m_memDC;
	CDC		m_bkDC;
	CDC		m_statDC;
	CBitmap m_bmp;
	CBitmap m_bkbmp;
	CBitmap m_statbmp;

public:
	CBitmapBtn m_btnSched;
	CBitmapBtn m_btnVas;
	CBitmapBtn m_btnFlr;
	CBitmapBtn m_btnMon;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	CStatWnd();
	void Set(float fF, int iR, int iS, UINT nSize, char* szTime);
	void Set(int nFreq, int nBitrate, int nStereo);
	void Set(UINT nSize, char* szTime);
	~CStatWnd();

	// Generated message map functions
protected:
	BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	//{{AFX_MSG(CStatWnd)
	afx_msg void OnBtnVas();
	afx_msg void OnBtnSched();
	afx_msg void OnBtnMon();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_STATWND_H__4F11F7E5_3092_4A02_BAAB_5974438BFDF0__INCLUDED_)

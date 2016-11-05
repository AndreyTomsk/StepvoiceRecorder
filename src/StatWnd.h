/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once
#include "BitmapBtn.h"

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
	CBitmapBtn m_btnVas;
	CBitmapBtn m_btnMon;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	CStatWnd();
	void Set(float fF, int iR, int iS, UINT nSize, const CString& szTime);
	void Set(int nFreq, int nBitrate, int nStereo);
	void Set(UINT nSize, const CString& szTime);
	~CStatWnd();

	// Generated message map functions
protected:
	BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	//{{AFX_MSG(CStatWnd)
	afx_msg void OnBtnVas();
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

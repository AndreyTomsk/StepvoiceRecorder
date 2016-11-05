/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Порядок иконок соответствует битмапу с иконками.
enum IconID {ICON_STOP, ICON_PAUSEP, ICON_PAUSER, ICON_PLAY, ICON_REC, ICON_RECVAS};

/////////////////////////////////////////////////////////////////////////////
class CIcoWnd : public CWnd
{
	CSize	m_wndsize;

	CDC		m_memDC;
	CBitmap m_bmp;
	CBrush  m_bkbrush;

	CDC		m_icoDC;
	CBitmap m_icobmp;

private:
	void Clear();
public:
	CIcoWnd();
	~CIcoWnd();
	void SetNewIcon(int nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIcoWnd)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CIcoWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

/////////////////////////////////////////////////////////////////////////////
class CTimeWnd : public CWnd
{
	CSize	m_wndsize;
	CDC		m_memDC;
	CDC		m_bkDC;
	CDC		m_timeDC;
	CBitmap m_bmp;
	CBitmap m_bkbmp;
	CBitmap m_timebmp;

	UINT  nCurSec;
	bool  m_bBlink;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeWnd)
	//}}AFX_VIRTUAL
private:
	void Convert(UINT nSec, char* pszTime);
public:
	void SetTime(int nHours, int nMinutes, int nSeconds);
	void Decrease();
	void Blink(bool bBlink);
	CTimeWnd();
	~CTimeWnd();
	void Reset();
	void Increase();
	void SetTime(UINT nSec);

protected:
	//{{AFX_MSG(CTimeWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

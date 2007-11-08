/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_TIMEWND_H__AA9AA506_7069_4058_90DF_6EC26A55FF08__INCLUDED_)
#define AFX_TIMEWND_H__AA9AA506_7069_4058_90DF_6EC26A55FF08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_TIMEWND_H__AA9AA506_7069_4058_90DF_6EC26A55FF08__INCLUDED_)

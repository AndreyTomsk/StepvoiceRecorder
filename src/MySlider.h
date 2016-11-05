/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMySlider window

class CMySlider : public CSliderCtrl
{
	int iCurPos;
	int iRangeMax;
	int iRangeMin;
	int iSizeLine;
	int iSizePage;

	bool bTrack;

	DWORD dwStyle;

// Construction
public:
	CMySlider();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySlider)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowThumb(bool bVisible);
	void SetTrack(bool bYes);
	bool IsTrack();
	bool IsDragging();
	DWORD GetStyle();
	int GetPos();
	bool AlreadyEnd();
	void SetCurPos(int iPos);
	void TurnLineRight();
	void TurnLineLeft();
	int TurnPageRight();
	int TurnPageLeft();
	void TurnPosMax();
	void TurnPosMin();
	int SetLineSize(int iSize);
	int SetPageSize(int iSize);
	void SetRange(int iMin, int iMax);
	virtual ~CMySlider();

private:
	bool SetThumb(const CPoint& pt);
	void PostMessageToParent(const int nTBCode);
	bool m_bDragging;
	bool m_bDragChanged;

protected:
// Generated message map functions
	//{{AFX_MSG(CMySlider)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

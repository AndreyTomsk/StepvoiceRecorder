/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#ifndef BITMAP_BTN_H
#define BITMAP_BTN_H
#pragma once

/////////////////////////////////////////////////////////////////////////////
// BN_CLICKED message posted on left button up, and on Check change.

class CBitmapBtn : public CWnd
{
public:
	CBitmapBtn();
	virtual ~CBitmapBtn();
	BOOL Create(CImageList *pImageList, const POINT& pt, CWnd *pParentWnd, UINT nID);

	//Emulating checkbox behaviour.
	bool IsChecked() const;
	void SetCheck(bool check);

	bool IsEnabled() const;
	void Enable(bool enable);

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapBtn)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CBitmapBtn)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void PostClickedToParent() const;

	CImageList m_btnIL;
	bool m_mouseInside;
	bool m_leftButtonPressed;
	bool m_checked;
	bool m_enabled;
};

/////////////////////////////////////////////////////////////////////////////
#endif

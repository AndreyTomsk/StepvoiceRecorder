/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

/////////////////////////////////////////////////////////////////////////////
class CControlButton : public CButton
{
public:
	CControlButton();
	virtual ~CControlButton();

	BOOL Create( const RECT &rect, CWnd* pParentWnd, UINT nID );
	void Press();
	HICON SetIcon( int nResourceID );

private:
	int m_nID;

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlButton)
	//}}AFX_VIRTUAL
protected:
// Generated message map functions
	//{{AFX_MSG(CControlButton)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
public:
};

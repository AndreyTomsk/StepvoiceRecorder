#pragma once
#include "UrlWnd.h"

class CNagScreenDlg : public CDialog
{
	enum { IDD = IDD_NAGSCREEN };

public:
	CNagScreenDlg(CWnd* pParent = NULL);   // standard constructor

private:
	CUrlWnd	m_wndOrder;
	CFont	m_BoldFont;

	afx_msg void OnEntercode();
	afx_msg void OnOrder();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};

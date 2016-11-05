/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

class CPageAN : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageAN)
	enum { IDD = IDD_AUTONAME };

	CFont m_BoldFont;
	CString	m_strTemplate;
	CString m_defaultTemplate;

public:
	CPageAN();
	~CPageAN();

protected:
	///Removes invalid characters from a template.
	CString FilterTemplate(CString a_template);

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	afx_msg void OnChangeNametemplate();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()
};

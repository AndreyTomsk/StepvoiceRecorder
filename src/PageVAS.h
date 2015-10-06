#pragma once

class CPageVAS : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageVAS)
	enum { IDD = IDD_VAS };

	int		m_action;
	int		m_thresholdIndex;
	int		m_delayIndex;
	CFont	m_FontWarn;

public:
	CPageVAS();
	~CPageVAS();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()
};

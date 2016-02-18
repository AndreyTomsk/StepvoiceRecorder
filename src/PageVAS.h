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
	virtual void OnOK();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()
};

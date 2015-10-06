#pragma once

class CPageCom : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageCom)
	enum { IDD = IDD_COMMON };

	CString m_outputFolder;
	BOOL	m_TrayIcon;
	BOOL	m_TrayMin;
	BOOL	m_multipleInstances;
	BOOL	m_storeInOutputFolder;
	BOOL	m_alwaysOnTop;

public:
	CPageCom();
	virtual ~CPageCom();
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnKillActive();
	virtual void OnOK();

	afx_msg void OnBnClickedOutputFolder();
	afx_msg void OnBnClickedStoreInOutputFolder();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()
};

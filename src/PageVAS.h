#if !defined(AFX_PAGEVAS_H__47251355_3516_4D39_AC4E_FCE43C04C65A__INCLUDED_)
#define AFX_PAGEVAS_H__47251355_3516_4D39_AC4E_FCE43C04C65A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

class CPageVAS : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageVAS)
	enum { IDD = IDD_VAS };

	BOOL	m_enable;
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

/////////////////////////////////////////////////////////////////////////////
#endif

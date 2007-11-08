#if !defined(AFX_PAGESH_H__2012BC65_FFE2_4EC7_B018_845E6CD6F6B1__INCLUDED_)
#define AFX_PAGESH_H__2012BC65_FFE2_4EC7_B018_845E6CD6F6B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSH.h : header file
//

#include "struct.h"
#include "Scheduler.h"

/////////////////////////////////////////////////////////////////////////////
// CPageSH dialog

class CPageSH : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageSH)

private:
	void UpdateTimeChange(int nColumn);
	//void SetNewRecTimeInfo();
	CTime m_ctStart, m_ctStop, m_ctRecTime;
	//CTimeSpan m_ctRecTime;
	CONF_DIAL_SH* m_pconfig;

// Construction
public:
	void SetConfig(CONF_DIAL_SH* pconfig);
	CPageSH();
	~CPageSH();

// Dialog Data
	//{{AFX_DATA(CPageSH)
	enum { IDD = IDD_SCHEDULER };
	BOOL	m_nEnable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageSH)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageSH)
	afx_msg void On_tpStopTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On_tpStartTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void On_tpRecTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESH_H__2012BC65_FFE2_4EC7_B018_845E6CD6F6B1__INCLUDED_)

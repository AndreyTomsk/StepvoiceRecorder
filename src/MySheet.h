/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MYSHEET_H__C92743F1_2EF0_4D8D_8069_E4BD0E9060BF__INCLUDED_)
#define AFX_MYSHEET_H__C92743F1_2EF0_4D8D_8069_E4BD0E9060BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"
#include "PageCom.h"
#include "PageRec.h"
#include "PageAN.h"
//#include "PageSH.h"
#include "PageVAS.h"
//#include "PageSH2.h"

/////////////////////////////////////////////////////////////////////////////
class CMySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMySheet)

public:
	CMySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	~CMySheet();

	CPageCom	m_pc;
	CPageRec	m_pr;
	CPageAN		m_pa;
	CPageVAS	m_pv;
	//CPageSH2	m_ps2;

	int			m_nPageIndex;

	//void SetConfig(CONF_DIAL_GEN* pconfig);
	//void SetConfig(CONF_DIAL_MP3* pconfig);
	//void SetConfig(CONF_DIAL_SH*  pconfig);
	//void SetConfig(CONF_DIAL_SH2* pconfig);
	//void SetConfig(CONF_DIAL_VAS* pconfig);
	//void SetConfig(CONF_DIAL_AN*  pconfig);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySheet)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CMySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_MYSHEET_H__C92743F1_2EF0_4D8D_8069_E4BD0E9060BF__INCLUDED_)

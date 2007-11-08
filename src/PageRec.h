/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PAGEREC_H__C53484A7_1FFB_4740_A0FB_9F50E2C37386__INCLUDED_)
#define AFX_PAGEREC_H__C53484A7_1FFB_4740_A0FB_9F50E2C37386__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"

/////////////////////////////////////////////////////////////////////////////
class CPageRec : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageRec)

private:
	CONF_DIAL_MP3*	pconfig;
	int		m_nBitrate;

	//{{AFX_DATA(CPageRec)
	enum { IDD = IDD_RECORD };
	BOOL	m_bAutoFreq;
	int		m_nSampleFreq;
	int		m_nStereo;
	//}}AFX_DATA

private:
	void EnableFreqSel(int nBitrate);
	void EnableFreqSel(bool bmpg2freq, bool bmpg1freq);
	void SetFreq(int nBitrate);

public:
	CPageRec();
	void SetConfig(CONF_DIAL_MP3* pconf_dial_mp3);

	//{{AFX_VIRTUAL(CPageRec)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CPageRec)
	virtual BOOL OnInitDialog();
	afx_msg void OnAutofreq();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PAGEREC_H__C53484A7_1FFB_4740_A0FB_9F50E2C37386__INCLUDED_)

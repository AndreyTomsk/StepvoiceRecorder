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
	enum { IDD = IDD_RECORD };

	int		m_bitrateIndex;
	BOOL	m_autoFrequency;
	int		m_frequencyIndex;
	int		m_isStereo;

public:
	CPageRec();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	afx_msg void OnAutofreq();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

private:
	void EnableFreqSel(int nBitrate);
	void EnableFreqSel(bool bmpg2freq, bool bmpg1freq);
	void SetFreq(int nBitrate);
};

/////////////////////////////////////////////////////////////////////////////
#endif

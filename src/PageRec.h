#pragma once

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

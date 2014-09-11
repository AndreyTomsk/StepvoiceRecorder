#ifndef _RECORDING_SOURCE_ITEM_H
#define _RECORDING_SOURCE_ITEM_H
#pragma once

/////////////////////////////////////////////////////////////////////////////
//Menu-like item with selection checkbox, device name, and its current volume level.

class CRecordingSourceItem : public CWnd
{
public:
	CRecordingSourceItem();
	~CRecordingSourceItem();

	void SetCheck(bool check);
	void SetLabel(const CString& newLabel);
	void SetLevel(unsigned levelPercent);

	bool GetCheck() const;

public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphWnd)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CRecordingSourceItem)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	/*
	CSize   m_wndsize;
	CPen    m_greenPen;
	CBrush  m_bkbrush;

	CDC     m_memDC;
	CDC     m_peakDC;
	CDC     m_skinDC;

	CBitmap m_bmp;
	CBitmap m_bgbmp[4];  // background bitmaps
	CBitmap m_pmbmp[3];  // peak bitmaps
	*/
	CButton m_itemCheckBox;
	CStatic m_itemLabel;
	CStatic m_itemLevel;

	DWORD m_timer_id;
};

/////////////////////////////////////////////////////////////////////////////

#endif // _RECORDING_SOURCE_ITEM_H

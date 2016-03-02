#ifndef _RECORDING_SOURCE_ITEM_H
#define _RECORDING_SOURCE_ITEM_H
#pragma once

/////////////////////////////////////////////////////////////////////////////
//Menu-like item with selection checkbox, device name, and its current volume level.
//To handle checkbox clicked event, use ON_BN_CLICKED with IDC_RECORDING_DEVICE id.

class CRecordingSourceItem : public CWnd
{
public:
	CRecordingSourceItem(const CString& caption, bool showCheckbox);
	~CRecordingSourceItem();

	bool GetCheck() const;

	void SetCheck(bool check);
	void SetLevel(unsigned levelPercent);

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
	afx_msg void OnCheckboxClicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CButton m_itemCheckBox;
	CStatic m_itemLabel;
	//CStatic m_itemLevel;
	CFont   m_menuFont;

	CString m_caption;
	bool m_mouseOverWindow;
	bool m_mouseOverCheckbox;
	bool m_showCheckbox;

	CDC     m_peakMeterDC;
	CDC     m_checkMarkDC;
	CBitmap m_peakMeterBitmap;
	CBitmap m_checkMarkBitmap;
	int     m_curLevel;
	int     m_oldLevel;
};

/////////////////////////////////////////////////////////////////////////////

#endif // _RECORDING_SOURCE_ITEM_H

#pragma once

class CUrlWnd : public CStatic
{
public:
	CUrlWnd();
	virtual ~CUrlWnd();

	void SetUrl(CString pszUrl);

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	CString m_strUrl;
	bool m_isUnderline;
	CFont m_underlineFont;
};

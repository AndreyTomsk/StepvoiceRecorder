#if !defined(AFX_SLIDERVOL_H__4FFCAF7A_8096_48CB_A489_1ACB3CAF88A8__INCLUDED_)
#define AFX_SLIDERVOL_H__4FFCAF7A_8096_48CB_A489_1ACB3CAF88A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////////
class CSliderVol : public CSliderCtrl
{
public:
	CSliderVol();
	virtual ~CSliderVol();

public:
	bool IsDragging() const { return m_is_dragging; }
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSliderVol)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CSliderVol)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool m_is_dragging;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_SLIDERVOL_H__4FFCAF7A_8096_48CB_A489_1ACB3CAF88A8__INCLUDED_)

#if !defined(AFX_SLIDERVOL_H__4FFCAF7A_8096_48CB_A489_1ACB3CAF88A8__INCLUDED_)
#define AFX_SLIDERVOL_H__4FFCAF7A_8096_48CB_A489_1ACB3CAF88A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SliderVol.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSliderVol window

class CSliderVol : public CSliderCtrl
{
// Construction
public:
	CSliderVol();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSliderVol)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSliderVol();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSliderVol)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDERVOL_H__4FFCAF7A_8096_48CB_A489_1ACB3CAF88A8__INCLUDED_)

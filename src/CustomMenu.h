/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_CUSTOMMENU_H__64921B6F_310D_4AA4_BAF2_495075433531__INCLUDED_)
#define AFX_CUSTOMMENU_H__64921B6F_310D_4AA4_BAF2_495075433531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CCustomMenu : public CMenu
{
// Construction
public:
	CCustomMenu();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomMenu)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCustomMenu();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomMenu)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_CUSTOMMENU_H__64921B6F_310D_4AA4_BAF2_495075433531__INCLUDED_)

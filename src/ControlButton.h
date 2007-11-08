/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_CONTROLBUTTON_H__95BD7841_CBF4_11D7_BC33_444553540000__INCLUDED_)
#define AFX_CONTROLBUTTON_H__95BD7841_CBF4_11D7_BC33_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CControlButton : public CButton
{
public:
	CControlButton();
	virtual ~CControlButton();

	BOOL  Create( const RECT &rect, CWnd* pParentWnd, UINT nID );
	void  Press();
	HICON SetIcon( int nResourceID );

private:
	int m_nID;

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlButton)
	//}}AFX_VIRTUAL
protected:
// Generated message map functions
	//{{AFX_MSG(CControlButton)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
#endif // !defined(AFX_CONTROLBUTTON_H__95BD7841_CBF4_11D7_BC33_444553540000__INCLUDED_)

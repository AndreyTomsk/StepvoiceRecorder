/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_BITMAPBTN_H__0DA36B97_9C65_4605_9C74_D81F41D4C0CB__INCLUDED_)
#define AFX_BITMAPBTN_H__0DA36B97_9C65_4605_9C74_D81F41D4C0CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
enum BtnState	// Порядок кнопок в битмапе (ВАЖНО!, исп. в OnPaint)
	{	BTN_NORMAL    = 0, // нормальная
		BTN_NORMAL_H  = 1, // нормальная - подсвеченная (H - highlighted)
		BTN_NORMAL_D  = 2, // нормальная - заблокированная (D - disabled)
		BTN_PRESSED   = 3, // нажатая
		BTN_PRESSED_H = 4, // нажатая - подсвеченная
		BTN_PRESSED_D = 5  // нажатая - заблокированная
	};

/////////////////////////////////////////////////////////////////////////////
class CBitmapBtn : public CWnd
{
public:
	CBitmapBtn();
	virtual ~CBitmapBtn();
	int  IsPressed();
	bool IsDisabled();
	void SetState(int nNewState);
	BOOL Create(CImageList *pImageList, const POINT& pt, CWnd *pParentWnd, UINT nID);

protected:
	int  IsLButtonPressed();

protected:
	BtnState	m_nBtnState;
	CImageList	m_btnIL;
	int			m_bMouseInside;
	int			m_bStatePressed;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapBtn)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CBitmapBtn)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_BITMAPBTN_H__0DA36B97_9C65_4605_9C74_D81F41D4C0CB__INCLUDED_)

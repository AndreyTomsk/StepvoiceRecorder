#include "stdafx.h"
#include "SliderVol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSliderVol, CSliderCtrl)
	//{{AFX_MSG_MAP(CSliderVol)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CSliderVol::CSliderVol()
	:m_is_dragging(false)
{
}

CSliderVol::~CSliderVol()
{
}

////////////////////////////////////////////////////////////////////////////////
void CSliderVol::OnSetFocus(CWnd* pOldWnd) 
{
	CSliderCtrl::OnSetFocus(pOldWnd);
	
	GetParent()->SetFocus();
	//GetParent()->PostMessage(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0),
	//	(LPARAM)GetSafeHwnd());
}

void CSliderVol::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_is_dragging = true;
	CSliderCtrl::OnLButtonDown(nFlags, point);
}

void CSliderVol::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_is_dragging = false;
	CSliderCtrl::OnLButtonUp(nFlags, point);
}

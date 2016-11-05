/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include "MySlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySlider

CMySlider::CMySlider()
{
	bTrack = false;
	iCurPos = 0;
	iRangeMax = iRangeMin = 0;
	iSizeLine = iSizePage = 0;

	//dwStyle = WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_BOTH;

	m_bDragging	   = false;
	m_bDragChanged = false;
}

CMySlider::~CMySlider()
{
}


BEGIN_MESSAGE_MAP(CMySlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CMySlider)
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMySlider message handlers
void CMySlider::SetRange(int iMin, int iMax)
{
	iRangeMin = iMin;
	iRangeMax = iMax;
	CSliderCtrl::SetRange(iRangeMin, iRangeMax);
}

//===========================================================================
int CMySlider::SetPageSize(int iSize)
{
	iSizePage = iSize;
	return CSliderCtrl::SetPageSize(iSizePage);
}

//===========================================================================
int CMySlider::SetLineSize(int iSize)
{
	iSizeLine = iSize;
	return CSliderCtrl::SetLineSize(iSizeLine);
}

//===========================================================================
void CMySlider::TurnPosMin()
{
	iCurPos = iRangeMin;
	CSliderCtrl::SetPos(iRangeMin);
}

//===========================================================================
void CMySlider::TurnPosMax()
{
	iCurPos = iRangeMax;
	CSliderCtrl::SetPos(iRangeMax);
}

//===========================================================================
int CMySlider::TurnPageLeft()
{
	//iCurPos = CSliderCtrl::GetPos();
	iCurPos = max(iRangeMin, iCurPos - iSizePage);
	CSliderCtrl::SetPos(iCurPos);

	return iCurPos;
}

//===========================================================================
int CMySlider::TurnPageRight()
{
	//iCurPos = CSliderCtrl::GetPos();
	iCurPos = min(iRangeMax, iCurPos + iSizePage);
	CSliderCtrl::SetPos(iCurPos);

	return iCurPos;
}

//===========================================================================
void CMySlider::TurnLineLeft()
{
	//iCurPos = CSliderCtrl::GetPos();
	iCurPos = max(iRangeMin, iCurPos - iSizeLine);
	CSliderCtrl::SetPos(iCurPos);
}

//===========================================================================
void CMySlider::TurnLineRight()
{
	//iCurPos = CSliderCtrl::GetPos();
	iCurPos = min(iRangeMax, iCurPos + iSizeLine);
	CSliderCtrl::SetPos(iCurPos);
}

//===========================================================================
void CMySlider::SetCurPos(int iPos)
{
	iCurPos = iPos;
	CSliderCtrl::SetPos(iCurPos);
}

//===========================================================================
bool CMySlider::AlreadyEnd()
{
	return iCurPos==iRangeMax-1;
}

//===========================================================================
int CMySlider::GetPos()
{
	iCurPos = CSliderCtrl::GetPos();
	return iCurPos;
}

//===========================================================================
DWORD CMySlider::GetStyle()
{
	return dwStyle;
}

//===========================================================================
void CMySlider::OnSetFocus(CWnd* pOldWnd) 
{
	CSliderCtrl::OnSetFocus(pOldWnd);
	
	// to restore buttons
	GetParent()->SetFocus();
	GetParent()->PostMessage(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0),
		(LPARAM)GetSafeHwnd());
}

//===========================================================================
BOOL CMySlider::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CSliderCtrl::OnNotify(wParam, lParam, pResult);
}

//===========================================================================
bool CMySlider::IsTrack()
{
	return bTrack;
}

//===========================================================================
void CMySlider::SetTrack(bool bYes)
{
	bTrack = bYes;
}

////////////////////////////////////////////////////////////////////////
void CMySlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bDragging = true;
	m_bDragChanged = false;
	SetCapture();
	SetFocus();
	if (SetThumb(point)) {
		m_bDragChanged = true;
		PostMessageToParent(TB_THUMBTRACK);
	}
}


//===========================================================================
void CMySlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
		if (SetThumb(point)) {
			m_bDragChanged = true;
			PostMessageToParent(TB_THUMBTRACK);
		}
	else
		CSliderCtrl::OnMouseMove(nFlags, point);
}


//===========================================================================
void CMySlider::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bDragging)
	{
		m_bDragging = false;
		::ReleaseCapture();
		if (SetThumb(point)) {
			PostMessageToParent(TB_THUMBTRACK);
			m_bDragChanged = true;
		}
		if (m_bDragChanged) {
			PostMessageToParent(TB_THUMBPOSITION);
			m_bDragChanged = false;
		}
	}
	else
		CSliderCtrl::OnLButtonUp(nFlags, point);
}

//===========================================================================
bool CMySlider::SetThumb(const CPoint& point)
{
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax()+1;
	CRect rc; GetChannelRect(rc);
	double dPos, dCorrectionFactor = 0.0;
/*
	if (GetStyle() & TBS_VERT) 
		// note: there is a bug in GetChannelRect, it gets the orientation of the rectangle mixed up
		dPos = (double)(point.y - rc.left)/(rc.right - rc.left);
	else
		dPos = (double)(point.x - rc.left)/(rc.right - rc.left);
*/
	// !!!!! только для горизонтального слайдера
	dPos = (double)(point.x - rc.left)/(rc.right - rc.left);
	
	// This correction factor is needed when you click inbetween tick marks
	// so that the thumb will move to the nearest one
	dCorrectionFactor = 0.5 *(1-dPos) - 0.5 *dPos;
	int nNewPos = (int)(nMin + (nMax-nMin)*dPos + dCorrectionFactor);

	const bool bChanged = (nNewPos != GetPos());
	if(bChanged)
		SetPos(nNewPos);

	return bChanged;
}

//===========================================================================
void CMySlider::PostMessageToParent(const int nTBCode)
{
	CWnd* pWnd = GetParent();
	if(pWnd) pWnd->PostMessage(WM_HSCROLL, MAKEWPARAM(nTBCode, GetPos()),
		(LPARAM)GetSafeHwnd());
}

//===========================================================================
bool CMySlider::IsDragging()
{
	return m_bDragging;
}

//===========================================================================
void CMySlider::ShowThumb(bool bVisible)
{
	if(bVisible == true)
		this->ModifyStyle(TBS_NOTHUMB|WS_DISABLED, 0, 0);
	else
		this->ModifyStyle(0, TBS_NOTHUMB|WS_DISABLED, 0);
}

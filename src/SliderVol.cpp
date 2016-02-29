#include "stdafx.h"
#include "SliderVol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSliderVol, CSliderCtrl)
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////
CSliderVol::CSliderVol()
	:m_is_dragging(false)
	,m_is_updating(false)
{
	m_brush.CreateSolidBrush(RGB(0, 0, 0));
	m_pen.CreatePen(PS_SOLID, 1, RGB(128, 128, 128)); // dark gray
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

afx_msg void CSliderVol::OnCustomDraw(NMHDR* pNotifyStruct, LRESULT* result)
{
	//changing style of thumb control, for details see:
	//http://www.codeproject.com/Articles/8985/Customizing-the-Appearance-of-CSliderCtrl-Using-Cu
	/*
    NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)pNotifyStruct;

    if (nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        //return CDRF_NOTIFYITEMDRAW so that we will get subsequent 
        //CDDS_ITEMPREPAINT notifications
        *result = CDRF_NOTIFYITEMDRAW ;
        return;
    }
    else if (nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        if (nmcd.dwItemSpec == TBCD_THUMB)
        {
			//nmcd.rc.right += 20;
			//nmcd.rc.bottom += 20;

            CDC* pDC = CDC::FromHandle(nmcd.hdc);
            //pDC->SelectObject(m_brush);
            pDC->SelectObject(m_pen);
			const int centerLeft = nmcd.rc.left + (nmcd.rc.right-nmcd.rc.left)/2;
			pDC->MoveTo(centerLeft, nmcd.rc.top+2);
			pDC->LineTo(centerLeft, nmcd.rc.bottom-2);
            pDC->Detach();

            *result = CDRF_SKIPDEFAULT;
        }
    } 
	*/
}
//---------------------------------------------------------------------------

float CSliderVol::GetVolume() const
{
	return float(GetPos()-GetRangeMin()) / (GetRangeMax()-GetRangeMin());
}
//------------------------------------------------------------------------------

void CSliderVol::SetVolume(float volumeLevel)
{
	volumeLevel = max(0.0f, min(volumeLevel, 1.0f));
	SetPos(GetRangeMin() + int((GetRangeMax()-GetRangeMin()) * volumeLevel));
}
//------------------------------------------------------------------------------


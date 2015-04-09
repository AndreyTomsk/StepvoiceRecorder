//===========================================================================
#include "stdafx.h"
#include "..\..\mp3_recorder.h"
#include "..\..\Debug.h"
#include "bitmapbtn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CBitmapBtn, CWnd)
	//{{AFX_MSG_MAP(CBitmapBtn)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//===========================================================================

CBitmapBtn::CBitmapBtn()
	:m_checked(false)
	,m_enabled(true)
	,m_mouseInside(false)
	,m_leftButtonPressed(false)
{
}
//===========================================================================

CBitmapBtn::~CBitmapBtn()
{
}
//===========================================================================

BOOL CBitmapBtn::Create(CImageList *pImageList, const POINT &pt, CWnd *pParentWnd, UINT nID)
{
	IMAGEINFO btnImageInfo;

	m_btnIL.Create(pImageList);

	m_btnIL.GetImageInfo(0, &btnImageInfo);
	int nBtnW = btnImageInfo.rcImage.right - btnImageInfo.rcImage.left;
	int nBtnH = btnImageInfo.rcImage.bottom - btnImageInfo.rcImage.top;

	// создаем окно по размерам битмапа кнопки
	return CWnd::Create(NULL, "", WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		CRect(pt.x, pt.y, pt.x+nBtnW, pt.y+nBtnH), pParentWnd, nID);
}
//===========================================================================

void CBitmapBtn::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_mouseInside)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.hwndTrack = this->GetSafeHwnd();
		tme.dwHoverTime = HOVER_DEFAULT;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);

		m_mouseInside = true;
		Invalidate(FALSE);
	}

	CWnd::OnMouseMove(nFlags, point);
}
//===========================================================================

BOOL CBitmapBtn::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CWnd::PreTranslateMessage(pMsg);
}
//===========================================================================

LRESULT CBitmapBtn::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_MOUSELEAVE)
	{
		m_mouseInside = false;
		Invalidate(FALSE);
	}
	return CWnd::WindowProc(message, wParam, lParam);
}
//===========================================================================

void CBitmapBtn::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_enabled)
	{
		m_leftButtonPressed = true;
		Invalidate(FALSE);
	}
	CWnd::OnLButtonDown(nFlags, point);
}
//===========================================================================

void CBitmapBtn::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_enabled)
	{
		m_leftButtonPressed = false;
		SetCheck(!m_checked);

		//Post "clicked" message to parent window.
		const int buttonID = GetDlgCtrlID();
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(buttonID, BN_CLICKED),
			(LPARAM)GetSafeHwnd());
	}
	CWnd::OnLButtonUp(nFlags, point);
}
//===========================================================================

void CBitmapBtn::OnPaint() 
{
	enum ImageID //H - highlighted, D - disabled
	{
		BTN_NORMAL    = 0,
		BTN_NORMAL_H  = 1,
		BTN_NORMAL_D  = 2,
		BTN_PRESSED   = 3,
		BTN_PRESSED_H = 4,
		BTN_PRESSED_D = 5
	}
	imageID;

	if (!m_enabled)
	{
		imageID = m_checked ? BTN_PRESSED_D : BTN_NORMAL_D;
	}
	else
	{
		if (m_checked)
			imageID = m_mouseInside ? BTN_PRESSED_H : BTN_PRESSED;
		else
		{
			imageID = m_mouseInside ? BTN_NORMAL_H : BTN_NORMAL;
			if (m_leftButtonPressed)
				imageID = BTN_PRESSED_H;
		}
	}

	CPaintDC dc(this); // device context for painting
	m_btnIL.Draw(&dc, imageID, CPoint(0, 0), ILD_NORMAL);
}
//===========================================================================

bool CBitmapBtn::IsChecked() const
{
	return m_checked;
}
//===========================================================================

void CBitmapBtn::SetCheck(bool checked)
{
	if (m_checked != checked)
	{
		m_checked = checked;
		Invalidate(FALSE);
	}
}
//===========================================================================

bool CBitmapBtn::IsEnabled() const
{
	return m_enabled;
}
//===========================================================================

void CBitmapBtn::Enable(bool enabled)
{
	if (m_enabled != enabled)
	{
		m_enabled = enabled;
		Invalidate(FALSE);
	}
}
//===========================================================================

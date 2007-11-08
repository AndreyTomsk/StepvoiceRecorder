//===========================================================================
#include "stdafx.h"
#include "..\..\mp3_recorder.h"
#include "bitmapbtn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CBitmapBtn, CWnd)
	//{{AFX_MSG_MAP(CBitmapBtn)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//===========================================================================
CBitmapBtn::CBitmapBtn()
{
	m_nBtnState = BTN_NORMAL;
	m_bMouseInside = false;
	m_bStatePressed = false;
}

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
int CBitmapBtn::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	RECT rt;
	POINT pt;

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	::GetCursorPos(&pt);
	this->GetWindowRect(&rt);
	m_bMouseInside = PtInRect(&rt, pt); // устанавл. флаг "мышь на кнопкой"

	return 0;
}

//===========================================================================
void CBitmapBtn::OnMouseMove(UINT nFlags, CPoint point) 
{
	TRACKMOUSEEVENT tme;

	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.hwndTrack = this->GetSafeHwnd();
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.dwFlags = TME_LEAVE;
	_TrackMouseEvent(&tme);

	if(!m_bMouseInside)
	{
		// добавляем подсветку кнопки
		switch(m_nBtnState)
		{
		case BTN_NORMAL:
			m_nBtnState = BTN_NORMAL_H;
			if(!IsLButtonPressed()) break;
		case BTN_PRESSED:
			m_nBtnState = BTN_PRESSED_H;
			break;
		}

		m_bMouseInside = true;
		this->InvalidateRect(NULL);
		this->UpdateWindow();
	}

	CWnd::OnMouseMove(nFlags, point);
}

//===========================================================================
LRESULT CBitmapBtn::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case WM_MOUSELEAVE:

		// убираем подсветку кнопки при выходе мыши
		switch(m_nBtnState)
		{
		case BTN_PRESSED_H:
			m_nBtnState = BTN_PRESSED;
			if(m_bStatePressed) break;		// см. SetState
			if(!IsLButtonPressed()) break;
		case BTN_NORMAL_H:
			m_nBtnState = BTN_NORMAL;
		}

		m_bMouseInside = false;
		this->InvalidateRect(NULL, false);
		this->UpdateWindow();
		break;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

//===========================================================================
BOOL CBitmapBtn::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CWnd::PreTranslateMessage(pMsg);
}

//===========================================================================
void CBitmapBtn::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!IsDisabled())
	{
		m_nBtnState = (m_bMouseInside) ? BTN_PRESSED_H : BTN_PRESSED;
		InvalidateRect(NULL);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

//===========================================================================
void CBitmapBtn::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int nBtnID;
	CWnd* pParentWnd;

	if(!IsDisabled())
	{
		m_nBtnState = (m_bMouseInside) ? BTN_NORMAL_H : BTN_NORMAL;
		m_bStatePressed = false;
		InvalidateRect(NULL);
		// посылаем сообщение нажатия родительскому окну
		nBtnID = GetDlgCtrlID();
		pParentWnd = GetParent();
		pParentWnd->SendMessage(WM_COMMAND, MAKEWPARAM(nBtnID, BN_CLICKED),
			(LPARAM)GetSafeHwnd());
	}

	CWnd::OnLButtonUp(nFlags, point);
}

//===========================================================================
void CBitmapBtn::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	m_btnIL.Draw(&dc, m_nBtnState, CPoint(0, 0), ILD_NORMAL);
}

//===========================================================================
int CBitmapBtn::IsLButtonPressed()
{
	return ::GetKeyState(VK_LBUTTON)&0x8000;
}

//===========================================================================
void CBitmapBtn::SetState(int nNewState)
{
	if((nNewState < 0) || (nNewState > 5)) // по кол-ву возм. состояний
		return;

	m_nBtnState = (BtnState)nNewState;
	if((m_nBtnState == BTN_PRESSED) && m_bMouseInside)
		m_nBtnState = BTN_PRESSED_H;
	if((m_nBtnState == BTN_NORMAL) && m_bMouseInside)
		m_nBtnState = BTN_NORMAL_H;

	// сохр. флаг "нажатости" кнопки (для перерисовки)
	m_bStatePressed = IsPressed() ? true : false;

	InvalidateRect(NULL);
}

//===========================================================================
int CBitmapBtn::IsPressed()
{
	return (m_nBtnState == BTN_PRESSED)  ||
		   (m_nBtnState == BTN_PRESSED_H)||
		   (m_nBtnState == BTN_PRESSED_D);
}

//===========================================================================
bool CBitmapBtn::IsDisabled()
{
	return (m_nBtnState == BTN_NORMAL_D) ||
		   (m_nBtnState == BTN_PRESSED_D);
}

//===========================================================================

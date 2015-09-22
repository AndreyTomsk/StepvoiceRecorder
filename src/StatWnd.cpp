/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "StatWnd.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CStatWnd, CWnd)
	//{{AFX_MSG_MAP(CStatWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDB_BTN_MON, OnBtnMon)
	ON_BN_CLICKED(IDB_BTN_VAS, OnBtnVas)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CStatWnd::CStatWnd()
{	
	m_wndsize.cx = 0;
	m_wndsize.cy = 0;
}

CStatWnd::~CStatWnd()
{	
	// delete graphic objects
	m_memDC.DeleteDC();
	m_bkDC.DeleteDC();
	m_statDC.DeleteDC();
	m_bmp.DeleteObject();
	m_bkbmp.DeleteObject();
	m_statbmp.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// Message functions
/////////////////////////////////////////////////////////////////////////////
int CStatWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// врубаем подсказки
	EnableToolTips(true);

	// get window size
	m_wndsize.cx = lpCreateStruct->cx;
	m_wndsize.cy = lpCreateStruct->cy;

	// create window view
	CClientDC DC(this);

	m_memDC.CreateCompatibleDC(&DC);
	m_bmp.CreateCompatibleBitmap(&DC, m_wndsize.cx, m_wndsize.cy);
	m_memDC.SelectObject(&m_bmp);
	
	// DC for digital window (stat part) loading
	m_bkDC.CreateCompatibleDC(&DC);
	m_bkbmp.LoadBitmap(IDB_STAT_WND);
	m_bkDC.SelectObject(&m_bkbmp);
	
	// DC for small digits and Mono-Stereo bitmaps
	m_statDC.CreateCompatibleDC(&DC);
	m_statbmp.LoadBitmap(IDB_STATBITMAPS);
	m_statDC.SelectObject(&m_statbmp);

	// fill window with background bitmap
	m_memDC.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_bkDC, 0, 0, SRCCOPY);

	// ***** Создаем кнопки *****
	CBitmap btnBmp;
	CImageList vasIL, monIL;

	vasIL.Create(42, 11, ILC_COLOR24, 0, 6);
	monIL.Create(&vasIL);

	btnBmp.LoadBitmap(IDB_BTN_VAS);
	vasIL.Add(&btnBmp, RGB(255, 0, 255));
	btnBmp.DeleteObject();

	btnBmp.LoadBitmap(IDB_BTN_MON);
	monIL.Add(&btnBmp, RGB(255, 0, 255));

	m_btnVas.Create(&vasIL, CPoint(88, 3), this, IDB_BTN_VAS);
	m_btnMon.Create(&monIL, CPoint(132, 3), this, IDB_BTN_MON);

#ifndef _DEBUG
	if(fsProtect_GetDaysLeft() <= 0)
	{	// дизаблим кнопки
		m_btnVas.Enable(false);
		m_btnMon.Enable(false);
		//m_btnVas.ModifyStyle(WS_VISIBLE, WS_DISABLED);
		//m_btnMon.ModifyStyle(WS_VISIBLE, WS_DISABLED);
	}
#endif
	
	return 0;
}

void CStatWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	dc.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_memDC, 0, 0, SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////
// Interface functions
/////////////////////////////////////////////////////////////////////////////
void CStatWnd::Set(float fF, int iR, int iS, UINT nSize, const CString& szTime)
{
//	Set(fF, iR, iS);			//
//	Set(nSize, szTime);	// 
}

void CStatWnd::Set(int nFreq, int nBitrate, int nStereo)
{
	int nDigits[3] = {0, 0, 0};

	// take digits of nBitrate (in kbps)
	if(nBitrate > 999) nBitrate = 999;
	nDigits[0] = nBitrate % 10, nBitrate /= 10; // get ls
	nDigits[1] = nBitrate % 10, nBitrate /= 10; // get 10s
	nDigits[2] = nBitrate % 10;					// get 100s
	// draw 1s, 10s, 100s
	m_memDC.BitBlt(16, 18, 7, 12, &m_statDC, 7*nDigits[0], 0, SRCCOPY);
	m_memDC.BitBlt( 8, 18, 7, 12, &m_statDC, 7*nDigits[1], 0, SRCCOPY);
	m_memDC.BitBlt( 0, 18, 7, 12, &m_statDC, 7*nDigits[2], 0, SRCCOPY);
	
	// take digits of nFreq (in Herz)
	nFreq /= 100; // 22050 -> 220
	nDigits[0] = nFreq % 10, nFreq /= 10; // get 1s
	nDigits[1] = nFreq % 10, nFreq /= 10; // get 10s
	nDigits[2] = nFreq % 10;			  // get 100s
	// draw 1s, 10s, 100s
	m_memDC.BitBlt(86, 18, 7, 12, &m_statDC, 7*nDigits[0], 0, SRCCOPY);
	m_memDC.BitBlt(73, 18, 7, 12, &m_statDC, 7*nDigits[1], 0, SRCCOPY);
	m_memDC.BitBlt(65, 18, 7, 12, &m_statDC, 7*nDigits[2], 0, SRCCOPY);

	// drawing mono & stereo 
	m_memDC.BitBlt(130, 20, 58, 8, &m_statDC, 0, (nStereo)?20:12, SRCCOPY);

	Invalidate(true);
}

void CStatWnd::Set(UINT nSize, const CString& szTime)
{
	int nDigits[7] = {0, 0, 0, 0, 0, 0, 0}, nOffset, nI = 0;

	// clear file size and time part of stat window
	m_memDC.BitBlt(0, 34, m_wndsize.cx, m_wndsize.cy, &m_bkDC, 0, 34, SRCCOPY);

	// take digits of nSize (in Kb)
	nDigits[0] = nSize % 10, nSize /= 10; // get 1s
	nDigits[1] = nSize % 10, nSize /= 10; // get 10s
	nDigits[2] = nSize % 10, nSize /= 10; // get 100s
	nDigits[3] = nSize % 10, nSize /= 10; // get 1,000s
	nDigits[4] = nSize % 10, nSize /= 10; // get 10,000s
	nDigits[5] = nSize % 10;			  // get 100,000s
	// draw 1s, 10s, 100s, 1000s, 10000s, 100000s
	m_memDC.BitBlt(40, 34, 7, 12, &m_statDC, 7*nDigits[0], 0, SRCCOPY);
	m_memDC.BitBlt(32, 34, 7, 12, &m_statDC, 7*nDigits[1], 0, SRCCOPY);
	m_memDC.BitBlt(24, 34, 7, 12, &m_statDC, 7*nDigits[2], 0, SRCCOPY);
	m_memDC.BitBlt(16, 34, 7, 12, &m_statDC, 7*nDigits[3], 0, SRCCOPY);
	if(nDigits[4]) m_memDC.BitBlt( 8, 34, 7, 12, &m_statDC, 7*nDigits[4], 0, SRCCOPY);
	if(nDigits[5]) m_memDC.BitBlt( 0, 34, 7, 12, &m_statDC, 7*nDigits[5], 0, SRCCOPY);

	// take digits of szTime (like "1:23:59")
	ZeroMemory(&nDigits[0], sizeof(nDigits));
	nOffset = szTime.GetLength();
	while(nOffset--) {
		if(szTime[nOffset] == ':') continue;
		nDigits[nI++] = szTime[nOffset] - 48;
	}
	// draw seconds, minutes, hours
	m_memDC.BitBlt(154, 34, 7, 12, &m_statDC, 7*nDigits[0], 0, SRCCOPY); // sec
	m_memDC.BitBlt(146, 34, 7, 12, &m_statDC, 7*nDigits[1], 0, SRCCOPY); 
	m_memDC.BitBlt(133, 34, 7, 12, &m_statDC, 7*nDigits[2], 0, SRCCOPY); // min
	m_memDC.BitBlt(125, 34, 7, 12, &m_statDC, 7*nDigits[3], 0, SRCCOPY);
	m_memDC.BitBlt(112, 34, 7, 12, &m_statDC, 7*nDigits[4], 0, SRCCOPY); //hours
	if(nDigits[5]) m_memDC.BitBlt(104, 34, 7, 12, &m_statDC, 7*nDigits[5], 0, SRCCOPY);
	if(nDigits[6]) m_memDC.BitBlt( 96, 34, 7, 12, &m_statDC, 7*nDigits[6], 0, SRCCOPY);

	Invalidate(true);
}

/////////////////////////////////////////////////////////////////////////////
void CStatWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));
	CWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CStatWnd::OnNcPaint() 
{
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnNcPaint() for painting messages
}

/////////////////////////////////////////////////////////////////////////////
void CStatWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_LBUTTONDBLCLK, 0, MAKELPARAM(point.x, point.y));
	CWnd::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CStatWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);
	GetParent()->SendMessage(WM_RBUTTONUP, 0, MAKELPARAM(point.x, point.y));
	CWnd::OnRButtonUp(nFlags, point);
}

//===========================================================================
// КНОПКИ (SHR, VAS, M)
//===========================================================================

/////////////////////////////////////////////////////////////////////////////
void CStatWnd::OnBtnMon()
{
#ifndef _DEBUG
	// дизаблим после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	// вызываем функцию обработки нажатия
	CMainFrame* pMainWnd = (CMainFrame *)GetParent();
	pMainWnd->OnBtnMonitoring();
}

/////////////////////////////////////////////////////////////////////////////
void CStatWnd::OnBtnVas()
{
#ifndef _DEBUG
	// дизаблим после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	// вызываем функцию обработки нажатия
	CMainFrame* pMainWnd = (CMainFrame *)GetParent();
	pMainWnd->OnBtnVas();
}

/////////////////////////////////////////////////////////////////////////////

BOOL CStatWnd::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	CMainFrame* pMainWnd = (CMainFrame *)GetParent();
	return pMainWnd->OnToolTipNotify(id, pNMHDR, pResult);
}

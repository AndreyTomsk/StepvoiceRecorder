/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "TimeWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CTimeWnd::CTimeWnd()
{	// init variables
	nCurSec = 0;
	m_bBlink = false;
}

CTimeWnd::~CTimeWnd()
{	// delete graphic objects
	m_memDC.DeleteDC();
	m_bkDC.DeleteDC();
	m_timeDC.DeleteDC();
	m_bmp.DeleteObject();
	m_bkbmp.DeleteObject();
	m_timebmp.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTimeWnd, CWnd)
	//{{AFX_MSG_MAP(CTimeWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
int CTimeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// get window size
	m_wndsize.cx = lpCreateStruct->cx;
	m_wndsize.cy = lpCreateStruct->cy;

	// create window view
	CClientDC DC(this);
	m_memDC.CreateCompatibleDC(&DC);
	m_bmp.CreateCompatibleBitmap(&DC, m_wndsize.cx, m_wndsize.cy);
	m_memDC.SelectObject(&m_bmp);

	// DC for digital window (time part) loading
	m_bkDC.CreateCompatibleDC(&DC);
	m_bkbmp.LoadBitmap(IDB_TIME_WND);
	m_bkDC.SelectObject(&m_bkbmp);

	// DC for digits
	m_timeDC.CreateCompatibleDC(&DC);
	m_timebmp.LoadBitmap(IDB_TIMEBITMAPS);
	m_timeDC.SelectObject(&m_timebmp);

	// clear window (set background bitmap)
	m_memDC.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_bkDC, 0, 0, SRCCOPY);

	return 0;
}

void CTimeWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	dc.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_memDC, 0, 0, SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////
// Interface functions
/////////////////////////////////////////////////////////////////////////////
void CTimeWnd::Reset()
{	// reset seconds counter
	nCurSec = 0;
	SetTime(nCurSec);
}

void CTimeWnd::Increase()
{	// increase seconds counter
	nCurSec += 1;
	SetTime(nCurSec);
}

void CTimeWnd::Decrease()
{	// уменьшаем счетчик секунд
	if(nCurSec > 0)
		nCurSec -= 1;
	SetTime(nCurSec);
}

void CTimeWnd::SetTime(int nHours, int nMinutes, int nSeconds)
{
	SetTime(nHours*24 + nMinutes*60 + nSeconds);
}

void CTimeWnd::SetTime(UINT nSeconds)
{
	int nDigits[7] = {0, 0, 0, 0, 0, 0, 0}, nOffset, nI = 0;
	char szTime[10];

	// clear window (set bk bitmap)
	m_memDC.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_bkDC, 0, 0, SRCCOPY);

	nCurSec = nSeconds;
	Convert(nCurSec, szTime);	// convert seconds into string "x:xx:xx"

	// take digits of szTime
	nOffset = strlen(szTime);
	while(nOffset--) {
		if(szTime[nOffset] == ':') continue;
		nDigits[nI++] = szTime[nOffset] - 48; // переводим строку в число и заполн. массив
	}
	// draw seconds, minutes, hours
	const int D = -12, F = -1;
	m_memDC.BitBlt(80+D, 6+F, 10, 18, &m_timeDC, 10*nDigits[0], 0, SRCCOPY); // sec
	m_memDC.BitBlt(68+D, 6+F, 10, 18, &m_timeDC, 10*nDigits[1], 0, SRCCOPY);
	m_memDC.BitBlt(54+D, 6+F, 10, 18, &m_timeDC, 10*nDigits[2], 0, SRCCOPY); // min
	m_memDC.BitBlt(42+D, 6+F, 10, 18, &m_timeDC, 10*nDigits[3], 0, SRCCOPY);
	m_memDC.BitBlt(28+D, 6+F, 10, 18, &m_timeDC, 10*nDigits[4], 0, SRCCOPY); // hours
	if(nDigits[5]) m_memDC.BitBlt(16+D, 6+F, 10, 18, &m_timeDC, 10*nDigits[5], 0, SRCCOPY);
	if(nDigits[6]) m_memDC.BitBlt( 4+D, 6+F, 10, 18, &m_timeDC, 10*nDigits[6], 0, SRCCOPY);

	Invalidate(true);
}


/////////////////////////////////////////////////////////////////////////////
void CTimeWnd::Convert(UINT nCurSec, char *pszTime)
{
	const char* szPattern[] = {"%d", "0%d"};
	int iHour, iMin, iSec;
	char szMin[3] = "";
	char szSec[3] = "";

	iHour= nCurSec/3600;
	iMin =(nCurSec - iHour*3600)/60;
	iSec = nCurSec - iHour*3600 - iMin*60;

	sprintf(szMin, szPattern[iMin<10], iMin);
	sprintf(szSec, szPattern[iSec<10], iSec);
	sprintf(pszTime, "%d:%s:%s", iHour, szMin, szSec);
}


/////////////////////////////////////////////////////////////////////////////
void CTimeWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	GetParent()->SendMessage(WM_LBUTTONDBLCLK, 0, MAKELPARAM(point.x, point.y));
	CWnd::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CTimeWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));

	CWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CTimeWnd::Blink(bool bBlink)
{

	if(bBlink)
		SetTimer(1, 1000, NULL);
	else
	{
		KillTimer(1);
		SetTime(nCurSec);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CTimeWnd::OnTimer(UINT nIDEvent) 
{
	static bool bShow = false;

	if(!bShow)
	{	m_memDC.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_bkDC, 0, 0, SRCCOPY);
		Invalidate(true);
	}
	else
	{	SetTime(nCurSec);
	}
	bShow = !bShow;

	CWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
void CTimeWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);
	GetParent()->SendMessage(WM_RBUTTONUP, 0, MAKELPARAM(point.x, point.y));
	
	CWnd::OnRButtonUp(nFlags, point);
}

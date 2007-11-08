/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "IcoWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CIcoWnd, CWnd)
	//{{AFX_MSG_MAP(CIcoWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
const int CX_ICON = 14;//9;
const int CY_ICON = 9;

//===========================================================================
CIcoWnd::CIcoWnd()
{
}

//===========================================================================
CIcoWnd::~CIcoWnd()
{
	m_memDC.DeleteDC();
	m_icoDC.DeleteDC();

	m_bmp.DeleteObject();
	m_icobmp.DeleteObject();

	m_bkbrush.DeleteObject();
}

//===========================================================================
int CIcoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ��������� ������ ����
	m_wndsize.cx = lpCreateStruct->cx;
	m_wndsize.cy = lpCreateStruct->cy;

	// ������� �������� ������
	CClientDC DC(this);
	m_memDC.CreateCompatibleDC(&DC);
	m_bmp.CreateCompatibleBitmap(&DC, m_wndsize.cx, m_wndsize.cy);
	m_bkbrush.CreateStockObject(BLACK_BRUSH);
	m_memDC.SelectObject(&m_bmp);
	m_memDC.SelectObject(&m_bkbrush);

	// ������� ��������� �������� ���������� ��� ������
	m_icoDC.CreateCompatibleDC(&DC);
	m_icobmp.LoadBitmap(IDB_PROC_ICONS);
	m_icoDC.SelectObject(&m_icobmp);

	// ������ ������ �� ���������
	SetNewIcon(ICON_STOP);

	return 0;
}

//===========================================================================
void CIcoWnd::OnPaint() 
{
	// ������ ����������� �� ������
	CPaintDC dc(this); // �������� ���������� ��� ���������
	dc.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_memDC, 0, 0, SRCCOPY);
}

//===========================================================================
void CIcoWnd::SetNewIcon(int nID)
{
// ���������� ����� ������.
// ��� ����� �� ������� ������� �����������, ����� �������� ������ ��
// m_icoDC � ����� m_memDC. ����� ����� �������������� ����������� �� ������.

	// ���������� ����������, ���� ����� ��������
	CPoint p((m_wndsize.cx - CX_ICON)/2,
			 (m_wndsize.cy - CY_ICON)/2);
	p.x--;

	// ���������� �������� � ��������� � ��������
	//int nIcoX = nID*CX_ICON + nID;
	int nIcoX = nID*CX_ICON;
	int nIcoY = 14;

	// ������ ������ � ��������� ����
	Clear();
	m_memDC.BitBlt(p.x, p.y, CX_ICON, CY_ICON, &m_icoDC, nIcoX, nIcoY, SRCCOPY);
	InvalidateRect(NULL);
}

//===========================================================================
void CIcoWnd::Clear()
{
	m_memDC.FillRect(CRect(0, 0, m_wndsize.cx, m_wndsize.cy), &m_bkbrush);
}

//===========================================================================
void CIcoWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
// ������������ ������� ������� ����� ������ ����.
// �������� ��������� �������� (.. ��������, ��� ������ ������� ��������).

	GetParent()->SendMessage(WM_LBUTTONDBLCLK, 0, MAKELPARAM(point.x, point.y));
	CWnd::OnLButtonDblClk(nFlags, point);
}

//===========================================================================
void CIcoWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
// ������������ ������� ����� ������ ����.
// �������� ��������� �������� (��� ������ Easy move).

	GetParent()->SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));
	CWnd::OnLButtonDown(nFlags, point);
}

//===========================================================================

void CIcoWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);
	GetParent()->SendMessage(WM_RBUTTONUP, 0, MAKELPARAM(point.x, point.y));
	
	CWnd::OnRButtonUp(nFlags, point);
}

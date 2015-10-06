#include "stdafx.h"
#include "GraphWnd.h"
#include <math.h>
#include <mmsystem.h> //for multimedia timer

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

const int MAXPEAK_TIMER_ID = 123;
const int PLAY_BUFFER_SIZE = 2048;
static float g_play_buffer[PLAY_BUFFER_SIZE];

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGraphWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_GRAPH_PEAKMETER, ID_GRAPH_NONE, OnGraphMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CGraphWnd::CGraphWnd()
	:m_bShowVASMark(false)
	,m_bMaxpeaks(true)
	,m_bMonitoring(true)
	,m_timer_id(0)
	,m_display_mode(E_DISPLAY_PEAKS)
	,m_peaks_func(NULL)
	,m_lines_func(NULL)
	,m_userData(NULL)
{
}
//---------------------------------------------------------------------------

CGraphWnd::~CGraphWnd()
{
}
//---------------------------------------------------------------------------

int CGraphWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	m_wndsize.cx = lpCreateStruct->cx;
	m_wndsize.cy = lpCreateStruct->cy;

	CClientDC DC(this);
	m_memDC.CreateCompatibleDC(&DC);
	m_bmp.CreateCompatibleBitmap(&DC, m_wndsize.cx, m_wndsize.cy);
	m_greenPen.CreatePen(PS_SOLID, 1, RGB(0, 200, 0));
	m_memDC.SelectObject(&m_bmp);		// assign bitmap
	m_memDC.SelectObject(&m_greenPen);	// select line color

	m_skinDC.CreateCompatibleDC(&DC);
	m_bgbmp[0].LoadBitmap(IDB_GWBG_LNP);
	m_bgbmp[1].LoadBitmap(IDB_GWBG_LGP);
	m_bgbmp[2].LoadBitmap(IDB_GWBG_DOTS);
	m_bgbmp[3].LoadBitmap(IDB_GWBG_NONE);
	m_skinDC.SelectObject(&m_bgbmp[0]);

	m_peakDC.CreateCompatibleDC(&DC);
	m_pmbmp[0].LoadBitmap(IDB_GWPM_LN);
	m_pmbmp[1].LoadBitmap(IDB_GWPM_LG);
	m_peakDC.SelectObject(&m_pmbmp[0]);

	m_bkbrush.CreateStockObject(BLACK_BRUSH);

	ResetMaxpeakMarks();
	Clear();

	m_bMaxpeaks = RegistryConfig::GetOption(_T("General\\Show max peaks"), true);
	SetDisplayMode(RegistryConfig::GetOption(_T("General\\Graph Type"), 0));
	return 0;
}
//---------------------------------------------------------------------------

void CGraphWnd::OnDestroy() 
{
	RegistryConfig::SetOption(_T("General\\Graph Type"), int(m_display_mode));
	RegistryConfig::SetOption(_T("General\\Show max peaks"), m_bMaxpeaks);
	CWnd::OnDestroy();
}
//---------------------------------------------------------------------------

void CGraphWnd::OnPaint() 
{
	CMyLock lock(m_sync_object);

	CPaintDC dc(this); // device context for painting
	dc.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_memDC, 0, 0, SRCCOPY);
}
//---------------------------------------------------------------------------

BOOL CGraphWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CWnd::PreTranslateMessage(pMsg);
}
//---------------------------------------------------------------------------

void CGraphWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Rotating displays

	DisplayMode l_new_mode = DisplayMode(m_display_mode + 1);
	if (l_new_mode > E_DISPLAY_NONE)
		l_new_mode = E_DISPLAY_PEAKS;

	SetDisplayMode(l_new_mode);
	CWnd::OnLButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------

void CGraphWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	const int markers[] = {ID_GRAPH_PEAKMETER, ID_GRAPH_PEAKMETERDB,
		ID_GRAPH_OSCILLOSCOPE, ID_GRAPH_NONE};

	CMenu GraphMenu, *pGraphMenu;
	GraphMenu.LoadMenu(IDR_GRAPHMENU);
	pGraphMenu = GraphMenu.GetSubMenu(0);
	
	pGraphMenu->CheckMenuItem(markers[m_display_mode], MF_CHECKED);

	ClientToScreen(&point);
	pGraphMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,
		point.x, point.y, this);
	
	CWnd::OnRButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------

void CGraphWnd::OnGraphMenu(UINT nID)
{
	switch (nID)
	{
	case ID_GRAPH_PEAKMETER:
		SetDisplayMode(E_DISPLAY_PEAKS);
		break;
	case ID_GRAPH_PEAKMETERDB:
		SetDisplayMode(E_DISPLAY_PEAKS_DB);
		break;
	case ID_GRAPH_OSCILLOSCOPE:
		SetDisplayMode(E_DISPLAY_LINES);
		break;
	case ID_GRAPH_NONE:
		SetDisplayMode(E_DISPLAY_NONE);
		break;
	}
}
//---------------------------------------------------------------------------

void CGraphWnd::Update(char* /*pSndBuf*/, int /*nBufSize*/)
{
	ClearWindow();
	switch (m_display_mode)
	{
	case E_DISPLAY_PEAKS:
	case E_DISPLAY_PEAKS_DB:
		DrawPeaks();
		break;
	case E_DISPLAY_LINES:
		DrawLines();
		break;
	default:
		return;
	}
	InvalidateRect(NULL);
}
//---------------------------------------------------------------------------

void CGraphWnd::ClearWindow()
{
	CMyLock lock(m_sync_object);

	CRect r;
	this->GetClientRect(&r);

	m_memDC.FillRect(&r, &m_bkbrush);
	m_memDC.BitBlt(0, 0, r.Width(), r.Height(), &m_skinDC, 0, 0, SRCCOPY);
	DrawVASMark();
}
//---------------------------------------------------------------------------

void CGraphWnd::Clear()
{
	ClearWindow();
	InvalidateRect(NULL);
}
//---------------------------------------------------------------------------

float CGraphWnd::GetPeakLevel(int a_channel) const
{
	return m_peaks_func ? m_peaks_func(a_channel, m_userData) : 0;
}
//---------------------------------------------------------------------------

int CGraphWnd::GetLinesLevel(int a_channel, float* a_buffer, int a_size)
{
	return m_lines_func ? m_lines_func(a_channel, a_buffer, a_size, m_userData) : 0;
}
//---------------------------------------------------------------------------

void CGraphWnd::DrawPeaks()
{
	const int START_Y[2] = {5, m_wndsize.cy/2 + 10};
	const int END_Y[2] =  {13, m_wndsize.cy/2 + 18};
	const float MAXPEAK = 1;
	const int GRAPH_MAXDB = 60;

	for (int channel = 0; channel < 2; channel++)
	{
		double l_cur_peak = GetPeakLevel(channel);

		if (m_display_mode == E_DISPLAY_PEAKS_DB)
		{
			if (l_cur_peak > 0)
			{
				l_cur_peak = 20 * log10(l_cur_peak / MAXPEAK); //is < 0
				l_cur_peak = m_wndsize.cx-6 + l_cur_peak/GRAPH_MAXDB * (m_wndsize.cx-6);
			}
		}
		else // linear
		{
			l_cur_peak = l_cur_peak / MAXPEAK * (m_wndsize.cx - 6);
		}

		double& l_old_peak = m_old_peaks[channel];
		if (l_cur_peak < l_old_peak)
		{
			l_cur_peak = (l_cur_peak >= 0) ? l_old_peak - 4 : 3;
		}
		l_old_peak = l_cur_peak;
		m_memDC.BitBlt(3, START_Y[channel], (int)l_cur_peak, END_Y[channel],
			&m_peakDC, 0, 0, SRCCOPY);

		if (m_bMaxpeaks)
		{
			double& l_max_peak = m_max_peaks[channel];
			if (l_cur_peak > l_max_peak)
			{
				l_max_peak = l_cur_peak;
				KillTimer(MAXPEAK_TIMER_ID);
				SetTimer(MAXPEAK_TIMER_ID, 3000, NULL);
			}
			m_memDC.BitBlt(int(3 + l_max_peak-2), START_Y[channel], 2,
				END_Y[channel], &m_peakDC, int(l_max_peak-2), 0, SRCCOPY);
		}
	} // for

	if (m_bShowVASMark)
	{
		DrawVASMark();
	}
}
//---------------------------------------------------------------------------

void CGraphWnd::DrawLines()
{
	for (int channel = 0; channel < 2; channel++)
	{
		int l_numbers = GetLinesLevel(channel, g_play_buffer, PLAY_BUFFER_SIZE);
		ASSERT(l_numbers <= PLAY_BUFFER_SIZE);

		const int CX_START = 3;
		const int CX_END   = 3;
		const int START_POS_Y = (1 + 2*channel) * m_wndsize.cy / 4;
		const int DIVIDER = l_numbers / (m_wndsize.cx - CX_END);
		//ASSERT(DIVIDER > 0);

		int nDrawPosY = START_POS_Y + int(g_play_buffer[0] * m_wndsize.cy / 2);
		
		m_memDC.MoveTo(CX_START, nDrawPosY);
		for (int j = CX_START + 1; j < m_wndsize.cx - CX_END; j++)
		{
			float l_level = g_play_buffer[j*DIVIDER];
			nDrawPosY = START_POS_Y + int(l_level * m_wndsize.cy / 2);
			m_memDC.LineTo(j, nDrawPosY);	// j++ !!!
		}
	}
}
//---------------------------------------------------------------------------

void CGraphWnd::ShowVASMark(bool a_enabled, double a_threshold)
{
	if (a_threshold)
	{
		//const int MAXPEAK = GetMaxPeakValue();
		const float MAXPEAK = 1;

		m_vas_marks[0] = MAXPEAK * pow(10, a_threshold / 20);
		m_vas_marks[0] = m_vas_marks[0] / MAXPEAK * (m_wndsize.cx - 6);
		m_vas_marks[0] = 2 + (m_vas_marks[0] == 0) ? 1 : m_vas_marks[0];

		m_vas_marks[1] = 2 + m_wndsize.cx-6 + a_threshold/60 * (m_wndsize.cx-6);
	}

	m_bShowVASMark = a_enabled;
	Clear();
}
//---------------------------------------------------------------------------

void CGraphWnd::DrawVASMark()
{
	CMyLock lock(m_sync_object);

	if ((m_display_mode != E_DISPLAY_PEAKS &&
		m_display_mode != E_DISPLAY_PEAKS_DB) || !m_bShowVASMark)
	{
		return;
	}
	const int START_X[] = {0, 0};
	const int START_Y[] = {5, 36};
	const int POS_OFFSET= (m_display_mode == E_DISPLAY_PEAKS) ?
		(int)m_vas_marks[0] : (int)m_vas_marks[1];
	
	CPen NewPen, *pOldPen;
	NewPen.CreatePen(PS_SOLID, 2, RGB(0, 64, 0));
	
	pOldPen = m_memDC.SelectObject(&NewPen);
	for (int i = 0; i < 2; i++)
	{
		m_memDC.MoveTo(START_X[i] + POS_OFFSET, START_Y[i]);
		m_memDC.LineTo(START_X[i] + POS_OFFSET, START_Y[i] + 7);
	}
	m_memDC.SelectObject(pOldPen);
}
//---------------------------------------------------------------------------

void CGraphWnd::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == MAXPEAK_TIMER_ID)
	{
		ResetMaxpeakMarks();
	}
	CWnd::OnTimer(nIDEvent);
}
//---------------------------------------------------------------------------

void CGraphWnd::ResetMaxpeakMarks()
{
	m_max_peaks[0] = 3;	// peaks window relative
	m_max_peaks[1] = 3;
	KillTimer(MAXPEAK_TIMER_ID);
}
//---------------------------------------------------------------------------

bool CGraphWnd::StartUpdate(
	PEAKS_CALLBACK a_peaks_func, LINES_CALLBACK a_lines_func, void* userData)
{
	ASSERT(a_peaks_func);
	ASSERT(a_lines_func);

	StopUpdate();
	m_peaks_func = a_peaks_func;
	m_lines_func = a_lines_func;
	m_userData = userData;

	m_timer_id = timeSetEvent(25, 25, (LPTIMECALLBACK)&UpdateVisualization,
		(DWORD_PTR)this, TIME_PERIODIC);
	return (m_timer_id != 0);
}
//---------------------------------------------------------------------------

void CGraphWnd::StopUpdate()
{
	if (m_timer_id)
	{
		timeKillEvent(m_timer_id);
		m_timer_id = 0;
	}
	m_peaks_func = NULL;
	m_lines_func = NULL;
	Clear();
}
//---------------------------------------------------------------------------

bool CGraphWnd::SetDisplayMode(int a_new_mode)
{
	CMyLock lock(m_sync_object);

	a_new_mode = max(E_DISPLAY_PEAKS, min(a_new_mode, E_DISPLAY_NONE));
	m_skinDC.SelectObject(&m_bgbmp[a_new_mode]);

	// Checking peak bitmap changed.
	if (a_new_mode == E_DISPLAY_PEAKS || a_new_mode == E_DISPLAY_PEAKS_DB)
	{
		m_peakDC.SelectObject(
			(a_new_mode == E_DISPLAY_PEAKS) ? &m_pmbmp[0] : &m_pmbmp[1]);
	}
	m_display_mode = DisplayMode(a_new_mode);
	m_old_peaks[0] = 0;
	m_old_peaks[1] = 0;
	ResetMaxpeakMarks();
	Clear();
	return true;
}
//---------------------------------------------------------------------------

void CALLBACK CGraphWnd::UpdateVisualization(UINT /*a_timer_id*/,UINT /*a_msg*/,
	DWORD_PTR a_user_ptr, DWORD_PTR /*a_dw1*/, DWORD_PTR /*a_dw2*/)
{
	CGraphWnd* l_graph_wnd = (CGraphWnd*)a_user_ptr;
	ASSERT(l_graph_wnd);

	CMyLock lock(l_graph_wnd->m_sync_object);
	l_graph_wnd->Update(NULL, 0);
}
//---------------------------------------------------------------------------

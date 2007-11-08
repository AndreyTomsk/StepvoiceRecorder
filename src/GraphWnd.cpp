////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "GraphWnd.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGraphWnd, CWnd)
	//{{AFX_MSG_MAP(CGraphWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_GRAPH_MAXPEAKS, OnGraphMaxpeaks)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_GRAPH_PEAKMETER, ID_GRAPH_NONE, OnGraphMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_GRAPH_PEAKMETER, ID_GRAPH_NONE, OnUpdateGraphMenu)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////
enum Channels {LEFT_CHANNEL, RIGHT_CHANNEL};
enum VisTypes {DRAW_PEAKS, DRAW_PEAKS_DB, DRAW_LINES, DRAW_NONE};

#define MAXPEAK_TIMER_ID 123

///////////////////////////////////////////////////////////////////////
CGraphWnd::CGraphWnd() : m_kX(0), m_kY(0), m_nSelector(0)
{
	ZeroMemory(&m_si, sizeof(SOUND_INFO));

	m_fVASThreshold = -90;
	m_bShowVASMark  = false;
	m_bMaxpeaks = true;
}

//--------------------------------------------------------------------
CGraphWnd::~CGraphWnd()
{
}

//====================================================================
// MESSAGE PROCESSING
//====================================================================
int CGraphWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
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

	m_wndRect.left	= 0;
	m_wndRect.top	= 0;
	m_wndRect.right	= m_wndsize.cx;
	m_wndRect.bottom= m_wndsize.cy;

	ResetMaxpeakMarks();
	Clear();
	return 0;
}

//--------------------------------------------------------------------
void CGraphWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	dc.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_memDC, 0, 0, SRCCOPY);
}

//--------------------------------------------------------------------
void CGraphWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// visualization - next by list
	m_nSelector = (m_nSelector == sizeof(VisTypes)-1) ? DRAW_PEAKS : m_nSelector+1;

	// peak bitmap change
	if(m_nSelector == DRAW_PEAKS || m_nSelector == DRAW_PEAKS_DB)
	{
		m_peakDC.SelectObject((m_nSelector == DRAW_PEAKS) ?
			&m_pmbmp[0] : &m_pmbmp[1]);
	}
	
	ResetMaxpeakMarks();
	Clear();

	CWnd::OnLButtonDown(nFlags, point);
}

//--------------------------------------------------------------------
void CGraphWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	const int markers[] = {ID_GRAPH_PEAKMETER, ID_GRAPH_PEAKMETERDB,
		ID_GRAPH_OSCILLOSCOPE, ID_GRAPH_NONE};

	CMenu GraphMenu, *pGraphMenu;

	GraphMenu.LoadMenu(IDR_GRAPHMENU);
	pGraphMenu = GraphMenu.GetSubMenu(0);
	pGraphMenu->CheckMenuItem(markers[m_nSelector], MF_CHECKED);
	if (m_bMaxpeaks)
		pGraphMenu->CheckMenuItem(ID_GRAPH_MAXPEAKS, MF_CHECKED);

	ClientToScreen(&point);
	pGraphMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,
		point.x, point.y, this);
	
	CWnd::OnRButtonDown(nFlags, point);
}

//--------------------------------------------------------------------
void CGraphWnd::OnGraphMenu(UINT nID)
{
	switch(nID)	{
	case ID_GRAPH_PEAKMETER:	m_nSelector = DRAW_PEAKS;	break;
	case ID_GRAPH_PEAKMETERDB:	m_nSelector = DRAW_PEAKS_DB;break;
	case ID_GRAPH_OSCILLOSCOPE:	m_nSelector = DRAW_LINES;	break;
	case ID_GRAPH_NONE:			m_nSelector = DRAW_NONE;	break;
	default:					m_nSelector = DRAW_PEAKS;
	}

	// peak bitmap change
	if(m_nSelector == DRAW_PEAKS || m_nSelector == DRAW_PEAKS_DB)
	{
		m_peakDC.SelectObject((m_nSelector == DRAW_PEAKS) ?
			&m_pmbmp[0] : &m_pmbmp[1]);
	}

	ResetMaxpeakMarks();
	Clear();
}

//--------------------------------------------------------------------
void CGraphWnd::OnGraphMaxpeaks() 
{
	m_bMaxpeaks = !m_bMaxpeaks;
	if (m_bMaxpeaks == false) {
		ResetMaxpeakMarks();
	}
}

//--------------------------------------------------------------------
void CGraphWnd::OnUpdateGraphMenu(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(TRUE);
}

//====================================================================
// INTERFACE FUNCTIONS 
//====================================================================
void CGraphWnd::SetConfig(SOUND_INFO* psi)
{
	memcpy(&m_si, psi, sizeof(SOUND_INFO));

	// height compression ratio
	int nMaxBufValue = (m_si.nBits == 16) ? 32767 : 128;
	m_kY  = (float)m_wndsize.cy / nMaxBufValue;
	m_kY /= 2;	// for stereo channels
	ResetMaxpeakMarks();
}

//--------------------------------------------------------------------
void CGraphWnd::Update(char* pSndBuf, int nBufSize)
{
	int nBufBlockSize = (m_si.nBits/8) * (m_si.nStereo+1);
	m_kX = (float(nBufSize-1)/nBufBlockSize) / m_wndsize.cx;
	
	if(!nBufBlockSize)
		return;

	Clear();

	switch(m_nSelector)
	{
/*	case DRAW_PEAKS:
	case DRAW_PEAKS_DB:
		
		DrawPeaks(pSndBuf, nBufSize, LEFT_CHANNEL,
			(m_nSelector == DRAW_PEAKS) ? 0 : 1);
		DrawPeaks(pSndBuf, nBufSize, RIGHT_CHANNEL,
			(m_nSelector == DRAW_PEAKS) ? 0 : 1);
		
		if(!m_bTimerOn)
		{
			SetTimer(PEAK_TIMER_ID, 60, NULL);
			m_bTimerOn = true;
		}
		break;
*/
	case DRAW_PEAKS:
		DrawPeaks(pSndBuf, nBufSize, LEFT_CHANNEL, 0);
		DrawPeaks(pSndBuf, nBufSize, RIGHT_CHANNEL, 0);
		break;
	case DRAW_PEAKS_DB:
		DrawPeaks(pSndBuf, nBufSize, LEFT_CHANNEL, 1);
		DrawPeaks(pSndBuf, nBufSize, RIGHT_CHANNEL, 1);
		break;
	case DRAW_LINES:
		DrawLines(pSndBuf, nBufSize, LEFT_CHANNEL, 0);
		DrawLines(pSndBuf, nBufSize, RIGHT_CHANNEL, 0);
		break;
	default:
		return;
		/*
		if(m_bTimerOn)
		{
			KillTimer(PEAK_TIMER_ID);
			m_bTimerOn = false;
		}
		*/
	}

	InvalidateRect(NULL);
}

//--------------------------------------------------------------------
void CGraphWnd::Clear()
{
	m_memDC.FillRect(&m_wndRect, &m_bkbrush);

	CBitmap* pOldBmp = m_skinDC.SelectObject(&m_bgbmp[m_nSelector]);
	m_memDC.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_skinDC, 0, 0, SRCCOPY);
	if(m_bShowVASMark && (m_nSelector == DRAW_PEAKS || m_nSelector == DRAW_PEAKS_DB))
		DrawVASMark();
	InvalidateRect(NULL);
	m_skinDC.SelectObject(pOldBmp);
}

//====================================================================
// DRAW GRAPHS AND PEAKS
//====================================================================
int CGraphWnd::GetPeakLevel(char* pSndBuf, int nBufSize, int nChannel)
{
	int nBlockSize = (m_si.nBits/8) * (m_si.nStereo+1);
	const int nPeakNum   = int(m_si.nFreq * 0.006);	// количество пиков для анализа
	int nCurPeak, nMaxPeak = (m_si.nBits == 16) ? 0 : 128;

	if(!nBufSize || !pSndBuf)
		return nMaxPeak;

	if(!m_si.nStereo)
		nChannel = LEFT_CHANNEL;

	// get last block offset
	int nOffset = (nBufSize - 2*nBlockSize);
	if(m_si.nBits == 16)
	{
		nOffset /= sizeof(short);
		nBlockSize /= sizeof(short);
	}
	// adjust offset to the block start
	nOffset = (nOffset/nBlockSize)*nBlockSize;

	// find max peak
	for(int i=nPeakNum; i>0 && nOffset>0; i--, nOffset-=nBlockSize)
	{
		if(m_si.nBits == 16)
			nCurPeak = abs(*((short*)pSndBuf + nOffset + nChannel));
		else
			nCurPeak = abs(*((BYTE *)pSndBuf + nOffset + nChannel));
		
		if(nCurPeak > nMaxPeak)
			nMaxPeak = nCurPeak;
	}

	return nMaxPeak;
}

//--------------------------------------------------------------------
void CGraphWnd::DrawPeaks(char* pSndBuf, int nBufSize, int nChannel, int nMode)
{
	// get peak draw coordinates
	const int nStartY = ((nChannel == LEFT_CHANNEL) ? 5 : m_wndsize.cy/2 + 10);
	const int nEndY	= nStartY + 8;
	const int nMaxPeak = (m_si.nBits == 16) ? 32767 : 128;

	int& max = (nChannel == LEFT_CHANNEL) ? m_maxpeakMarkL : m_maxpeakMarkR;
	static int nOldPeakL = 0, nOldPeakR = 0;
	int& nOldPeak = (nChannel == LEFT_CHANNEL) ? nOldPeakL : nOldPeakR;
	int nCurPeak = GetPeakLevel(pSndBuf, nBufSize, nChannel);

	// get peak value (window relative)
	if(nMode == 1)
	{	// logarithmic
		if(nCurPeak <= 0)
			nCurPeak = 1;
		nCurPeak = int(20*log10(float(nCurPeak)/nMaxPeak)); // less than 0
		nCurPeak = int(m_wndsize.cx-6 + float(nCurPeak)/60 * (m_wndsize.cx-6));
	}
	else
	{	// linear
		nCurPeak = int((float)nCurPeak/nMaxPeak * (m_wndsize.cx-6)); // cx = 105
	}

	if (nCurPeak < nOldPeak)
		nCurPeak = (nCurPeak >= 3) ? nOldPeak-3 : 3;
	nOldPeak = nCurPeak;
	m_memDC.BitBlt(3, nStartY, nCurPeak, nEndY, &m_peakDC, 0, 0, SRCCOPY);

	if (m_bMaxpeaks) {
		if (nCurPeak > max) {
			max = nCurPeak;
			KillTimer(MAXPEAK_TIMER_ID);
			SetTimer(MAXPEAK_TIMER_ID, 3000, NULL);
		}
		if (max > (m_wndsize.cx - 6)) // cx = 105, peaks bitmap width = 99
			max = m_wndsize.cx - 6;
		m_memDC.BitBlt(3 + max-2, nStartY, 2, nEndY, &m_peakDC, max-2, 0, SRCCOPY);
	}

	if (m_bShowVASMark)
		DrawVASMark();
}

//--------------------------------------------------------------------
void CGraphWnd::DrawLines(char* pSndBuf, int nBufSize, int nChannel, int nMode)
{
	const int CX_START  = 3, CX_END = 3;
	const int nStartPosY = (1 + 2*nChannel)*m_wndsize.cy / 4;
	int nDrawPosY;

	int nBlock = (m_si.nBits/8)*(m_si.nStereo+1);
	if(!nBlock)
		return;

	// move to start draw point
	if(m_si.nBits == 8)
		nDrawPosY = nStartPosY + int(*((BYTE *)pSndBuf  + (int(m_kX*CX_START)/nBlock)*nBlock + nChannel) * m_kY);
	else
		nDrawPosY = nStartPosY + int(*((short *)pSndBuf + (int(m_kX*CX_START)/nBlock)*nBlock + nChannel) * m_kY);
	m_memDC.MoveTo(CX_START, nDrawPosY);

	// drawing
	for(int i = CX_START+1; i < m_wndsize.cx - CX_END; i++)
	{
		if(m_si.nBits == 8)
			nDrawPosY = nStartPosY + int( *((BYTE *)pSndBuf  + (int(m_kX*i)/nBlock)*nBlock + nChannel) * m_kY);
		else
		{
			nDrawPosY = nStartPosY + int( *((short *)pSndBuf + (int(m_kX*i)/nBlock)*nBlock + nChannel) * m_kY);
			//nCurPeak =  abs( *((short *)pSndBuf + (int(m_kX*i)/nBlock)*nBlock + nChannel) );
			//if(nCurPeak > nMaxPeak)
			//	nMaxPeak = nCurPeak;
		}
		m_memDC.LineTo(i, nDrawPosY);
	}
	//nBlock++;
}

BOOL CGraphWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CWnd::PreTranslateMessage(pMsg);
}


//====================================================================
double CGraphWnd::GetMaxPeakdB(char *pSndBuf, int nBufSize, int nChannel)
{
	const int nMaxPeak = 32767;
	int		  nCurPeak;
	double	  dCurPeakdB;

	nCurPeak = GetPeakLevel(pSndBuf, nBufSize, nChannel);
	dCurPeakdB = 20*log10(double(nCurPeak)/nMaxPeak);
	
	return dCurPeakdB;
}

//====================================================================
void CGraphWnd::ShowVASMark(double fThreshold)
{
	m_fVASThreshold = fThreshold;
	m_bShowVASMark  = true;
	Clear();
}

//====================================================================
void CGraphWnd::HideVASMark()
{
	m_bShowVASMark = false;
	Clear();
}

//====================================================================
void CGraphWnd::DrawVASMark()
{
	CPen NewPen, *pOldPen;
	const int START_X[] = {0, 0};
	const int START_Y[] = {5, 36};
	const int HEIGHT	= 7;
	const int MAXPEAK	= (m_si.nBits == 16) ? 32767 : 128;
	int nOffset = 0;
	
	NewPen.CreatePen(PS_SOLID, 2, RGB(0, 64, 0));
	pOldPen = m_memDC.SelectObject(&NewPen);

	// get mark offset relative to graph
	// nCurPeak = int(m_wndsize.cx-6 + float(nCurPeak)/60 * (m_wndsize.cx-6));
	if (m_nSelector == DRAW_PEAKS) {
		nOffset = int(MAXPEAK * pow(10, (m_fVASThreshold/20)));
		nOffset = int((float)nOffset/MAXPEAK * (m_wndsize.cx-6));
		nOffset = (nOffset == 0) ? 1 : nOffset;
	}
	else {
		nOffset = int(m_wndsize.cx-6 + m_fVASThreshold/60 * (m_wndsize.cx-6));
		//nOffset-= 1;
	}
	nOffset+=2;

	for (int i = 0; i < 2; i++) {
		m_memDC.MoveTo(START_X[i] + nOffset, START_Y[i]);
		m_memDC.LineTo(START_X[i] + nOffset, START_Y[i] + HEIGHT);
	}

	m_memDC.SelectObject(pOldPen);
}

//====================================================================
void CGraphWnd::SetGraphType(int nGraphType)
{
	OnGraphMenu(nGraphType);
}

//====================================================================
int CGraphWnd::GetGraphType()
{
	int nGraphType = ID_GRAPH_PEAKMETER;

	switch(m_nSelector)	{
	case DRAW_PEAKS:	nGraphType = ID_GRAPH_PEAKMETER;	break;
	case DRAW_PEAKS_DB:	nGraphType = ID_GRAPH_PEAKMETERDB;	break;
	case DRAW_LINES:	nGraphType = ID_GRAPH_OSCILLOSCOPE;	break;
	case DRAW_NONE:		nGraphType = ID_GRAPH_NONE;			break;
	}

	return nGraphType;
}

//====================================================================
void CGraphWnd::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == MAXPEAK_TIMER_ID)
		ResetMaxpeakMarks();
	CWnd::OnTimer(nIDEvent);
}

//====================================================================
void CGraphWnd::ResetMaxpeakMarks()
{
	m_maxpeakMarkL = 3; // peaks window relative
	m_maxpeakMarkR = 3;
	KillTimer(MAXPEAK_TIMER_ID);
}

//====================================================================
void CGraphWnd::SetMaxpeaks(bool bEnabled)
{
	m_bMaxpeaks = bEnabled;
}

//====================================================================
bool CGraphWnd::GetMaxpeaks()
{
	return m_bMaxpeaks;
}

//====================================================================

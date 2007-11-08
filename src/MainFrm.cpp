/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <winuser.h>
#include "stdafx.h"
#include "Interface\Buttons\BitmapBtn.h"

#include "MP3_Recorder.h"
#include "MainFrm.h"
#include "common.h"

#include "system.h"

/////////////////////////////////////////////////////////////////////////////
//extern ShutDownComputer();

/////////////////////////////////////////////////////////////////////////////
#define WM_ICON_NOTIFY WM_USER+10
static const UINT UWM_ARE_YOU_ME = ::RegisterWindowMessage(_T("UWM_ARE_YOU_ME-{B87861B4-8BE0-4dc7-A952-E8FFEEF48FD3}"));

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_OPT_EM, OnOptEm)
	ON_COMMAND(IDM_OPT_TOP, OnOptTop)
	ON_BN_CLICKED(IDB_BTNOPEN, OnBtnOPEN)
	ON_BN_CLICKED(IDB_BTNPLAY, OnBtnPLAY)
	ON_BN_CLICKED(IDB_BTNSTOP,  OnBtnSTOP)
	ON_BN_CLICKED(IDB_BTNREC,  OnBtnREC)
	ON_BN_CLICKED(IDB_MIX_REC,  OnBtnMIX_REC)
	ON_BN_CLICKED(IDB_MIX_PLAY, OnBtnMIX_PLAY)
	ON_BN_CLICKED(IDB_MIX_INV, OnBtnMIX_INV)
	ON_BN_CLICKED(IDB_MIX_SEL, OnBtnMIX_SEL)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDM_MIX_REC, OnMixRec)
	ON_COMMAND(IDM_MIX_PLAY, OnMixPlay)
	ON_COMMAND(IDA_MIX_REC, OnMixRecA)
	ON_COMMAND(IDA_MIX_PLAY, OnMixPlayA)
	ON_WM_HSCROLL()
	ON_WM_MOVING()
	ON_WM_NCLBUTTONDOWN()
	ON_COMMAND(IDM_OPT_COM, OnOptCom)
	ON_COMMAND(IDM_FILE_FINDFILE, OnFileFindfile)
	ON_COMMAND(IDM_FILE_CLOSE, OnFileClose)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(IDM_SOUND_REC, OnUpdateSoundRec)
	ON_UPDATE_COMMAND_UI(IDM_OPT_TOP, OnUpdateOptTop)
	ON_UPDATE_COMMAND_UI(IDM_OPT_EM, OnUpdateOptEm)
	ON_COMMAND(IDA_SOUND_REC, OnSoundRecA)
	ON_COMMAND(IDA_SOUND_STOP, OnSoundStopA)
	ON_COMMAND(IDA_SOUND_STOP2, OnSoundStopA)
	ON_COMMAND(IDA_SOUND_PLAY, OnSoundPlayA)
	ON_COMMAND(IDA_SOUND_BEGIN, OnSoundBeginA)
	ON_COMMAND(IDA_SOUND_REW, OnSoundRewA)
	ON_COMMAND(IDA_SOUND_FF, OnSoundFfA)
	ON_COMMAND(IDA_SOUND_END, OnSoundEndA)
	ON_COMMAND(IDA_FILE_CREATEOPEN, OnFileCreateopenA)
	ON_COMMAND(IDM_SOUND_BEGIN, OnSoundBegin)
	ON_COMMAND(IDM_SOUND_REW, OnSoundRew)
	ON_COMMAND(IDM_SOUND_FF, OnSoundFf)
	ON_COMMAND(IDM_SOUND_END, OnSoundEnd)
	ON_WM_DROPFILES()
	ON_COMMAND(IDM_FILE_DELETE, OnFileDelete)
	ON_COMMAND(IDM_FILE_CLEAR, OnFileClear)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_COMMAND(IDM_OPT_SNDDEV, OnOptSnddev)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(IDM_TRAY_PLAY, OnUpdateTrayPlay)
	ON_UPDATE_COMMAND_UI(IDM_TRAY_REC, OnUpdateTrayRec)
	ON_COMMAND(ID_STAT_PREF, OnStatPref)
	ON_COMMAND(IDA_VOL_UP, OnVolUpA)
	ON_COMMAND(IDA_VOL_DOWN, OnVolDownA)
	ON_COMMAND(IDM_FILE_CREATEOPEN, OnBtnOPEN)
	ON_COMMAND(IDA_FILE_FINDFILE, OnFileFindfile)
	ON_COMMAND(IDA_OPT_COM, OnOptCom)
	ON_COMMAND(IDA_OPT_EM, OnOptEm)
	ON_COMMAND(IDA_OPT_TOP, OnOptTop)
	ON_COMMAND(IDM_SOUND_REC, OnBtnREC)
	ON_COMMAND(IDM_SOUND_STOP, OnBtnSTOP)
	ON_COMMAND(IDM_SOUND_PLAY, OnBtnPLAY)
	ON_COMMAND(IDA_FILE_CLEAR, OnFileClear)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UWM_ARE_YOU_ME, OnAreYouMe)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_SOUND_PLAY, IDM_SOUND_END,
							   OnUpdateSoundPlay)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_COMMAND_RANGE(ID_MIXITEM_REC0, ID_MIXITEM_REC0+50, OnRecMixMenuSelect)
	ON_COMMAND_RANGE(ID_MIXITEM_PLAY0, ID_MIXITEM_PLAY0+3, OnPlayMixMenuSelect)
END_MESSAGE_MAP()

//====================================================================
LRESULT CMainFrame::OnAreYouMe(WPARAM, LPARAM)
{
    return UWM_ARE_YOU_ME;
} // CMainFrame::OnAreYouMe

//====================================================================
CMainFrame* CMainFrame::m_pMainFrame = NULL;

void CMainFrame::_RecordProc(char* pBuf, int dwBufSize)
{
	static char pBufOut[10000];
	char* pBufIn     = pBuf;
	int	  nBufInSize = dwBufSize, nBufOutSize = 0;
	DWORD dwBytesWritten;

	// рисуем график
	if(!m_pMainFrame->IsIconic())
		m_pMainFrame->m_GraphWnd.Update(pBuf, dwBufSize);

	// выходим если работает мониторинг (без записи)
	if(m_pMainFrame->IsMonitoringOnly())
		return;

	// проверяем блок на VAS
	if(m_pMainFrame->m_vas.IsRunning())
	{
		double dParam1, dParam2, dResult;
		dParam1 = m_pMainFrame->m_GraphWnd.GetMaxPeakdB(pBuf, dwBufSize, 0);
		dParam2 = m_pMainFrame->m_GraphWnd.GetMaxPeakdB(pBuf, dwBufSize, 1);
		dResult = (dParam1 > dParam2) ? dParam1 : dParam2;

		m_pMainFrame->m_vas.ProcessThreshold(dResult);
		if(m_pMainFrame->m_vas.CheckVAS()) 
		{
			m_pMainFrame->ProcessVAS(true);
			return;
		}
		else
			m_pMainFrame->ProcessVAS(false);
	}

	// кодирование буфера и запись на диск
	while(m_pMainFrame->m_pEncoder->EncodeChunk(pBufIn, nBufInSize,
		pBufOut, nBufOutSize))
	{
		dwBytesWritten = m_pMainFrame->m_pSndFile->Write(pBufOut,
			nBufOutSize);
		// проверяем на конец диска
		if((int)dwBytesWritten < nBufOutSize)
		{
			AfxMessageBox(IDS_REC_NOSPACE, MB_OK|MB_ICONSTOP);
			m_pMainFrame->PostMessage(WM_COMMAND, IDB_BTNSTOP,
				LONG(m_pMainFrame->m_BtnSTOP.m_hWnd));
			break;
		}
		// зануляем указатель и размер для проведения итераций
		if(pBufIn)
		{
			pBufIn     = NULL;
			nBufInSize = 0;
		}
	}

}

//====================================================================
bool CMainFrame::_PlaybkProc(char* pBuf, int& nBufSize, int nBytesToShow)
{
	static char  cMp3Buffer[8192], *pMp3Buffer = NULL;
	static int   nMp3BufSize = 0;
	static bool  bNeedToRead = true;

	// рисуем график
	if(!m_pMainFrame->IsIconic())
		m_pMainFrame->m_GraphWnd.Update(pBuf, nBytesToShow);

read_again:
	if(bNeedToRead)
	{
		DWORD dwBytesRead = m_pMainFrame->m_pSndFile->Read(cMp3Buffer, 8192);
		// проверяем на конец файла
		if(dwBytesRead == 0)
		{
			m_pMainFrame->PostMessage(WM_COMMAND, IDB_BTNSTOP,
				LONG(m_pMainFrame->m_BtnSTOP.m_hWnd));
			// устанавливаем указатель на начало файла
			//m_pMainFrame->PostMessage(WM_HSCROLL,  MAKEWPARAM(SB_THUMBPOSITION, 0),
			//	(LPARAM)m_pMainFrame->m_SliderTime.m_hWnd);
			//m_pMainFrame->m_SliderTime.SetCurPos(0);
			return false;			
		}
		// успешное чтение - инициализируем буфер
		pMp3Buffer  = cMp3Buffer;
		nMp3BufSize = dwBytesRead;
	}
	// декодируем
	int nTemp = nBufSize;
	bNeedToRead = !m_pMainFrame->m_pDecoder->DecodeChunk(pMp3Buffer,
		nMp3BufSize, pBuf, nBufSize);
	if(!bNeedToRead)
	{
		pMp3Buffer  = NULL;
		nMp3BufSize = 0;
	}
	else
	{
		nBufSize = nTemp;
		goto read_again; // необходимо считать новые данные
	}

	return true;
}

//====================================================================
// КОНСТРУКТОР / ДЕСТРУКТОР
//====================================================================
CMainFrame::CMainFrame()
{	// файлы
	m_pWaveIn	= NULL;
	m_pWaveOut	= NULL;
	m_pEncoder	= NULL;
	m_pDecoder	= NULL;
	m_pSndFile	= NULL;
	m_title		= NULL;

	m_nState	= STOP_STATE;
	m_strDir	= "";

	m_pOptDialog = NULL;

	//m_bOwnerCreated = false;

	m_pMainFrame = this;
	m_bAutoMenuEnable = false;
	LoadFrame(IDR_MAINFRAME, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX);

	// init window snapping
	m_szMoveOffset.cx = 0;
	m_szMoveOffset.cy = 0;
	m_nSnapPixels = 7;
	//::GetWindowRect(::GetDesktopWindow(), &m_rDesktopRect);
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rDesktopRect, 0);

	m_nActiveMixerID = 0;
}

//====================================================================
CMainFrame::~CMainFrame()
{
	SAFE_DELETE(m_title);

	CloseMixerWindows();
}

/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", off)
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bMonitoringBtn = false;

	// в зарегистрированной версии убираем меню "Зарегистрировать"
	REG_CRYPT_BEGIN;
	#ifndef _DEBUG
		CMenu* pMenu = this->GetMenu();
		pMenu->RemoveMenu(4, MF_BYPOSITION);
		CMenu* pHelpSubMenu = pMenu->GetSubMenu(3);
		pHelpSubMenu->EnableMenuItem(5, MF_GRAYED|MF_BYPOSITION);
		this->DrawMenuBar();
	#endif
	REG_CRYPT_END;

	// корректируем вертикальный размер окна
	int nMenuHeight = GetSystemMetrics(SM_CYMENU);
	int nCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
	RECT rW;
	CPoint p(0, 0);
	this->GetWindowRect(&rW);
	this->ClientToScreen(&p);

	int nDifference = p.y - rW.top - nMenuHeight - nCaptionHeight - 3;
	rW.bottom += nDifference;

	this->SetWindowPos(&wndTop, rW.left, rW.top, rW.right-rW.left, rW.bottom-rW.top,
		SWP_NOZORDER | SWP_NOMOVE);

	// инициализируем окна состояний
	m_IcoWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(1, 0, 24, 26), this, IDW_ICO);
	m_TimeWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(24, 0, 106, 25), this, IDW_TIME);
	m_GraphWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(1, 24, 106, 77), this, IDW_GRAPH);
	m_StatWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(106, 0, 282, 50), this, IDW_STAT);
	
	m_IcoWnd.ShowWindow(SW_SHOW);   m_IcoWnd.UpdateWindow();
	m_TimeWnd.ShowWindow(SW_SHOW);  m_TimeWnd.UpdateWindow();
	m_GraphWnd.ShowWindow(SW_SHOW); m_GraphWnd.UpdateWindow();
	m_StatWnd.ShowWindow(SW_SHOW);  m_StatWnd.UpdateWindow();

	// устанавливаем тип графика
	m_GraphWnd.SetGraphType(m_conf.GetConfProg()->nGraphType);
	m_GraphWnd.SetMaxpeaks(m_conf.GetConfProg()->bGraphMaxpeaks != 0);

	// инициализируем слайдеры позиции и микшера
	CRect rT(106, 55, 286, 78);
	CRgn rgnT;
	rgnT.CreateRectRgn(0+6, 0, rT.right-rT.left-6, 26/*rT.bottom-rT.top*/);
	m_SliderTime.Create(WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_BOTH,
		rT, this, IDS_SLIDERTIME);
	m_SliderTime.SetWindowRgn(rgnT, false);
	m_SliderTime.SetRange(0, 1000);
	//m_SliderTime.ShowThumb(false);	// убираем ползунок слайдера

	m_SliderFrame.Create(_T("Static"), "", WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,
		CRect(109, 53, 283, 82), this, IDC_STATIC);

	CRect rV(185, 84, 185+70, 84+23);
	CRgn rgnV;
	rgnV.CreateRectRgn(0+6, 0, rV.right-rV.left-6, 26);
	m_SliderVol.Create(WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_BOTH,
		rV, this, IDS_SLIDERVOL);
	m_SliderVol.SetWindowRgn(rgnV, false);
	m_SliderVol.SetRange(0, 100);
	m_SliderVol.SetPageSize(5);
	m_SliderVol.SetLineSize(1);
	
	// инициализируем кнопки управления
	int	iX = 6, iY = 85, idx = 45, idy = 23;
	m_BtnOPEN.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNOPEN); iX += idx + 1;
	m_BtnREC.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNREC);  iX += idx + 1;
	m_BtnSTOP.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNSTOP); iX += idx + 1;
	m_BtnPLAY.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNPLAY); iX = 226; idx = 25;

	//iX = 243; idx = 25;
	//m_BtnMIX_INV.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_MIX_INV);
	//iX += idx+0; idx = 10;
	iX = 251; idx = 27;
	m_BtnMIX_SEL.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_MIX_SEL);

	m_BtnOPEN.SetIcon(IDI_OPEN);
	m_BtnPLAY.SetIcon(IDI_PLAY);
	m_BtnSTOP.SetIcon(IDI_STOP);
	m_BtnREC.SetIcon(IDI_REC);

	m_BtnMIX_SEL.SetIcon(IDI_MIC);

	m_BtnFrame.Create(_T("Static"), "", WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,
		CRect(1, 80, 283, 113), this, IDC_STATIC);

	// врубаем подсказки
	EnableToolTips(true);

	// применяем опцию "постоянно на вершине"
	OnOptTop(); OnOptTop();	// состояние не изменяется

	// настраиваем микшер
	m_RecMixer.Open(WAVE_MAPPER, GetSafeHwnd());
	m_PlayMixer.Open(WAVE_MAPPER, GetSafeHwnd());
	if(m_RecMixer.GetLinesNum() != 0)
		m_SliderVol.SetPos(m_RecMixer.GetVol(m_RecMixer.GetCurLine()));
	else
		m_SliderVol.EnableWindow(false);
	// Запоминаем системное значение громкости
	//m_nSysVolume = 0;
	//m_nSysVolume = m_PlayMixer.GetVol (m_PlayMixer.GetCurLine());
	//m_PlayMixer.SetVol (m_conf.GetConfProg()->nPlayVolume);

	// устанавливаем начальный вид окна
	m_title = new CTitleText(this->GetSafeHwnd());

	// получаем директорию екзешника
	m_strDir = GetProgramDir ();

	// обрабатываем опции загрузчика
	CString strName = "", strPath = "";
	switch (m_conf.GetConfDialGen()->nLoader)
	{
		case 1:		// создание нового файла - автоимя
			//strName = GetAutoName(CString("")) + ".mp3";
			strName = GetAutoName(CString(""));
			break;
		case 2:		// загрузка последнего файла
			strName = m_conf.GetConfProg()->strLastFileName;
			break;
	}
	// формируем директорию файла
	strPath = m_conf.GetConfProg()->strLastFilePath;
	if(strPath.IsEmpty())
		strPath = m_strDir;	// директория программы
	strPath += "\\";

	OnFileClose();
	// если имя не пустое - открываем файл
	if(!strName.IsEmpty())
	{	strName = strPath + strName;
		OpenFile(strName);
	}

	// создаем иконку в трее
	m_TrayIcon.Create(this, WM_ICON_NOTIFY, "StepVoice Recorder",
		LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TRAY_STOP)), IDR_TRAY_MENU);
	if(m_conf.GetConfDialGen()->bTrayIcon)
		m_TrayIcon.ShowIcon();
	else
		m_TrayIcon.HideIcon();
	UpdateTrayText();

	// Обработка функции мониторинга
	//m_bMonitoringBtn = m_conf.GetConfProg()->bMonitoring;
	if(m_conf.GetConfProg()->bMonitoring)
	{
		m_bMonitoringBtn = false;
		OnBtnMonitoring();
	}

	// устанавливаем функцию обработки срабатывания шедулера (SHR)
	m_sched2.SetCallbackFunc(Scheduler2Function);

	// !!!!! Обработка функции SHR (шедулера)
	if(m_conf.GetConfDialSH2()->bIsEnabled != 0)
	{	// включаем шедулер (имитируя нажатие на кнопку)
		m_conf.GetConfDialSH2()->bIsEnabled = false;
		OnBtnSched();
	}

	// Обработка функции VAS
	CONF_DIAL_VAS* pConfig = m_conf.GetConfDialVAS();
	if(pConfig->bEnable)
		OnBtnVas();

	return 0;
}
#pragma optimize("", on)

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.cx = 290;//285;//318;//255;
	cs.cy = 120 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
	
	// устанавливаем положение окна
	int nScreenCX = GetSystemMetrics(SM_CXSCREEN);
	int nScreenCY = GetSystemMetrics(SM_CYSCREEN);
	cs.x  = m_conf.GetConfProg()->nXcoord;
	cs.y  = m_conf.GetConfProg()->nYcoord;
	if(cs.x >= (nScreenCX-40) || cs.y >= (nScreenCY-40))
	{
		cs.x  = (nScreenCX - cs.cx)/2;
		cs.y  = (nScreenCY - cs.cy)/2;
	}

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	//cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
	//	::LoadCursor(NULL, IDC_ARROW), (HBRUSH)16/*(HBRUSH)COLOR_BACKGROUND+1*/,
	//	AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	cs.lpszClass	= "SvRec";
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.hInstance	= AfxGetInstanceHandle();
	wc.lpfnWndProc	= ::DefWindowProc;
	wc.style		= CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wc.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground= HBRUSH(16);
	wc.hIcon		= ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	wc.lpszClassName= "SvRec";
	AfxRegisterClass(&wc);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::ShowWindow()
{	
	// process show window, called by MP3_RecorderApp
	int nCmdShow = (m_conf.GetConfDialGen()->bMinimized) ? SW_MINIMIZE : SW_SHOW;

	if(nCmdShow == SW_MINIMIZE && m_conf.GetConfDialGen()->bTrayMin)
	{
		nCmdShow = SW_HIDE;
		m_TrayIcon.ShowIcon();
	}

	return CFrameWnd::ShowWindow(nCmdShow);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	// обрабатываем сообщ. изменения размеров экрана для snapping
	case WM_DISPLAYCHANGE:
		//::GetWindowRect(::GetDesktopWindow(),&m_rDesktopRect);
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rDesktopRect, 0);
		break;
	case MM_MIXM_CONTROL_CHANGE:
	case MM_MIXM_LINE_CHANGE:
		{
			//if(m_nActiveMixerID == 1)
			//	OnPlayMixMenuSelect(ID_MIXITEM_PLAY0 + m_PlayMixer.GetCurLine());
			//else
			//	OnRecMixMenuSelect(ID_MIXITEM_REC0 + m_RecMixer.GetCurLine());

			if(m_nActiveMixerID == 1)
			{
				m_SliderVol.SetPos(m_PlayMixer.GetVol(m_PlayMixer.GetCurLine()));
			}
			else
			{
				m_SliderVol.SetPos(m_RecMixer.GetVol(m_RecMixer.GetCurLine()));
				// меняем иконку в зависимости от типа линии
				const int ICON_ID[] = { IDI_MIXLINE00, IDI_MIXLINE01, IDI_MIXLINE02,
										IDI_MIXLINE03, IDI_MIXLINE04, IDI_MIXLINE05,
										IDI_MIXLINE06, IDI_MIXLINE07, IDI_MIXLINE08,
										IDI_MIXLINE09, IDI_MIXLINE10 };
				int i = m_RecMixer.GetLineType(m_RecMixer.GetCurLine());
				m_BtnMIX_SEL.SetIcon(ICON_ID[i]);
			}
		}
		break;
	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_conf.GetConfProg()->bEasyMove)
	{
		CPoint scrPoint;
		GetCursorPos(&scrPoint);
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(scrPoint.x, scrPoint.y));
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnMoving(fwSide, pRect);
	
	// calculate window size
	SIZE szSize = {pRect->right - pRect->left, pRect->bottom - pRect->top};
	POINT pt;
	GetCursorPos(&pt);
	pRect->left  = pt.x - m_szMoveOffset.cx;
	pRect->top   = pt.y - m_szMoveOffset.cy;
	pRect->right = pRect->left + szSize.cx;
	pRect->bottom= pRect->top  + szSize.cy;
	// snap window to screen edges
	if(pRect->top < m_nSnapPixels && pRect->top > -m_nSnapPixels)
	{// begin snap to top
		pRect->top = 0;
		pRect->bottom = szSize.cy;
	}// end snap to top
	if(pRect->left < m_nSnapPixels && pRect->left > -m_nSnapPixels)
	{// begin snap to left
		pRect->left = 0;
		pRect->right = szSize.cx;
	}// end snap to left
	if(pRect->right < m_rDesktopRect.right+m_nSnapPixels && pRect->right > m_rDesktopRect.right-m_nSnapPixels)
	{// begin snap to right
		pRect->right = m_rDesktopRect.right;
		pRect->left = m_rDesktopRect.right-szSize.cx;
	}// end snap to right
	if(pRect->bottom < m_rDesktopRect.bottom+m_nSnapPixels && pRect->bottom > m_rDesktopRect.bottom-m_nSnapPixels)
	{// begin snap to bottom
		pRect->bottom = m_rDesktopRect.bottom;
		pRect->top = m_rDesktopRect.bottom-szSize.cy;
	}// end snap to bottom
}// end OnMoving

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (nHitTest == HTCAPTION)
	{//begin save mouse coord
		// calculate drag offset
		RECT rRect;
		GetWindowRect(&rRect);
		m_szMoveOffset.cx = point.x - rRect.left;
		m_szMoveOffset.cy = point.y - rRect.top;
	}// end save mouse coord
	CFrameWnd::OnNcLButtonDown(nHitTest, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();

	// выключаем кнопку мониторинга (и останавл. его)
	//m_conf.GetConfProg()->bMonitoring = m_bMonitoringBtn;
	if(m_bMonitoringBtn)
	{	// останавливаем мониторинг и сохраняем значение
		OnBtnMonitoring();
		m_conf.GetConfProg()->bMonitoring = true;
	}

	// закрываем файл (все объекты)
	OnFileClose();

	// сохраняем позицию окна
	if(!IsIconic())
	{
		CRect r; GetWindowRect(&r);
		m_conf.GetConfProg()->nXcoord = r.left;
		m_conf.GetConfProg()->nYcoord = r.top;
	}

	// сохраняем тип графиков
	m_conf.GetConfProg()->nGraphType = m_GraphWnd.GetGraphType();
	m_conf.GetConfProg()->bGraphMaxpeaks = (int)m_GraphWnd.GetMaxpeaks();

	// сохр. громкость воспр. и восстанавливаем системную громкость
	//m_conf.GetConfProg()->nPlayVolume = m_PlayMixer.GetVol (
	//	m_PlayMixer.GetCurLine ());
	//m_PlayMixer.SetVol (m_nSysVolume);	
}


//===========================================================================
// Меню : File
//===========================================================================
void CMainFrame::OnFileClose() 
{
	OnBtnSTOP();

	if(m_pSndFile)
		SAFE_DELETE(m_pSndFile);

	// Устанавливаем текст по умолчанию: <no file> - StepVoice Recorder
	CString strTitle, strNoFile((LPCSTR)IDS_NOFILE);
	AfxFormatString1(strTitle, IDS_FILETITLE, strNoFile);
	m_title->SetTitleText(strTitle);

	// обновляем визуализацию (показываем настройки записи)
	// устанавливаем указатель на начало файла
	//if(m_pSndFile)
	//	m_pSndFile->Seek(0, SND_FILE_BEGIN);
	m_SliderTime.SetCurPos(0);
	//m_SliderTime.ShowThumb(false);	// убираем ползунок слайдера
	m_TimeWnd.Reset();
	UpdateStatWindow();
	UpdateTrayText();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileClear() 
{
	if(!m_pSndFile)
		return;

	CString strRes, strName = m_pSndFile->GetFileName();
	AfxFormatString1(strRes, IDS_CLEAR_ASK, strName);
	if(AfxMessageBox(strRes, MB_YESNO|MB_ICONWARNING) == IDYES)
	{	OnBtnSTOP();
		m_pSndFile->Seek(0, SND_FILE_BEGIN);
		m_pSndFile->Cut();
		//m_SliderTime.ShowThumb(false);	// убираем ползунок слайдера
		OpenFile(strName);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileDelete() 
{
	if(!m_pSndFile)
		return;

	CString strRes, strName = m_pSndFile->GetFileName();
	AfxFormatString1(strRes, IDS_DEL_ASK, strName);
	if(AfxMessageBox(strRes, MB_YESNO|MB_ICONWARNING) == IDYES)
	{	OnFileClose();
		if(!DeleteFile(strName))
		{	AfxFormatString1(strRes, IDS_DEL_UNABLE, strName);
			AfxMessageBox(strRes, MB_OK|MB_ICONWARNING);
		}
	}
}

//=======не из меню, но полезная :)===========================================
void CMainFrame::OpenFile(CString& str)
{
	if(m_pSndFile)
		OnFileClose();

	// показываем сообщение о загрузке файла
	CString strLoad((LPCSTR)IDS_LOADING);
	m_title->SetTitleText(strLoad);
	this->BeginWaitCursor();

	SOUND_INFO si;
	si.nBitrate	= m_conf.GetConfDialMp3()->nBitrate;
	si.nFreq	= m_conf.GetConfDialMp3()->nFreq;
	si.nStereo	= m_conf.GetConfDialMp3()->nStereo;
	si.nBits	= 16;

	m_pSndFile = new CSndFile_MP3;
	bool bRes = m_pSndFile->Open(str, &si);

	this->EndWaitCursor();
	if(bRes == false)
	{
		SAFE_DELETE(m_pSndFile);
		OnFileClose();
		// выдаем ошибку
		CString strRes;
		AfxFormatString1(strRes, IDS_OPEN_ERR, str);
		AfxMessageBox(strRes, MB_OK|MB_ICONWARNING);
		return;
	}

	// запоминаем имя и полный путь файла
	CString& strPath = m_conf.GetConfProg()->strLastFilePath;
	CString& strName = m_conf.GetConfProg()->strLastFileName;
	int nSlash = str.ReverseFind('\\');
	if(nSlash == -1)
		strName = str;
	else
	{	// !!!!!!
		strPath = str.Left(nSlash);	// запомнить путь, если в др. папке
		strName = str.Right(str.GetLength() - nSlash - 1);
	}

	// обновляем окно статистики
	m_SliderTime.SetCurPos(0);
	//if(m_pSndFile->GetFileSize(IN_SECONDS) > 0)
	//	m_SliderTime.ShowThumb(true);
	m_TimeWnd.Reset();
	UpdateStatWindow();

	// Устанавливаем заголовок: "<FILE> - StepVoice Recorder"
	CString strTitle;
	AfxFormatString1(strTitle, IDS_FILETITLE, strName);
	m_title->SetTitleText(strTitle);

	UpdateTrayText();
}

//===========================================================================
void CMainFrame::OnFileFindfile() 
{
	CString filePath = m_conf.GetConfProg()->strLastFilePath + "\\" +
		m_conf.GetConfProg()->strLastFileName;
	
	ShellExecute(NULL, "open", "explorer", "/select, """+filePath+"""",
		NULL, SW_NORMAL);
	//this->SetForegroundWindow();
}

//===========================================================================
// Меню : Sound (перемещение по файлу)
//===========================================================================
void CMainFrame::OnSoundBegin() 
{
	int	nAllSec = 0;
	if (m_pSndFile)
	{
		m_pSndFile->Seek(0, SND_FILE_BEGIN);
		m_TimeWnd.SetTime(0);
		m_SliderTime.SetCurPos(0);
		nAllSec = m_pSndFile->GetFileSize(IN_SECONDS);
	}

	CString strRes;
	char szAllSec[12];
	Convert(nAllSec, szAllSec);
	strRes.Format(IDS_SEEKTO, _T("0:00:00"), szAllSec, 0);
	m_title->SetTitleText(strRes, 1200);
}

//===========================================================================
void CMainFrame::OnSoundRew() 
{
	CString strRes;
	char	szCurSec[12], szAllSec[12];
	int		nCurSec = 0, nAllSec = 0;

	if(m_pSndFile)
	{	m_pSndFile->Seek(-5, SND_FILE_CURRENT);
		// обновляем окно времени и слайдер позиции
		nAllSec = m_pSndFile->GetFileSize(IN_SECONDS);
		nCurSec = m_pSndFile->GetFilePos(IN_SECONDS);
		m_TimeWnd.SetTime(nCurSec);
		m_SliderTime.SetCurPos(int(float(nCurSec)/nAllSec * 1000));
	}

	Convert(nCurSec, szCurSec);
	Convert(nAllSec, szAllSec);
	strRes.Format(IDS_SEEKTO, szCurSec, szAllSec, int((float)nCurSec*100/nAllSec));
	m_title->SetTitleText(strRes, 1200);
}

//===========================================================================
void CMainFrame::OnSoundFf() 
{
	CString strRes;
	char	szCurSec[12], szAllSec[12];
	int		nCurSec = 0, nAllSec = 0;

	if(m_pSndFile)
	{
		m_pSndFile->Seek(5, SND_FILE_CURRENT);
		// обновляем окно времени и слайдер позиции
		nAllSec = m_pSndFile->GetFileSize(IN_SECONDS);
		nCurSec = m_pSndFile->GetFilePos(IN_SECONDS);
		m_TimeWnd.SetTime(nCurSec);
		m_SliderTime.SetCurPos(int(float(nCurSec)/nAllSec * 1000));
	}

	Convert(nCurSec, szCurSec);
	Convert(nAllSec, szAllSec);
	strRes.Format(IDS_SEEKTO, szCurSec, szAllSec, int((float)nCurSec*100/nAllSec));
	m_title->SetTitleText(strRes, 1200);
}

//===========================================================================
void CMainFrame::OnSoundEnd() 
{
	CString strRes;
	char	szCurSec[12], szAllSec[12];
	int		nCurSec = 0, nAllSec = 0;

	if(m_pSndFile)
	{
		m_pSndFile->Seek(0, SND_FILE_END);
		// обновляем окно времени и слайдер позиции
		nAllSec = m_pSndFile->GetFileSize(IN_SECONDS);
		nCurSec = m_pSndFile->GetFilePos(IN_SECONDS);
		m_TimeWnd.SetTime(nAllSec);
		m_SliderTime.SetCurPos(int(float(nCurSec)/nAllSec * 1000));
	}

	Convert(nCurSec, szCurSec);
	Convert(nAllSec, szAllSec);
	strRes.Format(IDS_SEEKTO, szCurSec, szAllSec, int((float)nCurSec*100/nAllSec));
	m_title->SetTitleText(strRes, 1200);
}

//===========================================================================
// Меню : Options
//===========================================================================
void CMainFrame::OnStatPref() 
{
	//m_conf.GetConfProg()->nDialogIndex = -1; // влияем на выбор вкладки
	OnOptCom();
}

void CMainFrame::OnOptCom()
{
	// открытыми могут быть вкладки "General" или "Record" :)
	int nDialogIndex = m_conf.GetConfProg()->nDialogIndex;
	//nDialogIndex = (nDialogIndex == -1) ? 1 : 0;

	//CMySheet optDlg("Preferences", this, m_conf.GetConfProg()->nDialogIndex);
	CMySheet optDlg("Preferences", this, nDialogIndex);
	// настраиваем вид диалога и состояние его элементов
	optDlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	//optDlg.m_psh.dwFlags |= PSH_HASHELP;

	optDlg.SetConfig( m_conf.GetConfDialGen() );
	optDlg.SetConfig( m_conf.GetConfDialMp3() );
	optDlg.SetConfig( m_conf.GetConfDialSH2()  );
	optDlg.SetConfig( m_conf.GetConfDialVAS() );
	optDlg.SetConfig( m_conf.GetConfDialAN()  );
	// !!!
	m_pOptDialog = &optDlg;

	// сохраним состояние шедулера
	bool bOldSchedState = m_conf.GetConfDialSH2()->bIsEnabled != 0;
	bool bNewSchedState = bOldSchedState;

	if(optDlg.DoModal() == IDOK)
	{
		m_conf.saveConfig();		
		//m_conf.RegisterWrite(); // сохраняем состояние настроек

		if(m_pSndFile)
		{
			SOUND_INFO si;
			si.nBitrate	= m_conf.GetConfDialMp3()->nBitrate;
			si.nFreq	= m_conf.GetConfDialMp3()->nFreq;
			si.nStereo	= m_conf.GetConfDialMp3()->nStereo;
			si.nBits	= 16;
			// пытаемся изменить параметры файла (для пустого сработает)
			m_pSndFile->ChangeSoundInfo(si);
		}
		UpdateStatWindow();

		// проверяем опции трея
		if(m_conf.GetConfDialGen()->bTrayIcon) //Taskbar only
			m_TrayIcon.ShowIcon();
		else
			m_TrayIcon.HideIcon();

		// !!!!! Проверка изменения шедулера (проверка SHR)
		bNewSchedState = m_conf.GetConfDialSH2()->bIsEnabled != 0;
		if(bNewSchedState != bOldSchedState)
		{	// инвертируем состояние шедулера (имитируя нажатие на кнопку)
			m_conf.GetConfDialSH2()->bIsEnabled = !bNewSchedState;
			OnBtnSched();
		}
		else if(bNewSchedState) {	// перезапускаем шедулер
			OnBtnSched(); // выкл.
			OnBtnSched(); // вкл.
		}

		// Проверка VAS
		CONF_DIAL_VAS* pConfig = m_conf.GetConfDialVAS();
		if((pConfig->bEnable != 0) != m_vas.IsRunning())
			OnBtnVas();
		if(m_vas.IsRunning())
		{
			m_vas.InitVAS(pConfig->nThreshold, pConfig->nWaitTime);
			m_GraphWnd.ShowVASMark(pConfig->nThreshold);
		}
	}

	m_conf.GetConfProg()->nDialogIndex = optDlg.m_nPageIndex;

	// !!!
	m_pOptDialog = NULL;
}

//===========================================================================
void CMainFrame::OnOptEm() 
{	// состояние "быстрое перемещение"
	m_conf.GetConfProg()->bEasyMove = m_conf.GetConfProg()->bEasyMove ? 0 : 1;
}

//===========================================================================
void CMainFrame::OnOptTop() 
{	// инвертируем текущий тип окна
	int& top = m_conf.GetConfProg()->bAlwaysOnTop;
	top = top ? 0 : 1;

	const CWnd* pWndType = (top) ? &wndTopMost : &wndNoTopMost;
	SetWindowPos(pWndType, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);
}

//===========================================================================
void CMainFrame::OnMixPlay() 
{
	CloseMixerWindows();
	WinExec("sndvol32", SW_SHOW);
}

//===========================================================================
void CMainFrame::OnMixRec() 
{
	CloseMixerWindows();
	WinExec("sndvol32 -r", SW_SHOW);

	//AfxMessageBox(m_RecMixer.GetLineName(m_RecMixer.GetCurLine()), MB_OK);
}


//===========================================================================
// Кнопки
//===========================================================================
void CMainFrame::OnBtnOPEN()
{
	this->SetFocus(); // убираем фокус с кнопки

	CString str, strName, strLastPath, strFullName;

	str.LoadString(IDS_FILEFILTER);			// фильтр - MPEG audio files
	strName = GetAutoName(CString(""));		// Имя вида 08jun_05.mp3
	strLastPath = m_conf.GetConfProg()->strLastFilePath;

	// создаем объект стандартного диалога
	CFileDialog NewOpenFileDialog(true,
		"mp3",
		strName.GetBuffer(strName.GetLength()),
		OFN_HIDEREADONLY | OFN_EXPLORER,
		str);

	// настраиваем параметры окна диалога
	str.LoadString(IDS_FILENEWOPENTITLE);
	NewOpenFileDialog.m_ofn.lpstrTitle = str.GetBuffer(str.GetLength());
	
	NewOpenFileDialog.m_ofn.lpstrInitialDir = "";
	if(!strLastPath.IsEmpty())
	{	
		strLastPath = strLastPath + "\\";
		NewOpenFileDialog.m_ofn.lpstrInitialDir = strLastPath;	
	}

	if(NewOpenFileDialog.DoModal() == IDOK)
		OpenFile(NewOpenFileDialog.GetPathName());

	/*OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	CString strFilter, strTitle, strName, strLastPath;

	strFilter.LoadString(IDS_FILEFILTER);	// фильтр - MPEG audio files
	strTitle.LoadString(IDS_FILENEWOPENTITLE);
	strName = GetAutoName(CString(""));		// Имя вида 08jun_05.mp3
	strLastPath = m_conf.GetConfProg()->strLastFilePath;

	strcpy(szFile, strName.GetBuffer(strName.GetLength()));

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	//ofn.hwndOwner = this->GetSafeHwnd();
	ofn.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrDefExt = "mp3";
	ofn.lpstrFilter = "MPEG audio files (*.MP3)\0*.mp3\0All files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = strTitle;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_HIDEREADONLY | OFN_EXPLORER;
	ofn.lpstrInitialDir = NULL;
	if(!strLastPath.IsEmpty()) {	
		strLastPath = strLastPath + "\\";
		ofn.lpstrInitialDir = strLastPath;	
	}

	// Display the Open dialog box. 
	if (GetOpenFileName(&ofn) == TRUE) {
		strName = ofn.lpstrFile;
		OpenFile(strName);
	}*/
}

//===========================================================================
void CMainFrame::OnBtnPLAY()
{
	SOUND_INFO si;
	int nRes;

	SetFocus();
	switch(m_nState)
	{
	case STOP_STATE:
		// проверяем, открыт ли файл
		if(!m_pSndFile)
		{	OnBtnOPEN();
			if(!m_pSndFile)
				return;
		}
		m_pSndFile->GetSoundInfo(si);

		// инициализируем декодер
		m_pDecoder = new CDecoder_MP3(m_strDir);
		if(!m_pDecoder->InitDecoder(NULL))
		{
			SAFE_DELETE(m_pDecoder);
			// выдаем сообщение об ошибке
			CString strRes;
			AfxFormatString2(strRes, IDS_DEC_INIT, m_strDir, "mpglib.dll");
			AfxMessageBox(strRes, MB_OK|MB_ICONSTOP);
			return;
		}

		// останавливаем мониторинг перед воспроизведением
		if(m_bMonitoringBtn)
		{
			//MonitoringStop();
			OnBtnMonitoring();
		}

		// инициализируем воспр.
		m_pWaveOut = new CWaveOut(_PlaybkProc);
		nRes = m_pWaveOut->Open(si.nFreq, si.nBits, si.nStereo);
		if(nRes)
		{	// произошла ошибка открытия
			SAFE_DELETE(m_pWaveOut);
			// выдаем сообщение об ошибке
			if(nRes != ERR_BUFFER)
				AfxMessageBox(IDS_WAVEOUT_OPEN, MB_OK|MB_ICONWARNING);
			return;
		}

		// запускаем воспр.
		if(!m_pWaveOut->Start())
		{
			SAFE_DELETE(m_pWaveOut);
			// выдаем сообщение об ошибке
			AfxMessageBox(IDS_WAVEOUT_START, MB_OK|MB_ICONWARNING);
			return;
		}

		// делаем. доп. действия
		m_nState = PLAY_STATE;
		SetTimer(1, 1000, NULL);
		// обновляем вид окон
		m_GraphWnd.SetConfig(&si);
		m_BtnPLAY.SetIcon(IDI_PAUSE);
		//m_BtnREC.ModifyStyle(0, WS_DISABLED);
		//m_BtnREC.Invalidate(false);
		// обновляем иконку в трее
		m_TrayIcon.SetIcon(IDI_TRAY_PLAY);
		break;

	case PLAY_STATE:
		m_pWaveOut->Pause();
		// делаем доп. действия
		m_nState = PAUSEPLAY_STATE;
		KillTimer(1);
		m_BtnPLAY.SetIcon(IDI_PLAY);
		m_TimeWnd.Blink(true);
		// обновляем иконку в трее
		m_TrayIcon.SetIcon(IDI_TRAY_PAUSE);
		break;

	case PAUSEPLAY_STATE:
		m_pWaveOut->Start();
		// делаем доп. действия
		m_nState = PLAY_STATE;
		SetTimer(1, 1000, NULL);
		m_BtnPLAY.SetIcon(IDI_PAUSE);
		m_TimeWnd.Blink(false);
		// обновляем иконку в трее
		m_TrayIcon.SetIcon(IDI_TRAY_PLAY);
		break;
	}

	UpdateIcoWindow();
	UpdateButtonState(IDB_BTNREC);

	// меняем линию микшера
	OnPlayMixMenuSelect(ID_MIXITEM_PLAY0 + m_PlayMixer.GetCurLine());
}

//===========================================================================
void CMainFrame::OnBtnSTOP()
{
	SetFocus();
	switch(m_nState)
	{
	case RECORD_STATE:
	case PAUSEREC_STATE:

		SAFE_DELETE(m_pWaveIn);
		SAFE_DELETE(m_pEncoder);
		// дополнительные действия
		KillTimer(2);
		m_BtnREC.SetIcon(IDI_REC);
		//m_BtnPLAY.ModifyStyle(WS_DISABLED, 0);
		//m_BtnPLAY.Invalidate(false);
		break;

	case PLAY_STATE:
	case PAUSEPLAY_STATE:

		SAFE_DELETE(m_pWaveOut);
		SAFE_DELETE(m_pDecoder);
		// дополнительные действия
		KillTimer(1);
		m_BtnPLAY.SetIcon(IDI_PLAY);
		//m_BtnREC.ModifyStyle(WS_DISABLED, 0);
		//m_BtnREC.Invalidate(false);
		break;
	}

	m_nState = STOP_STATE;
	// запускаем мониторинг
	if(m_bMonitoringBtn)
		MonitoringStart();

	// устанавливаем указатель на начало файла
	PostMessage(WM_HSCROLL,  MAKEWPARAM(SB_THUMBPOSITION, 0),
		(LPARAM)m_pMainFrame->m_SliderTime.m_hWnd);
	m_SliderTime.SetCurPos(0);
	//if(m_pSndFile)
	//	m_pSndFile->Seek(0, SND_FILE_BEGIN);
	//m_SliderTime.SetCurPos(0);

	/*if(m_pSndFile)
	{
		if(m_pSndFile->GetFileSize(IN_SECONDS) > 0)
			m_SliderTime.ShowThumb(true);
	}*/

	// изменить окна визуализации и статистики
	UpdateIcoWindow();
	UpdateStatWindow();
	ProcessSliderTime(SB_THUMBPOSITION, 1000);
	m_TimeWnd.Blink(false);
	//m_TimeWnd.Reset();
	m_GraphWnd.Clear();

	UpdateButtonState(IDB_BTNPLAY);
	UpdateButtonState(IDB_BTNREC);

	// обновляем иконку в трее
	m_TrayIcon.SetIcon(IDI_TRAY_STOP);

	// меняем линию микшера
	OnRecMixMenuSelect(ID_MIXITEM_REC0 + m_RecMixer.GetCurLine());
}


//===========================================================================
void CMainFrame::OnBtnREC()
{
	SOUND_INFO    si;
	CONF_DIAL_MP3 confMp3;
	int nResult;

	CString strTest;

	// данные для старта шедулера
	bool bIsSchedEnabled= m_conf.GetConfDialSH2()->bIsEnabled  != 0;
	bool bSchedStart	= m_conf.GetConfDialSH2()->bSchedStart != 0;
	SHR_TIME* ptRec		= &m_conf.GetConfDialSH2()->t_rec;
	SHR_TIME* ptStop	= &m_conf.GetConfDialSH2()->t_stop;

	SetFocus();
	switch(m_nState)
	{
	case STOP_STATE:
		// если файл не открыт - вызываем диалог открытия
		if(!m_pSndFile)
		{	
			TRACE0("==> OnBtnREC: trying to create/open a file ... \n");
			OnBtnOPEN();
			if(!m_pSndFile)
				return;
			TRACE0("==> OnBtnREC: file created/opened!\n");
		}

		// инициализируем кодер
		m_pSndFile->GetSoundInfo(si);
		confMp3.nBitrate = si.nBitrate;
		confMp3.nFreq	 = si.nFreq;
		confMp3.nStereo  = si.nStereo;

		ASSERT(!m_pEncoder);
		m_pEncoder = new CEncoder_MP3(m_strDir);
		nResult = m_pEncoder->InitEncoder(&confMp3);
		if(nResult != ENC_NOERROR)
		{
			SAFE_DELETE(m_pEncoder);

			CString strRes;
			if(nResult == ENC_ERR_DLL || nResult == ENC_ERR_UNK)
			{	// критическая ошибка
				AfxFormatString2(strRes, IDS_ENC_ERR_DLL, m_strDir, "lame_enc.dll");
				AfxMessageBox(strRes, MB_OK|MB_ICONSTOP);
			}
			else if(nResult == ENC_ERR_INIT)
			{
				strRes.LoadString(IDS_ENC_ERR_INIT);
				AfxMessageBox(strRes, MB_OK | MB_ICONWARNING);
			}
			return;
		}

		// останавливаем мониторинг перед записью
		if(m_bMonitoringBtn)
		{
			//MonitoringStop();
			OnBtnMonitoring();
		}

		// инициализируем запись
		m_pWaveIn = new CWaveIn(_RecordProc);
		if(m_pWaveIn->Open(si.nFreq, si.nBits, si.nStereo))
		{
			SAFE_DELETE(m_pWaveIn);
			AfxMessageBox(IDS_WAVEIN_OPEN, MB_OK|MB_ICONWARNING);
			return;
		}

		// решение проблемы краша программы при старте записи с вкл. VAS
		m_GraphWnd.SetConfig(&si);

		// запускаем запись
		if(!m_pWaveIn->Start())
		{
			SAFE_DELETE(m_pWaveIn);
			AfxMessageBox(IDS_WAVEIN_START, MB_OK|MB_ICONWARNING);
			return;
		}

		// дополнительные действия
		m_nState = RECORD_STATE;
		SetTimer(2, 1000, NULL);
		// обновляем окна
		m_BtnREC.SetIcon(IDI_PAUSE);
		//m_BtnPLAY.ModifyStyle(0, WS_DISABLED);
		//m_BtnPLAY.Invalidate(false);
		// обновляем иконку в трее
		m_TrayIcon.SetIcon(IDI_TRAY_REC);
		
		// запускаем шедулер
		if(bIsSchedEnabled && (!bSchedStart)) {
			if(m_conf.GetConfDialSH2()->nStopByID == 1)
				m_sched2.SetRecTime(ptRec, NULL);
			else
				m_sched2.SetStopTime(ptStop, NULL);
			TRACE0("==> OnBtnREC: trying to run the scheduler ...\n");
			m_sched2.Start();
		}

		// перезапускаем VAS
		if(m_vas.IsRunning())
			m_vas.ResetVAS();
		break;

	case RECORD_STATE:
		m_pWaveIn->Pause();
		// дополнительные действия
		m_nState = PAUSEREC_STATE;
		KillTimer(2);
		m_BtnREC.SetIcon(IDI_REC);
		m_TimeWnd.Blink(true);
		// обновляем иконку в трее
		m_TrayIcon.SetIcon(IDI_TRAY_PAUSE);
		break;

	case PAUSEREC_STATE:
		m_pWaveIn->Start();
		// дополнительные действия
		m_nState = RECORD_STATE;
		SetTimer(2, 1000, NULL);
		m_BtnREC.SetIcon(IDI_PAUSE);
		m_TimeWnd.Blink(false);
		// обновляем иконку в трее
		m_TrayIcon.SetIcon(IDI_TRAY_REC);
		// перезапускаем VAS
		if(m_vas.IsRunning())
			m_vas.ResetVAS();
		break;
	}
	m_nStartSec = m_pSndFile->GetFilePos(IN_SECONDS);
	UpdateIcoWindow();
	UpdateButtonState(IDB_BTNPLAY);
}

//===========================================================================
void CMainFrame::OnBtnMIX_REC()
{
	SetFocus();
	OnMixRec();
}

//===========================================================================
void CMainFrame::OnBtnMIX_PLAY()
{
	SetFocus();
	OnMixPlay();
}

//===========================================================================
// Обработчики слайдеров громкости (уровня) и позиции
//===========================================================================
void CMainFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(pScrollBar->m_hWnd == m_SliderTime.m_hWnd)
		ProcessSliderTime(nSBCode, nPos);
	else if(pScrollBar->m_hWnd == m_SliderVol.m_hWnd)
		ProcessSliderVol(nSBCode, nPos);

	CFrameWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

//===========================================================================
void CMainFrame::ProcessSliderTime(UINT nSBCode, UINT nPos)
{
	CString strSeekTo;
	char	szCurTime[10], szAllTime[10];

	int nCurSec = 0;
	int nAllSec = 0;
	
	// получаем временные позиции
	if(m_pSndFile)
	{	nAllSec = m_pSndFile->GetFileSize(IN_SECONDS);
		nCurSec = int((float)nPos/1000 * nAllSec);
	}

	// преобразуем их в строки
	Convert(nCurSec, szCurTime);
	Convert(nAllSec, szAllTime);
	strSeekTo.Format(IDS_SEEKTO, szCurTime, szAllTime, nPos/10);

	// обрабатываем сообщения слайдера
	switch (nSBCode)
	{
	case SB_THUMBTRACK:
		m_title->SetTitleText(strSeekTo, 10000);
		break;
	case SB_THUMBPOSITION:
		m_title->Restore();
		if(m_pSndFile)
		{	nCurSec = int(float(nPos)/1000 * nAllSec);
			m_pSndFile->Seek(nCurSec, SND_FILE_BEGIN);
			m_TimeWnd.SetTime(nCurSec);
		}
		else
			m_SliderTime.SetCurPos(0);
		break;
	}
}

//===========================================================================
// Обработчик таймера
//===========================================================================
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if(m_pSndFile && (nIDEvent==1 || nIDEvent==2))
	{
		int nCurSec = m_pSndFile->GetFilePos(IN_SECONDS);
		int nAllSec = m_pSndFile->GetFileSize(IN_SECONDS);

		if(!m_SliderTime.IsDragging())
		{	float fPos = (float)nCurSec/nAllSec * 1000;
			m_SliderTime.SetCurPos(int(fPos));
		}
		m_TimeWnd.SetTime(nCurSec);
		UpdateStatWindow();
	}

	CFrameWnd::OnTimer(nIDEvent);
}


//===========================================================================
// Функия преобразования количества секунд в строку вида "Ч:ММ:СС"
//===========================================================================
void CMainFrame::Convert(UINT nCurSec, char *pszTime)
{
	const char* szPattern[] = {"%d", "0%d"};
	char szMin[3] = "", szSec[3] = "";
	int  iHour, iMin, iSec;

	iHour= nCurSec/3600;
	iMin =(nCurSec - iHour*3600)/60;
	iSec = nCurSec - iHour*3600 - iMin*60;

	sprintf(szMin, szPattern[iMin<10], iMin);
	sprintf(szSec, szPattern[iSec<10], iSec);
	sprintf(pszTime, "%d:%s:%s", iHour, szMin, szSec);
}


//===========================================================================
// Функии обновления окон визуализации и статистики
//===========================================================================
void CMainFrame::UpdateIcoWindow()
{
	int nIconID;

	// выбираем нужную иконку ...
	switch(m_nState)
	{
	case RECORD_STATE:
		nIconID = ICON_REC;
		break;
	case PLAY_STATE:
		nIconID = ICON_PLAY;
		break;
	case PAUSEREC_STATE:
		nIconID = ICON_PAUSER;
		break;
	case PAUSEPLAY_STATE:
		nIconID = ICON_PAUSEP;
		break;
	case STOP_STATE:
	default:
		nIconID = ICON_STOP;
		break;
	}
	// ... теперь устанавливаем ее
	m_IcoWnd.SetNewIcon(nIconID);
}

//===========================================================================
void CMainFrame::UpdateStatWindow()
{
	char szTime[10];
	int  nAllSec = 0, nAllKb = 0;

	// заполняем переменные значениями настроек
	SOUND_INFO si;
	si.nBitrate = m_conf.GetConfDialMp3()->nBitrate;
	si.nFreq	= m_conf.GetConfDialMp3()->nFreq;
	si.nStereo	= m_conf.GetConfDialMp3()->nStereo;

	if(m_pSndFile)
	{	//m_pSndFile->SetSoundInfo(si);	// пытаемся изменить параметры файла
		m_pSndFile->GetSoundInfo(si);
		nAllSec = m_pSndFile->GetFileSize(IN_SECONDS);
		nAllKb  = m_pSndFile->GetFileSize(IN_KBYTES);
	}

	// устанавливаем переменные в окно статистики
	Convert(nAllSec, szTime);
	m_StatWnd.Set(si.nFreq, si.nBitrate, si.nStereo);
	m_StatWnd.Set(nAllKb, szTime);
}

void CMainFrame::UpdateTrayText()
{
	CString wndText;

	GetWindowText(wndText);
	m_TrayIcon.SetTooltipText(wndText);
}

//===========================================================================
// Обновления элементов главного меню
//===========================================================================
void CMainFrame::OnUpdateSoundRec(CCmdUI* pCmdUI) 
{
	// определяем доступность элемента меню "запись" (если не воспроизведение)
	bool bEnable = !(m_nState==PLAY_STATE || m_nState== PAUSEPLAY_STATE);
	pCmdUI->Enable(bEnable);
}

//===========================================================================
void CMainFrame::OnUpdateSoundPlay(CCmdUI* pCmdUI)
{
	// определяем доступность элемента меню "воспр." (если не запись)
	bool bEnable = !(m_nState==RECORD_STATE || m_nState== PAUSEREC_STATE);
	pCmdUI->Enable(bEnable);
}

//===========================================================================
void CMainFrame::OnUpdateOptTop(CCmdUI* pCmdUI) 
{
	// ставим/убираем галочку "пост. на вершине" в зависимости от настроек
	int enable = m_conf.GetConfProg()->bAlwaysOnTop;
	pCmdUI->SetCheck(enable);
}

//===========================================================================
void CMainFrame::OnUpdateOptEm(CCmdUI* pCmdUI) 
{
	// ставим/убираем галочку "легкое премещ." в завис. от настроек
	int enable = m_conf.GetConfProg()->bEasyMove;
	pCmdUI->SetCheck(enable);
}

//===========================================================================
// Акселераторы
//===========================================================================
void CMainFrame::OnFileCreateopenA() 
{
	m_BtnOPEN.Press();
	OnBtnOPEN();
}

//===========================================================================
void CMainFrame::OnSoundRecA() 
{
	if(m_nState==PLAY_STATE || m_nState==PAUSEPLAY_STATE)
		return;
	m_BtnREC.Press();
	OnBtnREC();
}

//===========================================================================
void CMainFrame::OnSoundStopA() 
{
	m_BtnSTOP.Press();
	OnBtnSTOP();
}

//===========================================================================
void CMainFrame::OnSoundPlayA() 
{
	if(m_nState==RECORD_STATE || m_nState==PAUSEREC_STATE)
		return;
	m_BtnPLAY.Press();
	OnBtnPLAY();
}

//===========================================================================
void CMainFrame::OnSoundBeginA() 
{
	OnSoundBegin();
}

void CMainFrame::OnSoundRewA() 
{
	OnSoundRew();
}

void CMainFrame::OnSoundFfA() 
{
	OnSoundFf();
}

void CMainFrame::OnSoundEndA() 
{
	OnSoundEnd();
}

//===========================================================================
void CMainFrame::OnMixRecA()
{
	m_BtnMIX_REC.Press();
	OnMixRec();
}

//---------------------------------------------------------------------
void CMainFrame::OnMixPlayA()
{
	m_BtnMIX_PLAY.Press();
	OnMixPlay();
}

//===========================================================================
// Поддержка Drag-and-Drop
//===========================================================================
void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// делаем окно активным
	this->SetForegroundWindow();

	// получили имя файла
	TCHAR szFileName[_MAX_PATH];
	::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);

	// передаем его функции открытия файла
	CString str(szFileName);
	OpenFile(str);

	::DragFinish(hDropInfo);
}

/////////////////////////////////////////////////////////////////////////////
CString CMainFrame::GetProgramDir()
{
	CString RtnVal;
    char    FileName[1000];

    GetModuleFileName(AfxGetInstanceHandle(), FileName, 1000);
    RtnVal = FileName;
    RtnVal = RtnVal.Left(RtnVal.ReverseFind('\\'));

    return RtnVal;
}

/////////////////////////////////////////////////////////////////////////////
CString CMainFrame::GetAutoName( CString& strPattern )
{
	// Убираем все знаки "%" в конце строки.
	if( strPattern.Right(1) == "%")
		strPattern.TrimRight('%');

	CTime t = CTime::GetCurrentTime();
	CString s = t.Format( m_conf.GetConfDialAN()->strAutoName );
	s += ".mp3";

	if( s == ".mp3" )
		s = "Empty.mp3";

	return s;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnStatPref();
	CFrameWnd::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CRect r;
	CMenu StatMenu, *pStatMenu;

	ClientToScreen(&point);
	StatMenu.LoadMenu(IDR_STATMENU);
	pStatMenu = StatMenu.GetSubMenu(0);
	pStatMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, point.x,
		point.y, this);
	
	CFrameWnd::OnRButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnOptSnddev() 
{
	//WinExec("control mmsys.cpl,,0", SW_SHOW);
	//WinExec("control desk.cpl,,1", SW_SHOW);
	WinExec("rundll32.exe MMSYS.CPL,ShowAudioPropertySheet", SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
bool CMainFrame::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	if(!m_conf.GetConfDialGen()->bToolTips)
		return true;

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;

	// получаем идентификатор элемета и строку по нему
	if(pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
	   pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{	// idFrom is actually the HWND of the tool
		UINT nID = ::GetDlgCtrlID((HWND)pNMHDR->idFrom);
		switch(nID)
		{
		case IDB_BTNOPEN:		nID = IDS_TT_OPEN; break;
		case IDB_BTNREC:		nID = IDS_TT_REC;  break;
		case IDB_BTNSTOP:		nID = IDS_TT_STOP; break;
		case IDB_BTNPLAY:		nID = IDS_TT_PLAY; break;
		case IDB_MIX_REC:		nID = IDS_TT_MIXREC;  break;
		case IDB_MIX_PLAY:		nID = IDS_TT_MIXPLAY; break;
		case IDS_SLIDERTIME:	nID = IDS_TT_SEEKBAR; break;
		case IDW_GRAPH:			nID = IDS_TT_WAVEWND; break;
		case IDW_TIME:			nID = IDS_TT_TIMEWND; break;
		case IDW_STAT:			nID = IDS_TT_STATWND; break;
		case IDB_MIX_SEL:		nID = IDS_TT_MIXSEL; break;
		case IDS_SLIDERVOL:		nID = IDS_TT_VOLBAR; break;
		case IDB_BTN_SCHED:		nID = IDS_TT_SCHEDULER; break;
		case IDB_BTN_VAS:		nID = IDS_TT_VAS;		break;
		case IDB_BTN_MON:		nID = IDS_TT_MONITORING;break;
		default:
			return true;
		}
		strTipText = CString((LPCSTR)nID);
		if(nID == IDS_TT_VOLBAR)
		{
			CString strCurLine;
			if(m_nActiveMixerID == 1)
				strCurLine = m_PlayMixer.GetLineName(m_PlayMixer.GetCurLine());
			else
				strCurLine = m_RecMixer.GetLineName(m_RecMixer.GetCurLine());
			strTipText = strTipText + strCurLine;
		}
	}

	// копируем строку подсказок в структуру
	if (pNMHDR->code == TTN_NEEDTEXTA)
	  lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
	else
	  _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
	*pResult = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CloseMixerWindows()
{
	// закрываем все окна микшера
	HWND hMixWnd = ::FindWindow("Volume Control", NULL);
	while(hMixWnd)
	{
		::SendMessage(hMixWnd, WM_CLOSE, 0, 0);
		hMixWnd = ::FindWindow("Volume Control", NULL);
	}
}


/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	// Delegate all the work back to the default implementation in CSystemTray.
	return m_TrayIcon.OnTrayNotification(wParam, lParam);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	if(nType==SIZE_MINIMIZED && m_conf.GetConfDialGen()->bTrayMin)
	{
		if(!m_conf.GetConfDialGen()->bTrayIcon)
			m_TrayIcon.ShowIcon();
		::ShowWindow(this->GetSafeHwnd(), SW_HIDE);
	}
	else if(nType==SIZE_RESTORED && !m_conf.GetConfDialGen()->bTrayIcon)
	{
		m_TrayIcon.HideIcon();
	}

}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
	case IDM_TRAY_SHOW:
		{
			LONG wndStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
			int nCmdShow = (wndStyle&WS_MINIMIZE || !(wndStyle&WS_VISIBLE))
				? SW_RESTORE : SW_MINIMIZE;

			if(nCmdShow == SW_MINIMIZE && m_conf.GetConfDialGen()->bTrayMin)
				nCmdShow = SW_HIDE;

			::ShowWindow(this->GetSafeHwnd(), nCmdShow);
		}
		break;
	case IDM_TRAY_REC:
		OnBtnREC();
		break;
	case IDM_TRAY_STOP:
		OnBtnSTOP();
		break;
	case IDM_TRAY_PLAY:
		OnBtnPLAY();
		break;
	case IDM_TRAY_ABOUT:
		{
			CMP3_RecorderApp* my_app = (CMP3_RecorderApp *)AfxGetApp();
			my_app->OnAppAbout();
		}
		break;
	case IDM_TRAY_EXIT:
		// если открыт диалог настроек - нужно его закрыть до заверш. прогр.
		if(m_pOptDialog)
		{
			m_pOptDialog->PostMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
			m_pOptDialog = NULL;
		}
		PostMessage(WM_CLOSE);
		break;
	}
		
	return CFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnUpdateTrayPlay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nState != RECORD_STATE &&
		m_nState != PAUSEREC_STATE);
}

void CMainFrame::OnUpdateTrayRec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nState != PLAY_STATE &&
		m_nState != PAUSEPLAY_STATE);
}

/*
BOOL CMainFrame::ShowTaskBarButton(BOOL bVisible)
{
	if (!m_bOwnerCreated) return FALSE; 

	::ShowWindow(GetSafeHwnd(), SW_HIDE);

	if (bVisible) 
		ModifyStyleEx(0, WS_EX_APPWINDOW); 
	else 
		ModifyStyleEx(WS_EX_APPWINDOW, 0);

	::ShowWindow(GetSafeHwnd(), SW_SHOW);

	return TRUE; 
}
*/

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnMIX_INV()
{
	this->SetFocus();

	m_nActiveMixerID = (m_nActiveMixerID == 1) ? 0 : 1;
	if(m_nActiveMixerID == 1)
		OnPlayMixMenuSelect(ID_MIXITEM_PLAY0 + m_PlayMixer.GetCurLine());
	else
		OnRecMixMenuSelect(ID_MIXITEM_REC0 + m_RecMixer.GetCurLine());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnMIX_SEL()
{
	static bool bShow = true;

	this->SetFocus();

	if(bShow)
	{
		// перезапускаем микшер
		m_RecMixer.Open(WAVE_MAPPER, GetSafeHwnd());
		m_PlayMixer.Open(WAVE_MAPPER, GetSafeHwnd());
		if(m_nActiveMixerID == 1)
			OnPlayMixMenuSelect(ID_MIXITEM_PLAY0 + m_PlayMixer.GetCurLine());
		else
			OnRecMixMenuSelect(ID_MIXITEM_REC0 + m_RecMixer.GetCurLine());

		CRect r;
		m_BtnMIX_SEL.GetWindowRect(&r);

		CMenu mixMenu;
		mixMenu.CreatePopupMenu();

		//int j = 0;
		for(int j = 0; j < m_PlayMixer.GetLinesNum(); j++)
			mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_PLAY0 + j, m_PlayMixer.GetLineName(j));
		//if (m_PlayMixer.GetLinesNum () > 0)
		//	mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_PLAY0 + j, m_PlayMixer.GetLineName(j));
		mixMenu.AppendMenu(MF_SEPARATOR);
		for(int i = 0; i < m_RecMixer.GetLinesNum(); i++)
			mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC0 + i, m_RecMixer.GetLineName(i));

		if(m_nActiveMixerID == 1) // play_mixer
		{
			//mixMenu.CheckMenuItem(ID_MIXITEM_PLAY0 + m_PlayMixer.GetCurLine(),
			//	MF_CHECKED | MF_BYCOMMAND);
		}
		else
		{
			mixMenu.CheckMenuItem(ID_MIXITEM_REC0 + m_RecMixer.GetCurLine(),
				MF_CHECKED | MF_BYCOMMAND);
		}

		//выводим меню на экран и устанавливаем флаг показа
		int nItemID = mixMenu.TrackPopupMenu(TPM_VCENTERALIGN|TPM_LEFTBUTTON|
			TPM_RETURNCMD, r.right, r.top+(r.bottom-r.top)/2, this);
		if(!nItemID)
		{
			POINT p;
			::GetCursorPos(&p);
			RECT r;
			m_BtnMIX_SEL.GetWindowRect(&r);
			bShow = (PtInRect(&r, p)) ? false : true;
		}
		else
			this->PostMessage(WM_COMMAND, MAKEWPARAM(nItemID, 0), 0);
	}
	else
	{
		bShow = true;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRecMixMenuSelect(UINT nID)
{
	if(m_RecMixer.GetLinesNum() == 0)
	{	m_SliderVol.EnableWindow(false);
		return;
	}

	m_SliderVol.EnableWindow(true);
	m_RecMixer.SetLine(nID - ID_MIXITEM_REC0);
	m_SliderVol.SetPos(m_RecMixer.GetVol(m_RecMixer.GetCurLine()));
	m_nActiveMixerID = 0;
	//m_BtnMIX_INV.SetIcon(IDI_MIC);
	//m_BtnMIX_SEL.SetIcon(IDI_MIC);

	// меняем иконку в зависимости от типа линии
	/*const int ICON_ID[] = { IDI_MIXLINE00, IDI_MIXLINE01, IDI_MIXLINE02,
							IDI_MIXLINE03, IDI_MIXLINE04, IDI_MIXLINE05,
							IDI_MIXLINE06, IDI_MIXLINE07, IDI_MIXLINE08,
							IDI_MIXLINE09, IDI_MIXLINE10 };
	int i = m_RecMixer.GetLineType(m_RecMixer.GetCurLine());
	m_BtnMIX_SEL.SetIcon(ICON_ID[i]);*/
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnPlayMixMenuSelect(UINT nID)
{
	if(m_PlayMixer.GetLinesNum() == 0)
	{	m_SliderVol.EnableWindow(false);
		return;
	}

	m_SliderVol.EnableWindow(true);
	m_PlayMixer.SetLine(nID - ID_MIXITEM_PLAY0);
	m_SliderVol.SetPos(m_PlayMixer.GetVol(m_PlayMixer.GetCurLine()));
	m_nActiveMixerID = 1;
	//m_BtnMIX_INV.SetIcon(IDI_DIN);
	m_BtnMIX_SEL.SetIcon(IDI_MIXLINE);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ProcessSliderVol(UINT nSBCode, UINT nPos)
{
   // Get the minimum and maximum scroll-bar positions.
   int minpos, maxpos, curpos;
   m_SliderVol.GetRange(minpos, maxpos);
   curpos = m_SliderVol.GetPos();

/*   // Determine the new position of scroll box.
   switch (nSBCode)
   {
   case SB_LEFT:
      curpos = minpos;
      break;
   case SB_RIGHT:
      curpos = maxpos;
      break;
   case SB_LINELEFT:
      if (curpos < minpos)
         curpos--;
      break;
   case SB_LINERIGHT:
      if (curpos > maxpos)
         curpos++;
      break;
   case SB_PAGELEFT:
	  if (curpos > minpos)
		curpos = max(minpos, curpos - m_SliderVol.GetPageSize());
	  break;
   case SB_PAGERIGHT:
      if (curpos < maxpos)
         curpos = min(maxpos, curpos + m_SliderVol.GetPageSize());
      break;
   case SB_THUMBTRACK:
   case SB_THUMBPOSITION:
      curpos = nPos;
      break;
   default:
	   return;
   }
   // Set the new position of the thumb (scroll box).
   m_SliderVol.SetPos(curpos);
*/
	// информируем пользователя
	int nPercent = int(100.0 * curpos / (maxpos - minpos));

	CString strTitle;
	if(m_nActiveMixerID == 1)
	{
		if(m_PlayMixer.GetLinesNum() == 0)
			return;
		strTitle.Format(IDS_VOLUME_TITLE, nPercent,
			m_PlayMixer.GetLineName(m_PlayMixer.GetCurLine()));
		m_PlayMixer.SetVol(nPercent);
	}
	else
	{
		if(m_RecMixer.GetLinesNum() == 0)
			return;
		strTitle.Format(IDS_VOLUME_TITLE, nPercent,
			m_RecMixer.GetLineName(m_RecMixer.GetCurLine()));
		m_RecMixer.SetVol(nPercent);
	}

	// обрабатываем сообщения слайдера
	switch (nSBCode)
	{
	case SB_THUMBTRACK:
		m_title->SetTitleText(strTitle, 10000);
		break;
	case SB_THUMBPOSITION:
		m_title->Restore();
		break;
	case SB_PAGELEFT:
	case SB_PAGERIGHT:
	case SB_LINELEFT:
	case SB_LINERIGHT:
		//m_title->SetTitleText(strTitle, 1200);
		break;
	}
}
void CMainFrame::OnVolUpA() 
{
	// проверяем на доступность линий микшеров
	if(m_nActiveMixerID == 0 && m_RecMixer.GetLinesNum() == 0)
		return;
	if(m_nActiveMixerID == 1 && m_PlayMixer.GetLinesNum() == 0)
		return;

	int nPos = min(m_SliderVol.GetRangeMax(),
		m_SliderVol.GetPos() + m_SliderVol.GetPageSize());
	m_SliderVol.SetPos(nPos);

	this->PostMessage(WM_HSCROLL, MAKEWPARAM(0, SB_PAGERIGHT),
		(LPARAM)m_SliderVol.GetSafeHwnd());
}

void CMainFrame::OnVolDownA() 
{
	// проверяем на доступность линий микшеров
	if(m_nActiveMixerID == 0 && m_RecMixer.GetLinesNum() == 0)
		return;
	if(m_nActiveMixerID == 1 && m_PlayMixer.GetLinesNum() == 0)
		return;

	int nPos = max(m_SliderVol.GetRangeMin(),
		m_SliderVol.GetPos() - m_SliderVol.GetPageSize());
	m_SliderVol.SetPos(nPos);

	this->PostMessage(WM_HSCROLL, MAKEWPARAM(0, SB_PAGELEFT),
		(LPARAM)m_SliderVol.GetSafeHwnd());
}


/////////////////////////////////////////////////////////////////////////////
// ШЕДУЛЕР
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnSched()
{
#ifndef _DEBUG
	// кнопка задизаблена после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	bool bIsEnabled		= m_conf.GetConfDialSH2()->bIsEnabled  != 0;
	bool bSchedStart	= m_conf.GetConfDialSH2()->bSchedStart != 0;
	SHR_TIME* ptStart	= &m_conf.GetConfDialSH2()->t_start;
	SHR_TIME* ptRec		= &m_conf.GetConfDialSH2()->t_rec;
	SHR_TIME* ptStop	= &m_conf.GetConfDialSH2()->t_stop;

	if(!bIsEnabled) // если шедулер еще выключен, то включаем
	{	// если время старта не задано, то включим шедулер при записи
		if(bSchedStart) {
			if(m_conf.GetConfDialSH2()->nStopByID == 1) {
				m_sched2.SetRecTime(ptRec, ptStart);
				TRACE("==> OnBtnSched: SetRecTime(%d:%d:%d, %d:%d:%d)\n",
					ptRec->h, ptRec->m, ptRec->s, ptStart->h, ptStart->m, ptStart->s);
			}
			else {
				m_sched2.SetStopTime(ptStop, ptStart);
				TRACE("==> OnBtnSched: SetStopTime(%d:%d:%d, %d:%d:%d)\n",
					ptStop->h, ptStop->m, ptStop->s, ptStart->h, ptStart->m, ptStart->s);
			}
			if(!m_sched2.Start())
				return;
		}
		// если состояние программы - запись, то вкл. шед
		else if(m_nState == RECORD_STATE) {
			if(m_conf.GetConfDialSH2()->nStopByID == 1)
				m_sched2.SetRecTime(ptRec, NULL);
			else
				m_sched2.SetStopTime(ptStop, NULL);
			if(!m_sched2.Start())
				return;
		}

		/*if(bSchedStart)
		{	// дизаблим кн. записи при старте по-расписанию
			m_BtnREC.ModifyStyle(0, WS_DISABLED);
			m_BtnREC.Invalidate(false);
		}*/
		m_StatWnd.m_btnSched.SetState(BTN_PRESSED);
	}
	else
	{	// выключаем шедулер
		m_sched2.Stop();

		/*if(bSchedStart)
		{	// возвращаем кнопку записи в нормальное состояние
			m_BtnREC.ModifyStyle(WS_DISABLED, 0);
			m_BtnREC.Invalidate(false);
		}*/
		m_StatWnd.m_btnSched.SetState(BTN_NORMAL);
	}

	// сохраняем состояние шедулера
	m_conf.GetConfDialSH2()->bIsEnabled = !bIsEnabled;

	// обновляем состояние кнопки "Запись" при автостарте шедулера
	UpdateButtonState(IDB_BTNREC);

	/*// обновляем окно времени
	int nRecTime = 0;
	if(m_conf.GetConfDialSH2()->bIsEnabled)
		nRecTime = m_sched2.GetRecTimeInSec();
	m_TimeWnd.SetTime(nRecTime);
	*/
}

/////////////////////////////////////////////////////////////////////////////
// IN: nAction - запланированное действие
//		0 - старт записи
//		1 - стоп записи
void Scheduler2Function(int nAction)
{
	CMainFrame* pMainWnd = (CMainFrame *)AfxGetMainWnd();

	if(nAction == 0)		// обрабатываем СТАРТ записи
	{
		// если файл не открыт - сами создаем файл
		if(!pMainWnd->m_pSndFile) {
			CString strName = pMainWnd->GetAutoName(CString(""));
			CString strPath = pMainWnd->m_conf.GetConfProg()->strLastFilePath;
			if(strPath.IsEmpty())
				strPath = pMainWnd->m_strDir;
			strName = strPath + "\\" + strName;
			TRACE1("==> Scheduler2Function: trying to open file: %s\n", strName);
			pMainWnd->OpenFile(strName);
		}
		
		// запускаем внешний файл (если есть)
		if(pMainWnd->m_conf.GetConfDialSH2()->bRunExternal)	{
			ShellExecute(0, NULL, pMainWnd->m_conf.GetConfDialSH2()->strFileName,
				NULL, NULL, SW_SHOWNORMAL);
		}

		// если состояние - не "запись", то начинаем записывать
		if(pMainWnd->m_nState != RECORD_STATE) {
			TRACE0("==> Scheduler2Function: call OnBtnREC to start recording\n");
			pMainWnd->OnBtnSTOP();
			pMainWnd->OnBtnREC();
		}
	}
	else if(nAction == 1)	// обрабатываем СТОП записи
	{
		// останавливаем запись
		pMainWnd->OnBtnSTOP();

		// нужно ли выключить компьютер?
		if(pMainWnd->m_conf.GetConfDialSH2()->action.shutDown) {
			CTimerDlg dlg;
			dlg.m_tdi.strDlgText = "Recording complete. Shutdown computer?";
			if(dlg.DoModal() == IDOK)
				ShutDownComputer();
			return;
		}

		// нужно ли закрыть все программы?
		if(pMainWnd->m_conf.GetConfDialSH2()->action.closeAll) {
			CTimerDlg dlg;
			dlg.m_tdi.strDlgText = "Recording complete. Close the program?";
			if(dlg.DoModal() == IDOK)
				CloseAllPrograms();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// МОНИТОРИНГ УРОВНЯ ЗВУКА
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnMonitoring()
{
#ifndef _DEBUG
	// кнопка задизаблена после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	if(!m_bMonitoringBtn)
	{
		m_bMonitoringBtn = true;
		if(!MonitoringStart() && (m_nState == STOP_STATE))
		{	
			AfxMessageBox("Monitoring error!", MB_OK);
			m_bMonitoringBtn = false;
			return;
		}
		m_StatWnd.m_btnMon.SetState(BTN_PRESSED);
	}
	else
	{
		MonitoringStop();
		m_bMonitoringBtn = false;
		m_StatWnd.m_btnMon.SetState(BTN_NORMAL);
	}

	//m_bMonitoringBtn = !m_bMonitoringBtn;
	m_conf.GetConfProg()->bMonitoring = m_bMonitoringBtn;
}

/////////////////////////////////////////////////////////////////////////////
bool CMainFrame::IsMonitoringOnly()
{
	return m_bMonitoringBtn && (m_nState == STOP_STATE);
}

/////////////////////////////////////////////////////////////////////////////
bool CMainFrame::MonitoringStart()
{
	SOUND_INFO si;
	int nResult;

	// проверка на не "стоп" состояние
	if(m_nState != STOP_STATE)
		return false;

	si.nFreq	= m_conf.GetConfDialMp3()->nFreq;
	si.nStereo	= m_conf.GetConfDialMp3()->nStereo;
	si.nBitrate	= m_conf.GetConfDialMp3()->nBitrate;
	si.nBits	= 16;
	m_GraphWnd.SetConfig(&si);

	// запускаем запись
	if(m_pWaveIn)
		SAFE_DELETE(m_pWaveIn);
	m_pWaveIn = new CWaveIn(_RecordProc);
	nResult = m_pWaveIn->Open(si.nFreq, si.nBits, si.nStereo) == 0;
	nResult = m_pWaveIn->Start();
	if(!nResult)
		SAFE_DELETE(m_pWaveIn);

	return (nResult != 0);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::MonitoringStop()
{
	// проверка на не "стоп" состояние
	if(m_nState != STOP_STATE)
		return;

	SAFE_DELETE(m_pWaveIn);
	m_GraphWnd.Clear();
}

/////////////////////////////////////////////////////////////////////////////
// VAS 
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnVas()
{
#ifndef _DEBUG
	// кнопка задизаблена после завершения триального периода
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	CONF_DIAL_VAS* pConfig = m_conf.GetConfDialVAS();

	if(!m_vas.IsRunning())
	{
		m_vas.InitVAS(pConfig->nThreshold, pConfig->nWaitTime);
		m_StatWnd.m_btnVas.SetState(BTN_PRESSED);
		m_GraphWnd.ShowVASMark(pConfig->nThreshold);
	}
	else
	{
		m_vas.StopVAS();
		m_StatWnd.m_btnVas.SetState(BTN_NORMAL);
		UpdateIcoWindow();
		if(m_nState == RECORD_STATE)
			m_TrayIcon.SetIcon(IDI_TRAY_REC);
		m_GraphWnd.HideVASMark();
	}

	pConfig->bEnable = m_vas.IsRunning();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ProcessVAS(bool bVASResult)
{
	static bool bOldVASResult = false;

	if(bVASResult == true)
	{	// обработка команд при срабатывании VAS
		switch (m_conf.GetConfDialVAS()->nAction)
		{
		case 0:
			m_IcoWnd.SetNewIcon(ICON_RECVAS);
			m_TrayIcon.SetIcon(IDI_TRAY_PAUSE);
			break;
		case 1:
			this->PostMessage(WM_COMMAND, IDB_BTNSTOP,
				LONG(this->m_BtnSTOP.m_hWnd));
			break;
		}
	}
	else if((bVASResult == false)&&(bOldVASResult == true))
	{	// выход из области VAS
		m_IcoWnd.SetNewIcon(ICON_REC);
		m_TrayIcon.SetIcon(IDI_TRAY_REC);
	}

	bOldVASResult = bVASResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	//AfxMessageBox("Mouse wheel!");
	if(zDelta > 0)
		OnVolUpA();
	if(zDelta < 0)
		OnVolDownA();

	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::UpdateButtonState(UINT nID)
{
	bool bSchedStart   = m_conf.GetConfDialSH2()->bSchedStart != 0;
	bool bSchedEnabled = m_conf.GetConfDialSH2()->bIsEnabled != 0;
	CButton* pBtn = NULL;

	// получаем указатель на дочернюю кнопку
	pBtn = (CButton *)GetDlgItem(nID);
	ASSERT(pBtn);

	pBtn->ModifyStyle(WS_DISABLED, 0);	// сбрасываем состояние кнопки

	// обработка кнопки "запись"
	if(nID == IDB_BTNREC)
	{
		// дизаблим кнопку "запись" в состоянии "стоп" и при вкл. старте шедулера
		if(bSchedEnabled && bSchedStart && (m_nState == STOP_STATE))
			pBtn->ModifyStyle(0, WS_DISABLED);

		// дизаблим кнопку "запись" при воспроизведении
		if((m_nState == PLAY_STATE) || (m_nState == PAUSEPLAY_STATE))
			pBtn->ModifyStyle(0, WS_DISABLED);
	}

	// обработка кнопки "воспроизведение"
	if(nID == IDB_BTNPLAY)
	{
		// дизаблим кнопку "воспроизведение" при записи
		if((m_nState == RECORD_STATE) || (m_nState == PAUSEREC_STATE))
			pBtn->ModifyStyle(0, WS_DISABLED);
	}

	pBtn->Invalidate(false); // обновляем кнопку
}

/////////////////////////////////////////////////////////////////////////////

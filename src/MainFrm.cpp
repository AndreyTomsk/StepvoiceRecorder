#include "stdafx.h"
#include <map>
#include <cmath>

#include "MP3_Recorder.h"
#include "MainFrm.h"
#include "MainFrm_Helpers.h"
#include "BASS_Functions.h"
#include "RecordingSourceDlg.h"
#include <basswasapi.h>

//#include "FilterChain.h"
#include "WasapiRecorder.h"
#include "WasapiRecorderMulti.h"
#include "VASFilter.h"
#include "Encoder_MP3.h"
#include "FilterFileWriter.h"
#include "FilterFileWriterWAV.h"
#include "MySheet.h"
#include "FileUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
// shared data
#pragma data_seg(".SHARED")

TCHAR g_command_line[MAX_PATH] = {0};

#pragma data_seg()

#pragma comment(linker, "/section:.SHARED,RWS")
// end shared data


////////////////////////////////////////////////////////////////////////////////
bool IsPlaying(const ProgramState& a_state)
{
	return (a_state == PLAY_STATE || a_state == PAUSEPLAY_STATE);
};

bool IsRecording(const ProgramState& a_state)
{
	return (a_state == RECORD_STATE || a_state == PAUSEREC_STATE);
};

////////////////////////////////////////////////////////////////////////////////
#define WM_ICON_NOTIFY   WM_USER+10
#define WM_FILTER_NOTIFY WM_USER+11

static const UINT UWM_ARE_YOU_ME = ::RegisterWindowMessage(
	_T("UWM_ARE_YOU_ME-{B87861B4-8BE0-4dc7-A952-E8FFEEF48FD3}"));

static const UINT UWM_PARSE_LINE = ::RegisterWindowMessage(
	_T("UWM_PARSE_LINE-{FE0907E6-B77E-46da-8D2B-15F41F32F440}"));

LRESULT CMainFrame::OnAreYouMe(WPARAM, LPARAM)
{
	return UWM_ARE_YOU_ME;
}

LRESULT CMainFrame::OnParseLine(WPARAM, LPARAM)
{
	CString l_cmd_line = g_command_line;
	CString l_file_cmd = _T("/file=\"");
	CString l_file_name = _T("{Desktop}/{Autoname}.mp3");
	bool l_cmd_record = false;

	if (l_cmd_line.Find(_T("/close")) != -1)
	{
		// Using IDM_TRAY_EXIT to correctly close if options dialog is opened.
		PostMessage(WM_COMMAND, MAKEWPARAM(IDM_TRAY_EXIT, 0), 0);
		return UWM_PARSE_LINE;
	}
	if (l_cmd_line.Find(_T("/record")) != -1)
	{
		l_cmd_record = true;
	}
	if (l_cmd_line.Find(l_file_cmd) != -1)
	{
		// Getting last position of a file name
		int l_start_pos = l_cmd_line.Find(l_file_cmd) + l_file_cmd.GetLength();
		int l_end_pos = l_start_pos;
		for (; l_end_pos < l_cmd_line.GetLength(); l_end_pos++)
		{
			if (_T("\"") == l_cmd_line.Mid(l_end_pos, 1))
				break;
		}
		//Extracting file name
		l_file_name = l_cmd_line.Mid(l_start_pos, l_end_pos - l_start_pos);
	}

	l_file_name = ParseFileName(l_file_name);
	if (l_cmd_record)
	{
		// Change file name if it is currently exist
		int l_number = 2;
		DWORD l_error_code = 0;
		while (!Helpers::IsSuitableForRecording(l_file_name, &l_error_code))
		{
			if (ERROR_PATH_NOT_FOUND == l_error_code)
			{
				AfxMessageBox(IDS_ERROR_DIRECTORY, MB_OK|MB_ICONWARNING);
				return UWM_PARSE_LINE;
			}

			CString l_addition;

			// Removing old addition.
			l_addition.Format(_T("[%d]"), l_number - 1);
			l_file_name.Replace(l_addition, _T(""));

			// Adding new addition, [N] like.
			l_addition.Format(_T("[%d]"), l_number++);
			int l_dot_pos = l_file_name.ReverseFind(_T('.'));
			l_dot_pos = (l_dot_pos != -1) ? l_dot_pos : l_file_name.GetLength();
			l_file_name.Insert(l_dot_pos, l_addition);
		}
	}
	
	OpenFile(l_file_name);
	if (l_cmd_record)
	{
		PostMessage(WM_COMMAND, MAKEWPARAM(IDM_SOUND_REC, 0), NULL);
	}
	return UWM_PARSE_LINE;
}

////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()

	ON_COMMAND(IDM_OPT_TOP, OnOptTop)
	ON_COMMAND(IDM_OPT_EM, OnOptEm)
	ON_COMMAND(IDM_OPT_MONITOR, OnOptMonitor)
	ON_COMMAND(IDM_OPT_VAS, OnOptVAS)
	ON_COMMAND(ID_TOOLS_AUTOGAINCONTROL, OnOptAutoGainControl)

	ON_UPDATE_COMMAND_UI(IDM_OPT_MONITOR, OnUpdateOptMonitor)
	ON_UPDATE_COMMAND_UI(IDM_OPT_VAS, OnUpdateOptVAS)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_AUTOGAINCONTROL, OnUpdateOptAutoGainControl)

	ON_BN_CLICKED(IDB_BTNOPEN, OnBtnOPEN)
	ON_BN_CLICKED(IDB_BTNPLAY, OnBtnPLAY)
	ON_BN_CLICKED(IDB_BTNSTOP, OnBtnSTOP)
	ON_BN_CLICKED(IDB_BTNREC,  OnBtnREC)
	ON_BN_CLICKED(IDB_MIX_SEL, OnBtnMIX_SEL)
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_MOVING()
	ON_WM_NCLBUTTONDOWN()
	ON_COMMAND(IDM_OPT_COM, OnOptCom)
	ON_COMMAND(IDM_FILE_FINDFILE, OnFileFindfile)
	ON_COMMAND(IDM_FILE_CLOSE, OnFileClose)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(IDM_SOUND_REC, OnUpdateSoundRec)
	ON_COMMAND(IDA_SOUND_REC, OnSoundRecA)
	ON_COMMAND(IDA_SOUND_PLAY, OnSoundPlayA)
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
	ON_COMMAND(IDA_VOL_UP, OnVolUpA)
	ON_COMMAND(IDA_VOL_DOWN, OnVolDownA)
	ON_COMMAND(IDM_FILE_CREATEOPEN, OnBtnOPEN)
	ON_COMMAND(IDM_SOUND_REC, OnBtnREC)
	ON_COMMAND(IDM_SOUND_STOP, OnBtnSTOP)
	ON_COMMAND(IDM_SOUND_PLAY, OnBtnPLAY)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UWM_ARE_YOU_ME, OnAreYouMe)
	ON_REGISTERED_MESSAGE(UWM_PARSE_LINE, OnParseLine)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_SOUND_PLAY, IDM_SOUND_END, OnUpdateSoundPlay)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_MESSAGE(WM_FILTER_NOTIFY, OnFilterNotify)
	ON_MESSAGE(WM_RECSOURCE_DLGCLOSED, OnRecSourceDialogClosed)
	ON_MESSAGE(WM_RECSOURCE_CHANGED, OnRecSourceChanged)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////

float CMainFrame::PeaksCallback_Wasapi(int channel, void* userData)
{
	IWasapiRecorder* recorder = static_cast<IWasapiRecorder*>(userData);
	return (recorder != NULL) ? recorder->GetPeakLevel(channel) : 0;
}
//------------------------------------------------------------------------------

int CMainFrame::LinesCallback_Wasapi(int channel, float* buffer, int bufferSize, void* userData)
{
	IWasapiRecorder* recorder = static_cast<IWasapiRecorder*>(userData);
	return (recorder != NULL) ? recorder->GetChannelData(channel, buffer, bufferSize) : 0;
}

////////////////////////////////////////////////////////////////////////////////

float CMainFrame::PeaksCallback(int channel, void* userData)
{
	CMainFrame* mainFrame = static_cast<CMainFrame*>(userData);

	DWORD peakLevel = BASS_ChannelGetLevel(mainFrame->m_bassPlaybackHandle);
	if (peakLevel == -1 || channel < 0 || channel > 1)
		return 0;

	peakLevel = (channel == 0) ? LOWORD(peakLevel) : HIWORD(peakLevel);
	return float(peakLevel) / 32768;
}

//------------------------------------------------------------------------------
int CMainFrame::LinesCallback(int channel, float* buffer, int bufferSize, void* userData)
{
	CMainFrame* mainFrame = static_cast<CMainFrame*>(userData);

	float* l_buffer_ptr = buffer + channel;
	const int l_bytes_2_copy = (bufferSize - channel) * sizeof(float);

	const int l_bytes_copied = BASS_ChannelGetData(
		mainFrame->m_bassPlaybackHandle, l_buffer_ptr, (l_bytes_2_copy | BASS_DATA_FLOAT));

	return (l_bytes_copied == -1) ? 0 : l_bytes_copied / sizeof(float);
}

////////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
	:m_playback_volume(1.0) //full volume
	,m_recording_volume(1.0)
	,m_monitoringChain(HandleFilterNotification, this)
	,m_recordingChain(HandleFilterNotification, this)
	,m_bassPlaybackHandle(0)
	,m_destroyingMainWindow(false)
{
	m_title		= NULL;

	m_nState    = STOP_STATE;

	m_bAutoMenuEnable = false;

	// Init window snapping
	m_szMoveOffset.cx = 0;
	m_szMoveOffset.cy = 0;
	m_nSnapPixels = 7;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rDesktopRect, 0);

	BASS_SetConfig(BASS_CONFIG_FLOATDSP, TRUE);
	BASS_SetConfig(BASS_CONFIG_REC_BUFFER, 500);

	// Note: calls OnCreate handler.
	LoadFrame(IDR_MAINFRAME, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX);
}

//====================================================================
CMainFrame::~CMainFrame()
{
	//!!!test
	BASS_WASAPI_Stop(TRUE);
	BASS_WASAPI_Free();

	SAFE_DELETE(m_title);
}

/////////////////////////////////////////////////////////////////////////////

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Adjusting vertical window size
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

	//Setting window on top.
	bool curOnTop = RegistryConfig::GetOption(_T("General\\Always on Top"), false);
	Helpers::SetOnTop(this, curOnTop);

	// Creating interface windows
	m_IcoWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(1, 0, 24, 25), this, IDW_ICO);
	m_TimeWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(24, 0, 106, 25), this, IDW_TIME);
	m_GraphWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(1, 24, 106, 77), this, IDW_GRAPH);
	m_StatWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(106, 0, 282, 50), this, IDW_STAT);
	
	m_IcoWnd.ShowWindow(SW_SHOW);   m_IcoWnd.UpdateWindow();
	m_TimeWnd.ShowWindow(SW_SHOW);  m_TimeWnd.UpdateWindow();
	m_GraphWnd.ShowWindow(SW_SHOW); m_GraphWnd.UpdateWindow();
	m_StatWnd.ShowWindow(SW_SHOW);  m_StatWnd.UpdateWindow();

	// Adjusting the position and volume sliders
	CRect rT(106, 55, 286, 78);
	CRgn rgnT;
	rgnT.CreateRectRgn(0+6, 0, rT.right-rT.left-6, 26/*rT.bottom-rT.top*/);
	m_SliderTime.Create(WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_BOTH,
		rT, this, IDS_SLIDERTIME);
	m_SliderTime.SetWindowRgn(rgnT, false);
	m_SliderTime.SetRange(0, 1000);

	m_SliderFrame.Create(_T("Static"), _T(""), WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,
		CRect(109, 53, 283, 82), this, IDC_STATIC);

	CRect rV(185, 84, 185+70, 84+23);
	CRgn rgnV;
	rgnV.CreateRectRgn(0+6, 0, rV.right-rV.left-6, 26);
	m_SliderVol.Create(WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_BOTH,
	//m_SliderVol.Create(WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_NOTHUMB,
		rV, this, IDS_SLIDERVOL);
	m_SliderVol.SetWindowRgn(rgnV, false);
	m_SliderVol.SetRange(0, 100);
	m_SliderVol.SetPageSize(5);
	m_SliderVol.SetLineSize(1);
	
	// Creating the control buttons
	int	iX = 6, iY = 85, idx = 45, idy = 23;
	m_BtnOPEN.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNOPEN); iX += idx + 1;
	m_BtnREC.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNREC);  iX += idx + 1;
	m_BtnSTOP.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNSTOP); iX += idx + 1;
	m_BtnPLAY.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_BTNPLAY); iX = 226; idx = 25;

	iX = 251; idx = 27;
	m_BtnMIX_SEL.Create(CRect(iX, iY, iX+idx, iY+idy), this, IDB_MIX_SEL);

	m_BtnOPEN.SetIcon(IDI_OPEN);
	m_BtnPLAY.SetIcon(IDI_PLAY);
	m_BtnSTOP.SetIcon(IDI_STOP);
	m_BtnREC.SetIcon(IDI_REC);

	m_BtnFrame.Create(_T("Static"), _T(""), WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,
		CRect(1, 80, 283, 113), this, IDC_STATIC);

	EnableToolTips(true);


	m_title = new CTitleText(this->GetSafeHwnd());

	OnFileClose();


	// Creating icon for the system tray
	m_TrayIcon.Create(this, WM_ICON_NOTIFY, _T("Stepvoice Recorder"),
		LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TRAY_STOP)), IDR_TRAY_MENU);
	UpdateTrayText();
	if(RegistryConfig::GetOption(_T("General\\Show icon in tray"), 0))
		m_TrayIcon.ShowIcon();

	// Setting up Monitoring
	const bool monEnabled = RegistryConfig::GetOption(_T("General\\Sound Monitor"), 0);
	m_StatWnd.m_btnMon.SetCheck(monEnabled);


	//Disabling Pro features by default.
	m_StatWnd.m_btnVas.Enable(false);
	CMenu* toolsSubMenu = GetMenu()->GetSubMenu(2);
	toolsSubMenu->EnableMenuItem(ID_TOOLS_AUTOGAINCONTROL, MF_GRAYED|MF_BYCOMMAND);
	toolsSubMenu->EnableMenuItem(IDM_OPT_VAS, MF_GRAYED|MF_BYCOMMAND);

	//Enabling Pro features in registered version (silence detection and input auto gain)
	REG_CRYPT_BEGIN;
	const bool vasEnabled = RegistryConfig::GetOption(_T("Tools\\VAS\\Enable"), 0);
	m_StatWnd.m_btnVas.SetCheck(vasEnabled);
	m_StatWnd.m_btnVas.Enable(true);
	toolsSubMenu->EnableMenuItem(ID_TOOLS_AUTOGAINCONTROL, MF_ENABLED|MF_BYCOMMAND);
	toolsSubMenu->EnableMenuItem(IDM_OPT_VAS, MF_ENABLED|MF_BYCOMMAND);
	if (RegistryConfig::GetOption(_T("General\\AutoGainControl"), false))
		m_autoGainControl.reset(new CAutoGainControl());
	REG_CRYPT_END;

	//Removing unnecessary menu entries in Pro version.
	#ifndef _DEBUG
	REG_CRYPT_BEGIN;
	GetMenu()->RemoveMenu(ID_UPGRADE, MF_BYCOMMAND);
	GetMenu()->GetSubMenu(3)->RemoveMenu(ID_HELP_ENTERCODE, MF_BYCOMMAND);
	REG_CRYPT_END;
	#endif


	//CString test = Helpers::GetNewRecordingFilePath(_T("D:\\Feb26_2016_02.mp3"));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.cx = 290;
	cs.cy = 120 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
	
	int nScreenCX = GetSystemMetrics(SM_CXSCREEN);
	int nScreenCY = GetSystemMetrics(SM_CYSCREEN);
	cs.x  = RegistryConfig::GetOption(_T("General\\Xcoord"), 300);
	cs.y  = RegistryConfig::GetOption(_T("General\\Ycoord"), 200);
	if (cs.x >= (nScreenCX-40) || cs.y >= (nScreenCY-40))
	{
		cs.x  = (nScreenCX - cs.cx)/2;
		cs.y  = (nScreenCY - cs.cy)/2;
	}

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = _T("SvRec");

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.hInstance	= AfxGetInstanceHandle();
	wc.lpfnWndProc	= ::DefWindowProc;
	wc.style		= CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wc.hbrBackground= HBRUSH(16);
	wc.hIcon		= ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	wc.lpszClassName= _T("SvRec");
	AfxRegisterClass(&wc);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_DISPLAYCHANGE)
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rDesktopRect, 0);

	return CFrameWnd::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (RegistryConfig::GetOption(_T("General\\Easy Move"), 1))
	{
		CPoint pt;
		GetCursorPos(&pt);
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
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
	m_destroyingMainWindow = true;
	CFrameWnd::OnDestroy();

	OnFileClose();
	MonitoringStop();
	BASS_Free();

	if (!IsIconic())
	{
		CRect r; GetWindowRect(&r);
		RegistryConfig::SetOption(_T("General\\Xcoord"), r.left);
		RegistryConfig::SetOption(_T("General\\Ycoord"), r.top);
	}
	RegistryConfig::SetOption(_T("General\\Playback volume"), int(m_playback_volume * 10000));
}

//===========================================================================
// MENU : File
//===========================================================================
void CMainFrame::OnFileClose() 
{
	OnBtnSTOP();

	//Recording and playback parameters should be mutually exclusive.
	//But, calling OnFileClose with nothing opened, should not assert.

	if (!m_recordingFileName.IsEmpty())
	{
		m_recordingFileName.Empty();
	}
	if (m_bassPlaybackHandle != NULL)
	{
		BASS_StreamFree(m_bassPlaybackHandle);
		m_bassPlaybackHandle = 0;
	}

	// Setting the default window text: no_file - Stepvoice Recorder
	CString strTitle, strNoFile((LPCSTR)IDS_NOFILE);
	AfxFormatString1(strTitle, IDS_FILETITLE, strNoFile);
	m_title->SetTitleText(strTitle);

	m_TimeWnd.Reset();
	UpdateStatWindow();
	UpdateTrayText();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileClear() 
{
	if (m_recordingFileName.IsEmpty() && !m_bassPlaybackHandle)
		return;

	const CString lastFilePath = RegistryConfig::GetOption(_T("General\\LastFile"), CString());
	if (lastFilePath.IsEmpty())
		return;

	CString msgWarning;
	AfxFormatString1(msgWarning, IDS_CLEAR_ASK, lastFilePath);
	if (AfxMessageBox(msgWarning, MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	OnFileClose();
	try
	{
		CFile file(lastFilePath, CFile::modeCreate);
	}
	catch (CException *e)
	{
		e->ReportError();
		e->Delete();
		return;
	}
	OpenFile(lastFilePath);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileDelete() 
{
	if (m_recordingFileName.IsEmpty() && !m_bassPlaybackHandle)
		return;

	const CString lastFilePath = RegistryConfig::GetOption(_T("General\\LastFile"), CString());
	if (lastFilePath.IsEmpty())
		return;

	//CString msgWarning;
	//AfxFormatString1(msgWarning, IDS_DEL_ASK, lastFilePath);
	//if (AfxMessageBox(msgWarning, MB_YESNO|MB_ICONWARNING) != IDYES)
	//	return;

	OnFileClose();

	TCHAR fromBuffer[MAX_PATH+2]; //String must be double-null terminated.
	ZeroMemory(fromBuffer, sizeof(fromBuffer));
	lstrcpy(fromBuffer, lastFilePath);

	//Removing file to RecycleBin.
	SHFILEOPSTRUCT fileOp;
	ZeroMemory(&fileOp, sizeof(fileOp));
	fileOp.wFunc = FO_DELETE;
	fileOp.pFrom = fromBuffer;
	fileOp.fFlags = FOF_ALLOWUNDO | FOF_SILENT;
	
	SHFileOperation(&fileOp);
}

//=======not from menu, but useful one :)===========================================
void CMainFrame::OpenFile(CString filePath)
{
	OnFileClose(); //updates m_recordingFileName and m_bassPlaybackHandle.

	if (Helpers::IsSuitableForRecording(filePath))
	{
		LOG_DEBUG() << __FUNCTION__ << ", " << filePath << " (NEW)";
		m_recordingFileName = filePath;
	}
	else
	{
		LOG_DEBUG() << __FUNCTION__ << ", " << filePath << " (EXISTING)";

		BOOL result = BASS_Init(-1, 44100, 0, GetSafeHwnd(), NULL);
		if (!result)
		{
			//DWORD currentDeviceID = BASS_GetDevice();
			//bool debug = true;
			//Bass::ShowErrorFrom(_T("BASS_Init"));
			//return;
		}
		m_bassPlaybackHandle = BASS_StreamCreateFile(false, filePath, 0, 0, 0);
		if (!m_bassPlaybackHandle)
		{
			DWORD currentDeviceID = BASS_GetDevice();
			Bass::ShowErrorFrom(_T("BASS_StreamCreateFile"));
			BASS_Free();
			return;
		}
	}

	RegistryConfig::SetOption(_T("General\\LastFile"), filePath);

	// Modifying window caption to "<FILE> - StepVoice Recorder".

	CString newCaption;
	AfxFormatString1(newCaption, IDS_FILETITLE, FileUtils::GetFileName(filePath));
	m_title->SetTitleText(newCaption);
	
	UpdateStatWindow();
	UpdateTrayText();
}

//===========================================================================
void CMainFrame::OnFileFindfile() 
{
	const CString lastFilePath = RegistryConfig::GetOption(_T("General\\LastFile"), CString());
	ShellExecute(NULL, _T("open"), _T("explorer"), _T("/select, \"")+lastFilePath+_T("\""),
		NULL, SW_NORMAL);
}

//===========================================================================
// MENU : Sound
//===========================================================================
void CMainFrame::OnSoundBegin()
{
	double l_seconds_all = 0;

	if (m_bassPlaybackHandle)
	{
		l_seconds_all = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));

		// Moving to the beginning on file
		BOOL l_result = BASS_ChannelSetPosition(m_bassPlaybackHandle,
			BASS_ChannelSeconds2Bytes(m_bassPlaybackHandle, 0), BASS_POS_BYTE);
		ASSERT(l_result);

		m_TimeWnd.SetTime(0);
		m_SliderTime.SetCurPos(0);
	}

	UpdateWindowTitle_SeekTo(0.0, l_seconds_all, 1200);
}

//===========================================================================
void CMainFrame::OnSoundRew() 
{
	double l_allsec = 0;
	double l_cursec = 0;

	if (m_bassPlaybackHandle)
	{
		l_cursec = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetPosition(m_bassPlaybackHandle, BASS_POS_BYTE));
		l_allsec = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));

		l_cursec = ((l_cursec - 5) > 0) ? l_cursec - 5 : 0;

		BOOL l_result = BASS_ChannelSetPosition(
			m_bassPlaybackHandle,
			BASS_ChannelSeconds2Bytes(m_bassPlaybackHandle, l_cursec),
			BASS_POS_BYTE);
		ASSERT(l_result);

		m_TimeWnd.SetTime((UINT)l_cursec);
		m_SliderTime.SetCurPos(int(l_cursec/l_allsec*1000));
	}

	UpdateWindowTitle_SeekTo(l_cursec, l_allsec, 1200);
}

//===========================================================================
void CMainFrame::OnSoundFf() 
{
	double l_allsec = 0;
	double l_cursec = 0;

	if (m_bassPlaybackHandle)
	{
		l_cursec = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetPosition(m_bassPlaybackHandle, BASS_POS_BYTE));
		l_allsec = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));

		l_cursec = ((l_cursec + 5) < l_allsec) ? l_cursec + 5 : l_allsec;

		BOOL l_result = BASS_ChannelSetPosition(
			m_bassPlaybackHandle,
			BASS_ChannelSeconds2Bytes(m_bassPlaybackHandle, l_cursec),
			BASS_POS_BYTE);
		ASSERT(l_result);

		m_TimeWnd.SetTime((UINT)l_cursec);
		m_SliderTime.SetCurPos(int(l_cursec/l_allsec*1000));
	}

	UpdateWindowTitle_SeekTo(l_cursec, l_allsec, 1200);
}

//===========================================================================
void CMainFrame::OnSoundEnd() 
{
	double l_allsec = 0;

	if (m_bassPlaybackHandle)
	{
		l_allsec = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));

		BOOL l_result = BASS_ChannelSetPosition(
			m_bassPlaybackHandle,
			BASS_ChannelSeconds2Bytes(m_bassPlaybackHandle, l_allsec - 0.3),
			BASS_POS_BYTE);
		ASSERT(l_result);

		m_TimeWnd.SetTime((UINT)l_allsec);
		m_SliderTime.SetCurPos(1000);
	}

	UpdateWindowTitle_SeekTo(l_allsec, l_allsec, 1200);
}

//===========================================================================
// MENU : Options
//===========================================================================
void CMainFrame::OnOptCom()
{
	CMySheet optDlg;
	if (optDlg.DoModal() != IDOK)
		return

	UpdateStatWindow();

	// Handling "Always on Top" option
	Helpers::SetOnTop(this, RegistryConfig::GetOption(_T("General\\Always on Top"), false));

	// Checking tray options

	if(RegistryConfig::GetOption(_T("General\\Show icon in tray"), 0))
		m_TrayIcon.ShowIcon();
	else
		m_TrayIcon.HideIcon();

	//Updating GUI and filter with new VAS parameters.

	if (m_StatWnd.m_btnVas.IsChecked())
	{
		//Button already pressed and VAS is running. Its
		//parameters could be updated in options dialog.
		//We should update VAS filter (from OnBtnVas handler).
		m_StatWnd.m_btnVas.SetCheck(false);
		m_StatWnd.m_btnVas.SetCheck(true);
	}
}

//===========================================================================
void CMainFrame::OnOptEm() 
{
	//Reverting feature of dragging window from any point
	int curEM = RegistryConfig::GetOption(_T("General\\Easy Move"), 1);
	RegistryConfig::SetOption(_T("General\\Easy Move"), curEM ? 0 : 1);
}

//===========================================================================
void CMainFrame::OnOptTop() 
{
	//Reverting feature

	bool curOnTop = RegistryConfig::GetOption(_T("General\\Always on Top"), false);
	RegistryConfig::SetOption(_T("General\\Always on Top"), !curOnTop);
	Helpers::SetOnTop(this, !curOnTop);
}
//--------------------------------------------------------------------

//===========================================================================
// BUTTONS
//===========================================================================

void CMainFrame::OnBtnOPEN()
{
	// Creating a standard file open dialog

	const CString generatedName = Helpers::GetMp3AutonameFromConfig();
	const CString outputFolder = Helpers::GetOutputFolder();

	CString dialogTitle, filesFilter;
	dialogTitle.LoadString(IDS_FILENEWOPENTITLE);
	filesFilter.LoadString(IDS_FILEFILTER);

	CFileDialog openDialog(true, _T("mp3"), generatedName, OFN_HIDEREADONLY | OFN_EXPLORER, filesFilter);
	openDialog.m_ofn.lpstrTitle= dialogTitle;
	openDialog.m_ofn.lpstrInitialDir = outputFolder;

	if (openDialog.DoModal() == IDOK)
	{	
		OpenFile(openDialog.GetPathName());
		return;
	}
}

//===========================================================================
void CMainFrame::OnBtnPLAY()
{
	LOG_DEBUG() << __FUNCTION__ << ", m_nState=" << m_nState;

	if (!this->CanPlay())
		return;

	if (!m_bassPlaybackHandle)
	{
		OnBtnOPEN();
		if (!m_bassPlaybackHandle)
			return;
	}

	MonitoringStop();

	BASS_ChannelSetAttribute(m_bassPlaybackHandle, BASS_ATTRIB_VOL, m_playback_volume);
	const DWORD CHANNEL_STATE = BASS_ChannelIsActive(m_bassPlaybackHandle);
	switch (CHANNEL_STATE)
	{
	case BASS_ACTIVE_PLAYING:
		BASS_ChannelPause(m_bassPlaybackHandle);
		KillTimer(1);
		m_nState = PAUSEPLAY_STATE;
		break;

	case BASS_ACTIVE_STOPPED:
	case BASS_ACTIVE_PAUSED:
		BASS_ChannelPlay(m_bassPlaybackHandle, false);
		SetTimer(1, 1000, NULL);
		m_nState = PLAY_STATE;
		break;
	}
}

//===========================================================================
void CMainFrame::OnBtnSTOP()
{
	LOG_DEBUG() << __FUNCTION__ << ", m_nState=" << m_nState;

	if (m_nState == STOP_STATE)
		return;

	//Setting stop state in the method beginning, to avoid extra OnBtnSTOP
	//calls from OpenFile below (after recording finished).
	m_nState = STOP_STATE;

	if (m_bassPlaybackHandle)
	{
		ASSERT(m_recordingFileName.IsEmpty());
		KillTimer(1);

		BOOL result = BASS_ChannelStop(m_bassPlaybackHandle);
		ASSERT(result);
		result = BASS_ChannelSetPosition(m_bassPlaybackHandle,
			BASS_ChannelSeconds2Bytes(m_bassPlaybackHandle, 0), BASS_POS_BYTE);
		ASSERT(result);
	}

	if (!m_recordingChain.IsEmpty())
	{
		ASSERT(!m_recordingFileName.IsEmpty());
		KillTimer(2);
		m_GraphWnd.StopUpdate(); //need it here, before recorder chain is destroyed
		//if (m_autoGainControl.get() != NULL)
		//	m_autoGainControl->Stop();

		m_recordingChain.GetFilter<IWasapiRecorder>()->Stop();
		m_recordingChain.GetFilter<FileWriter>()->ForceClose();
		m_recordingChain.GetFilter<CEncoder_MP3>()->WriteVBRHeader(m_recordingFileName);
		m_recordingChain.Empty();

		OpenFile(m_recordingFileName);
	}

	//TODO: refactor. This thing resets playback time in time wnd.
	PostMessage(WM_HSCROLL,  MAKEWPARAM(SB_THUMBPOSITION, 0),
		(LPARAM)m_SliderTime.m_hWnd);

	//if (m_monitoringChain.IsEmpty()) //Removes multiple restart on stop processing.
		MonitoringRestart();
}

//===========================================================================
void CMainFrame::OnBtnREC()
{
	LOG_DEBUG() << __FUNCTION__ << ", m_nState=" << m_nState;

	if (m_recordingFileName.IsEmpty())
	{
		//Automatically creating file, suitable for recording.
		OpenFile(Helpers::GetNewRecordingFilePath());
		ASSERT(!m_recordingFileName.IsEmpty());
	}

	MonitoringStop();

	if (m_recordingChain.IsEmpty())
	{
		const bool vasEnabled = m_StatWnd.m_btnVas.IsEnabled() && m_StatWnd.m_btnVas.IsChecked();
		const int vasThresholdDB = RegistryConfig::GetOption(_T("Tools\\VAS\\Threshold"), -30);
		const int vasWaitTimeMS = RegistryConfig::GetOption(_T("Tools\\VAS\\WaitTime"), 2000);

		int bitrate = RegistryConfig::GetOption(_T("File types\\MP3\\Bitrate"), 128);
		int frequency = RegistryConfig::GetOption(_T("File types\\MP3\\Freq"), 44100);
		int channels = RegistryConfig::GetOption(_T("File types\\MP3\\Stereo"), 1) + 1;

		using namespace WasapiHelpers;
		DevicesArray selectedDevices = CRecordingSourceDlg::GetInstance()->GetSelectedDevices();
		if (selectedDevices.empty() || (selectedDevices.size() == 1 && selectedDevices[0] == EMPTY_DEVICE)) {
			AfxMessageBox(_T("Recording devices not found."), MB_OK);
			return;
		}

		try
		{
			//TODO: temp fix for Multi-recorder bugs - using simple recorder for one device.
			IWasapiRecorder* recorder = NULL;
			if (selectedDevices.size() == 1)
				recorder = new CWasapiRecorder(selectedDevices[0].first, frequency, channels);
			else
				recorder = new CWasapiRecorderMulti(selectedDevices, frequency, channels);

			recorder->AddEvent(VolumeChangedEvent, this);
			recorder->SetVolume(m_recording_volume);

			frequency = recorder->GetActualFrequency();
			channels = recorder->GetActualChannelCount();

			m_recordingChain.AddFilter(dynamic_cast<Filter*>(recorder));
			m_recordingChain.AddFilter(new VasFilter(vasThresholdDB, vasWaitTimeMS, vasEnabled));
			m_recordingChain.AddFilter(new CEncoder_MP3(bitrate, frequency, channels));
			m_recordingChain.AddFilter(new FileWriter(m_recordingFileName));
			//m_recordingChain.AddFilter(new FileWriterWAV(m_recordingFileName, frequency, channels));
		}
		catch (CException* e)
		{
			e->ReportError();
			e->Delete();
			m_recordingChain.Empty();
			return;
		}
	}

	IWasapiRecorder* recorder = m_recordingChain.GetFilter<IWasapiRecorder>();
	if (recorder->IsStarted())
	{
		recorder->Pause();
		KillTimer(2);
		m_nState = PAUSEREC_STATE;
	}
	else if (recorder->IsPaused() || recorder->IsStopped())
	{
		m_recordingChain.GetFilter<VasFilter>()->ResetDetection();
		recorder->Start();
		SetTimer(2, 1000, NULL);
		m_nState = RECORD_STATE;
	}
}

////////////////////////////////////////////////////////////////////////////////
// WM_TIMER message handler
////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);

	double curSeconds = 0.0;
	double allSeconds = 0.0;

	if (nIDEvent == 1)
	{
		ASSERT(m_bassPlaybackHandle != NULL);

		curSeconds = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetPosition(m_bassPlaybackHandle, BASS_POS_BYTE));
		allSeconds = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));

		//Autostop on file end.
		if (BASS_ChannelIsActive(m_bassPlaybackHandle) == BASS_ACTIVE_STOPPED)
			PostMessage(WM_COMMAND, IDB_BTNSTOP, LONG(this->m_BtnSTOP.m_hWnd));
	}
	else if (nIDEvent == 2)
	{
		ASSERT(!m_recordingChain.IsEmpty());
		const int kbitSec = RegistryConfig::GetOption(_T("File types\\MP3\\Bitrate"), 128);
		const int bytesSec = kbitSec * (1000/8);
		const ULONGLONG fileSize = m_recordingChain.GetFilter<FileWriter>()->GetFileLength();

		curSeconds = double(fileSize / bytesSec);
		allSeconds = curSeconds;
	}

	if (!m_SliderTime.IsDragging())
		m_SliderTime.SetCurPos(int(curSeconds/allSeconds * 1000));

	m_TimeWnd.SetTime((UINT)curSeconds);
	UpdateStatWindow();
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::UpdateStatWindow()
{
	int bitrate = RegistryConfig::GetOption(_T("File types\\MP3\\Bitrate"), 128);
	int frequency = RegistryConfig::GetOption(_T("File types\\MP3\\Freq"), 44100);
	int stereo = RegistryConfig::GetOption(_T("File types\\MP3\\Stereo"), 1);
	ULONGLONG fileSize = 0;
	double fileSeconds = 0.0;

	if (m_bassPlaybackHandle != NULL)
	{
		BASS_CHANNELINFO channelInfo;
		BASS_ChannelGetInfo(m_bassPlaybackHandle, &channelInfo);

		fileSize = BASS_StreamGetFilePosition(m_bassPlaybackHandle, BASS_FILEPOS_END);
		fileSeconds = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));

		bitrate = int(fileSize / (125*fileSeconds) + 0.5); //to kbit/sec
		frequency = channelInfo.freq;
		stereo = (channelInfo.chans > 1) ? 1 : 0;
	}
	else if (!m_recordingChain.IsEmpty())
	{
		IWasapiRecorder* recorder = m_recordingChain.GetFilter<IWasapiRecorder>();
		frequency = recorder->GetActualFrequency();
		stereo = recorder->GetActualChannelCount() > 1;

		const int bytesSec = bitrate * (1000/8); //bitrate is kbit/sec
		fileSize = m_recordingChain.GetFilter<FileWriter>()->GetFileLength();
		fileSeconds = double(fileSize / bytesSec);
	}

	const CString strFileSeconds = Helpers::ToString_HMMSS(fileSeconds);
	m_StatWnd.Set(frequency, bitrate, stereo);
	m_StatWnd.Set(fileSize/1024, strFileSeconds);
}

//------------------------------------------------------------------------------
void CMainFrame::UpdateTrayText()
{
	CString wndText;

	GetWindowText(wndText);
	m_TrayIcon.SetTooltipText(wndText);
}

////////////////////////////////////////////////////////////////////////////////
// Main menu update handlers.
////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateSoundRec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsPlaying(m_nState));
}
//------------------------------------------------------------------------------

void CMainFrame::OnUpdateSoundPlay(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!IsRecording(m_nState));
}
//------------------------------------------------------------------------------

void CMainFrame::OnOptMonitor()
{
	const UINT itemState = GetMenu()->GetMenuState(IDM_OPT_MONITOR, MF_BYCOMMAND);
	m_StatWnd.m_btnMon.SetCheck(!(itemState == MF_CHECKED));
}
//------------------------------------------------------------------------------

void CMainFrame::OnOptVAS()
{
	const UINT itemState = GetMenu()->GetMenuState(IDM_OPT_VAS, MF_BYCOMMAND);
	m_StatWnd.m_btnVas.SetCheck(!(itemState == MF_CHECKED));
}
//------------------------------------------------------------------------------

void CMainFrame::OnOptAutoGainControl()
{
	const bool doAutoGain = !RegistryConfig::GetOption(_T("General\\AutoGainControl"), false);
	RegistryConfig::SetOption(_T("General\\AutoGainControl"), doAutoGain);

	if (doAutoGain) {
		/*
		IWasapiRecorder* recorder = NULL;
		if (!m_monitoringChain.IsEmpty())
			recorder = m_monitoringChain.GetFilter<IWasapiRecorder>();
		if (!m_recordingChain.IsEmpty())
			recorder = m_recordingChain.GetFilter<IWasapiRecorder>();

		m_autoGainControl.reset(new CAutoGainControl());
		if (recorder != NULL)
			m_autoGainControl->Start(recorder);
		*/
		if (m_autoGainControl.get() == NULL)
			m_autoGainControl.reset(new CAutoGainControl());
		if (!m_monitoringChain.IsEmpty()) {
			m_autoGainControl->Start(m_monitoringChain.GetFilter<IWasapiRecorder>());
			m_SliderVol.ShowAutomaticVolume(true);
		}
	}
	else {
		m_autoGainControl.reset();
		m_SliderVol.ShowAutomaticVolume(false);
	}
}
//------------------------------------------------------------------------------

void CMainFrame::OnUpdateOptMonitor(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_StatWnd.m_btnMon.IsChecked());
}
//------------------------------------------------------------------------------

void CMainFrame::OnUpdateOptVAS(CCmdUI* pCmdUI)
{
	//Pro-version interface update (main menu)
	REG_CRYPT_BEGIN;
	pCmdUI->SetCheck(m_StatWnd.m_btnVas.IsChecked());
	REG_CRYPT_END;
}
//------------------------------------------------------------------------------

void CMainFrame::OnUpdateOptAutoGainControl(CCmdUI* pCmdUI)
{
	//Pro-version interface update (main menu)
	REG_CRYPT_BEGIN;
	bool isAutoGain = RegistryConfig::GetOption(_T("General\\AutoGainControl"), false);
	pCmdUI->SetCheck(isAutoGain);
	REG_CRYPT_END;
}

//===========================================================================
// ACCELERATORS
//===========================================================================
void CMainFrame::OnSoundRecA() 
{
	if (CanRecord())
	{
		m_BtnREC.Press();
		OnBtnREC();
	}
}

//===========================================================================
void CMainFrame::OnSoundPlayA() 
{
	if (CanPlay())
	{
		m_BtnPLAY.Press();
		OnBtnPLAY();
	}
}

//===========================================================================
// Drag-and-Drop support
//===========================================================================
void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	this->SetForegroundWindow();

	TCHAR szFileName[_MAX_PATH];
	::DragQueryFile(hDropInfo, 0, szFileName, _MAX_PATH);

	OpenFile(CString(szFileName));

	::DragFinish(hDropInfo);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnOptCom();
	CFrameWnd::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRButtonUp(UINT nFlags, CPoint point) 
{
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
	LPCSTR l_old_exec = "rundll32.exe MMSYS.CPL,ShowAudioPropertySheet";
	LPCSTR l_new_exec = "control MMSYS.CPL";

	CMP3_RecorderApp* l_app = (CMP3_RecorderApp* )AfxGetApp();
	WinExec(l_app->IsVistaOS() ? l_new_exec : l_old_exec, SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT nID = ::GetDlgCtrlID((HWND)pNMHDR->idFrom);
	switch (nID)
	{
	case IDB_BTNOPEN:		nID = IDS_TT_OPEN; break;
	case IDB_BTNREC:		nID = IDS_TT_REC;  break;
	case IDB_BTNSTOP:		nID = IDS_TT_STOP; break;
	case IDB_BTNPLAY:		nID = IDS_TT_PLAY; break;
	case IDS_SLIDERTIME:	nID = IDS_TT_SEEKBAR; break;
	case IDW_GRAPH:			nID = IDS_TT_WAVEWND; break;
	case IDW_TIME:			nID = IDS_TT_TIMEWND; break;
	case IDW_STAT:			nID = IDS_TT_STATWND; break;
	case IDB_MIX_SEL:		nID = IDS_TT_MIXSEL; break;
	case IDS_SLIDERVOL:		nID = IDS_TT_VOLBAR; break;
	case IDB_BTN_VAS:		nID = IDS_TT_VAS;	break;
	case IDB_BTN_MON:		nID = IDS_TT_MONITORING;break;
	default:
		return TRUE;
	}

	TOOLTIPTEXT *pText = (TOOLTIPTEXT *)pNMHDR;
	pText->lpszText = MAKEINTRESOURCE(nID);
	pText->hinst = AfxGetInstanceHandle();
	return TRUE;
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

	const bool showIconInTray = RegistryConfig::GetOption(_T("General\\Show icon in tray"), 0);
	const bool minimizeToTray = RegistryConfig::GetOption(_T("General\\Minimize to tray"), 0);
	if (nType==SIZE_MINIMIZED && minimizeToTray)
	{
		if (!showIconInTray)
			m_TrayIcon.ShowIcon();
		ShowWindow(SW_HIDE);
	}
	else if (nType==SIZE_RESTORED && !showIconInTray)
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

			const bool minimizeToTray = RegistryConfig::GetOption(_T("General\\Minimize to tray"), 0);
			if (nCmdShow == SW_MINIMIZE && minimizeToTray)
				nCmdShow = SW_HIDE;

			ShowWindow(nCmdShow);
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
		{
			CWnd* wndFound = this->FindWindow(NULL, CMySheet::GetWindowTitle());
			if (wndFound != NULL)
			{
				CMySheet* optionsDialog = dynamic_cast<CMySheet*>(wndFound);
				optionsDialog->SendMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
			}
		}
		PostMessage(WM_CLOSE);
		break;
	}
		
	return CFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnUpdateTrayPlay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsRecording(m_nState));
}

void CMainFrame::OnUpdateTrayRec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsPlaying(m_nState));
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnMIX_SEL()
{
	CRect r;
	m_BtnMIX_SEL.GetWindowRect(&r);
	CRecordingSourceDlg::GetInstance()->Execute(CPoint(r.right, r.top));
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->m_hWnd == m_SliderTime.m_hWnd)
	{
		ProcessSliderTime(nSBCode, nPos);
	}
	else if (pScrollBar->m_hWnd == m_SliderVol.m_hWnd)
	{
		ProcessSliderVol(nSBCode, nPos);
	}
	CFrameWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

//------------------------------------------------------------------------------
void CMainFrame::ProcessSliderTime(UINT nSBCode, UINT nPos)
{
	double l_seconds_pos = 0;
	double l_seconds_all = 0;

	if (m_bassPlaybackHandle)
	{
		l_seconds_all = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));
		l_seconds_pos = l_seconds_all * nPos / 1000;
	}

	if (SB_THUMBTRACK == nSBCode)
	{
		UpdateWindowTitle_SeekTo(l_seconds_pos, l_seconds_all, 10000);
	}
	else if (SB_THUMBPOSITION == nSBCode)
	{
		m_title->Restore();
		if (m_bassPlaybackHandle)
		{
			if (l_seconds_pos >= l_seconds_all - 0.3)
			{
				l_seconds_pos = max(l_seconds_all - 0.3, 0);
			}
			BOOL l_result = BASS_ChannelSetPosition(
				m_bassPlaybackHandle,
				BASS_ChannelSeconds2Bytes(m_bassPlaybackHandle, l_seconds_pos),
				BASS_POS_BYTE);
			ASSERT(l_result);
			m_TimeWnd.SetTime((UINT)l_seconds_pos);
		}
		else
		{
			m_SliderTime.SetCurPos(0);
		}
	}
}

//------------------------------------------------------------------------------
void CMainFrame::ProcessSliderVol(UINT nSBCode, UINT nPos)
{
	// Get the minimum and maximum scroll-bar positions.
	int minpos, maxpos, curpos;
	m_SliderVol.GetRange(minpos, maxpos);
	curpos = m_SliderVol.GetPos();

	// Informing user
	const int nPercent = int(100.0 * curpos / (maxpos - minpos));

	CString strTitle;
	if (!m_recordingFileName.IsEmpty() || m_nState == STOP_STATE)
	{
		strTitle.Format(IDS_VOLUME_TITLE, nPercent, CString(_T("recording")));
		CSliderUpdateLock lock(m_SliderVol);

		m_recording_volume = float(nPercent) / 100;
		if (!m_recordingChain.IsEmpty())
			m_recordingChain.GetFilter<IWasapiRecorder>()->SetVolume(m_recording_volume);
		if (!m_monitoringChain.IsEmpty())
			m_monitoringChain.GetFilter<IWasapiRecorder>()->SetVolume(m_recording_volume);
	}
	else if (m_bassPlaybackHandle != NULL)
	{
		strTitle.Format(IDS_VOLUME_TITLE, nPercent, CString(_T("playback")));
		m_playback_volume = (float)nPercent / 100;
		BASS_ChannelSetAttribute(m_bassPlaybackHandle, BASS_ATTRIB_VOL, m_playback_volume);
	}

	// Processing slider messages
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
		m_title->SetTitleText(strTitle, 1200);
		break;
	}
}
//------------------------------------------------------------------------------

void CMainFrame::VolumeChangedEvent(float curVolume, void* user)
{
	CMainFrame* mainFrame = static_cast<CMainFrame*>(user);
	mainFrame->m_recording_volume = curVolume; //volume changed from auto gain

	if (!mainFrame->m_SliderVol.IsUpdating())
		mainFrame->m_SliderVol.SetVolume(curVolume);

	//LOG_DEBUG() << __FUNCTION__
	//	<< ", isUpdating=" << mainFrame->m_SliderVol.IsUpdating()
	//	<< ", curVolume=" << curVolume;
}
//------------------------------------------------------------------------------

BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	(zDelta > 0) ?  OnVolUpA() : OnVolDownA();

	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}

//------------------------------------------------------------------------------
void CMainFrame::OnVolUpA() 
{
	if (m_SliderVol.IsWindowEnabled())
	{
		int nPos = min(m_SliderVol.GetRangeMax(),
			m_SliderVol.GetPos() + m_SliderVol.GetPageSize());
		m_SliderVol.SetPos(nPos);

		this->PostMessage(WM_HSCROLL, MAKEWPARAM(0, SB_PAGERIGHT),
			(LPARAM)m_SliderVol.GetSafeHwnd());
	}
}

//------------------------------------------------------------------------------
void CMainFrame::OnVolDownA()
{
	if (m_SliderVol.IsWindowEnabled())
	{
		int nPos = max(m_SliderVol.GetRangeMin(),
			m_SliderVol.GetPos() - m_SliderVol.GetPageSize());
		m_SliderVol.SetPos(nPos);

		this->PostMessage(WM_HSCROLL, MAKEWPARAM(0, SB_PAGELEFT),
			(LPARAM)m_SliderVol.GetSafeHwnd());
	}
}
//------------------------------------------------------------------------------

void CMainFrame::OnBtnMonitoring()
{
	LOG_DEBUG() << __FUNCTION__;

	const bool monitoringButtonChecked = m_StatWnd.m_btnMon.IsChecked();
	RegistryConfig::SetOption(_T("General\\Sound Monitor"), monitoringButtonChecked);

	if (!monitoringButtonChecked)
	{
		MonitoringStop();
	}
	else if ((m_nState == STOP_STATE) && !MonitoringStart())
	{
		AfxMessageBox(_T("Monitoring error."), MB_OK);
		m_StatWnd.m_btnMon.SetCheck(false);
	}
}
//------------------------------------------------------------------------------

bool CMainFrame::MonitoringRestart()
{
	LOG_DEBUG() << __FUNCTION__;

	if (m_StatWnd.m_btnMon.IsChecked() && m_nState == STOP_STATE && !m_destroyingMainWindow)
	{
		MonitoringStop();
		return MonitoringStart();
	}
	return false;
}
//------------------------------------------------------------------------------

bool CMainFrame::MonitoringStart()
{
	LOG_DEBUG() << __FUNCTION__;

	ASSERT(m_monitoringChain.IsEmpty());
	if (!m_monitoringChain.IsEmpty())
		MonitoringStop();

	using namespace WasapiHelpers;
	DevicesArray selectedDevices = CRecordingSourceDlg::GetInstance()->GetSelectedDevices();
	if (selectedDevices.empty() || (selectedDevices.size() == 1 && selectedDevices[0] == EMPTY_DEVICE))
		return false;

	//TODO: temp fix for Multi-recorder bugs - using simple recorder for one device.
	IWasapiRecorder* recorder = NULL;
	if (selectedDevices.size() == 1)
		recorder = new CWasapiRecorder(selectedDevices[0].first, 44100, 2);
	else
		recorder = new CWasapiRecorderMulti(selectedDevices, 44100, 2);

	recorder->AddEvent(VolumeChangedEvent, this);
	recorder->SetVolume(m_recording_volume);

	m_monitoringChain.AddFilter(dynamic_cast<Filter*>(recorder));
	if (recorder->Start())
	{
		m_GraphWnd.StartUpdate(PeaksCallback_Wasapi, LinesCallback_Wasapi, recorder);
		if (m_autoGainControl.get() != NULL)
		{
			m_autoGainControl->Start(recorder);
			m_SliderVol.ShowAutomaticVolume(true);
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
void CMainFrame::MonitoringStop()
{
	LOG_DEBUG() << __FUNCTION__;

	if (!m_monitoringChain.IsEmpty())
	{
		m_GraphWnd.StopUpdate();
		if (m_autoGainControl.get() != NULL)
		{
			m_autoGainControl->Stop();
			m_SliderVol.ShowAutomaticVolume(false);
		}

		m_monitoringChain.GetFilter<IWasapiRecorder>()->Stop();
		m_monitoringChain.Empty();
	}
}
//------------------------------------------------------------------------------

void CMainFrame::OnBtnVas()
{
	const bool vasEnabled = m_StatWnd.m_btnVas.IsChecked();
	RegistryConfig::SetOption(_T("Tools\\VAS\\Enable"), vasEnabled);

	const int vasThresholdDB = RegistryConfig::GetOption(_T("Tools\\VAS\\Threshold"), -30);
	const int vasDurationMS = RegistryConfig::GetOption(_T("Tools\\VAS\\WaitTime"), 2000);

	m_GraphWnd.ShowVASMark(vasEnabled, vasThresholdDB);
	if (!m_recordingChain.IsEmpty())
	{
		m_recordingChain.GetFilter<VasFilter>()->Enable(vasEnabled);
		m_recordingChain.GetFilter<VasFilter>()->SetTreshold(vasThresholdDB);
		m_recordingChain.GetFilter<VasFilter>()->SetDuration(vasDurationMS);
	}
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::UpdateButtonState(UINT nID)
{
	CButton* pBtn = (CButton *)GetDlgItem(nID);
	ASSERT(pBtn);

	pBtn->ModifyStyle(WS_DISABLED, 0);	// Resetting button state
	if (IDB_BTNREC == nID)
	{
		if (IsPlaying(m_nState))
			pBtn->ModifyStyle(0, WS_DISABLED);
	}
	else if (IDB_BTNPLAY == nID)
	{
		if (IsRecording(m_nState))
			pBtn->ModifyStyle(0, WS_DISABLED);
	}
	pBtn->Invalidate(false);
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::UpdateInterface()
{
	static int g_currentState = -1;
	if (g_currentState == m_nState)
		return;

	IWasapiRecorder* recorder = NULL;
	switch (m_nState)
	{
	case PLAY_STATE:
		m_GraphWnd.StartUpdate(PeaksCallback, LinesCallback, this);
		m_SliderVol.SetVolume(m_playback_volume);

		m_BtnPLAY.SetIcon(IDI_PAUSE);
		m_TrayIcon.SetIcon(IDI_TRAY_PLAY);
		m_IcoWnd.SetNewIcon(ICON_PLAY);
		m_BtnMIX_SEL.SetIcon(IDI_MIXLINE);
		break;

	case PAUSEPLAY_STATE:
		m_GraphWnd.StopUpdate();
		m_BtnPLAY.SetIcon(IDI_PLAY);
		m_TrayIcon.SetIcon(IDI_TRAY_PAUSE);
		m_IcoWnd.SetNewIcon(ICON_PAUSEP);
		break;

	case RECORD_STATE:
		recorder = m_recordingChain.GetFilter<IWasapiRecorder>();
		m_GraphWnd.StartUpdate(PeaksCallback_Wasapi, LinesCallback_Wasapi, recorder);		
		//if (m_autoGainControl.get() != NULL)
		//	m_autoGainControl->Start(recorder);

		m_BtnREC.SetIcon(IDI_PAUSE);
		m_TrayIcon.SetIcon(IDI_TRAY_REC);
		m_IcoWnd.SetNewIcon(ICON_REC);
		break;

	case PAUSEREC_STATE:
		m_GraphWnd.StopUpdate();
		//if (m_autoGainControl.get() != NULL)
		//	m_autoGainControl->Stop();

		m_BtnREC.SetIcon(IDI_REC);
		m_TrayIcon.SetIcon(IDI_TRAY_PAUSE);
		m_IcoWnd.SetNewIcon(ICON_PAUSER);
		break;

	case STOP_STATE:
	default:
		if (!m_StatWnd.m_btnMon.IsChecked())
			m_GraphWnd.StopUpdate();

		m_SliderTime.SetCurPos(0);
		m_SliderVol.SetVolume(m_recording_volume);

		m_BtnREC.SetIcon(IDI_REC);
		m_BtnPLAY.SetIcon(IDI_PLAY);
		m_TrayIcon.SetIcon(IDI_TRAY_STOP);
		m_IcoWnd.SetNewIcon(ICON_STOP);
		m_BtnMIX_SEL.SetIcon(IDI_MIXLINE03);
		UpdateStatWindow();
		break;
	}

	UpdateButtonState(IDB_BTNPLAY);
	UpdateButtonState(IDB_BTNREC);
	g_currentState = m_nState;
}
//------------------------------------------------------------------------------

void CMainFrame::UpdateWindowTitle_SeekTo(
	double curSeconds, double allSeconds, int displayTimeMs)
{
	const int percent = int(curSeconds/allSeconds)*100;
	const CString strCurSec = Helpers::ToString_HMMSS(curSeconds);
	const CString strAllSec = Helpers::ToString_HMMSS(allSeconds);

	CString title;
	title.Format(IDS_SEEKTO, strCurSec, strAllSec, percent);
	m_title->SetTitleText(title, displayTimeMs);
}
//------------------------------------------------------------------------------

CString CMainFrame::ParseFileName(CString a_file_name)
{
	// Writing real values instead of patterns

	TCHAR l_path[2 * MAX_PATH] = {0};
	std::map<CString, CString> l_patterns;

	CString l_auto_name = Helpers::GetMp3AutonameFromConfig();
	l_auto_name.Replace(_T(".mp3"), _T(""));	// Removing extension from name
	l_patterns[_T("{Autoname}")] = l_auto_name;

	SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, l_path);
	l_patterns[_T("{Desktop}")] = l_path;
	SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS , NULL, SHGFP_TYPE_CURRENT, l_path);
	l_patterns[_T("{MyDocuments}")] = l_path;

	std::map<CString, CString>::iterator i;
	for (i = l_patterns.begin(); i != l_patterns.end(); i++)
	{
		std::pair<CString, CString> l_pair = *i;
		if (a_file_name.Find(l_pair.first) != -1)
		{
			a_file_name.Replace(l_pair.first, l_pair.second);
		}
	}

	a_file_name.Replace(_T('/'), _T('\\'));
	return a_file_name;
}

//------------------------------------------------------------------------------
bool CMainFrame::CanPlay() const
{
	return m_recordingFileName.IsEmpty();
	//return (m_record_file.m_hFile == CFile::hFileNull);
}

//------------------------------------------------------------------------------
bool CMainFrame::CanRecord() const
{
	return !m_recordingFileName.IsEmpty();
}
//------------------------------------------------------------------------------

LRESULT CMainFrame::OnRecSourceDialogClosed(WPARAM wParam, LPARAM lParam)
{
	MonitoringRestart();
	return 0;
}
//------------------------------------------------------------------------------

LRESULT CMainFrame::OnRecSourceChanged(WPARAM wParam, LPARAM lParam)
{
	LOG_DEBUG() << __FUNCTION__;
	return 0;
}
//------------------------------------------------------------------------------

void CMainFrame::HandleFilterNotification(
	const Filter* /*fromFilter*/, const Parameter& parameter, void* userData)
{
	//Request notification processing in main thread.
	CMainFrame* mainFrame = static_cast<CMainFrame*>(userData);
	mainFrame->m_filterNotifications.push_back(parameter);
	mainFrame->PostMessage(WM_FILTER_NOTIFY, NULL, NULL);
}
//------------------------------------------------------------------------------

LRESULT CMainFrame::OnFilterNotify(WPARAM wParam, LPARAM lParam)
{
	for (size_t i = 0; i < m_filterNotifications.size(); i++)
	{
		const Parameter& param = m_filterNotifications[i];
		if (param.name == _T("VAS.HandleSilence"))
		{
			const int vasAction = RegistryConfig::GetOption(_T("Tools\\VAS\\Action"), 0);
			const bool stopOnSilence = vasAction == 1;
			const bool newFileOnSilence = vasAction == 2;
			const bool isSilence = param.valueInt == 1;
			LOG_DEBUG() << __FUNCTION__ << "VAS.HandleSilence: isSilence=" << isSilence << ", vasAction=" << vasAction;

			if (isSilence && stopOnSilence)
				OnBtnSTOP();
			else
			{
				//Creating new file when high signal detected
				if (newFileOnSilence && !isSilence)
				{
					ASSERT(!m_recordingFileName.IsEmpty());
					m_recordingFileName = Helpers::GetNewRecordingFilePath(m_recordingFileName);

					LOG_DEBUG() << __FUNCTION__ << "VAS.HandleSilence: creating_file=" << m_recordingFileName;
					m_recordingChain.GetFilter<FileWriter>()->CreateNewFile(m_recordingFileName);

					// Modifying window caption to "<FILE> - StepVoice Recorder".
					CString newCaption;
					AfxFormatString1(newCaption, IDS_FILETITLE, FileUtils::GetFileName(m_recordingFileName));
					m_title->SetTitleText(newCaption);
				}

				const int icoWndIcon = isSilence ? ICON_RECVAS : ICON_REC;
				const int trayIcon = isSilence ? IDI_TRAY_PAUSE : IDI_TRAY_REC;
				m_IcoWnd.SetNewIcon(icoWndIcon);
				m_TrayIcon.SetIcon(trayIcon);
			}
			continue;
		}
		if (param.name == _T("Recorder.Stopped"))
		{
			OnBtnSTOP();
			continue;
		}

		LOG_ERROR() << __FUNCTION__ << "UNHANDLED PARAMETER: " << param.name;
	}
	m_filterNotifications.clear();
	return 0;
}
//------------------------------------------------------------------------------

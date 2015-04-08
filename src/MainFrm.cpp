////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include <map>
#include <math.h>

#include "MP3_Recorder.h"
#include "MainFrm.h"
#include "MainFrm_Helpers.h"
#include "Interface\Buttons\BitmapBtn.h" 
#include "common.h"
#include "system.h"
#include "BASS_Functions.h"
#include "RecordingSourceDlg.h"
#include <basswasapi.h>

//#include "FilterChain.h"
#include "WasapiRecorder.h"
#include "VASFilter.h"
#include "Encoder_MP3.h"
#include "FilterFileWriter.h"

HSTREAM m_bassPlaybackHandle = 0;   // Playback
//HSTREAM g_update_handle = 0;   // Graph window update (used by callback func)
//HSTREAM g_loopback_handle = 0; // Handle for Loopback stream in Vista

//HRECORD g_record_handle = 0; 
//HRECORD g_monitoring_handle = 0;

//static CWasapiRecorder* g_wasapi_recorder = NULL;
//static FileWriter* g_fileWriter = NULL;
//static CEncoder_MP3* g_mp3Encoder = NULL;

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
	ON_COMMAND(IDM_OPT_EM, OnOptEm)
	ON_COMMAND(IDM_OPT_TOP, OnOptTop)
	ON_BN_CLICKED(IDB_BTNOPEN, OnBtnOPEN)
	ON_BN_CLICKED(IDB_BTNPLAY, OnBtnPLAY)
	ON_BN_CLICKED(IDB_BTNSTOP,  OnBtnSTOP)
	ON_BN_CLICKED(IDB_BTNREC,  OnBtnREC)
	ON_BN_CLICKED(IDB_MIX_REC,  OnBtnMIX_REC)
	ON_BN_CLICKED(IDB_MIX_PLAY, OnBtnMIX_PLAY)
	ON_BN_CLICKED(IDB_MIX_SEL, OnBtnMIX_SEL)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDM_MIX_REC, OnMixRec)
	ON_COMMAND(IDM_MIX_PLAY, OnMixPlay)
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
	ON_COMMAND(ID_STAT_PREF, OnStatPref)
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
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_SOUND_PLAY, IDM_SOUND_END,
							   OnUpdateSoundPlay)
	ON_COMMAND_RANGE(ID_MIXITEM_REC0, ID_MIXITEM_REC0+50, OnRecMixMenuSelect)
	ON_COMMAND_RANGE(ID_MIXITEM_PLAY0, ID_MIXITEM_PLAY0+3, OnPlayMixMenuSelect)
	ON_COMMAND(ID_MIXITEM_REC_LOOPBACK, OnRecLoopbackSelect)
	ON_COMMAND(ID_MIXITEM_REC_LOOPBACK_MIX, OnRecLoopbackMixSelect)
	ON_COMMAND(ID_MIXITEM_PLAY_VOLUME, OnPlayVolumeSelect)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_MESSAGE(WM_FILTER_NOTIFY, OnFilterNotify)
	ON_MESSAGE(WM_RECSOURCE_DLGCLOSED, OnRecSourceDialogClosed)
	ON_MESSAGE(WM_RECSOURCE_CHANGED, OnRecSourceChanged)
END_MESSAGE_MAP()
/*
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CMainFrame::MonitoringProc(HRECORD , void* a_buffer,
										DWORD a_length, void* a_user)
{
	CMainFrame* l_main_window = (CMainFrame *)a_user;
	ASSERT(l_main_window);

	// Updating monitoring buffer for StereoMix levels in Vista (can't get it
	// from bass lib, because it doesn't show sound after DSP).
	if (l_main_window->m_visualization_data)
		l_main_window->m_visualization_data->SetSourceBuffer((float*)a_buffer, a_length);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CMainFrame::NewRecordProc(HRECORD a_handle, void* a_buffer,
										DWORD a_length, void* a_user)
{
	static char pBufOut[128 * 1024] = {0}; //128k buffer

	CMainFrame* l_main_window = (CMainFrame *)a_user;
	ASSERT(l_main_window);

	VisualizationData* l_vis_data = l_main_window->m_visualization_data;
	if (l_vis_data)
		l_vis_data->SetSourceBuffer((float*)a_buffer, a_length);

	if (l_main_window->m_vas.IsRunning())
	{
		if (l_vis_data)
		{
			double l_peak_float = max(l_vis_data->GetPeaksLevel(0),
				l_vis_data->GetPeaksLevel(1));
			l_main_window->m_vas.ProcessThreshold(20 * log10(l_peak_float));
		}
		else
		{
			l_main_window->m_vas.ProcessThreshold(Bass::GetMaxPeakDB(a_handle));
		}
		if (l_main_window->m_vas.CheckVAS()) 
		{
			l_main_window->ProcessVAS(true); 
			return TRUE;
		}
		else
		{
			l_main_window->ProcessVAS(false);
		}
	}

	try
	{
		int l_encoded_bytes_count = 0;
		bool result = l_main_window->m_pEncoder->EncodeChunkFloat(
			(float*)a_buffer, a_length / sizeof(float), pBufOut, l_encoded_bytes_count);
		if (!result)
		{
			//TODO: logging, not all data were encoded.
		}

		l_main_window->m_record_file.Write(pBufOut, l_encoded_bytes_count);
	}
	catch (CException *e)
	{
		// Possible, the disk is full, etc.
		e->ReportError();
		e->Delete();
		l_main_window->PostMessage(WM_COMMAND, IDB_BTNSTOP,
			LONG(l_main_window->m_BtnSTOP.m_hWnd));
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Bass WASAPI routine

//!!!test
DWORD CALLBACK CMainFrame::WasapiRecordingProc(void *buffer, DWORD length, void *user)
{
	OutputDebugString(__FUNCTION__"\n");

	CMainFrame* l_main_window = static_cast<CMainFrame *>(user);
	ASSERT(l_main_window);

	// Updating monitoring buffer for StereoMix levels in Vista (can't get it
	// from bass lib, because it doesn't show sound after DSP).
	if (l_main_window->m_visualization_data)
		l_main_window->m_visualization_data->SetSourceBuffer((float*)buffer, length);

	return 1; //0 stops a device
}
//------------------------------------------------------------------------------
*/
float CMainFrame::PeaksCallback_Wasapi(int channel, void* userData)
{
	CWasapiRecorder* recorder = static_cast<CWasapiRecorder*>(userData);
	return recorder->GetPeakLevel(channel);
}
//------------------------------------------------------------------------------

int CMainFrame::LinesCallback_Wasapi(int channel, float* buffer, int bufferSize, void* userData)
{
	CWasapiRecorder* recorder = static_cast<CWasapiRecorder*>(userData);
	return recorder->GetChannelData(channel, buffer, bufferSize);
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//CMainFrame* CMainFrame::m_pMainFrame = NULL;

//------------------------------------------------------------------------------
float CMainFrame::PeaksCallback(int channel, void* userData)
{
	CMainFrame* mainFrame = static_cast<CMainFrame*>(userData);

	if (mainFrame->m_visualization_data && (
		mainFrame->m_recording_mixer == E_REC_LOOPBACK ||
		mainFrame->m_recording_mixer == E_REC_LOOPBACK_MIX))
		return mainFrame->m_visualization_data->GetPeaksLevel(channel);

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

	if (mainFrame->m_visualization_data && (
		mainFrame->m_recording_mixer == E_REC_LOOPBACK ||
		mainFrame->m_recording_mixer == E_REC_LOOPBACK_MIX))
		return mainFrame->m_visualization_data->GetLinesLevel(channel, buffer, bufferSize);

	float* l_buffer_ptr = buffer + channel;
	const int l_bytes_2_copy = (bufferSize - channel) * sizeof(float);

	const int l_bytes_copied = BASS_ChannelGetData(
		mainFrame->m_bassPlaybackHandle, l_buffer_ptr, (l_bytes_2_copy | BASS_DATA_FLOAT));

	return (l_bytes_copied == -1) ? 0 : l_bytes_copied / sizeof(float);
}

////////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
	:m_vista_loopback(NULL)
	,m_visualization_data(NULL)
	,m_loopback_hdsp(0)
	,m_mute_hdsp(0)
	,m_playback_volume(0)
	,m_active_mixer(E_REC_MIXER)
	,m_recording_mixer(E_REC_MIXER)
	,m_monitoringChain(HandleFilterNotification, this)
	,m_recordingChain(HandleFilterNotification, this)
	,m_bassPlaybackHandle(0)
{
	m_pEncoder	= NULL;
	m_title		= NULL;

	m_nState    = STOP_STATE;
	m_strDir    = ((CMP3_RecorderApp*)AfxGetApp())->GetProgramDir();

	m_pOptDialog = NULL;

	//m_pMainFrame = this;
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
	//if (g_wasapi_recorder != NULL)
	//	g_wasapi_recorder->Stop();
	//if (g_mp3Encoder != NULL)
	//	g_mp3Encoder->WriteVBRHeader(_T("d:\\test.mp3"));

	//SAFE_DELETE(g_wasapi_recorder);
	//SAFE_DELETE(g_fileWriter);
	//SAFE_DELETE(g_mp3Encoder);

	//!!!test
	BASS_WASAPI_Stop(TRUE);
	BASS_WASAPI_Free();

	SAFE_DELETE(m_title);
	SAFE_DELETE(m_visualization_data);
}

/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", off)
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bMonitoringBtn = false;

	// Removing the "Register" menu in registered mode
	REG_CRYPT_BEGIN;
	#ifndef _DEBUG
		CMenu* pMenu = this->GetMenu();
		pMenu->RemoveMenu(4, MF_BYPOSITION);
		CMenu* pHelpSubMenu = pMenu->GetSubMenu(3);
		pHelpSubMenu->EnableMenuItem(5, MF_GRAYED|MF_BYPOSITION);
		this->DrawMenuBar();
	#endif
	REG_CRYPT_END;

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

	// Creating interface windows
	m_IcoWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(1, 0, 24, 25), this, IDW_ICO);
	m_TimeWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(24, 0, 106, 25), this, IDW_TIME);
	m_GraphWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(1, 24, 106, 77), this, IDW_GRAPH);
	m_StatWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(106, 0, 282, 50), this, IDW_STAT);
	
	m_IcoWnd.ShowWindow(SW_SHOW);   m_IcoWnd.UpdateWindow();
	m_TimeWnd.ShowWindow(SW_SHOW);  m_TimeWnd.UpdateWindow();
	m_GraphWnd.ShowWindow(SW_SHOW); m_GraphWnd.UpdateWindow();
	m_StatWnd.ShowWindow(SW_SHOW);  m_StatWnd.UpdateWindow();

	// Setting graphs display mode
	m_GraphWnd.SetDisplayMode(CGraphWnd::DisplayMode(m_conf.GetConfProg()->nGraphType));
	m_GraphWnd.ShowMaxpeaks(m_conf.GetConfProg()->bGraphMaxpeaks != 0);

	// Adjusting the position and mixer sliders
	CRect rT(106, 55, 286, 78);
	CRgn rgnT;
	rgnT.CreateRectRgn(0+6, 0, rT.right-rT.left-6, 26/*rT.bottom-rT.top*/);
	m_SliderTime.Create(WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_BOTH,
		rT, this, IDS_SLIDERTIME);
	m_SliderTime.SetWindowRgn(rgnT, false);
	m_SliderTime.SetRange(0, 1000);

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

	m_BtnFrame.Create(_T("Static"), "", WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,
		CRect(1, 80, 283, 113), this, IDC_STATIC);

	EnableToolTips(true);

	// "Always on Top" option
	OnOptTop(); OnOptTop();	// state is not changing

	// Adjusting the mixer
	m_playback_volume = min(1, abs((float)m_conf.GetConfProg()->nPlayVolume / 10000));
	m_RecMixer.Open(WAVE_MAPPER, GetSafeHwnd());
	m_PlayMixer.Open(WAVE_MAPPER, GetSafeHwnd());

	//m_BtnMIX_SEL.SetIcon(IDI_MIC);
	//UpdateMixerState();
	SetRecordingLine(m_conf.GetConfProg()->nRecLineID);

	if (((CMP3_RecorderApp*)AfxGetApp())->IsVistaOS())
	{
		// Making system playback volume for our recorder to maximum
		m_PlayMixer.SetVol(99);
	}

	m_title = new CTitleText(this->GetSafeHwnd());

	m_strDir = GetProgramDir();

	// Processing loader options
	CString strName = "";
	switch (m_conf.GetConfDialGen()->nLoader)
	{
		case 1:		// Create new file with autoname
			//strName = GetAutoName(CString("")) + ".mp3";
			strName = GetAutoName(CString(""));
			break;
		case 2:		// Loading last file used
			strName = m_conf.GetConfProg()->strLastFileName;
			break;
	}
	OnFileClose();
	if (!strName.IsEmpty())
	{
		CString strPath = m_conf.GetConfProg()->strLastFilePath;
		if (strPath.IsEmpty())
		{
			strPath = GetProgramDir();
		}
		OpenFile(strPath + "\\" + strName);
	}

	// Creating icon for the system tray
	m_TrayIcon.Create(this, WM_ICON_NOTIFY, "Stepvoice Recorder",
		LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TRAY_STOP)), IDR_TRAY_MENU);
	if(m_conf.GetConfDialGen()->bTrayIcon)
		m_TrayIcon.ShowIcon();
	else
		m_TrayIcon.HideIcon();
	UpdateTrayText();

	// Setting up Monitoring
	if (m_conf.GetConfProg()->bMonitoring)
	{
		m_bMonitoringBtn = false;
		OnBtnMonitoring();
	}

	// Setting up Scheduler
	m_sched2.SetCallbackFunc(Scheduler2Function);
	if (m_conf.GetConfDialSH2()->bIsEnabled)
	{
		m_conf.GetConfDialSH2()->bIsEnabled = false;
		OnBtnSched();
	}

	// Setting up Voice Activation System
	if (m_conf.GetConfDialVAS()->bEnable)
	{
		OnBtnVas();
	}

	/*
	// Adding device selection menu items. This feature is disabled, since
	// planned to implement a separate control for device/line selection.

	CMenu* pRecDeviceMenu = this->GetMenu()->GetSubMenu(2)->GetSubMenu(0);
	ASSERT(pRecDeviceMenu);
	pRecDeviceMenu->RemoveMenu(0, MF_BYPOSITION); // Removing separator

	if (((CMP3_RecorderApp*)AfxGetApp())->IsVistaOS())
	{
		CStringArray playbackDevices;
		BassVistaLoopback::GetPlaybackDevicesNames(playbackDevices);

		for (int i = 0; i < playbackDevices.GetSize(); i++)
			pRecDeviceMenu->AppendMenu(MF_STRING, ID_MIXITEM_LOOPBACK_DEVICE+i,
			                           playbackDevices[i]);
	}
	*/
	CMenu* pRecDeviceMenu = this->GetMenu()->GetSubMenu(2);
	ASSERT(pRecDeviceMenu);
	pRecDeviceMenu->RemoveMenu(0, MF_BYPOSITION); // Removing loopback device menu

	return 0;
}
#pragma optimize("", on)

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.cx = 290;
	cs.cy = 120 + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
	
	int nScreenCX = GetSystemMetrics(SM_CXSCREEN);
	int nScreenCY = GetSystemMetrics(SM_CYSCREEN);
	cs.x  = m_conf.GetConfProg()->nXcoord;
	cs.y  = m_conf.GetConfProg()->nYcoord;
	if (cs.x >= (nScreenCX-40) || cs.y >= (nScreenCY-40))
	{
		cs.x  = (nScreenCX - cs.cx)/2;
		cs.y  = (nScreenCY - cs.cy)/2;
	}

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = "SvRec";

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
	///@note Called by MP3_RecorderApp

	int nCmdShow = (m_conf.GetConfDialGen()->bMinimized) ? SW_MINIMIZE : SW_SHOW;
	if (nCmdShow == SW_MINIMIZE && m_conf.GetConfDialGen()->bTrayMin)
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
	if (message == WM_DISPLAYCHANGE)
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rDesktopRect, 0);

	/*
	switch (message)
	{
	// Processing a display resolution changed for snapping
	case WM_DISPLAYCHANGE:
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rDesktopRect, 0);
		break;

	case MM_MIXM_CONTROL_CHANGE:
		{
			// Ignoring message when it has come from inactive recording line.
			if ((HMIXER)wParam == m_RecMixer.GetMixerHandle())
			{
				CControlVolume* l_vol_control = m_RecMixer.GetCurrentVolControl();
				if ((l_vol_control && l_vol_control->GetControlID() != (DWORD)lParam))
					break;
			}
		}
	case MM_MIXM_LINE_CHANGE:
		{
			//OutputDebugString(__FUNCTION__ " :: Mixer line/control changed\n");

			// Ignoring these messages when dragging or loopback mixer is active
			if (m_SliderVol.IsDragging() ||
				m_recording_mixer == E_REC_LOOPBACK ||
				m_recording_mixer == E_REC_LOOPBACK_MIX)
				break;

			// Ignoring messages from our opened mixer devices.
			///@NOTE: if changing volume level from windows, it acts like we have
			///changed it from out control and so the level is not synced.
			//if ((HMIXER)wParam == m_RecMixer.GetMixerHandle() ||
			//	(HMIXER)wParam == m_PlayMixer.GetMixerHandle())
			//	break;

			//OutputDebugString(__FUNCTION__ " :: changed not by our device (or from our message post :)\n");
			std::map<ActiveSoundMixer, int> l_mixer_vol;
			l_mixer_vol[E_REC_MIXER] = m_RecMixer.GetVol(m_RecMixer.GetCurLine());
			l_mixer_vol[E_REC_LOOPBACK] = m_SliderVol.GetRangeMax();
			l_mixer_vol[E_REC_LOOPBACK_MIX] = m_SliderVol.GetRangeMax();
			l_mixer_vol[E_PLAY_MIXER] = m_PlayMixer.GetVol(m_PlayMixer.GetCurLine());
			l_mixer_vol[E_PLAY_STREAM] = int(m_playback_volume * 100);

			ASSERT(l_mixer_vol.find(m_active_mixer) != l_mixer_vol.end());
			m_SliderVol.SetPos(l_mixer_vol[m_active_mixer]);

			if (m_active_mixer == E_PLAY_MIXER || m_active_mixer == E_PLAY_STREAM)
				break;

			//OutputDebugString(__FUNCTION__ " :: recording mixer is active\n");
			if (m_loopback_hdsp || m_mute_hdsp)
			{
				OutputDebugString(__FUNCTION__ " :: loopback mixer is active. Removing both DSP.");

				HRECORD handle = g_record_handle ? g_record_handle : g_monitoring_handle;
				BASS_ChannelRemoveDSP(handle, m_loopback_hdsp);
				BASS_ChannelRemoveDSP(handle, m_mute_hdsp);

				m_loopback_hdsp = 0;
				m_mute_hdsp = 0;
			}
			UpdateMixerState();
		}
		break;
	}
	*/
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_conf.GetConfProg()->bEasyMove)
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
	CFrameWnd::OnDestroy();

	if (m_bMonitoringBtn)
	{
		OnBtnMonitoring();
		m_conf.GetConfProg()->bMonitoring = true;
	}
	OnFileClose();
	m_GraphWnd.StopUpdate(); // Forcing stop to remove assertions in debug

	if (BASS_GetDevice())
		BASS_Free();

	CloseMixerWindows();

	if (!IsIconic())
	{
		CRect r; GetWindowRect(&r);
		m_conf.GetConfProg()->nXcoord = r.left;
		m_conf.GetConfProg()->nYcoord = r.top;
	}
	m_conf.GetConfProg()->nGraphType = m_GraphWnd.GetDisplayMode();
	m_conf.GetConfProg()->bGraphMaxpeaks = (int)m_GraphWnd.MaxpeaksVisible();
	m_conf.GetConfProg()->nPlayVolume = int(m_playback_volume * 10000);

	//Not keeping ID for standard lines, thus selecting current on next start.
	if (m_recording_mixer != E_REC_LOOPBACK && m_recording_mixer != E_REC_LOOPBACK_MIX)
		m_conf.GetConfProg()->nRecLineID = 0;
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

	// Setting the default window text: <no file> - Stepvoice Recorder
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
void CMainFrame::OpenFile(CString fileName)
{
	OnFileClose();

	if (Helpers::IsSuitableForRecording(fileName))
	{
		m_recordingFileName = fileName;
	}
	else
	{
		BASS_Init(-1, 44100, 0, GetSafeHwnd(), NULL);
		m_bassPlaybackHandle = BASS_StreamCreateFile(false, fileName, 0, 0, 0);
		if (!m_bassPlaybackHandle)
		{
			//ReportBassError(_T("Unable to open file."), BASS_ErrorGetCode());
			BASS_Free();
			return;
		}
	}

	RegistryConfig::SetOption(_T("General\\LastFile"), fileName);

	/*
	// Storing last opened file name and path.
	CString& l_last_path = m_conf.GetConfProg()->strLastFilePath;
	CString& l_last_name = m_conf.GetConfProg()->strLastFileName;

	int l_slash_pos = str.ReverseFind('\\');
	if (l_slash_pos == -1)
	{
		l_last_name = str;
	}
	else
	{	
		l_last_path = str.Left(l_slash_pos);
		l_last_name = str.Right(str.GetLength() - l_slash_pos - 1);
	}

	if (Helpers::IsSuitableForRecording(str))
	{
		UINT l_open_flags = CFile::modeCreate | CFile::modeReadWrite |
			CFile::typeBinary | CFile::shareDenyWrite;
		if (!m_record_file.Open(str, l_open_flags, NULL))
			return;
	}
	else
	{
		if (BASS_GetDevice() == -1)
		{
			int deviceID = m_conf.GetConfDialGen()->nPlayDevice + 1; // BASS starts devices from 1
			BASS_Init(deviceID <= 0 ? -1 : deviceID, 44100, 0, GetSafeHwnd(), NULL);
		}

		m_bassPlaybackHandle = BASS_StreamCreateFile(false, str, 0, 0, 0);
		if (!m_bassPlaybackHandle)
		{
			// Not calling BASS_Free, because it can be used by monitoring
			//BASS_Free();
			return;
		}
	}
	*/

	// Modifying window caption to "<FILE> - StepVoice Recorder".

	const int slashPos = fileName.ReverseFind('\\');
	const CString shortName = (slashPos == -1) ? fileName
		: fileName.Right(fileName.GetLength() - slashPos - 1);

	CString newCaption;
	AfxFormatString1(newCaption, IDS_FILETITLE, shortName);
	m_title->SetTitleText(newCaption);
	
	UpdateStatWindow();
	UpdateTrayText();
}

//===========================================================================
void CMainFrame::OnFileFindfile() 
{
	const CString lastFilePath = RegistryConfig::GetOption(_T("General\\LastFile"), CString());
	ShellExecute(NULL, _T("open"), _T("explorer"), _T("/select, """)+lastFilePath+_T(""""),
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
void CMainFrame::OnStatPref() 
{
	//m_conf.GetConfProg()->nDialogIndex = -1; // Selecting tab sheet for display
	OnOptCom();
}

void CMainFrame::OnOptCom()
{
	int nDialogIndex = m_conf.GetConfProg()->nDialogIndex;
	CMySheet optDlg("Preferences", this, nDialogIndex);

	optDlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	optDlg.SetConfig( m_conf.GetConfDialGen() );
	optDlg.SetConfig( m_conf.GetConfDialMp3() );
	optDlg.SetConfig( m_conf.GetConfDialSH2() );
	optDlg.SetConfig( m_conf.GetConfDialVAS() );
	optDlg.SetConfig( m_conf.GetConfDialAN()  );

	m_pOptDialog = &optDlg;		// Saving dialog pointer for tray exit process

	// Saving the scheduler state
	bool bOldSchedState = m_conf.GetConfDialSH2()->bIsEnabled != 0;
	bool bNewSchedState = bOldSchedState;

	if (optDlg.DoModal() == IDOK)
	{
		m_conf.saveConfig();		
		UpdateStatWindow();

		// Checking tray options
		if(m_conf.GetConfDialGen()->bTrayIcon) //Taskbar only
			m_TrayIcon.ShowIcon();
		else
			m_TrayIcon.HideIcon();

		// !!!!! Scheduler change check (SHR check)
		bNewSchedState = m_conf.GetConfDialSH2()->bIsEnabled != 0;
		if(bNewSchedState != bOldSchedState)
		{	// Inverting the scheduler state (by pressing the button)
			m_conf.GetConfDialSH2()->bIsEnabled = !bNewSchedState;
			OnBtnSched();
		}
		else if(bNewSchedState) {	// Restarting the scheduler
			OnBtnSched(); // Off.
			OnBtnSched(); // On.
		}

		// Checking VAS
		CONF_DIAL_VAS* pConfig = m_conf.GetConfDialVAS();
		if((pConfig->bEnable != 0) != m_vas.IsRunning())
			OnBtnVas();
		if(m_vas.IsRunning())
		{
			m_vas.InitVAS(pConfig->nThreshold, pConfig->nWaitTime);
			m_GraphWnd.ShowVASMark(true, pConfig->nThreshold);
		}
	}

	m_conf.GetConfProg()->nDialogIndex = optDlg.m_nPageIndex;

	m_pOptDialog = NULL;	// No dialog is open (pointer is for tray)
}

//===========================================================================
void CMainFrame::OnOptEm() 
{
	//Reverting the state of dragging window from any point
	m_conf.GetConfProg()->bEasyMove = m_conf.GetConfProg()->bEasyMove ? 0 : 1;
}

//===========================================================================
void CMainFrame::OnOptTop() 
{
	// Link to the window top option
	int& l_top = m_conf.GetConfProg()->bAlwaysOnTop;
	l_top = l_top ? 0 : 1;

	const CWnd* pWndType = (l_top) ? &wndTopMost : &wndNoTopMost;
	SetWindowPos(pWndType, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE);
}

//===========================================================================
void CMainFrame::OnMixPlay() 
{
	CMP3_RecorderApp* l_app = (CMP3_RecorderApp* )AfxGetApp();
	if (l_app->IsVistaOS())
	{
		WinExec("sndvol", SW_SHOW);
	}
	else
	{
		CloseMixerWindows();
		WinExec("sndvol32", SW_SHOW);
	}
}

//===========================================================================
void CMainFrame::OnMixRec() 
{
	CMP3_RecorderApp* l_app = (CMP3_RecorderApp* )AfxGetApp();
	if (l_app->IsVistaOS())
	{
		WinExec("control MMSYS.CPL,,1", SW_SHOW);
	}
	else
	{
		CloseMixerWindows();
		WinExec("sndvol32 -r", SW_SHOW);
	}
}


//===========================================================================
// BUTTONS
//===========================================================================
void CMainFrame::OnBtnOPEN()
{
	CString dialogTitle, filesFilter;
	dialogTitle.LoadString(IDS_FILENEWOPENTITLE);
	filesFilter.LoadString(IDS_FILEFILTER);

	const CString generatedName = GetAutoName(CString("")); // Name like 08jun_05.mp3
	
	const CString lastFileName = RegistryConfig::GetOption(_T("General\\LastFile"), CString());
	const int slashPos = lastFileName.ReverseFind(_T('\\'));
	const CString lastFilePath = (slashPos != -1) ? lastFileName.Left(slashPos) : CString();

	// Creating a standard file open dialog
	CFileDialog openDialog(true, "mp3", generatedName, OFN_HIDEREADONLY | OFN_EXPLORER, filesFilter);
	openDialog.m_ofn.lpstrTitle= dialogTitle;
	openDialog.m_ofn.lpstrInitialDir = lastFilePath;	

	if (openDialog.DoModal() == IDOK)
	{	
		OpenFile(openDialog.GetPathName());
		return;
	}
}

//===========================================================================
void CMainFrame::OnBtnPLAY()
{
	if (!this->CanPlay())
		return;

	if (!m_bassPlaybackHandle)
	{
		OnBtnOPEN();
		if (!m_bassPlaybackHandle)
			return;
	}

	if (m_bMonitoringBtn)
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
	
	//OnPlayVolumeSelect();
}

//===========================================================================
void CMainFrame::OnBtnSTOP()
{
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

		m_recordingChain.GetFilter<CWasapiRecorder>()->Stop();
		m_recordingChain.GetFilter<FileWriter>()->ForceClose();
		m_recordingChain.GetFilter<CEncoder_MP3>()->WriteVBRHeader(m_recordingFileName);
		m_recordingChain.Empty();

		OpenFile(m_recordingFileName);
	}

	//TODO: refactor. This thing resets playback time in time wnd.
	PostMessage(WM_HSCROLL,  MAKEWPARAM(SB_THUMBPOSITION, 0),
		(LPARAM)m_SliderTime.m_hWnd);

	m_nState = STOP_STATE;
	if (m_bMonitoringBtn)
		MonitoringStart();


	/*
	m_nState = STOP_STATE;
	if (m_bassPlaybackHandle)
	{
		KillTimer(1);
		BOOL l_result = BASS_ChannelStop(m_bassPlaybackHandle);
		ASSERT(l_result);

		l_result = BASS_ChannelSetPosition(m_bassPlaybackHandle,
			BASS_ChannelSeconds2Bytes(m_bassPlaybackHandle, 0), BASS_POS_BYTE);
		ASSERT(l_result);
	}
	if (g_record_handle)
	{
		KillTimer(2);
		BOOL l_result = BASS_ChannelStop(g_record_handle);
		ASSERT(l_result);

		l_result = BASS_ChannelStop(g_loopback_handle);
		ASSERT(l_result);

		g_loopback_handle = 0;
		g_record_handle = 0;
		m_loopback_hdsp = 0;
		m_mute_hdsp = 0;
		BASS_Free();
		
		BASS_RecordFree();

		SAFE_DELETE(m_vista_loopback);
		SAFE_DELETE(m_visualization_data);

		CString l_recorded_file = m_record_file.GetFilePath();
		m_record_file.Flush();
		m_record_file.Close();

		m_pEncoder->WriteVBRHeader(l_recorded_file);
		SAFE_DELETE(m_pEncoder);
		OpenFile(l_recorded_file);
	}

	PostMessage(WM_HSCROLL,  MAKEWPARAM(SB_THUMBPOSITION, 0),
		(LPARAM)m_pMainFrame->m_SliderTime.m_hWnd);

	if (m_bMonitoringBtn)
		MonitoringStart();

	SetRecordingLine(m_conf.GetConfProg()->nRecLineID);
	*/
}

//===========================================================================
void CMainFrame::OnBtnREC()
{
	if (m_recordingFileName.IsEmpty())
	{
		OnBtnOPEN();
		if (m_recordingFileName.IsEmpty())
			return;
	}

	if (m_bMonitoringBtn)
		MonitoringStop();

	if (m_recordingChain.IsEmpty())
	{
		WasapiHelpers::DevicesArray selectedDevices = CRecordingSourceDlg::GetInstance()->GetSelectedDevices();
		ASSERT(!selectedDevices.empty());
		const DWORD deviceID = selectedDevices[0].first;

		const bool vasEnabled = RegistryConfig::GetOption(_T("Tools\\VAS\\Enable"), 0);
		const int vasThresholdDB = RegistryConfig::GetOption(_T("Tools\\VAS\\Threshold"), -30);
		const int vasWaitTimeMS = RegistryConfig::GetOption(_T("Tools\\VAS\\WaitTime"), 2000);

		int bitrate = RegistryConfig::GetOption(_T("File types\\MP3\\Bitrate"), 128);
		int frequency = RegistryConfig::GetOption(_T("File types\\MP3\\Freq"), 44100);
		int channels = RegistryConfig::GetOption(_T("File types\\MP3\\Stereo"), 1) + 1;

		CWasapiRecorder* recorder = new CWasapiRecorder(deviceID, frequency, channels);
		frequency = recorder->GetActualFrequency();
		channels = recorder->GetActualChannelCount();

		m_recordingChain.AddFilter(recorder);
		m_recordingChain.AddFilter(new VasFilter(vasThresholdDB, vasWaitTimeMS, vasEnabled));
		m_recordingChain.AddFilter(new CEncoder_MP3(bitrate, frequency, channels));
		m_recordingChain.AddFilter(new FileWriter(m_recordingFileName));
	}

	CWasapiRecorder* recorder = m_recordingChain.GetFilter<CWasapiRecorder>();
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


	/*
	// Data for the scheduler start
	bool bIsSchedEnabled= m_conf.GetConfDialSH2()->bIsEnabled  != 0;
	bool bSchedStart	= m_conf.GetConfDialSH2()->bSchedStart != 0;
	SHR_TIME* ptRec		= &m_conf.GetConfDialSH2()->t_rec;
	SHR_TIME* ptStop	= &m_conf.GetConfDialSH2()->t_stop;

	if (m_record_file.m_hFile == CFile::hFileNull)
	{
		OnBtnOPEN();
		if (m_record_file.m_hFile == CFile::hFileNull)
			return;
	}

	MonitoringStop();

	if (!g_record_handle)
	{
		if (!BASS_RecordInit(m_conf.GetConfDialGen()->nRecDevice)) //default device
			return;

		int l_bitrate = m_conf.GetConfDialMp3()->nBitrate;
		int l_frequency = m_conf.GetConfDialMp3()->nFreq;
		int l_channels = m_conf.GetConfDialMp3()->nStereo + 1;

		int deviceID = m_conf.GetConfDialGen()->nPlayDevice + 1; // BASS starts devices from 1
		BASS_Init(deviceID <= 0 ? -1 : deviceID, l_frequency, 0, GetSafeHwnd(), NULL);

		::OutputDebugString(__FUNCTION__ " ::6");
		try
		{
			SAFE_DELETE(m_pEncoder);
			m_pEncoder = new CEncoder_MP3(l_bitrate, l_frequency, l_channels);
		}
		catch (CString& e)
		{
			SAFE_DELETE(m_pEncoder);
			BASS_RecordFree();
			AfxMessageBox(e, MB_OK | MB_ICONSTOP);
			return;
		}

		SAFE_DELETE(m_visualization_data);
		m_visualization_data = new VisualizationData(l_frequency, l_channels);

		g_record_handle = BASS_RecordStart(l_frequency, l_channels,
			MAKELONG(BASS_RECORD_PAUSE|BASS_SAMPLE_FLOAT, 25), (RECORDPROC *)&NewRecordProc, this);
		if (FALSE == g_record_handle)
		{
			SAFE_DELETE(m_pEncoder);
			g_record_handle = 0;
			return;
		}

		// Creating the Loopback stream
		//int deviceID = m_conf.GetConfDialGen()->nPlayDevice + 1; // BASS starts devices from 1
		//BASS_Init(deviceID <= 0 ? -1 : deviceID, l_frequency, 0, GetSafeHwnd(), NULL);

		SAFE_DELETE(m_vista_loopback);
		m_vista_loopback = new BassVistaLoopback(m_conf.GetConfDialGen()->nPlayDevice);
		HSTREAM l_stream_handle = m_vista_loopback->GetLoopbackStream();

		ASSERT(g_loopback_handle == 0);
		g_loopback_handle = BASS_Mixer_StreamCreate(l_frequency, l_channels,
			BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);
		ASSERT(g_loopback_handle);

		BASS_Mixer_StreamAddChannel(g_loopback_handle, l_stream_handle,
			BASS_MIXER_DOWNMIX);

		if (m_recording_mixer == E_REC_LOOPBACK)
		{
			m_mute_hdsp = BASS_ChannelSetDSP(g_record_handle,
				Bass::StreamMuteDSP, NULL, 0xFF); //higher priority called 1st
		}
		if (m_recording_mixer == E_REC_LOOPBACK || m_recording_mixer == E_REC_LOOPBACK_MIX)
		{
			m_loopback_hdsp = BASS_ChannelSetDSP(g_record_handle,
				Bass::LoopbackStreamDSP, &g_loopback_handle, 0xFE);
		}
	}

	const DWORD CHANNEL_STATE = BASS_ChannelIsActive(g_record_handle);
	switch (CHANNEL_STATE)
	{
	case BASS_ACTIVE_PLAYING:
		BASS_ChannelPause(g_record_handle);
		KillTimer(2);
		m_nState = PAUSEREC_STATE;
		break;

	case BASS_ACTIVE_PAUSED:
	case BASS_ACTIVE_STOPPED:
		BASS_ChannelPlay(g_record_handle, false);
		SetTimer(2, 1000, NULL);

		// Running the scheduler
		if (bIsSchedEnabled && (!bSchedStart))
		{
			if (m_conf.GetConfDialSH2()->nStopByID == 1)
			{
				m_sched2.SetRecTime(ptRec, NULL);
			}
			else
			{
				m_sched2.SetStopTime(ptStop, NULL);
			}
			TRACE0("==> OnBtnREC: trying to run the scheduler ...\n");
			m_sched2.Start();
		}
		m_nState = RECORD_STATE;
		break;
	}
	*/
}

//===========================================================================
void CMainFrame::OnBtnMIX_REC()
{
	OnMixRec();
}

//===========================================================================
void CMainFrame::OnBtnMIX_PLAY()
{
	OnMixPlay();
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

	/*
	HSTREAM l_handle = 0;
	switch (nIDEvent)
	{
	case 1:
		l_handle = m_bassPlaybackHandle;
		break;
	case 2:
		l_handle = (HSTREAM)g_record_handle;
		break;
	default:
		return;
	}

	double l_seconds_all = BASS_ChannelBytes2Seconds(l_handle,
		BASS_ChannelGetLength(l_handle, BASS_POS_BYTE));
	double l_seconds_cur = BASS_ChannelBytes2Seconds(l_handle,
		BASS_ChannelGetPosition(l_handle, BASS_POS_BYTE));

	if (l_handle == g_record_handle)
	{
		int l_stream_rate = m_conf.GetConfDialMp3()->nBitrate;
		QWORD l_size_bytes = (QWORD)m_record_file.GetLength();
		l_seconds_cur = (double)l_size_bytes / (l_stream_rate * 125);
	}
	if (!m_SliderTime.IsDragging())
	{
		m_SliderTime.SetCurPos(int(l_seconds_cur / l_seconds_all * 1000));
	}
	m_TimeWnd.SetTime((UINT)l_seconds_cur);
	UpdateStatWindow();

	if (BASS_ACTIVE_STOPPED == BASS_ChannelIsActive(l_handle))
	{
		PostMessage(WM_COMMAND, IDB_BTNSTOP, LONG(this->m_BtnSTOP.m_hWnd));
	}
	*/
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
		const int bytesSec = bitrate * (1000/8); //bitrate is kbit/sec
		fileSize = m_recordingChain.GetFilter<FileWriter>()->GetFileLength();
		fileSeconds = double(fileSize / bytesSec);
	}

	const CString strFileSeconds = Helpers::ToString_HMMSS(fileSeconds);
	m_StatWnd.Set(frequency, bitrate, stereo);
	m_StatWnd.Set(fileSize/1024, strFileSeconds);

	/*
	int l_stream_rate = m_conf.GetConfDialMp3()->nBitrate;
	int l_stream_freq = m_conf.GetConfDialMp3()->nFreq;
	int l_stream_mode = m_conf.GetConfDialMp3()->nStereo;

	QWORD l_size_bytes = 0;
	double l_size_seconds = 0;

	if (m_bassPlaybackHandle)
	{
		BASS_CHANNELINFO l_channel_info;
		BASS_ChannelGetInfo(m_bassPlaybackHandle, &l_channel_info);

		l_size_bytes = BASS_StreamGetFilePosition(m_bassPlaybackHandle,
			BASS_FILEPOS_END);

		l_size_seconds = BASS_ChannelBytes2Seconds(m_bassPlaybackHandle,
			BASS_ChannelGetLength(m_bassPlaybackHandle, BASS_POS_BYTE));

		l_stream_rate = int(l_size_bytes / (125* l_size_seconds) + 0.5); //Kbps
		l_stream_freq = l_channel_info.freq;
		l_stream_mode = (l_channel_info.chans > 1) ? 1 : 0;
	}
	else if (g_record_handle)
	{
		l_size_bytes   = (QWORD)m_record_file.GetLength();
		l_size_seconds = (double)l_size_bytes / (l_stream_rate * 125);
	}

	const CString allSeconds = Helpers::ToString_HMMSS(l_size_seconds);
	m_StatWnd.Set(l_stream_freq, l_stream_rate, l_stream_mode);
	m_StatWnd.Set((UINT)l_size_bytes/1024, allSeconds);
	*/
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
void CMainFrame::OnUpdateOptTop(CCmdUI* pCmdUI) 
{
	int enable = m_conf.GetConfProg()->bAlwaysOnTop;
	pCmdUI->SetCheck(enable);
}

//------------------------------------------------------------------------------
void CMainFrame::OnUpdateOptEm(CCmdUI* pCmdUI) 
{
	int enable = m_conf.GetConfProg()->bEasyMove;
	pCmdUI->SetCheck(enable);
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

////////////////////////////////////////////////////////////////////////////////
CString CMainFrame::GetAutoName( CString& strPattern )
{
	// Removing all '%' symbols from the end of string
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
	if(!m_conf.GetConfDialGen()->bToolTips)
		return true;

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;

	// Getting an element ID and a string from it
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
			std::map<ActiveSoundMixer, CString> l_line_names;
			l_line_names[E_REC_MIXER] = m_RecMixer.GetLineName(m_RecMixer.GetCurLine());
			l_line_names[E_REC_LOOPBACK]= CString(_T("Speakers (Software)"));
			l_line_names[E_REC_LOOPBACK_MIX]= CString(_T("Speakers + Microphone (Software)"));
			l_line_names[E_PLAY_MIXER] = m_PlayMixer.GetLineName(m_PlayMixer.GetCurLine());
			l_line_names[E_PLAY_STREAM] = CString(_T("Playback volume"));

			ASSERT(l_line_names.find(m_active_mixer) != l_line_names.end());
			strTipText = strTipText + l_line_names[m_active_mixer];
		}
	}

	// Copying hint string into the structure
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
	HWND hMixWnd = ::FindWindow(_T("Volume Control"), NULL);
	while (hMixWnd)
	{
		::SendMessage(hMixWnd, WM_CLOSE, 0, 0);
		hMixWnd = ::FindWindow(_T("Volume Control"), NULL);
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

			if (nCmdShow == SW_MINIMIZE && m_conf.GetConfDialGen()->bTrayMin)
			{
				nCmdShow = SW_HIDE;
			}
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
		if (m_pOptDialog)
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
	pCmdUI->Enable(!IsRecording(m_nState));
}

void CMainFrame::OnUpdateTrayRec(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsPlaying(m_nState));
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnMIX_SEL()
{
	{
		CRect r;
		m_BtnMIX_SEL.GetWindowRect(&r);
		CRecordingSourceDlg::GetInstance()->Execute(CPoint(r.right, r.top));
		return;
	}

	/*
	// TEST code
	CMenu testMenu;
	testMenu.CreatePopupMenu();
	//testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("FiiO USB DAC-E10"));
	//testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("Устройство с поддержкой High Definition Audio"));
	//testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("HD Webcam C270"));

	testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("Микрофон (HD Webcam C270)"));
	testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("Микрофон (Устройство с поддержкой High Definition Audio)"));
	testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("Интерфейс SPDIF (FiiO USB DAC-E10)"));
	testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("Динамики (Устройство с поддержкой High Definition Audio)"));
	testMenu.AppendMenu(MF_SEPARATOR);
	testMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("Item after separator"));

	CRect r0;
	m_BtnMIX_SEL.GetWindowRect(&r0);

	testMenu.TrackPopupMenu(TPM_VCENTERALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,
		r0.right, r0.top+(r0.bottom-r0.top)/2, this);
	return;
	*/


	static bool bShowMenu = true;
	if (!bShowMenu)
	{
		bShowMenu = true;
		return;
	}
	/*
	int l_input[] = {
		BASS_INPUT_TYPE_DIGITAL,
		BASS_INPUT_TYPE_LINE,
		BASS_INPUT_TYPE_MIC,
		BASS_INPUT_TYPE_SYNTH,
		BASS_INPUT_TYPE_CD,
		BASS_INPUT_TYPE_PHONE,
		BASS_INPUT_TYPE_SPEAKER,
		BASS_INPUT_TYPE_WAVE,
		BASS_INPUT_TYPE_AUX,
		BASS_INPUT_TYPE_ANALOG,
		BASS_INPUT_TYPE_UNDEF
	};
	{
		int count=0;
		BASS_DEVICEINFO info;
		for (int device_id=0; BASS_RecordGetDeviceInfo(device_id, &info); device_id++)
		{
			if (info.flags&BASS_DEVICE_ENABLED) // device is enabled
			{
				BASS_RecordInit(device_id);
				const char *name;
				for (int n=-1; name=BASS_RecordGetInputName(n); n++)
				{
					float vol;
					int s=BASS_RecordGetInput(n, &vol);
					printf("%s [%s : %g]\n", name, s&BASS_INPUT_OFF?"off":"on", vol);

					if ((s&BASS_INPUT_TYPE_MASK) == BASS_INPUT_TYPE_MIC)
					{
						bool l_found_mic = true;
					}
				}
				BASS_RecordFree();
				int l_error = BASS_ErrorGetCode();
				count++; // count it 
			}
		}
		count += 0;
	}
	*/
	// Restarting the mixer
	m_RecMixer.Open(WAVE_MAPPER, GetSafeHwnd());
	m_PlayMixer.Open(WAVE_MAPPER, GetSafeHwnd());
	/* ///@TODO: Check this code
	if (m_recording_mixer != E_REC_LOOPBACK)
	{
		if (m_active_mixer == E_REC_MIXER)
			OnRecMixMenuSelect(ID_MIXITEM_REC0 + m_RecMixer.GetCurLine());
		else if (m_active_mixer == E_PLAY_MIXER)
			OnPlayMixMenuSelect(ID_MIXITEM_PLAY0 + m_PlayMixer.GetCurLine());
		else if (m_active_mixer == E_PLAY_STREAM)
			OnPlayVolumeSelect();
	}
	*/
	CRect r;
	m_BtnMIX_SEL.GetWindowRect(&r);

	CMenu mixMenu;
	mixMenu.CreatePopupMenu();

	mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_PLAY_VOLUME, _T("Playback volume"));

	// Not displaying system playback mixer lines
	//for(int j = 0; j < m_PlayMixer.GetLinesNum(); j++)
	//	mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_PLAY0 + j, m_PlayMixer.GetLineName(j));

	mixMenu.AppendMenu(MF_SEPARATOR);
	for(int i = 0; i < m_RecMixer.GetLinesNum(); i++)
		mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC0 + i, m_RecMixer.GetLineName(i));

	if (((CMP3_RecorderApp* )AfxGetApp())->IsVistaOS())
	{
		mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK, _T("Speakers"));
		mixMenu.AppendMenu(MF_STRING, ID_MIXITEM_REC_LOOPBACK_MIX, _T("Speakers + Microphone"));
	}

	UINT itemID = ID_MIXITEM_REC0 + m_RecMixer.GetCurLine();
	if (m_recording_mixer == E_REC_LOOPBACK)
		itemID = ID_MIXITEM_REC_LOOPBACK;
	if (m_recording_mixer == E_REC_LOOPBACK_MIX)
		itemID = ID_MIXITEM_REC_LOOPBACK_MIX;
	mixMenu.CheckMenuItem(itemID, MF_CHECKED | MF_BYCOMMAND);

	/*
	mixMenu.CheckMenuItem(ID_MIXITEM_REC0 + m_RecMixer.GetCurLine(), MF_CHECKED | MF_BYCOMMAND);
	//Now there is an additional checkbox, if mixing mic. with a loopback stream
	//if (m_recording_mixer == E_REC_LOOPBACK)
	if (m_loopback_hdsp != 0)
		mixMenu.CheckMenuItem(ID_MIXITEM_REC_LOOPBACK, MF_CHECKED | MF_BYCOMMAND);
	*/

	// Starting a context menu and setting the display flag
	int nItemID = mixMenu.TrackPopupMenu(TPM_VCENTERALIGN|TPM_LEFTBUTTON|
		TPM_RETURNCMD, r.right, r.top+(r.bottom-r.top)/2, this);
	if(!nItemID)
	{
		POINT p;
		::GetCursorPos(&p);
		RECT r;
		m_BtnMIX_SEL.GetWindowRect(&r);
		bShowMenu = (PtInRect(&r, p)) ? false : true;
	}
	else
	{
		this->PostMessage(WM_COMMAND, MAKEWPARAM(nItemID, 0), 0);
	}
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRecMixMenuSelect(UINT nID)
{
	OutputDebugString(__FUNCTION__"\n");

	if (m_RecMixer.GetLinesNum() > 0)
	{
		m_RecMixer.SetLine(nID - ID_MIXITEM_REC0);
	}
	m_active_mixer = E_REC_MIXER;
	m_recording_mixer = E_REC_MIXER;

	///@NOTE: Pretending that change has come from outside, for interface
	///updating. Will update it in WindowProc.
	PostMessage(MM_MIXM_LINE_CHANGE, 0, 0);

	m_conf.GetConfProg()->nRecLineID = nID;
}

//------------------------------------------------------------------------------
void CMainFrame::OnPlayMixMenuSelect(UINT nID)
{
	OutputDebugString(__FUNCTION__"\n");

	if (m_PlayMixer.GetLinesNum() > 0)
	{
		m_PlayMixer.SetLine(nID - ID_MIXITEM_PLAY0);
	}
	m_active_mixer = E_PLAY_MIXER;
	UpdateMixerState();
}

////////////////////////////////////////////////////////////////////////////////
// Seek and volume slider handlers
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
	int nPercent = int(100.0 * curpos / (maxpos - minpos));

	CString strTitle;
	if (m_active_mixer == E_PLAY_MIXER)
	{
		if(m_PlayMixer.GetLinesNum() == 0)
			return;
		strTitle.Format(IDS_VOLUME_TITLE, nPercent,
			m_PlayMixer.GetLineName(m_PlayMixer.GetCurLine()));
		m_PlayMixer.SetVol(nPercent);
	}
	else if (m_active_mixer == E_REC_MIXER)
	{
		if(m_RecMixer.GetLinesNum() == 0)
			return;
		strTitle.Format(IDS_VOLUME_TITLE, nPercent,
			m_RecMixer.GetLineName(m_RecMixer.GetCurLine()));
		m_RecMixer.SetVol(nPercent);
	}
	else if (m_active_mixer == E_PLAY_STREAM)
	{
		strTitle.Format(IDS_VOLUME_TITLE, nPercent, CString(_T("Playback volume")));
		m_playback_volume = (float)nPercent / 100;

		if (IsPlaying(m_nState))
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
		if (m_active_mixer != E_PLAY_STREAM)
		{
			m_title->SetTitleText(strTitle, 1200);
		}
		break;
	}
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
	// Checking for mixer lines available
	if (m_active_mixer == E_REC_MIXER && m_RecMixer.GetLinesNum() == 0)
		return;
	if (m_active_mixer == E_PLAY_MIXER && m_PlayMixer.GetLinesNum() == 0)
		return;

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
	// Checking for mixer lines available
	if (m_active_mixer == E_REC_MIXER && m_RecMixer.GetLinesNum() == 0)
		return;
	if (m_active_mixer == E_PLAY_MIXER && m_PlayMixer.GetLinesNum() == 0)
		return;

	if (m_SliderVol.IsWindowEnabled())
	{
		int nPos = max(m_SliderVol.GetRangeMin(),
			m_SliderVol.GetPos() - m_SliderVol.GetPageSize());
		m_SliderVol.SetPos(nPos);

		this->PostMessage(WM_HSCROLL, MAKEWPARAM(0, SB_PAGELEFT),
			(LPARAM)m_SliderVol.GetSafeHwnd());
	}
}

////////////////////////////////////////////////////////////////////////////////
// SCHEDULER
////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnSched()
{
	/*
#ifndef _DEBUG
	// Button is disabled after the trial period is over
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	bool bIsEnabled		= m_conf.GetConfDialSH2()->bIsEnabled  != 0;
	bool bSchedStart	= m_conf.GetConfDialSH2()->bSchedStart != 0;
	SHR_TIME* ptStart	= &m_conf.GetConfDialSH2()->t_start;
	SHR_TIME* ptRec		= &m_conf.GetConfDialSH2()->t_rec;
	SHR_TIME* ptStop	= &m_conf.GetConfDialSH2()->t_stop;

	if (!bIsEnabled)
	{	// If the start time is not set, run the scheduler after recording started
		if (bSchedStart)
		{
			if (m_conf.GetConfDialSH2()->nStopByID == 1)
			{
				m_sched2.SetRecTime(ptRec, ptStart);
				TRACE("==> OnBtnSched: SetRecTime(%d:%d:%d, %d:%d:%d)\n",
					ptRec->h, ptRec->m, ptRec->s, ptStart->h, ptStart->m, ptStart->s);
			}
			else
			{
				m_sched2.SetStopTime(ptStop, ptStart);
				TRACE("==> OnBtnSched: SetStopTime(%d:%d:%d, %d:%d:%d)\n",
					ptStop->h, ptStop->m, ptStop->s, ptStart->h, ptStart->m, ptStart->s);
			}
			if (!m_sched2.Start())
			{
				return;
			}
		}
		// If recording, when starting scheduler
		else if (BASS_ACTIVE_PLAYING == BASS_ChannelIsActive(g_record_handle))
		{
			if (m_conf.GetConfDialSH2()->nStopByID == 1)
			{
				m_sched2.SetRecTime(ptRec, NULL);
			}
			else
			{
				m_sched2.SetStopTime(ptStop, NULL);
			}
			if (!m_sched2.Start())
			{
				return;
			}
		}
		m_StatWnd.m_btnSched.SetState(BTN_PRESSED);
	}
	else
	{
		m_sched2.Stop();
		m_StatWnd.m_btnSched.SetState(BTN_NORMAL);
	}

	m_conf.GetConfDialSH2()->bIsEnabled = !bIsEnabled;
	// Updating the "Record" button when scheduler started
	//UpdateButtonState(IDB_BTNREC);
	*/

	/*
	// Updating time window
	int nRecTime = 0;
	if (m_conf.GetConfDialSH2()->bIsEnabled)
	{
		nRecTime = m_sched2.GetRecTimeInSec();
	}
	m_TimeWnd.SetTime(nRecTime);
	*/
}

////////////////////////////////////////////////////////////////////////////////
// IN: nAction - planned action
//		0 - start recording
//		1 - stop recording
void Scheduler2Function(int nAction)
{
	/*
	//CMainFrame* pMainWnd = (CMainFrame *)AfxGetMainWnd();
	CMainFrame* pMainWnd = CMainFrame::m_pMainFrame;

	if (nAction == 0)
	{
		if (pMainWnd->m_conf.GetConfDialSH2()->bRunExternal)
		{
			ShellExecute(0, NULL, pMainWnd->m_conf.GetConfDialSH2()->strFileName,
				NULL, NULL, SW_SHOWNORMAL);
		}

		if (pMainWnd->m_record_file.m_hFile == CFile::hFileNull)
		{
			CString strName = pMainWnd->GetAutoName(CString(""));
			CString strPath = pMainWnd->m_conf.GetConfProg()->strLastFilePath;
			if (strPath.IsEmpty())
			{
				strPath = pMainWnd->m_strDir;
			}
			strName = strPath + "\\" + strName;
			TRACE1("==> Scheduler2Function: trying to open file: %s\n", strName);
			pMainWnd->OpenFile(strName);
		}
		
		if (BASS_ChannelIsActive(g_record_handle) != BASS_ACTIVE_PLAYING)
		{
			TRACE0("==> Scheduler2Function: call OnBtnREC to start recording\n");
			//pMainWnd->OnBtnSTOP();
			pMainWnd->OnBtnREC();
			//pMainWnd->UpdateButtonState(IDB_BTNREC);
		}
	}
	else if (nAction == 1)
	{
		TRACE0("==> Scheduler2Function: call OnBtnSTOP\n");
		pMainWnd->OnBtnSTOP();

		if (pMainWnd->m_conf.GetConfDialSH2()->action.shutDown)
		{
			CTimerDlg dlg;
			dlg.m_tdi.strDlgText = "Recording complete. Shutdown computer?";
			if (dlg.DoModal() == IDOK)
			{
				ShutDownComputer();
			}
			return;
		}
		else if (pMainWnd->m_conf.GetConfDialSH2()->action.closeAll)
		{
			CTimerDlg dlg;
			dlg.m_tdi.strDlgText = "Recording complete. Close the program?";
			if (dlg.DoModal() == IDOK)
			{
				CloseAllPrograms();
			}
		}
	}
	*/
}

////////////////////////////////////////////////////////////////////////////////
// Processing sound level monitoring
////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnMonitoring()
{
#ifndef _DEBUG
	// Button is disabled after the trial period is over
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	if (!m_bMonitoringBtn)
	{
		if (m_nState == STOP_STATE && !MonitoringStart())
		{
			AfxMessageBox("Monitoring error!", MB_OK);
			return;
		}
		m_bMonitoringBtn = true;
		m_StatWnd.m_btnMon.SetState(BTN_PRESSED);
	}
	else
	{
		MonitoringStop();
		m_bMonitoringBtn = false;
		m_StatWnd.m_btnMon.SetState(BTN_NORMAL);
	}
	m_conf.GetConfProg()->bMonitoring = m_bMonitoringBtn;
}

//------------------------------------------------------------------------------
bool CMainFrame::IsMonitoringOnly()
{
	return m_bMonitoringBtn && (m_nState == STOP_STATE);
}

//------------------------------------------------------------------------------
bool CMainFrame::MonitoringStart()
{
	if (!m_monitoringChain.IsEmpty())
		return true;

	//TODO: add check if recording currently. Possible should avoid two
	//CWasapiRecorders in one thread (BASS_Wasapi_Stop etc. can affect also main recorder).

	WasapiHelpers::DevicesArray selectedDevices = CRecordingSourceDlg::GetInstance()->GetSelectedDevices();
	ASSERT(!selectedDevices.empty());
	const DWORD deviceID = selectedDevices[0].first;

	CWasapiRecorder* recorder = new CWasapiRecorder(deviceID, 44100, 2);
	m_monitoringChain.AddFilter(recorder); //will keep recorder object
	if (recorder->Start())
	{
		m_GraphWnd.StartUpdate(PeaksCallback_Wasapi, LinesCallback_Wasapi, recorder);
		return true;
	}
	return false;

	/*
	if (!BASS_RecordInit(m_conf.GetConfDialGen()->nRecDevice))
		return false;

	if (BASS_GetDevice() == -1)
	{
		int deviceID = m_conf.GetConfDialGen()->nPlayDevice + 1; // BASS starts devices from 1
		BASS_Init(deviceID <= 0 ? -1 : deviceID, 44100, 0, GetSafeHwnd(), NULL);
	}

	SAFE_DELETE(m_visualization_data);
	m_visualization_data = new VisualizationData(44100, 2);

	g_monitoring_handle = BASS_RecordStart(44100, 2, MAKELONG(BASS_SAMPLE_FLOAT, 25),
		(RECORDPROC *)&MonitoringProc, this);
	if (g_monitoring_handle)
	{
		SAFE_DELETE(m_vista_loopback);
		m_vista_loopback = new BassVistaLoopback(m_conf.GetConfDialGen()->nPlayDevice);
		HSTREAM l_stream_handle = m_vista_loopback->GetLoopbackStream();

		ASSERT(g_loopback_handle == 0);
		g_loopback_handle = BASS_Mixer_StreamCreate(44100, 2, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);
		ASSERT(g_loopback_handle);
		BASS_Mixer_StreamAddChannel(g_loopback_handle, l_stream_handle, BASS_MIXER_DOWNMIX);

		if (m_recording_mixer == E_REC_LOOPBACK)
		{
			m_mute_hdsp = BASS_ChannelSetDSP(g_monitoring_handle,
				Bass::StreamMuteDSP, NULL, 0xFF);
		}
		if (m_recording_mixer == E_REC_LOOPBACK || m_recording_mixer == E_REC_LOOPBACK_MIX)
		{
			m_loopback_hdsp = BASS_ChannelSetDSP(g_monitoring_handle,
				Bass::LoopbackStreamDSP, &g_loopback_handle, 0xFE);
		}

		///@bug Testing new functionality
		g_update_handle = g_monitoring_handle;
		m_GraphWnd.StartUpdate(PeaksCallback, LinesCallback, NULL);
	}
	return g_monitoring_handle != 0;
	*/
}

//------------------------------------------------------------------------------
void CMainFrame::MonitoringStop()
{
	m_GraphWnd.StopUpdate();
	m_monitoringChain.GetFilter<CWasapiRecorder>()->Stop();
	m_monitoringChain.Empty();


	/*
	if (g_monitoring_handle)
	{
		m_GraphWnd.StopUpdate();

		BOOL l_result = BASS_ChannelStop(g_monitoring_handle);
		ASSERT(l_result);
		l_result = BASS_ChannelStop(g_loopback_handle);
		ASSERT(l_result);

		SAFE_DELETE(m_vista_loopback);
		SAFE_DELETE(m_visualization_data);
		g_loopback_handle = 0;
		g_monitoring_handle = 0;
		m_loopback_hdsp = 0;
		m_mute_hdsp = 0;

		if (!m_bassPlaybackHandle && !g_loopback_handle)
			BASS_Free();
		BASS_RecordFree();
	}
	*/
}

////////////////////////////////////////////////////////////////////////////////
// VAS 
////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnBtnVas()
{
#ifndef _DEBUG
	// Button is disabled after the trial period is over
	if(fsProtect_GetDaysLeft() <= 0)
		return;
#endif

	CONF_DIAL_VAS* pConfig = m_conf.GetConfDialVAS();

	if(!m_vas.IsRunning())
	{
		m_vas.InitVAS(pConfig->nThreshold, pConfig->nWaitTime);
		m_StatWnd.m_btnVas.SetState(BTN_PRESSED);
		m_GraphWnd.ShowVASMark(true, pConfig->nThreshold);
	}
	else
	{
		m_vas.StopVAS();
		m_StatWnd.m_btnVas.SetState(BTN_NORMAL);
		if(m_nState == RECORD_STATE)
			m_TrayIcon.SetIcon(IDI_TRAY_REC);
		
		m_GraphWnd.ShowVASMark(false);
	}
	pConfig->bEnable = m_vas.IsRunning();
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::ProcessVAS(bool bVASResult)
{
	static bool bOldVASResult = false;

	if (bVASResult == true)
	{
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
	else if ((bVASResult == false) && (bOldVASResult == true))
	{	// Going out of VAS area ?
		m_IcoWnd.SetNewIcon(ICON_REC);
		m_TrayIcon.SetIcon(IDI_TRAY_REC);
	}

	bOldVASResult = bVASResult;
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::UpdateButtonState(UINT nID)
{
	CButton* pBtn = (CButton *)GetDlgItem(nID);
	ASSERT(pBtn);

	pBtn->ModifyStyle(WS_DISABLED, 0);	// Resetting button state
	if (IDB_BTNREC == nID)
	{
		bool bSchedStart   = m_conf.GetConfDialSH2()->bSchedStart != 0;
		bool bSchedEnabled = m_conf.GetConfDialSH2()->bIsEnabled != 0;
		bool l_reserved_4shr = bSchedEnabled && bSchedStart && CanRecord();

		if (l_reserved_4shr || IsPlaying(m_nState))
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
	static int l_current_state = -1;
	if (l_current_state == m_nState)
		return;

	switch (m_nState)
	{
	case PLAY_STATE:
		///@bug Testing new functionality
		//g_update_handle = m_bassPlaybackHandle;
		m_GraphWnd.StartUpdate(PeaksCallback, LinesCallback, this);

		m_BtnPLAY.SetIcon(IDI_PAUSE);
		m_TrayIcon.SetIcon(IDI_TRAY_PLAY);
		m_IcoWnd.SetNewIcon(ICON_PLAY);
		break;

	case PAUSEPLAY_STATE:
		m_GraphWnd.StopUpdate();
		m_BtnPLAY.SetIcon(IDI_PLAY);
		m_TrayIcon.SetIcon(IDI_TRAY_PAUSE);
		m_IcoWnd.SetNewIcon(ICON_PAUSEP);
		break;

	case RECORD_STATE:
		m_GraphWnd.StartUpdate(PeaksCallback_Wasapi, LinesCallback_Wasapi,
			m_recordingChain.GetFilter<CWasapiRecorder>());

		///@bug Testing new functionality
		//g_update_handle = g_record_handle;
		//m_GraphWnd.StartUpdate(PeaksCallback, LinesCallback, this);

		m_BtnREC.SetIcon(IDI_PAUSE);
		m_TrayIcon.SetIcon(IDI_TRAY_REC);
		m_IcoWnd.SetNewIcon(ICON_REC);
		break;

	case PAUSEREC_STATE:
		m_GraphWnd.StopUpdate();
		m_BtnREC.SetIcon(IDI_REC);
		m_TrayIcon.SetIcon(IDI_TRAY_PAUSE);
		m_IcoWnd.SetNewIcon(ICON_PAUSER);
		break;

	case STOP_STATE:
	default:
		m_SliderTime.SetCurPos(0);
		if (!m_bMonitoringBtn)
		{
			m_GraphWnd.StopUpdate();
		}
		m_BtnREC.SetIcon(IDI_REC);
		m_BtnPLAY.SetIcon(IDI_PLAY);
		m_TrayIcon.SetIcon(IDI_TRAY_STOP);
		m_IcoWnd.SetNewIcon(ICON_STOP);
		UpdateStatWindow();
		break;
	}

	UpdateButtonState(IDB_BTNPLAY);
	UpdateButtonState(IDB_BTNREC);
	l_current_state = m_nState;
}

//------------------------------------------------------------------------------
void CMainFrame::SetRecordingLine(int nLineID)
{
	if (nLineID <= 0)
		nLineID = ID_MIXITEM_REC0 + m_RecMixer.GetCurLine();

	this->PostMessage(WM_COMMAND, nLineID, 0);
}
//------------------------------------------------------------------------------

void CMainFrame::UpdateMixerState()
{
	// Current rec. mixer could have several lines, we need to update button
	//static int l_current_mixer = -1;
	//if (l_current_mixer == m_active_mixer)
	//	return;

	m_SliderVol.EnableWindow(false);
	switch (m_active_mixer)
	{
	case E_REC_MIXER:
		{
			int l_line_type = 0;
			if (m_RecMixer.GetLinesNum() > 0)
			{
				m_SliderVol.EnableWindow(true);
				m_SliderVol.SetPos(m_RecMixer.GetVol(m_RecMixer.GetCurLine()));
				l_line_type = m_RecMixer.GetLineType(m_RecMixer.GetCurLine());
			}
			const int ICON_ID[] = {
				IDI_MIXLINE00, IDI_MIXLINE01, IDI_MIXLINE02, IDI_MIXLINE03,
				IDI_MIXLINE04, IDI_MIXLINE05, IDI_MIXLINE06, IDI_MIXLINE07,
				IDI_MIXLINE08, IDI_MIXLINE09, IDI_MIXLINE10
			};
			ASSERT(l_line_type < sizeof(ICON_ID)/sizeof(int));
			m_BtnMIX_SEL.SetIcon(ICON_ID[l_line_type]);
		}
		break;

	case E_REC_LOOPBACK:
		m_SliderVol.SetPos(m_SliderVol.GetRangeMax());
		m_BtnMIX_SEL.SetIcon(IDI_MIXLINE04);
		break;

	case E_PLAY_MIXER:
		if (m_RecMixer.GetLinesNum() > 0)
		{
			m_SliderVol.EnableWindow(true);
			m_SliderVol.SetPos(m_PlayMixer.GetVol(m_PlayMixer.GetCurLine()));
			m_BtnMIX_SEL.SetIcon(IDI_MIXLINE);
		}
		break;

	case E_PLAY_STREAM:
		m_SliderVol.EnableWindow(true);
		m_SliderVol.SetPos(int(m_playback_volume * 100)); 
		m_BtnMIX_SEL.SetIcon(IDI_MIXLINE);
		break;
	}
	//l_current_mixer = m_active_mixer;
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

	CString l_auto_name = GetAutoName(CString(_T("")));
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

////////////////////////////////////////////////////////////////////////////////
/*
void CALLBACK CMainFrame::LoopbackStreamDSP(HDSP a_handle, DWORD a_channel,
	void *a_buffer, DWORD a_length, void *a_user)
{
	// Loopback is a decoding stream and have same parameters (freq, channels)
	// as a main recording stream, for direct copying.

	//1. Fill buffer with required length from Loopback stream (a_user)
	const int BUFFER_LENGTH = 256 * 1024;
	ASSERT(a_length <= BUFFER_LENGTH);
	ASSERT(a_user);

	static char l_src_buffer[BUFFER_LENGTH] = {0}; //256k buffer
	HSTREAM l_src_stream = *((HSTREAM*)a_user);

	DWORD l_length = BASS_ChannelGetData(l_src_stream, l_src_buffer, a_length);
	if (l_length == -1)
	{
		int l_error_code = BASS_ErrorGetCode();
		return;
	}
	ASSERT(l_length == a_length);

	//2. Replace data in the recording buffer (assuming we have float samples).
	float* l_dst_buffer = (float*)a_buffer;
	float* l_src_buffer_float = (float*)l_src_buffer;
	for (int i = 0; i < a_length / sizeof(float); i++)
		l_dst_buffer[i] = max(-1.0, min((l_dst_buffer[i] + l_src_buffer_float[i]), 1.0));

	/*
	char* l_dst_buffer = (char*)a_buffer;
	for (DWORD i = 0; i < a_length; i++)
		l_dst_buffer[i] = l_src_buffer[i];
	*/
//}
//*/

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRecLoopbackMixSelect()
{
	/*
	OutputDebugString(__FUNCTION__" :: setting rec. loopback mix flag\n");

	OnRecLoopbackSelect();

	m_active_mixer = E_REC_LOOPBACK_MIX;
	m_recording_mixer = E_REC_LOOPBACK_MIX;
	if (g_record_handle)
	{
		OutputDebugString(__FUNCTION__ " :: removing mute DSP on rec. stream\n");
		BASS_ChannelRemoveDSP(g_record_handle, m_mute_hdsp);
	}
	else if (g_monitoring_handle)
	{
		OutputDebugString(__FUNCTION__ " :: removing mute DSP on mon. stream\n");
		BASS_ChannelRemoveDSP(g_monitoring_handle, m_mute_hdsp);
	}
	m_mute_hdsp = 0;
	m_conf.GetConfProg()->nRecLineID = ID_MIXITEM_REC_LOOPBACK_MIX;
	*/
}

void CMainFrame::OnRecLoopbackSelect()
{
	/*
	//Reverting loopback DSP state.
	if (m_loopback_hdsp != 0)
	{
		HRECORD destHandle = (g_record_handle) ? g_record_handle : g_monitoring_handle; 
		BASS_ChannelRemoveDSP(destHandle, m_loopback_hdsp);
		m_loopback_hdsp = 0;
	}
	else if (g_record_handle || g_monitoring_handle)
	{
		m_active_mixer = E_REC_LOOPBACK;
		m_recording_mixer = E_REC_LOOPBACK;
		HRECORD destHandle = (g_record_handle) ? g_record_handle : g_monitoring_handle; 
		m_loopback_hdsp = BASS_ChannelSetDSP(destHandle,
			Bass::LoopbackStreamDSP, &g_loopback_handle, 0xFF);
	}
	*/

	/*
	OutputDebugString(__FUNCTION__"\n");

	// If we are in recording state, when set or remove DSP function,
	// otherwise just set the Loopback recording flag

	if (m_active_mixer != E_REC_LOOPBACK || m_active_mixer != E_REC_LOOPBACK_MIX)
	{
		OutputDebugString(__FUNCTION__" :: setting rec. loopback mixer flag\n");
		m_active_mixer = E_REC_LOOPBACK;
		m_recording_mixer = E_REC_LOOPBACK;
		UpdateMixerState();

		HRECORD handle = g_record_handle ? g_record_handle : g_monitoring_handle;
		if (handle)
		{
			if (!m_mute_hdsp)
			{
				OutputDebugString(__FUNCTION__ " :: setting mute DSP");
				m_mute_hdsp = BASS_ChannelSetDSP(handle, Bass::StreamMuteDSP, NULL, 0xFF);
			}
			if (!m_loopback_hdsp)
			{
				OutputDebugString(__FUNCTION__ " :: setting loopback DSP");
				m_loopback_hdsp = BASS_ChannelSetDSP(handle, Bass::LoopbackStreamDSP, &g_loopback_handle, 0xFE);
			}
		}
	}
	m_conf.GetConfProg()->nRecLineID = ID_MIXITEM_REC_LOOPBACK;
	*/
}

//------------------------------------------------------------------------------
void CMainFrame::OnPlayVolumeSelect()
{
	OutputDebugString(__FUNCTION__"\n");

	m_active_mixer = E_PLAY_STREAM;
	UpdateMixerState();
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
	//return BASS_ChannelIsActive(m_bassPlaybackHandle)==BASS_ACTIVE_STOPPED;
}
//------------------------------------------------------------------------------

//!!!test
LRESULT CMainFrame::OnRecSourceDialogClosed(WPARAM wParam, LPARAM lParam)
{
	/*
	FilterChain chain(NULL, NULL);
	chain.AddFilter(new FileWriter(_T("d:\\test.mp3")));
	chain.AddFilter(new CEncoder_MP3(128, 44100, 2));

	FileWriter* curWriter = chain.GetFilter<FileWriter>();
	FileWriterWAV* curWriterWav = chain.GetFilter<FileWriterWAV>();
	CEncoder_MP3* curEncoder = chain.GetFilter<CEncoder_MP3>();
	CWasapiRecorder* curRecorder = chain.GetFilter<CWasapiRecorder>();

	bool debug = true;
	*/

	//NEW version, commented.
	/*
	OutputDebugString(__FUNCTION__"\n");
	m_GraphWnd.StopUpdate();

	WasapiHelpers::DevicesArray selectedDevices = CRecordingSourceDlg::GetInstance()->GetSelectedDevices();
	ASSERT(!selectedDevices.empty());
	const DWORD deviceID = selectedDevices[0].first;

	try
	{
		SAFE_DELETE(g_wasapi_recorder);
		SAFE_DELETE(g_fileWriter);
		SAFE_DELETE(g_mp3Encoder);

		g_wasapi_recorder = new CWasapiRecorder(deviceID, 44100, 2);//, NULL, NULL);
		const int actualFreq = g_wasapi_recorder->GetActualFrequency();
		const int actualChannels = g_wasapi_recorder->GetActualChannelCount();

		g_mp3Encoder = new CEncoder_MP3(128, actualFreq, actualChannels);
		g_wasapi_recorder->SetChildFilter(g_mp3Encoder);

		g_fileWriter = new FileWriter(_T("d:\\test.mp3"));
		g_mp3Encoder->SetChildFilter(g_fileWriter);
	}
	catch (CException* e)
	{
		SAFE_DELETE(g_wasapi_recorder);
		SAFE_DELETE(g_fileWriter);
		SAFE_DELETE(g_mp3Encoder);
		throw;
	}
	catch (...)
	{
		AfxMessageBox(_T("Unknown exception occured."));
	}

	if (!g_wasapi_recorder->Start())
		return 0;

	m_GraphWnd.StartUpdate(PeaksCallback_Wasapi, LinesCallback_Wasapi, g_wasapi_recorder);
	*/


	/*
	BOOL result = BASS_WASAPI_Stop(TRUE);
	result = BASS_WASAPI_Free();
	m_GraphWnd.StopUpdate();
	SAFE_DELETE(m_visualization_data);

	//graph_wnd start update with our test stream.
	
	WasapiHelpers::DevicesArray selectedDevices = CRecordingSourceDlg::GetInstance()->GetSelectedDevices();
	ASSERT(!selectedDevices.empty());
	const DWORD deviceID = selectedDevices[0].first;
	DWORD errorCode = 0;

	result = BASS_WASAPI_Init(deviceID, 44100, 2, BASS_WASAPI_AUTOFORMAT, 0.5, 0, WasapiRecordingProc, this);
	if (result)
	{
		OutputDebugString(__FUNCTION__" ::SUCCESS!\n");
		result = BASS_WASAPI_Start();
	}
	else
	{
		OutputDebugString(__FUNCTION__" ::ERROR!\n");
		errorCode = BASS_ErrorGetCode();
		return 0;
	}

	m_visualization_data = new VisualizationData(44100, 2);
	m_GraphWnd.StartUpdate(PeaksCallback_Wasapi, LinesCallback_Wasapi);
	*/
	return 0;
}
//------------------------------------------------------------------------------

LRESULT CMainFrame::OnRecSourceChanged(WPARAM wParam, LPARAM lParam)
{
	OutputDebugString(__FUNCTION__"\n");
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
			const int icoWndIcon = param.valueInt == 1 ? ICON_RECVAS : ICON_REC;
			const int trayIcon = param.valueInt == 1 ? IDI_TRAY_PAUSE : IDI_TRAY_REC;
			m_IcoWnd.SetNewIcon(icoWndIcon);
			m_TrayIcon.SetIcon(trayIcon);
			continue;
		}

		CString debugString;
		if (param.type == Parameter::eInteger)
			debugString.Format(_T("%s :: '%s'=%d"), __FUNCTION__, param.name, param.valueInt);
		if (param.type == Parameter::eFloat)
			debugString.Format(_T("%s :: '%s'=%f"), __FUNCTION__, param.name, param.valueFloat);
		if (param.type == Parameter::eString)
			debugString.Format(_T("%s :: '%s'=%s"), __FUNCTION__, param.name, param.valueString);
		::OutputDebugString(debugString);
	}
	m_filterNotifications.clear();
	return 0;
}

//------------------------------------------------------------------------------

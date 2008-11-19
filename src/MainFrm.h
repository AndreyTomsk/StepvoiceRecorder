////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MAINFRM_H__837D6DC8_C6B3_11D7_BC33_444553540000__INCLUDED_)
#define AFX_MAINFRM_H__837D6DC8_C6B3_11D7_BC33_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IcoWnd.h"
#include "StatWnd.h"
#include "TimeWnd.h"
#include "GraphWnd.h"

#include "Config.h"
#include "ControlButton.h"

#include "MySlider.h"
#include "SliderVol.h"

#include "MySheet.h"

#include "WaveIn.h"
#include "WaveOut.h"
#include "Encoder_MP3.h"
#include "Decoder_MP3.h"
#include "MappedMP3.h"

#include "struct.h"
#include "SndFile.h"
#include "SndFile_MP3.h"

#include "TitleText.h"

#include "htmlhelp.h"

#include "SystemTray.h"

//#include "Mixer.h"
#include "MixerRec.h"
#include "MixerPlay.h"

#include "Scheduler.h"
#include "Scheduler2.h"
#include "vas.h"
#include "TimerDlg.h"

#include <bass.h>
#include <bassmix.h>
#include "BASS_VistaLoopback.h"
#include "VisualizationData.h"

////////////////////////////////////////////////////////////////////////////////
enum ProgramState
{
	STOP_STATE,
	PLAY_STATE,
	RECORD_STATE,
	PAUSEREC_STATE,
	PAUSEPLAY_STATE
};

////////////////////////////////////////////////////////////////////////////////
extern DWORD WINAPI SchedProc( LPVOID lpParam );
extern void Scheduler2Function(int nAction);

class CMainFrame : public CFrameWnd
{
	static void CALLBACK LoopbackStreamDSP(HDSP handle, DWORD channel,
		void *buffer, DWORD length, void *user);

	static CMainFrame* m_pMainFrame;

	static float PeaksCallback(int a_channel);
	static int   LinesCallback(int a_channel, float* a_buffer, int a_size);

	friend DWORD WINAPI SchedProc( LPVOID lpParam );
	friend void Scheduler2Function(int nAction);
	
	///Callback function to process sample data
	static BOOL CALLBACK NewRecordProc(HRECORD a_handle, void* a_buffer,
		DWORD a_length, void* a_user);

	VisualizationData* m_visualization_data;
	BassVistaLoopback* m_vista_loopback;
	bool m_loopback_recording;
	HDSP m_loopback_hdsp;

	CWaveIn*		m_pWaveIn;
	CWaveOut*		m_pWaveOut;
	CEncoder*		m_pEncoder;
	CDecoder*		m_pDecoder;
	CSndFile*		m_pSndFile;
	CTitleText*		m_title;
	CFile			m_record_file;

	CString m_strDir;
	int m_bMonitoringBtn;
	// program state
	int				m_nState;
	// program configuration
	CConfig			m_conf;
	// Display windows
	CIcoWnd			m_IcoWnd;
	CTimeWnd		m_TimeWnd;
	CStatWnd		m_StatWnd;
	CGraphWnd		m_GraphWnd;

	// Buttons
	CWnd m_BtnFrame;
	CControlButton m_BtnOPEN;
	CControlButton m_BtnPLAY;
	CControlButton m_BtnSTOP;
	CControlButton m_BtnREC;
	CControlButton m_BtnMIX_SEL;
	//CControlButton m_BtnMIX_REC;
	//CControlButton m_BtnMIX_PLAY;

	// Sliders
	CWnd			m_SliderFrame;
	CMySlider		m_SliderTime;
	CSliderVol		m_SliderVol;

	// Window snap
	int				m_nSnapPixels;
	RECT			m_rDesktopRect;
	SIZE			m_szMoveOffset;

	CSystemTray		m_TrayIcon;

	CMixerRec		m_RecMixer;
	CMixerPlay		m_PlayMixer;
	int				m_nActiveMixerID;

	CMySheet*		m_pOptDialog;	// указатель на запущенный диалог

public:
	CScheduler2		m_sched2;	// планировщик
	CVAS			m_vas;		// поддержка голосовой активации

public:
	CMainFrame();
	virtual ~CMainFrame();

	void UpdateInterface();
	bool IsMonitoringOnly();
	void OnBtnSched();
	void OnBtnMonitoring();
	void OnBtnVas();
	BOOL ShowWindow();
	CString GetProgramDir() { return m_strDir; }
	BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

protected:
	void ProcessSliderTime(UINT nSBCode, UINT nPos);
	void ProcessSliderVol(UINT nSBCode, UINT nPos);
	void Convert(UINT nCurSec, char* pszTime, int nStrSize);
	void OpenFile(CString& strFileName);

	void UpdateStatWindow();
	void UpdateButtonState(UINT nID);
	CString GetAutoName(CString& strPattern);

	/**
	 * Parses string, replacing patterns by real values. Currently supports:
	 * {Desktop}		- user desktop dir;
	 * {MyDocuments}	- user documents dir;
	 * {Autoname}		- generated file name (see options dialog for changing).
	 */
	CString ParseFileName(CString a_file_name);

protected:
	DECLARE_DYNAMIC(CMainFrame)
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	void ProcessVAS(bool bVASResult);
	void MonitoringStop();
	bool MonitoringStart();
	void UpdateTrayText();
	void CloseMixerWindows();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnOptEm();
	afx_msg void OnOptTop();
	afx_msg void OnBtnOPEN();
	afx_msg void OnBtnPLAY();
	afx_msg void OnBtnSTOP();
	afx_msg void OnBtnREC();
	afx_msg void OnBtnMIX_REC();
	afx_msg void OnBtnMIX_PLAY();
	afx_msg void OnBtnMIX_INV();
	afx_msg void OnBtnMIX_SEL();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMixRec();
	afx_msg void OnMixPlay();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnOptCom();
	afx_msg void OnFileFindfile();
	afx_msg void OnFileClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateSoundRec(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptTop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptEm(CCmdUI* pCmdUI);
	afx_msg void OnSoundRecA();
	afx_msg void OnSoundPlayA();
	afx_msg void OnSoundBegin();
	afx_msg void OnSoundRew();
	afx_msg void OnSoundFf();
	afx_msg void OnSoundEnd();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFileDelete();
	afx_msg void OnFileClear();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnOptSnddev();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateTrayPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrayRec(CCmdUI* pCmdUI);
	afx_msg void OnStatPref();
	afx_msg void OnVolUpA();
	afx_msg void OnVolDownA();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg LRESULT OnAreYouMe(WPARAM, LPARAM);
	afx_msg LRESULT OnParseLine(WPARAM, LPARAM);
	afx_msg void OnUpdateSoundPlay(CCmdUI* pCmdUI);
	afx_msg void OnRecMixMenuSelect(UINT nID);
	afx_msg void OnPlayMixMenuSelect(UINT nID);
	afx_msg void OnRecLoopbackSelect();

protected:
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
#endif // AFX_MAINFRM_H__837D6DC8_C6B3_11D7_BC33_444553540000__INCLUDED_

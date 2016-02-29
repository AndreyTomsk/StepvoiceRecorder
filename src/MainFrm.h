#pragma once
#include "IcoWnd.h"
#include "StatWnd.h"
#include "TimeWnd.h"
#include "GraphWnd.h"

#include "ControlButton.h"

#include "MySlider.h"
#include "SliderVol.h"

#include "TitleText.h"

#include "SystemTray.h"

#include <bass.h>
#include "VisualizationData.h"

#include "Filter.h" //for NOTIFY_CALLBACK
#include "FilterChain.h"
#include "AutoGainControl.h"
#include <vector>
#include <memory>

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

class CMainFrame : public CFrameWnd
{
	friend class CGraphWnd;

	static float PeaksCallback(int a_channel, void* userData);
	static int   LinesCallback(int a_channel, float* a_buffer, int a_size, void* userData);
	static float PeaksCallback_Wasapi(int a_channel, void* userData);
	static int   LinesCallback_Wasapi(int a_channel, float* a_buffer, int a_size, void* userData);

	bool CanPlay() const;
	bool CanRecord() const;

	CTitleText*		m_title;
	ProgramState	m_nState;

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

	// Sliders
	CWnd			m_SliderFrame;
	CMySlider		m_SliderTime;
	CSliderVol		m_SliderVol;

	// Window snap
	int				m_nSnapPixels;
	RECT			m_rDesktopRect;
	SIZE			m_szMoveOffset;

	CSystemTray		m_TrayIcon;

public:
	CMainFrame();
	virtual ~CMainFrame();

	void UpdateInterface();
	void OnBtnMonitoring();
	void OnBtnVas();
	BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);

protected:
	void ProcessSliderTime(UINT nSBCode, UINT nPos);
	void ProcessSliderVol(UINT nSBCode, UINT nPos);
	void OpenFile(CString strFileName);

	void UpdateStatWindow();
	void UpdateButtonState(UINT nID);
	void UpdateWindowTitle_SeekTo(double curSeconds, double allSeconds, int displayTimeMs);

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
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	void MonitoringStop();
	bool MonitoringStart();
	bool MonitoringRestart();

	void UpdateTrayText();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnOptEm();
	afx_msg void OnOptTop();
	afx_msg void OnOptMonitor();
	afx_msg void OnOptVAS();
	afx_msg void OnOptAutoGainControl();
	afx_msg void OnBtnOPEN();
	afx_msg void OnBtnPLAY();
	afx_msg void OnBtnSTOP();
	afx_msg void OnBtnREC();
	afx_msg void OnBtnMIX_SEL();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnOptCom();
	afx_msg void OnFileFindfile();
	afx_msg void OnFileClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateSoundRec(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptMonitor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptVAS(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptAutoGainControl(CCmdUI* pCmdUI);
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
	afx_msg void OnVolUpA();
	afx_msg void OnVolDownA();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg LRESULT OnAreYouMe(WPARAM, LPARAM);
	afx_msg LRESULT OnParseLine(WPARAM, LPARAM);
	afx_msg void OnUpdateSoundPlay(CCmdUI* pCmdUI);

protected:
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	LRESULT OnRecSourceDialogClosed(WPARAM wParam, LPARAM lParam);
	LRESULT OnRecSourceChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnFilterNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	static void HandleFilterNotification(
		const Filter* fromFilter, const Parameter& parameter, void* userData);

	FilterChain m_monitoringChain;
	FilterChain m_recordingChain;

	//These parameters should be mutually exclusive:
	CString m_recordingFileName; //Not empty name indicates that able to record.
	HSTREAM m_bassPlaybackHandle;

	std::vector<Parameter> m_filterNotifications;
	std::auto_ptr<CAutoGainControl> m_autoGainControl;

	float m_playback_volume;
	float m_recording_volume;
	static void VolumeChangedEvent(float curVolume, void* user);

	//This flag helps to avoid monitoring start on window destruction. We have
	//a CRecordingSourceDlg::GetInstance() call to get a selected devices. If
	//this object was created during WM_DESTROY processing, PostNcDestroy not
	//called in dialog -> not deleted -> memory leak. Monitoring is restarted
	//from OnBtnSTOP handler.
	bool m_destroyingMainWindow;
};

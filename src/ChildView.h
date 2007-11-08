/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_CHILDVIEW_H__837D6DCA_C6B3_11D7_BC33_444553540000__INCLUDED_)
#define AFX_CHILDVIEW_H__837D6DCA_C6B3_11D7_BC33_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlButton.h"
#include "MySlider.h"
#include "MySheet.h"
#include "config.h"

#include "IcoWnd.h"
#include "StatWnd.h"
#include "TimeWnd.h"
#include "GraphWnd.h"

#include "ProgManager.h"

/////////////////////////////////////////////////////////////////////////////
class CChildView : public CWnd
{
	// Visualisation windows
	CIcoWnd   m_IcoWnd;
	CTimeWnd  m_TimeWnd;
	CStatWnd  m_StatWnd;
	CGraphWnd m_GraphWnd;

	// Buttons
	CButton btnframe;
	CControlButton m_BtnNEW;
	CControlButton m_BtnREC;
	CControlButton m_BtnPLAY;
	CControlButton m_BtnOPEN;
	CControlButton m_BtnREC_MIX;
	CControlButton m_BtnPLAY_MIX; 
	
	// Time slider
	CButton sliderframe;
	CMySlider m_TimeSlider;

	// Timer variables
	int m_nTimerRecID;
	int m_nTimerPlayID;
	int m_nTimerGraphID;

	// main manager
	CProgManager p_m;

public:
	CConfig conf;	// must be bublic! (used in FrameWnd)

	CChildView();
	virtual ~CChildView();

protected:
	void InitControlButtons();
	void InitSliderTime();
	void InitVisualization();

	void ProcessSliderTime(UINT nSBCode, UINT nPos);
	void GetTimeStr(char* szTimeStr, int iTime);

	void AutoFileName(char* szName);
	void UpdateStat();
	void OnBtnSTOP();
	void OnBtnSTOP_REC();
	void OnBtnSTOP_PLAY();
	void ResetAll();

public:
	void ShowDlg(int nIndex);

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CChildView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBtnNEW();
	afx_msg void OnBtnREC();
	afx_msg void OnBtnPLAY();
	afx_msg void OnBtnOPEN();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNewA();
	afx_msg void OnFileOpenA();
	afx_msg void OnFileClose();
	afx_msg void OnFileFindTarget();
	afx_msg void OnOptcom();
	afx_msg void OnOptComA();
	afx_msg void OnSoundBeginA();
	afx_msg void OnSoundEndA();
	afx_msg void OnSoundFfA();
	afx_msg void OnSoundPlayA();
	afx_msg void OnSoundRecA();
	afx_msg void OnSoundRewA();
	afx_msg void OnSoundBegin();
	afx_msg void OnSoundEnd();
	afx_msg void OnSoundFf();
	afx_msg void OnSoundPlay();
	afx_msg void OnSoundRec();
	afx_msg void OnSoundRew();
	afx_msg void OnOptTop();
	afx_msg void OnOptTopA();
	afx_msg void OnMixRecA();
	afx_msg void OnMixPlayA();
	afx_msg void OnMixRec();
	afx_msg void OnMixPlay();
	afx_msg void OnHelpWww();
	afx_msg void OnHelpEmail();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnOptEm();
	afx_msg void OnOptEmA();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_CHILDVIEW_H__837D6DCA_C6B3_11D7_BC33_444553540000__INCLUDED_)

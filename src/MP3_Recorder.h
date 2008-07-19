////////////////////////////////////////////////////////////////////////////////
// MP3_Recorder.h : main header file for the MP3_RECORDER application
//
#if !defined(AFX_MP3_RECORDER_H__837D6DC4_C6B3_11D7_BC33_444553540000__INCLUDED_)
#define AFX_MP3_RECORDER_H__837D6DC4_C6B3_11D7_BC33_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

////////////////////////////////////////////////////////////////////////////////
class CMP3_RecorderApp : public CWinApp
{
public:
	CMP3_RecorderApp();
	CString GetProgramDir();
	bool IsVistaOS() { return m_is_vista; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMP3_RecorderApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

public:
	//{{AFX_MSG(CMP3_RecorderApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHelpWww();
	afx_msg void OnHelpEmail();
	afx_msg void OnHelpDoc();
	afx_msg void OnHelpEntercode();
	afx_msg void OnHelpHowto();
	afx_msg void OnUpdateHelpEntercode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHelpHowto(CCmdUI* pCmdUI);
	afx_msg void OnHelpOrderOnline();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool IsNeedOneInstance();
	bool IsAlreadyRunning();
	static BOOL CALLBACK searcher(HWND hWnd, LPARAM lParam);

private:
	bool m_is_vista;
public:
	virtual BOOL OnIdle(LONG lCount);
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert add. declarations before the previous line.
#endif //AFX_MP3_RECORDER_H__837D6DC4_C6B3_11D7_BC33_444553540000__INCLUDED_)

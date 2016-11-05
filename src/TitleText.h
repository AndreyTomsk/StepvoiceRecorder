/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

#define TITLE_TIMER_ID	987

class CTitleText  
{
	HWND	m_hWnd;
	CString strMainTitle;
	CString strTempTitle;
	static  CTitleText*  m_TitleObject;
	static  void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
public:
	CTitleText(HWND hWnd);
	void SetTitleText(CString& strNewTitle, UINT nMSecToShow = -1);
	void Restore();
};

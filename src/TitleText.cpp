//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TitleText.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CTitleText* CTitleText::m_TitleObject = NULL;

//////////////////////////////////////////////////////////////////////
CTitleText::CTitleText(HWND hWnd)
{	// init all variables
	m_hWnd			= hWnd;
	m_TitleObject	= this;
	strMainTitle	= "";
	strTempTitle	= "";
}

//////////////////////////////////////////////////////////////////////
void CTitleText::SetTitleText(CString& strNewTitle, UINT nMSecToShow)
{	// set main String as default
	CString* pstr = &strMainTitle;
	// kill previous timer
	KillTimer(m_hWnd, TITLE_TIMER_ID);
	if(nMSecToShow != -1)
	{	// change pointer to Temp string
		pstr = &strTempTitle;
		SetTimer(m_hWnd, TITLE_TIMER_ID, nMSecToShow, TimerProc);
	}
	// change Main (or Temt) string text
	*pstr = strNewTitle;
	// show new window title
	SendMessage(m_hWnd, WM_SETTEXT, 0, (long)pstr->GetBuffer(pstr->GetLength()));
}

//////////////////////////////////////////////////////////////////////
void CTitleText::Restore()
{	// restore main title
	TimerProc(m_hWnd, WM_TIMER, TITLE_TIMER_ID, 0);
}

//////////////////////////////////////////////////////////////////////
void CALLBACK CTitleText::TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
	DWORD dwTime)
{	//
	if(idEvent == TITLE_TIMER_ID)
	{	// restore main title
		KillTimer(hWnd, idEvent);
		CString& str = m_TitleObject->strMainTitle;
		SendMessage(hWnd, WM_SETTEXT, 0, (long)str.GetBuffer(str.GetLength()));
	}
}

//////////////////////////////////////////////////////////////////////

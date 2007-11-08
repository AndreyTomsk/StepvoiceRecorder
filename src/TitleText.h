//////////////////////////////////////////////////////////////////////
#if !defined(AFX_TITLETEXT_H__CE95971C_A534_4FC1_A777_C14CA15535E8__INCLUDED_)
#define AFX_TITLETEXT_H__CE95971C_A534_4FC1_A777_C14CA15535E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#define TITLE_TIMER_ID	987

//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_TITLETEXT_H__CE95971C_A534_4FC1_A777_C14CA15535E8__INCLUDED_)

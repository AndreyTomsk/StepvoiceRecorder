// SliderVol.cpp : implementation file
//

#include "stdafx.h"
#include "mp3_recorder.h"
#include "SliderVol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSliderVol

CSliderVol::CSliderVol()
{
}

CSliderVol::~CSliderVol()
{
}


BEGIN_MESSAGE_MAP(CSliderVol, CSliderCtrl)
	//{{AFX_MSG_MAP(CSliderVol)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSliderVol message handlers

void CSliderVol::OnSetFocus(CWnd* pOldWnd) 
{
	CSliderCtrl::OnSetFocus(pOldWnd);
	
	// to restore buttons
	GetParent()->SetFocus();
	//GetParent()->PostMessage(WM_HSCROLL, MAKEWPARAM(SB_ENDSCROLL, 0),
	//	(LPARAM)GetSafeHwnd());
}

/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "ControlButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CControlButton, CButton)
	//{{AFX_MSG_MAP(CControlButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CControlButton::CControlButton() : m_nID(0)
{}

CControlButton::~CControlButton()
{}

//===========================================================================
BOOL CControlButton::Create( const RECT &rect, CWnd* pParentWnd, UINT nID )
{
	m_nID = nID;

	return CButton::Create(
		NULL,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON,
		rect,
		pParentWnd,
		m_nID );
}

//===========================================================================
void CControlButton::Press()
{
	SetState( true );
	Sleep( 100 );
	SetState( false );
}


//===========================================================================
HICON CControlButton::SetIcon( int nResourceID )
{
	return CButton::SetIcon(
		LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE(nResourceID) )
		);
}

//===========================================================================
BOOL CControlButton::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	//Avoid focus on button, restore it on parent window.
	if (message == WM_COMMAND && HIWORD(wParam) == BN_CLICKED)
		GetParent()->SetFocus();

	return CButton::OnChildNotify(message, wParam, lParam, pLResult);
}

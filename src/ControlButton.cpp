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

/////////////////////////////////////////////////////////////////////////////
/*void CControlButton::OnClicked() 
{
	if(GetButtonStyle() & WS_DISABLED) return; // exit if disabled
	//
	GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(m_nID, BN_CLICKED));
	GetParent()->SetFocus();
}
*/

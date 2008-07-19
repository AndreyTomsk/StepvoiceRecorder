// UrlWnd.cpp : implementation file
//

#include "stdafx.h"
#include "mp3_recorder.h"
#include "UrlWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CUrlWnd, CStatic)
	//{{AFX_MSG_MAP(CUrlWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CUrlWnd::CUrlWnd()
{
}

CUrlWnd::~CUrlWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
/*
void CUrlWnd::Init()
{
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor (AfxGetInstanceHandle (), MAKEINTRESOURCE (IDC_HAND)));
	GetWindowText(m_strUrl);
}
*/

void CUrlWnd::SetUrl(CString pszUrl)
{
	m_strUrl = pszUrl;
}

int CUrlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//Init();
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)LoadCursor (AfxGetInstanceHandle (),
		MAKEINTRESOURCE(IDC_HAND)));
	
	return 0;
}


void CUrlWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ShellExecute(::GetDesktopWindow(), "open", m_strUrl, NULL, NULL, SW_SHOW);
	CStatic::OnLButtonDown(nFlags, point);
}

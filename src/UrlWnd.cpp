#include "stdafx.h"
#include "UrlWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CUrlWnd, CStatic)
	ON_WM_CTLCOLOR_REFLECT() //just took from internet
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CUrlWnd::CUrlWnd() : m_isUnderline(false)
{
}
//---------------------------------------------------------------------------

CUrlWnd::~CUrlWnd()
{
	if ((HFONT)m_underlineFont)
		m_underlineFont.DeleteObject();
}
//---------------------------------------------------------------------------

void CUrlWnd::SetUrl(CString pszUrl)
{
	m_strUrl = pszUrl;
}
//---------------------------------------------------------------------------

BOOL CUrlWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	::SetCursor(::LoadCursor(NULL, IDC_HAND));
	return TRUE;
}
//---------------------------------------------------------------------------

HBRUSH CUrlWnd::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetTextColor(RGB(0, 0, 230));
	pDC->SetBkMode(TRANSPARENT);

	if (m_isUnderline && !(HFONT)m_underlineFont)
	{
		LOGFONT lf;
		GetFont()->GetLogFont(&lf);
		lf.lfUnderline = TRUE;
		m_underlineFont.CreateFontIndirect(&lf);
		pDC->SelectObject(&m_underlineFont);
	}

	return (HBRUSH)GetStockObject(NULL_BRUSH);
}
//---------------------------------------------------------------------------

void CUrlWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ShellExecute(::GetDesktopWindow(), "open", m_strUrl, NULL, NULL, SW_SHOW);
	CStatic::OnLButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------

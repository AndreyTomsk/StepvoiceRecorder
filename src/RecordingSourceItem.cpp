#include "stdafx.h"
#include "MP3_Recorder.h"
#include "RecordingSourceItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRecordingSourceItem, CWnd)
	//{{AFX_MSG_MAP(CRecordingSourceItem)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_RECORDING_DEVICE, OnCheckboxClicked)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CRecordingSourceItem::CRecordingSourceItem(const CString& caption)
:m_caption(caption)
,m_mouseOverWindow(false)
,m_mouseOverCheckbox(false)
{
}
//---------------------------------------------------------------------------

CRecordingSourceItem::~CRecordingSourceItem()
{
}
//---------------------------------------------------------------------------

int CRecordingSourceItem::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Creating child windows

	CSize wndSize(lpCreateStruct->cx, lpCreateStruct->cy);
	const CRect rCheckbox = CRect(8, 1, 30, wndSize.cy-1);
	const CRect rLevel    = CRect(wndSize.cx-51, 1, wndSize.cx-1, wndSize.cy-1);
	const CRect rLabel    = CRect(rCheckbox.right, 1, rLevel.left, wndSize.cy-1);

	m_itemCheckBox.Create(NULL, WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, rCheckbox, this, IDC_RECORDING_DEVICE);
	m_itemLabel.Create(m_caption, WS_CHILD|WS_VISIBLE|SS_WORDELLIPSIS|SS_CENTERIMAGE, rLabel, this);
	m_itemLevel.Create(_T("0%"), WS_CHILD|WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE, rLevel, this);
	
	// Updating font
	// Such complex code thanks to article: "SystemParametersInfo with SPI_GETNONCLIENTMETRICS may fail"
	// http://chabster.blogspot.ru/2010/01/systemparametersinfo-with.html

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);

	#if (WINVER >= 0x0600)
		OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO), 0 };
		VERIFY(GetVersionEx(&osvi));
		if (osvi.dwMajorVersion < 6) { ncm.cbSize -= sizeof(int); }
	#endif
	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
	VERIFY(m_menuFont.CreateFontIndirect(&ncm.lfMenuFont));
	m_itemLabel.SetFont(&m_menuFont);
	m_itemLevel.SetFont(&m_menuFont);

	return 0;
}
//---------------------------------------------------------------------------

static CRect MyGetClientRect(const CWnd* window)
{
	CRect r;
	window->GetClientRect(&r);
	return r;
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::OnPaint() 
{
	CPaintDC dc(this);

	static const COLORREF dialogColor = GetSysColor(COLOR_BTNFACE);
	static const COLORREF menuFrameColor = RGB(120,174,229);
	static const CRect rClient = MyGetClientRect(this); //same for all items

	if (m_mouseOverWindow || m_mouseOverCheckbox)
	{
		CBrush dialogBruch(dialogColor);
		CBrush* pOldBrush = dc.SelectObject(&dialogBruch);
		CPen borderPen(PS_SOLID, 1, menuFrameColor);
		CPen* pOldPen = dc.SelectObject(&borderPen);

		dc.Rectangle(&rClient);

		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}
	else
	{
		dc.FillSolidRect(&rClient, dialogColor);
	}

	CWnd::OnPaint();
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceItem::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
	{
		pMsg->message = WM_LBUTTONDOWN;
	}
	return CWnd::PreTranslateMessage(pMsg);
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCheck(!GetCheck());
	CWnd::OnLButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------

bool CRecordingSourceItem::GetCheck() const
{
	return m_itemCheckBox.GetCheck() == BST_CHECKED;
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::SetCheck(bool check)
{
	m_itemCheckBox.SetCheck(check ? BST_CHECKED : BST_UNCHECKED);
	OnCheckboxClicked();
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::SetLevel(unsigned levelPercent)
{
	CString newLevel;
	newLevel.Format(_T("%02d%%"), levelPercent);
	m_itemLevel.SetWindowTextA(newLevel);
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::OnCheckboxClicked()
{
	GetParent()->PostMessageA(WM_COMMAND,
		MAKEWPARAM(IDC_RECORDING_DEVICE, BN_CLICKED), 0);
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::OnMouseMove(UINT /*nFlags*/, CPoint /*point*/)
{
	if (!m_mouseOverWindow || m_mouseOverCheckbox)
	{
		if (!m_mouseOverCheckbox)
			Invalidate();

		m_mouseOverWindow = true;
		m_mouseOverCheckbox = false;

		//Tracking next "mouse leave" event.

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = HOVER_DEFAULT;
		tme.hwndTrack = this->GetSafeHwnd();
		VERIFY(TrackMouseEvent(&tme));
	}
}
//---------------------------------------------------------------------------

LRESULT CRecordingSourceItem::OnMouseLeave(WPARAM, LPARAM)
{
	m_mouseOverWindow = false;

	//Check if mouse position is over the checkbox.

	CPoint cursorPos;
	GetCursorPos(&cursorPos);
	CRect rCheckbox;
	m_itemCheckBox.GetWindowRect(&rCheckbox);
	m_mouseOverCheckbox = (rCheckbox.PtInRect(cursorPos) == TRUE);

	if (!m_mouseOverCheckbox)
		Invalidate();

	return 0;
}
//---------------------------------------------------------------------------

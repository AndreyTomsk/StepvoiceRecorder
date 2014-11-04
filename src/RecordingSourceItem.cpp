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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CRecordingSourceItem::CRecordingSourceItem(const CString& caption)
:m_caption(caption)
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
	const CRect rCheckbox = CRect(5, 0, 25, wndSize.cy);
	const CRect rLevel    = CRect(wndSize.cx-50, 0, wndSize.cx, wndSize.cy);	
	const CRect rLabel    = CRect(rCheckbox.right+1, 0, rLevel.left-1, wndSize.cy);

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
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	VERIFY(m_menuFont.CreateFontIndirect(&ncm.lfMenuFont));
	m_itemLabel.SetFont(&m_menuFont);
	m_itemLevel.SetFont(&m_menuFont);

	return 0;
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::OnPaint() 
{
	CWnd::OnPaint();
	/*
	CMyLock lock(m_sync_object);

	CPaintDC dc(this); // device context for painting
	dc.BitBlt(0, 0, m_wndsize.cx, m_wndsize.cy, &m_memDC, 0, 0, SRCCOPY);
	*/
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
	//Toggle checkbox state
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


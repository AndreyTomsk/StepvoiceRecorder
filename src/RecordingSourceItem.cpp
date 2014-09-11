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
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CRecordingSourceItem::CRecordingSourceItem()
	:m_timer_id(0)
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

	CSize wndSize(lpCreateStruct->cx, lpCreateStruct->cy);
	const CRect rCheckbox = CRect(5, 0, 25, wndSize.cy);
	const CRect rLevel    = CRect(wndSize.cx-50, 0, wndSize.cx, wndSize.cy);	
	const CRect rLabel    = CRect(rCheckbox.right+1, 0, rLevel.left-1, wndSize.cy);

	m_itemCheckBox.Create(NULL, WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, rCheckbox, this, IDC_RECORDING_DEVICE);
	m_itemLabel.Create(_T("Sample label"), WS_CHILD|WS_VISIBLE|SS_WORDELLIPSIS|SS_SUNKEN, rLabel, this);
	m_itemLevel.Create(_T("0%"), WS_CHILD|WS_VISIBLE|SS_CENTER|SS_SUNKEN, rLevel, this);

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
	const bool isChecked = m_itemCheckBox.GetCheck() == BST_CHECKED;
	m_itemCheckBox.SetCheck(isChecked ? BST_UNCHECKED : BST_CHECKED);

	CWnd::OnLButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::OnTimer(UINT nIDEvent) 
{
	/*
	if (nIDEvent == MAXPEAK_TIMER_ID)
	{
		ResetMaxpeakMarks();
	}
	*/
	CWnd::OnTimer(nIDEvent);
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::SetCheck(bool check)
{
	m_itemCheckBox.SetCheck(check ? BST_CHECKED : BST_UNCHECKED);
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::SetLabel(const CString& newLabel)
{
	m_itemLabel.SetWindowTextA(newLabel);
}
//---------------------------------------------------------------------------

void CRecordingSourceItem::SetLevel(unsigned levelPercent)
{
	CString newLevel;
	newLevel.Format(_T("%02d%%"), levelPercent);
	m_itemLevel.SetWindowTextA(newLevel);
}
//---------------------------------------------------------------------------

bool CRecordingSourceItem::GetCheck() const
{
	return m_itemCheckBox.GetCheck() == BST_CHECKED;
}
//---------------------------------------------------------------------------


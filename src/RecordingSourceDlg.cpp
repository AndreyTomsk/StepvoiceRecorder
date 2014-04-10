/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "mp3_recorder.h"
#include "RecordingSourceDlg.h"
#include <basswasapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRecordingSourceDlg, CDialog)
	//{{AFX_MSG_MAP(CRecordingSourceDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CRecordingSourceDlg* g_dialogInstance = NULL;

/////////////////////////////////////////////////////////////////////////////

CRecordingSourceDlg::CRecordingSourceDlg(CWnd* pParent /*=NULL*/)
	:CDialog(CRecordingSourceDlg::IDD, pParent)
{
  //{{AFX_DATA_INIT(CRecordingSourceDlg)
  //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------

/*static*/ void CRecordingSourceDlg::Execute(CWnd* parent)
{
	if (!g_dialogInstance)
	{
		g_dialogInstance = new CRecordingSourceDlg(parent);
		g_dialogInstance->Create(CRecordingSourceDlg::IDD);
	}
	g_dialogInstance->ShowWindow(SW_SHOW);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::PostNcDestroy()
{
	g_dialogInstance = NULL;
	CDialog::PostNcDestroy();
	delete this;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecordingSourceDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST3, m_checkList);
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitWasapiDevicesList(m_checkList);

	return TRUE;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::InitWasapiDevicesList(CCheckListBox& devicesList)
{
	devicesList.ResetContent();

	BASS_WASAPI_DEVICEINFO info;
	for (int id = 0; BASS_WASAPI_GetDeviceInfo(id, &info); id++)
	{
		const bool isEnabled = info.flags & BASS_DEVICE_ENABLED;
		const bool isDefault = info.flags & BASS_DEVICE_DEFAULT;
		const bool isInputDevice = info.flags & BASS_DEVICE_INPUT;
		const bool isLoopback = info.flags & BASS_DEVICE_LOOPBACK;

		if (isEnabled && (isInputDevice || isLoopback))
		{
			devicesList.AddString(info.name);
			const int lastIndex = devicesList.GetCount()-1;

			devicesList.SetItemData(lastIndex, id);
			if (isDefault)
				devicesList.SetCheck(lastIndex, BST_CHECKED);
		}
	}
}
//---------------------------------------------------------------------------

HBRUSH CRecordingSourceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//Hiding dialog when changing focus to other windows (similar to menus)
	if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE)
	{
		this->ShowWindow(SW_HIDE);
		this->GetParent()->PostMessage(WM_RECSOURCE_DLGCLOSED);
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}
//---------------------------------------------------------------------------

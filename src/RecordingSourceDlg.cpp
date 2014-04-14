/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "mp3_recorder.h"
#include "RecordingSourceDlg.h"
#include <algorithm> //for std::find
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
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
	ON_LBN_SELCHANGE(IDC_DEVICES_LIST, &CRecordingSourceDlg::OnDevicesListSelChange)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CRecordingSourceDlg* g_dialogInstance = NULL;

//---------------------------------------------------------------------------

CRecordingSourceDlg* CRecordingSourceDlg::GetInstance()
{
	if (!g_dialogInstance)
	{
		g_dialogInstance = new CRecordingSourceDlg(NULL); //deleted by dialog
		g_dialogInstance->Create(CRecordingSourceDlg::IDD);
	}
	return g_dialogInstance;
}

/////////////////////////////////////////////////////////////////////////////

CRecordingSourceDlg::CRecordingSourceDlg(CWnd* pParent /*=NULL*/)
	:CDialog(CRecordingSourceDlg::IDD, pParent)
{
	m_selectedDevices.push_back(Bass::GetDefaultRecordingDevice());

	//{{AFX_DATA_INIT(CRecordingSourceDlg)
	//}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::Execute(CPoint& pos)
{
	m_allDevices = Bass::GetWasapiDevicesList();
	UpdateDevicesListBox(m_allDevices, m_checkList);

	SetWindowPos(NULL, pos.x, pos.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	ShowWindow(SW_SHOW);
	SetTimer(IDD_RECORDING_SOURCE, 200, NULL);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	//Hiding dialog when changing focus to other windows (similar to menus)
	if (nState == WA_INACTIVE && IsWindowVisible())
	{
		KillTimer(IDD_RECORDING_SOURCE);
		this->ShowWindow(SW_HIDE);
		this->GetParent()->PostMessage(WM_RECSOURCE_DLGCLOSED);
	}
}
//---------------------------------------------------------------------------

Bass::DevicesArray CRecordingSourceDlg::GetSelectedDevices() const
{
	return m_selectedDevices;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::SelectDevices(const Bass::DevicesArray& src)
{
	m_selectedDevices = src;
	//And devices list box update on next display.
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
	DDX_Control(pDX, IDC_DEVICES_LIST, m_checkList);
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::UpdateDevicesListBox(
	const Bass::DevicesArray& devices, CCheckListBox& listBox)
{
	listBox.ResetContent();

	Bass::DevicesArray::const_iterator it = devices.begin();
	while (it != devices.end())
	{
		Bass::DeviceIdNamePair p = *it++;
		const CString deviceName = p.second;
		const DWORD deviceID = p.first;
		const float deviceLevel = BASS_WASAPI_GetDeviceLevel(deviceID, -1); //all channels

		//Adding device volume level to items
		CString itemName;
		itemName.Format(deviceName + _T("\t   (%d%%)"), int(deviceLevel*100));
		listBox.AddString(itemName);

		const int lastIndex = listBox.GetCount()-1;
		listBox.SetItemData(lastIndex, deviceID);

		//Checking list item
		const Bass::DevicesArray& sd = m_selectedDevices;
		if (std::find(sd.begin(), sd.end(), p) != sd.end())
			listBox.SetCheck(lastIndex, BST_CHECKED);
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
	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnDevicesListSelChange()
{
	//::OutputDebugString(__FUNCTION__"\n");

	//Updating internal array of selected devices and notifying parent.
	m_selectedDevices.clear();
	for (int i = 0; i < m_checkList.GetCount(); i++)
	{
		if (m_checkList.GetCheck(i) == BST_CHECKED)
		{
			DWORD id = m_checkList.GetItemData(i);
			CString deviceName;
			m_checkList.GetText(i, deviceName);
			m_selectedDevices.push_back(Bass::DeviceIdNamePair(id, deviceName));
		}
	}
	if (m_selectedDevices.empty())
		m_selectedDevices.push_back(Bass::GetDefaultRecordingDevice());

	this->GetParent()->PostMessage(WM_RECSOURCE_CHANGED);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateDevicesListBox(m_allDevices, m_checkList);
	CDialog::OnTimer(nIDEvent);
}

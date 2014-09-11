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
	ON_BN_CLICKED(IDC_RECORDING_DEVICE, OnRecodingSourceItemClicked)
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
	m_selectedDevices.push_back(WasapiHelpers::GetDefaultRecordingDevice());

	//{{AFX_DATA_INIT(CRecordingSourceDlg)
	//}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::Execute(CPoint& pos)
{
	m_allDevices = WasapiHelpers::GetWasapiDevicesList();
	WasapiHelpers::InitRecordingDevices(m_allDevices);
	UpdateDevicesListBox(m_allDevices, m_checkList);

	CreateDeviceItems(m_allDevices.size());
	UpdateDeviceItems();

	SetWindowPos(NULL, pos.x, pos.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	ShowWindow(SW_SHOW);
	SetTimer(IDD_RECORDING_SOURCE, 200, NULL);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	//Hiding dialog when changing focus to other windows (similar to menus)

	CDialog::OnActivate(nState, pWndOther, bMinimized);
	if (nState == WA_INACTIVE && IsWindowVisible())
	{
		KillTimer(IDD_RECORDING_SOURCE);
		this->ShowWindow(SW_HIDE);
		this->GetParent()->PostMessage(WM_RECSOURCE_DLGCLOSED);
		WasapiHelpers::FreeRecordingDevices(m_allDevices);
	}
}
//---------------------------------------------------------------------------

WasapiHelpers::DevicesArray CRecordingSourceDlg::GetSelectedDevices() const
{
	return m_selectedDevices;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::SelectDevices(const WasapiHelpers::DevicesArray& src)
{
	m_selectedDevices = src;
	//And devices list box update on next display.
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::PostNcDestroy()
{
	DeleteDeviceItems();
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

void CRecordingSourceDlg::UpdateDevicesListBox(
	const WasapiHelpers::DevicesArray& devices, CCheckListBox& listBox)
{
	listBox.SetRedraw(FALSE);
	listBox.ResetContent();

	WasapiHelpers::DevicesArray::const_iterator it = devices.begin();
	while (it != devices.end())
	{
		WasapiHelpers::DeviceIdNamePair p = *it++;
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
		const WasapiHelpers::DevicesArray& sd = m_selectedDevices;
		if (std::find(sd.begin(), sd.end(), p) != sd.end())
			listBox.SetCheck(lastIndex, BST_CHECKED);
	}
	listBox.SetRedraw(TRUE);
	listBox.Invalidate();
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

static CString FindDeviceNameByID(const WasapiHelpers::DevicesArray& arr, DWORD id)
{
	WasapiHelpers::DevicesArray::const_iterator it = arr.begin();
	while (it != arr.end())
	{
		WasapiHelpers::DeviceIdNamePair p = *it++;
		if (p.first == id)
			return p.second;
	}
	return CString();
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
			const DWORD id = m_checkList.GetItemData(i);
			const CString deviceName = FindDeviceNameByID(m_allDevices, id);
			m_selectedDevices.push_back(WasapiHelpers::DeviceIdNamePair(id, deviceName));
		}
	}
	if (m_selectedDevices.empty())
		m_selectedDevices.push_back(WasapiHelpers::GetDefaultRecordingDevice());

	this->GetParent()->PostMessage(WM_RECSOURCE_CHANGED);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateDevicesListBox(m_allDevices, m_checkList);
	UpdateDeviceItems();
	CDialog::OnTimer(nIDEvent);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::CreateDeviceItems(unsigned count)
{
	CRect dialogRect;
	GetWindowRect(&dialogRect);
	const int itemWidth = dialogRect.Width();
	const int itemHeight = 20;

	DeleteDeviceItems();
	for (unsigned i = 0; i < count; i++)
	{
		const int top = 150 + itemHeight*i;
		const CRect itemRect(0, top, itemWidth, top+itemHeight);

		CRecordingSourceItem* newItem = new CRecordingSourceItem();
		newItem->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, itemRect, this, IDW_RECORDING_ITEM+i);
		m_recordingSourceItems.push_back(newItem);
	}
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::DeleteDeviceItems()
{
	std::vector<CRecordingSourceItem*>::iterator it = m_recordingSourceItems.begin();
	while (it != m_recordingSourceItems.end())
	{
		CRecordingSourceItem* item = *it++;
		item->DestroyWindow();
		delete item;
	}
	m_recordingSourceItems.clear();
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::UpdateDeviceItems()
{
	ASSERT(m_recordingSourceItems.size() == m_allDevices.size());
	for (unsigned i = 0; i < m_allDevices.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair p = m_allDevices[i];

		const CString deviceName = p.second;
		const DWORD deviceID = p.first;
		const float deviceLevel = BASS_WASAPI_GetDeviceLevel(deviceID, -1); //all channels
		const int deviceLevelPercent = deviceLevel == -1 ? 0 : unsigned(deviceLevel*100);

		m_recordingSourceItems[i]->SetLabel(deviceName);
		m_recordingSourceItems[i]->SetLevel(deviceLevelPercent);
	}
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnRecodingSourceItemClicked()
{
	OutputDebugString(__FUNCTION__);
}
//---------------------------------------------------------------------------

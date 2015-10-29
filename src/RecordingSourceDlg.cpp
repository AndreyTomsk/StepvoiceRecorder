#include "stdafx.h"
#include "RecordingSourceDlg.h"
#include "DeviceLevels.h"
#include <algorithm> //for std::find

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
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RECORDING_DEVICE, OnRecodingSourceItemClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

static CRecordingSourceDlg* g_dialogInstance = NULL;
static CDeviceLevels* g_deviceLevels = NULL;

//---------------------------------------------------------------------------

CRecordingSourceDlg* CRecordingSourceDlg::GetInstance()
{
	if (!g_dialogInstance)
	{
		g_dialogInstance = new CRecordingSourceDlg(NULL); //deleted by dialog (see PostNcDestroy)
		g_dialogInstance->Create(CRecordingSourceDlg::IDD);
	}
	g_dialogInstance->UpdateDeviceArrays();
	return g_dialogInstance;
}

/////////////////////////////////////////////////////////////////////////////

CRecordingSourceDlg::CRecordingSourceDlg(CWnd* pParent /*=NULL*/)
	:CDialog(CRecordingSourceDlg::IDD, pParent)
{
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::UpdateDeviceArrays()
{
	//Re-initializing all devices list in case of disconnecting/installing
	//new recording devices to system. Selected devices are stored by names
	//in system registry (updated on dialog close), we should also update
	//its IDs in array (in case of devices change in system).
	m_allDevices = WasapiHelpers::GetRecordingDevicesList();
	LoadSelectedDevicesFromConfig();
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::Execute(CPoint& pos)
{
	if (m_allDevices.empty())
	{
		AfxMessageBox(_T("Recording devices not found."));
		return;
	}

	SAFE_DELETE(g_deviceLevels);
	g_deviceLevels = new CDeviceLevels(m_allDevices);

	CreateRecordingSourceItems(m_allDevices);
	UpdateRecordingSourceItems();

	// Adjusting dialog's size and position on screen.

	CRect dialogRect;
	GetWindowRect(&dialogRect);

	const int newDialogWidth = dialogRect.Width();
	const int newDialogHeight = m_allDevices.size() * GetDefaultItemHeight() + 6;
	dialogRect = CRect(pos, CSize(newDialogWidth, newDialogHeight));
	AdjustDialogCoordinates(dialogRect);

	// Displaying.

	const CRect& r = dialogRect;
	SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER);
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

		SAFE_DELETE(g_deviceLevels);
		SaveSelectedDevices();
	}
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::AdjustDialogCoordinates(CRect& dialogRect) const
{
	//Making dialog rectangle fully visible on screen.

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	HMONITOR hMonitor = MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTONEAREST);
	GetMonitorInfo(hMonitor, &mi);

	const int rightBorderOffset = dialogRect.right - mi.rcMonitor.right;
	const int bottomBorderOffset = dialogRect.bottom - mi.rcMonitor.bottom;

	if (rightBorderOffset > 0)
	{
		dialogRect.left -= rightBorderOffset;
		dialogRect.right -= rightBorderOffset;
	}
	if (bottomBorderOffset > 0)
	{
		dialogRect.top -= bottomBorderOffset;
		dialogRect.bottom -= bottomBorderOffset;
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
}
//---------------------------------------------------------------------------

BOOL CRecordingSourceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Adding shadow to window.
	const DWORD wndClassStyle = GetClassLong(this->GetSafeHwnd(), GCL_STYLE);
	SetClassLong(this->GetSafeHwnd(), GCL_STYLE, wndClassStyle | CS_DROPSHADOW);

	return TRUE;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::PostNcDestroy()
{
	DeleteRecordingSourceItems();
	g_dialogInstance = NULL;

	CDialog::PostNcDestroy();
	delete this;
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

void CRecordingSourceDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateRecordingSourceItems();
	CDialog::OnTimer(nIDEvent);
}
//---------------------------------------------------------------------------

int CRecordingSourceDlg::GetDefaultItemHeight() const
{
	return 26;
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::CreateRecordingSourceItems(const WasapiHelpers::DevicesArray& src)
{
	CRect dialogRect;
	GetWindowRect(&dialogRect);
	const int itemWidth = dialogRect.Width();
	const int itemHeight = GetDefaultItemHeight();
	const int margin = 2;

	DeleteRecordingSourceItems();
	for (unsigned i = 0; i < src.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair p = m_allDevices[i];
		const CString deviceName = p.second;

		const int top = itemHeight*i;
		const CRect itemRect(0+margin, top+margin, itemWidth-2*margin, top+margin+itemHeight);

		CRecordingSourceItem* newItem = new CRecordingSourceItem(deviceName);
		newItem->Create(NULL, NULL, WS_CHILD|WS_VISIBLE, itemRect, this, IDW_RECORDING_ITEM+i);
		m_recordingSourceItems.push_back(newItem);
	}
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::DeleteRecordingSourceItems()
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

void CRecordingSourceDlg::UpdateRecordingSourceItems()
{
	ASSERT(m_recordingSourceItems.size() == m_allDevices.size());
	const WasapiHelpers::DevicesArray& sd = m_selectedDevices;

	for (unsigned i = 0; i < m_allDevices.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair p = m_allDevices[i];
		const DWORD deviceID = p.first;

		const float deviceLevel = g_deviceLevels->GetLevel(deviceID);
		const int deviceLevelPercent = deviceLevel == -1 ? 0 : unsigned(deviceLevel*100);

		if (std::find(sd.begin(), sd.end(), p) != sd.end())
			m_recordingSourceItems[i]->SetCheck(true);
		m_recordingSourceItems[i]->SetLevel(deviceLevelPercent);
	}
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::OnRecodingSourceItemClicked()
{
	//Filling selected devices list and notifying parent.

	ASSERT(m_recordingSourceItems.size() == m_allDevices.size());
	m_selectedDevices.clear();
	for (unsigned i = 0; i < m_allDevices.size(); i++)
	{
		const WasapiHelpers::DeviceIdNamePair p = m_allDevices[i];
		const CString name = p.second;
		const DWORD id = p.first;
		if (m_recordingSourceItems[i]->GetCheck())
			m_selectedDevices.push_back(WasapiHelpers::DeviceIdNamePair(id, name));
	}
	if (m_selectedDevices.empty())
		m_selectedDevices.push_back(WasapiHelpers::GetDefaultRecordingDevice());

	this->GetParent()->PostMessage(WM_RECSOURCE_CHANGED);
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::LoadSelectedDevicesFromConfig()
{
	const CString deviceNames = RegistryConfig::GetOption(_T("General\\SelectedDevices"), CString());
	m_selectedDevices = WasapiHelpers::DevicesFromString(deviceNames, m_allDevices);
	if (m_selectedDevices.empty())
		m_selectedDevices.push_back(WasapiHelpers::GetDefaultRecordingDevice());
}
//---------------------------------------------------------------------------

void CRecordingSourceDlg::SaveSelectedDevices()
{
	const CString deviceNames = WasapiHelpers::DevicesToString(m_selectedDevices);
	RegistryConfig::SetOption(_T("General\\SelectedDevices"), deviceNames);
}
//---------------------------------------------------------------------------

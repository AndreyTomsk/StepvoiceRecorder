/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "PageCom.h"
#include <bass.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CPageCom, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
CPageCom::CPageCom()
	:CPropertyPage(CPageCom::IDD)
	,m_playbackDeviceID(0)
	,m_recordingDeviceID(0)
	,m_default_play_id(0)
	,m_default_rec_id(0)
{
	//{{AFX_DATA_INIT(CPageCom)
	m_Loader	= FALSE;
	m_AutoRun	= FALSE;
	m_Minimized	= FALSE;
	m_ToolTipEnable = FALSE;
	m_TrayIcon = FALSE;
	m_TrayMin = FALSE;
	m_MInstances = FALSE;
	//}}AFX_DATA_INIT

	pconfig = NULL;
}
 
CPageCom::~CPageCom()
{
}

/////////////////////////////////////////////////////////////////////////////
void CPageCom::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageCom)
	//DDX_Radio(pDX, IDR_OPT_BLANK,	m_Loader);
	//DDX_Check(pDX, IDC_AUTORUN,		m_AutoRun);
	//DDX_Check(pDX, IDC_MINIMIZED,	m_Minimized);
	//DDX_Check(pDX, IDC_TOOLTIP_ENABLE, m_ToolTipEnable);
	DDX_Check(pDX, IDC_TRAYICON, m_TrayIcon);
	DDX_Check(pDX, IDC_TRAYMIN, m_TrayMin);
	DDX_Check(pDX, IDC_GEN_MINSTANCES, m_MInstances);
	DDX_CBIndex(pDX, IDC_PLAYBACK_DEVICE, m_playbackDeviceID);
	DDX_CBIndex(pDX, IDC_RECORDING_DEVICE, m_recordingDeviceID);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
void CPageCom::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pconfig)
	{
		pconfig->nLoader	= m_Loader;
		pconfig->bAutoRun	= m_AutoRun;
		pconfig->bMinimized	= m_Minimized;
		pconfig->bToolTips	= m_ToolTipEnable;
		//pconfig->nShowIn	= m_ShowIn;
		pconfig->bTrayIcon	= m_TrayIcon;
		pconfig->bTrayMin	= m_TrayMin;
		pconfig->bMInstances = m_MInstances;

		// Combobox ID -> device ID conversion.

		m_playbackDeviceID++; // BASS playback device IDs starts from 1
		pconfig->nPlayDevice = (m_playbackDeviceID != m_default_play_id) ?
			m_playbackDeviceID : -1;
		pconfig->nRecDevice  = (m_recordingDeviceID != m_default_rec_id) ?
			m_recordingDeviceID : -1;
	}
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPageCom, CPropertyPage)
	//{{AFX_MSG_MAP(CPageCom)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CPageCom::SetConfig(CONF_DIAL_GEN* pconf_dial_gen)
{
	pconfig			= pconf_dial_gen;

	m_Loader		= pconfig->nLoader;
	m_AutoRun		= pconfig->bAutoRun;
	m_Minimized		= pconfig->bMinimized;
	m_ToolTipEnable	= pconfig->bToolTips;
	//m_ShowIn		= pconfig->nShowIn;
	m_TrayIcon		= pconfig->bTrayIcon;
	m_TrayMin		= pconfig->bTrayMin;
	m_MInstances	= pconfig->bMInstances;

	m_playbackDeviceID  = pconfig->nPlayDevice; // default device is -1
	m_recordingDeviceID = pconfig->nRecDevice; // default device is -1
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPageCom::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CONTEXTHELP;
	return CPropertyPage::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPageCom::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	GetParent()->SendMessage(WM_COMMAND, IDHELP, (LPARAM)GetSafeHwnd());
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

BOOL CPageCom::OnInitDialog()
{
	CComboBox* cPlaybackDevice = static_cast<CComboBox*>(GetDlgItem(IDC_PLAYBACK_DEVICE));
	CComboBox* cRecordingDevice = static_cast<CComboBox*>(GetDlgItem(IDC_RECORDING_DEVICE));

	BASS_DEVICEINFO info;

	for (int i = 1; BASS_GetDeviceInfo(i, &info); i++) // 0 - "No sound" device
	{
		if (info.flags&BASS_DEVICE_ENABLED) // device is enabled
			cPlaybackDevice->AddString(info.name);
		if (info.flags&BASS_DEVICE_DEFAULT)
			m_default_play_id = i;
	}
	for (int i = 0; BASS_RecordGetDeviceInfo(i, &info); i++)
	{
		if (info.flags&BASS_DEVICE_ENABLED) // device is enabled
			cRecordingDevice->AddString(info.name);
		if (info.flags&BASS_DEVICE_DEFAULT)
			m_default_rec_id = i;
	}

	// Device ID -> combobox ID conversion.

	if (m_playbackDeviceID == -1) // default
		m_playbackDeviceID = m_default_play_id;
	if (m_playbackDeviceID > 0)
		m_playbackDeviceID -= 1;  // combo IDs starts from 0

	if (m_recordingDeviceID == -1) // default
		m_recordingDeviceID = m_default_rec_id;

	// This functions internally calls also DoDataExchange, so prepared
	// a selected item IDSs above.
	CPropertyPage::OnInitDialog();
	return TRUE;
}

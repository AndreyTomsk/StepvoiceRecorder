/////////////////////////////////////////////////////////////////////////////
#include <mmsystem.h>
#include "stdafx.h"
#include "MP3_Recorder.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
char szWindowTitle[255];

/////////////////////////////////////////////////////////////////////////////
#define _VOL_MESSAGE  "Volume: %d%%"
#define _TIME_MESSAGE "Seek to: %s/%s (%d%%)"

#define _WINDOW_TITLE "%s - Stepvoice Recorder"

/////////////////////////////////////////////////////////////////////////////
#define TIMER_SLIDER	1
#define TIMER_FILE_SIZE	2
#define TIMER_TIME_PLAY	3
#define TIMER_TIME_REC	4

#define TR_SHOW_GRAPH 5

/////////////////////////////////////////////////////////////////////////////
// Constructor/destructor
/////////////////////////////////////////////////////////////////////////////
CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDB_BTNNEW,	OnBtnNEW)
	ON_BN_CLICKED(IDB_BTNREC,	OnBtnREC)
	ON_BN_CLICKED(IDB_BTNPLAY,	OnBtnPLAY)
	ON_BN_CLICKED(IDB_BTNOPEN,	OnBtnOPEN)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_COMMAND(IDM_FILE_NEW,	OnFileNew)
	ON_COMMAND(IDM_FILE_OPEN,	OnFileOpen)
	ON_COMMAND(IDA_FILE_NEW,	OnFileNewA)
	ON_COMMAND(IDA_FILE_OPEN,	OnFileOpenA)
	ON_COMMAND(IDM_FILE_CLOSE,  OnFileClose)
	ON_COMMAND(IDM_FILE_FINDTARGET,	OnFileFindTarget)
	ON_COMMAND(IDM_OPT_COM,		OnOptcom)
	ON_COMMAND(IDA_OPT_COM,		OnOptComA)
	ON_COMMAND(IDA_SOUND_BEGIN,	OnSoundBeginA)
	ON_COMMAND(IDA_SOUND_END,	OnSoundEndA)
	ON_COMMAND(IDA_SOUND_FF,	OnSoundFfA)
	ON_COMMAND(IDA_SOUND_PLAY,	OnSoundPlayA)
	ON_COMMAND(IDA_SOUND_REC,	OnSoundRecA)
	ON_COMMAND(IDA_SOUND_REW,	OnSoundRewA)
	ON_COMMAND(IDM_SOUND_BEGIN,	OnSoundBegin)
	ON_COMMAND(IDM_SOUND_END,	OnSoundEnd)
	ON_COMMAND(IDM_SOUND_FF,	OnSoundFf)
	ON_COMMAND(IDM_SOUND_PLAY,	OnSoundPlay)
	ON_COMMAND(IDM_SOUND_REC,	OnSoundRec)
	ON_COMMAND(IDM_SOUND_REW,	OnSoundRew)
	ON_COMMAND(IDM_OPT_TOP, OnOptTop)
	ON_COMMAND(IDA_OPT_TOP, OnOptTopA)
	ON_COMMAND(IDA_MIX_REC,  OnMixRecA)
	ON_COMMAND(IDA_MIX_PLAY, OnMixPlayA)
	ON_COMMAND(IDM_MIX_REC, OnMixRec)
	ON_COMMAND(IDM_MIX_PLAY, OnMixPlay)
	ON_COMMAND(IDM_HELP_WWW, OnHelpWww)
	ON_COMMAND(IDM_HELP_EMAIL, OnHelpEmail)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_OPT_EM, OnOptEm)
	ON_COMMAND(IDA_OPT_EM, OnOptEmA)
	ON_WM_NCLBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW),
		HBRUSH(COLOR_WINDOW),
		//(HBRUSH) ::GetStockObject(WHITE_BRUSH),
		NULL);
	
	return CWnd ::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	// init interface objects
	InitControlButtons();
	InitSliderTime();
	InitVisualization();

	// check "Always on Top" property
	if(conf.GetConfProg()->bAlwaysOnTop) {
		AfxGetMainWnd()->GetMenu()->CheckMenuItem(IDM_OPT_TOP, MF_UNCHECKED);
		OnOptTop();
	}
	// check "Easy Move" property
	if(conf.GetConfProg()->bEasyMove) {
		AfxGetMainWnd()->GetMenu()->CheckMenuItem(IDM_OPT_EM, MF_UNCHECKED);
		OnOptEm();
	}

	// check "Startup loader" property
	/*
	switch (OptSheet.m_pc.m_Loader) {
	case 0:
	case 1:
	case 2:
	}
	*/
	OnFileClose();
	return 0;
}



/////////////////////////////////////////////////////////////////////////////
// Initialize controls
/////////////////////////////////////////////////////////////////////////////


void CChildView::InitControlButtons()
{
	int	  iX = 5, iYT = 85, iYB = 108, idx = 48;
	DWORD dwBtnStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_ICON;

	btnframe.Create(NULL, WS_CHILD|WS_VISIBLE|BS_GROUPBOX, CRect(0, 72, 312, 113), this, IDC_STATIC);
/*
	m_BtnNEW.Create(NULL, dwBtnStyle,
		CRect(iX, iYT, iX+idx, iYB), this, IDB_BTNNEW);
	iX += idx + 3;
	m_BtnREC.Create(NULL, dwBtnStyle,
		CRect(iX, iYT, iX+idx, iYB), this, IDB_BTNREC);
	iX += idx + 3;
	m_BtnPLAY.Create(NULL, dwBtnStyle,
		CRect(iX, iYT, iX+idx, iYB), this, IDB_BTNPLAY);
	iX += idx + 3;
	m_BtnOPEN.Create(NULL, dwBtnStyle,
		CRect(iX, iYT, iX+idx, iYB), this, IDB_BTNOPEN);
	iX = 254; idx = 25;
	m_BtnREC_MIX.Create(NULL, dwBtnStyle,
		CRect(iX, iYT, iX+idx, iYB), this, IDB_BTNREC_MIX);
	iX += idx + 3;
	m_BtnPLAY_MIX.Create(NULL, dwBtnStyle,
		CRect(iX, iYT, iX+idx, iYB), this, IDB_BTNPLAY_MIX);

	m_BtnNEW.SetIcon(IDI_NEW);
	m_BtnREC.SetIcon(IDI_REC);
	m_BtnPLAY.SetIcon(IDI_PLAY);
	m_BtnOPEN.SetIcon(IDI_OPEN);
	m_BtnREC_MIX.SetIcon(IDI_MIC);
	m_BtnPLAY_MIX.SetIcon(IDI_DIN);
*/
}



void CChildView::InitSliderTime()
{
/*	sliderframe.Create(NULL, WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		CRect(118, 45, 312, 82), this, IDC_STATIC);

	m_TimeSlider.Create(WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_NOTICKS|TBS_BOTH|TBS_NOTHUMB,
		CRect(120, 55, 310, 78), this, IDS_TIMESLIDER);
*/
}



void CChildView::InitVisualization()
{
	m_IcoWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
		CRect(0, 0, 26, 26),	this, IDW_ICO);
	m_TimeWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
		CRect(26, 0, 115, 26),	this, IDW_TIME);
	m_GraphWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
		CRect(0, 26, 115, 77),	this, IDW_GRAPH);
	m_StatWnd.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
		CRect(115, 0, 311, 50),	this, IDW_STAT);
	
//	m_GraphWnd.CreateEx(WS_EX_TOPMOST, NULL, NULL,
//		WS_POPUP | WS_THICKFRAME,
//		CRect(0, 26, 115, 77),	this, 0, NULL);

	m_IcoWnd.ShowWindow(SW_SHOW);
	m_TimeWnd.ShowWindow(SW_SHOW);
	m_GraphWnd.ShowWindow(SW_SHOW);
	m_StatWnd.ShowWindow(SW_SHOW);
}




//////////////////////////////////////////////////////////////////////////
// Buttons processing
//////////////////////////////////////////////////////////////////////////


void CChildView::OnBtnNEW()
{
	this->SetFocus();
	OnFileNew();
}


void CChildView::OnBtnOPEN()
{
	this->SetFocus();
	OnFileOpen();
}


/////////////////////////////////////////////////////////////////////////////


void CChildView::OnBtnREC()
{
	this->SetFocus();
/*
	if (p_m->waveIn.bIsRunning) {
		OnBtnSTOP(); return;
	}
	p_m->Init(pConf->conf_dial_mp3);
	p_m->Record();
*/
}



void CChildView::OnBtnPLAY()
{
	this->SetFocus();
}


/////////////////////////////////////////////////////////////////////////////


void CChildView::OnBtnSTOP()
{
/*
	if(recorder.IsRecording())
		OnBtnSTOP_REC();
	if(player.IsPlaying())
		OnBtnSTOP_PLAY();

	// common operations for both

*/
}

void CChildView::OnBtnSTOP_REC()
{
}

void CChildView::OnBtnSTOP_PLAY()
{
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnBtnREC_MIX() {
	this->SetFocus();
	WinExec("sndvol32 -r", SW_SHOW);
}

void CChildView::OnBtnPLAY_MIX() {
	this->SetFocus();
	WinExec("sndvol32", SW_SHOW);
}




/////////////////////////////////////////////////////////////////////////////
// Menu
/////////////////////////////////////////////////////////////////////////////


void CChildView::OnFileNew() 
{
	char szDefName[20] = "";
	AutoFileName(szDefName);

	CFileDialog dialog(false, "mp3", szDefName, OFN_HIDEREADONLY |
		OFN_OVERWRITEPROMPT, "MPEG audio files (*.MP3)|*.mp3||", this);
	dialog.m_ofn.lpstrTitle = "Create file";

	if(dialog.DoModal() != IDOK) return;

	CString& strShrt = conf.GetConfProg()->strLastFileName;
	CString& strLong = conf.GetConfProg()->strLastFilePath;
	strShrt = dialog.m_ofn.lpstrFileTitle;
	strLong = dialog.m_ofn.lpstrFile;
	strLong = strLong.Left(strLong.Find(strShrt));

	p_m.sndfile.SetFile(dialog.m_ofn.lpstrFile); // set new file name !
	ResetAll();
}



void CChildView::OnFileOpen()
{
	CFileDialog dialog(true, "mp3", NULL, OFN_HIDEREADONLY |
		OFN_OVERWRITEPROMPT, "MPEG audio files (*.MP3)|*.mp3||", this);
	dialog.m_ofn.lpstrTitle = "Open file";

	if(dialog.DoModal() == IDCANCEL) return;

	CString& strShrt = conf.GetConfProg()->strLastFileName;
	CString& strLong = conf.GetConfProg()->strLastFilePath;
	strShrt = dialog.m_ofn.lpstrFileTitle;
	strLong = dialog.m_ofn.lpstrFile;
	strLong = strLong.Left(strLong.Find(strShrt));

	p_m.sndfile.SetFile(dialog.m_ofn.lpstrFile);	// set new file name !
	ResetAll();
}




/////////////////////////////////////////////////////////////////////////////


void CChildView::ResetAll()
{
	OnFileClose();	// close file

	// open file
	p_m.sndfile.OpenFile();

	// get file parameters - bitrate, frequency, stereo mode
	// update statistic window

	// change window title
	sprintf(szWindowTitle, _WINDOW_TITLE, conf.GetConfProg()->strLastFileName);
	AfxGetMainWnd()->SetWindowText(szWindowTitle);
}



void CChildView::OnFileClose()
{
	OnBtnSTOP();	// stop playing/recording

	// close file/encoder/decoder
	//p_m.enc_mp3.Close();
	//p_m.dec_mp3.Close();
	//p_m.sndfile.CloseFile();

	// change window title
	//sprintf(szWindowTitle, _WINDOW_TITLE, "<No file>");
	//AfxGetMainWnd()->SetWindowText(szWindowTitle);

	// reset windows
	//m_TimeWnd.Reset();
	//UpdateStat();
}




/////////////////////////////////////////////////////////////////////////////



void CChildView::AutoFileName(char* szName)
{
	char pszDHM[3][3] = {"  ", "  ", "  "};
	const char* pszPattern[]= {"%d", "0%d"};	
	const char* pszMonth[] = {"jan_", "feb_", "mar_", "apr_", "may_",
		"jun_", "jul_", "aug_", "sep_", "oct_", "nov_", "dec_"};

	CTime curtime = CTime::GetCurrentTime();
	struct tm *newtime = curtime.GetLocalTm();

	sprintf(pszDHM[0], pszPattern[newtime->tm_mday < 10], newtime->tm_mday);
	sprintf(pszDHM[1], pszPattern[newtime->tm_hour < 10], newtime->tm_hour);
	sprintf(pszDHM[2], pszPattern[newtime->tm_min  < 10], newtime->tm_min );

	sprintf(szName, "%s%s%s%s", pszDHM[0], pszMonth[newtime->tm_mon],
		pszDHM[1], pszDHM[2]);
}



/////////////////////////////////////////////////////////////////////////////
void CChildView::OnFileFindTarget()
{
	ShellExecute(0, "explore", conf.GetConfProg()->strLastFilePath,
		NULL, NULL, SW_SHOWNORMAL);
}



/////////////////////////////////////////////////////////////////////////////
void CChildView::OnSoundPlay() {
	OnBtnPLAY();
}
void CChildView::OnSoundRec() {
	OnBtnREC();
}

/////////////////////////////////////////////////////////////////////////////
void CChildView::OnSoundBegin() {
}
void CChildView::OnSoundEnd() {
}

void CChildView::OnSoundRew() {
}

void CChildView::OnSoundFf() {
}


/////////////////////////////////////////////////////////////////////////////


void CChildView::OnOptTop()
{
	UINT		uFlags = SWP_FRAMECHANGED|SWP_NOSIZE|SWP_NOMOVE;
	CWnd*		pMainFrame = AfxGetMainWnd();
	CMenu*		menu = pMainFrame->GetMenu();

	bool bChecked = menu->GetMenuState(IDM_OPT_TOP, MF_BYCOMMAND)==MF_CHECKED;
	menu->CheckMenuItem(IDM_OPT_TOP, (bChecked) ? MF_UNCHECKED : MF_CHECKED);

	const CWnd* pWndType = (bChecked) ? &wndNoTopMost : &wndTopMost;
	pMainFrame->SetWindowPos(pWndType, 0, 0, 0, 0, uFlags);

	conf.GetConfProg()->bAlwaysOnTop = !bChecked;
}



void CChildView::OnOptEm() 
{
	CMenu*		menu = AfxGetMainWnd()->GetMenu();
	
	bool bChecked = menu->GetMenuState(IDM_OPT_EM, MF_BYCOMMAND)==MF_CHECKED;
	menu->CheckMenuItem(IDM_OPT_EM, (bChecked) ? MF_UNCHECKED : MF_CHECKED);

	conf.GetConfProg()->bEasyMove = !bChecked;
}

void CChildView::OnOptEmA() 
{
	OnOptEm();
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnOptcom()
{
	ShowDlg(conf.GetConfProg()->nDialogIndex);
}



void CChildView::ShowDlg(int nIndex)
{
	CMySheet options("Preferences", NULL, nIndex);
	options.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	options.SetConfig(conf.GetConfDialGen());
	options.SetConfig(conf.GetConfDialMp3());
	options.DoModal();

	conf.GetConfProg()->nDialogIndex = options.m_nPageIndex;
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnMixRec() {
	OnBtnREC_MIX();	
}



void CChildView::OnMixPlay() {
	OnBtnPLAY_MIX();
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnHelpWww() 
{
	ShellExecute(0, NULL, "http://www.stepvoice.com", NULL, NULL, SW_SHOWNORMAL);
}

void CChildView::OnHelpEmail() 
{
	ShellExecute(0, NULL, "mailto:support@stepvoice.com?subject=SV%20Recorder,%20version%201.1",
		NULL, NULL, SW_SHOWNORMAL);
}




/////////////////////////////////////////////////////////////////////////////
// Accelerators
/////////////////////////////////////////////////////////////////////////////



void CChildView::OnFileNewA() {
	m_BtnNEW.Press();
	OnFileNew();
}



void CChildView::OnFileOpenA() {
	m_BtnOPEN.Press();
	OnFileOpen();
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnSoundRecA() {
	if(m_BtnREC.GetStyle() & WS_DISABLED)
		return;
	m_BtnREC.Press();
	OnBtnREC();
}


void CChildView::OnSoundPlayA() {
	if(m_BtnPLAY.GetStyle() & WS_DISABLED) {
		OnSoundRecA();	// Space - stop
		return;
	}
	m_BtnPLAY.Press();
	OnBtnPLAY();
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnSoundBeginA() {
	OnSoundBegin();
}


void CChildView::OnSoundRewA() {
	OnSoundRew();
}


void CChildView::OnSoundFfA() {
	OnSoundFf();
}


void CChildView::OnSoundEndA() {
	OnSoundEnd();
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnOptTopA() {
	OnOptTop();	
}
void CChildView::OnOptComA() {
	OnOptcom();
}


/////////////////////////////////////////////////////////////////////////////


void CChildView::OnMixRecA() {
	m_BtnREC_MIX.Press();
	OnBtnREC_MIX();
}
void CChildView::OnMixPlayA() {
	m_BtnPLAY_MIX.Press();
	OnBtnPLAY_MIX();
}



//////////////////////////////////////////////////////////////////////////
// Timers processing function
//////////////////////////////////////////////////////////////////////////


void CChildView::OnTimer(UINT nIDEvent)
{
	CWnd ::OnTimer(nIDEvent);
}


/////////////////////////////////////////////////////////////////////////////
// Processing sliders
/////////////////////////////////////////////////////////////////////////////


void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
	this->SetFocus();
	if(pScrollBar->m_hWnd == m_TimeSlider.m_hWnd)
		ProcessSliderTime(nSBCode, nPos);
}



void CChildView::ProcessSliderTime(UINT nSBCode, UINT nPos)
{
}




/////////////////////////////////////////////////////////////////////////////
// Visualisation update functions
/////////////////////////////////////////////////////////////////////////////


void CChildView::UpdateStat()
{
	char szTime[10];
	int  nBitrTbl[] = {8, 16, 24, 32, 48, 64, 98, 128, 160, 192, 256};
	int  nFreqTbl[] = {8, 11, 22, 44};

	int nBitr = nBitrTbl[conf.GetConfDialMp3()->nBitrate];
	int nFreq = nFreqTbl[conf.GetConfDialMp3()->nFreq];
	int nSter = conf.GetConfDialMp3()->nStereo;

	//m_StatWnd.Set(nFreq, nBitr, nSter);
	//GetTimeStr(szTime, 0);
//	m_StatWnd.Set(0, szTime);
}


/////////////////////////////////////////////////////////////////////////////



void CChildView::GetTimeStr(char *szTimeStr, int iTime)
{
	int iHour= iTime/3600;
	int iMin =(iTime - iHour*3600)/60;
	int iSec = iTime - iHour*3600 - iMin*60;

	sprintf(szTimeStr, "%i:%02i:%02i", iHour, iMin, iSec);
}



/////////////////////////////////////////////////////////////////////////////



void CChildView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));

	CWnd ::OnLButtonDown(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	GetParent()->SendMessage(WM_RBUTTONDOWN, 0, MAKELPARAM(point.x, point.y));
	
	CWnd ::OnRButtonDown(nFlags, point);
}



void CChildView::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd ::OnNcLButtonDown(nHitTest, point);
}

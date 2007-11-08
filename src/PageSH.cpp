//===========================================================================
//
#include "stdafx.h"
#include "mp3_recorder.h"
#include "PageSH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPageSH, CPropertyPage)

//===========================================================================
CPageSH::CPageSH() : CPropertyPage(CPageSH::IDD)
{
	// инициализируем переменные времени
	m_ctStart = m_ctStop = m_ctRecTime = CTime(2004, 1, 1, 0, 0, 0);
	//m_ctRecTime = CTimeSpan(0, 0, 0, 0);

	//{{AFX_DATA_INIT(CPageSH)
	m_nEnable = FALSE;
	//}}AFX_DATA_INIT
}

CPageSH::~CPageSH()
{
}

//===========================================================================
void CPageSH::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSH)
	DDX_Check(pDX, IDC_SH_ENABLE, m_nEnable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPageSH, CPropertyPage)
	//{{AFX_MSG_MAP(CPageSH)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_SH_TPSTOP, On_tpStopTimeChange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_SH_TPSTART, On_tpStartTimeChange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_SH_TPRECTIME, On_tpRecTimeChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//===========================================================================
void CPageSH::SetConfig(CONF_DIAL_SH *pconfig)
{
	// (!) Устанавливаем CTime переменные класса.
	struct tm tmStart, tmStop;
	m_ctStart.GetLocalTm( &tmStart );
	m_ctStop.GetLocalTm( &tmStop );

	tmStart.tm_hour = pconfig->nStartH;
	tmStart.tm_min = pconfig->nStartM;
	tmStart.tm_sec = pconfig->nStartS;

	tmStop.tm_hour = pconfig->nStopH;
	tmStop.tm_min = pconfig->nStopM;
	tmStop.tm_sec = pconfig->nStopS;

	m_ctStart = mktime( &tmStart );
	m_ctStop = mktime( &tmStop );

	m_nEnable = pconfig->bEnable; // Флаг запуска шедулера
	m_pconfig = pconfig; // Сохраняем ук-ль для возможности изм. настроек
}

//===========================================================================
BOOL CPageSH::OnInitDialog() 
{
	CTimeSpan ctsDiff;

	CDateTimeCtrl* pRecStartCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPSTART );
	CDateTimeCtrl* pRecTimeCtrl  = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPRECTIME );
	CDateTimeCtrl* pRecStopCtrl  = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPSTOP );

	CTime ctZeroTime = CTime( 2004, 1, 1,  0,  0,  0 );
	//CTime ctMinRange = CTime( 2004, 1, 1,  0,  0,  1 );
	//CTime ctMaxRange = CTime( 2004, 1, 1, 23, 59, 59 );

	// Устанавливаем свой формат вывода времени вида "ЧЧ:ММ:СС"
	DateTime_SetFormat(pRecStartCtrl->GetSafeHwnd(), "HH':'mm':'ss");
	DateTime_SetFormat(pRecTimeCtrl->GetSafeHwnd(),  "HH':'mm':'ss");
	DateTime_SetFormat(pRecStopCtrl->GetSafeHwnd(),  "HH':'mm':'ss");

	CPropertyPage::OnInitDialog();

	// устанавливаем значения в диалоге
	ctsDiff = m_ctStop - m_ctStart;
	m_ctRecTime = ctZeroTime + ctsDiff;
	UpdateTimeChange( 0 ); // обновить все элементы

	// устанавливаем диапазон изменения длительности записи
	//pRecTimeCtrl->SetRange( &ctMinRange, &ctMaxRange);

	return TRUE;
}

//===========================================================================
void CPageSH::OnOK() 
{
	// Изменяем настройки запуска шедулера.
	m_pconfig->bEnable = m_nEnable;

	// Изменяем настройки времени шедулера.
	m_pconfig->nStartH = m_ctStart.GetHour();
	m_pconfig->nStartM = m_ctStart.GetMinute();
	m_pconfig->nStartS = m_ctStart.GetSecond();

	m_pconfig->nStopH = m_ctStop.GetHour();
	m_pconfig->nStopM = m_ctStop.GetMinute();
	m_pconfig->nStopS = m_ctStop.GetSecond();

	CPropertyPage::OnOK();
}

//===========================================================================
// Обработка изменения времени в контроле "Старт".
void CPageSH::On_tpStartTimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTimeSpan ctsDiff;
	CTime ctNewStart, ctNewStop;

	CTime& ctOldStart = m_ctStart;
	CTime& ctOldStop  = m_ctStop;
	CDateTimeCtrl* pStartCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPSTART );

	pStartCtrl->GetTime( ctNewStart );
	if( ctNewStart > ctOldStart )
	{
		ctsDiff = ctNewStart - ctOldStart;
		ctNewStop = ctOldStop + ctsDiff;
	}
	else
	{
		ctsDiff = ctOldStart - ctNewStart;
		ctNewStop = ctOldStop - ctsDiff;
	}

	m_ctStart = ctNewStart;
	m_ctStop  = ctNewStop;
	UpdateTimeChange( 2 ); // обновляем Stop время

	*pResult = 0;
}

//===========================================================================
void CPageSH::On_tpStopTimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTimeSpan ctsDiff;
	CTime ctNewStop, ctNewRecTime;

	CTime& ctOldStop    = m_ctStop;
	CTime& ctOldRecTime	= m_ctRecTime;
	CDateTimeCtrl* pStopCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPSTOP );

	pStopCtrl->GetTime( ctNewStop );
	if( ctNewStop > ctOldStop)
	{
		ctsDiff = ctNewStop - ctOldStop;
		ctNewRecTime = ctOldRecTime + ctsDiff;
	}
	else
	{
		ctsDiff = ctOldStop - ctNewStop;
		ctNewRecTime = ctOldRecTime - ctsDiff;
	}

	m_ctStop    = ctNewStop;
	m_ctRecTime = ctNewRecTime;
	UpdateTimeChange( 3 ); // Обновляем RecTime время

	*pResult = 0;
}

//===========================================================================
void CPageSH::On_tpRecTimeChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTimeSpan ctsDiff;
	CTime ctNewRecTime, ctNewStop;

	CTime& ctOldRecTime = m_ctRecTime;
	CTime& ctOldStop    = m_ctStop;
	CDateTimeCtrl* pRecTimeCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPRECTIME );

	pRecTimeCtrl->GetTime( ctNewRecTime );
	if( ctNewRecTime > ctOldRecTime )
	{
		ctsDiff = ctNewRecTime - ctOldRecTime;
		ctNewStop = ctOldStop + ctsDiff;
	}
	else
	{
		ctsDiff = ctOldRecTime - ctNewRecTime;
		ctNewStop = ctOldStop - ctsDiff;
	}

	m_ctRecTime = ctNewRecTime;
	m_ctStop = ctNewStop;
	UpdateTimeChange( 2 );

	*pResult = 0;
}

//===========================================================================
void CPageSH::UpdateTimeChange(int nColumn)
{
	CDateTimeCtrl* pStartCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPSTART );
	CDateTimeCtrl* pStopCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPSTOP );
	CDateTimeCtrl* pRecTimeCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPRECTIME );
	
	switch ( nColumn )
	{
	case 1: // Start
		pStartCtrl->SetTime( &m_ctStart );
		break;
	case 2: // Stop
		pStopCtrl->SetTime( &m_ctStop );
		break;
	case 3: // RecTime
		pRecTimeCtrl->SetTime( &m_ctRecTime );
		break;
	default:// otherwise All
		pStartCtrl->SetTime( &m_ctStart );
		pStopCtrl->SetTime( &m_ctStop );
		pRecTimeCtrl->SetTime( &m_ctRecTime );
	}
}

//===========================================================================
BOOL CPageSH::OnKillActive() 
{
	// проверяем на неравенство времени Start и Stop
	if( m_ctStart == m_ctStop)
	{
		AfxMessageBox("Start and Stop times must be different.");
		CDateTimeCtrl* pStopCtrl = (CDateTimeCtrl *) GetDlgItem( IDC_SH_TPSTOP );
		pStopCtrl->SetFocus();
		return 0;
	}

	return CPropertyPage::OnKillActive();
}

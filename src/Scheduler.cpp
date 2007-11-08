// Scheduler.cpp: implementation of the CScheduler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mp3_recorder.h"
#include "Scheduler.h"
#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScheduler::CScheduler()
{
	m_bRunThread = false;
	m_pMyFrame = NULL;
	ZeroMemory( &m_StartTime, sizeof(SchedTime) );
	ZeroMemory( &m_StopTime, sizeof(SchedTime) );
}

CScheduler::~CScheduler()
{
	Stop();
}

//////////////////////////////////////////////////////////////////////
// Thread Control Functions
//////////////////////////////////////////////////////////////////////

bool CScheduler::Start()
{
	DWORD dwThreadID;
	HANDLE hThread;

	hThread = CreateThread( NULL, 0, SchedProc, this, CREATE_SUSPENDED,
		&dwThreadID );
	if( hThread == NULL )
		return false;

	m_bRunThread = true;
	SetThreadPriority( hThread, THREAD_PRIORITY_LOWEST );
	ResumeThread( hThread );

	return true;
}

bool CScheduler::Stop()
{
	m_bRunThread = false;
	Sleep( 50 );

	return true;
}

//////////////////////////////////////////////////////////////////////
// Configuration Functions
//////////////////////////////////////////////////////////////////////

void CScheduler::SetStartTime( int nHour, int nMin, int nSec )
{
	m_StartTime.nHour = nHour;
	m_StartTime.nMin = nMin;
	m_StartTime.nSec = nSec;
}

void CScheduler::SetStopTime( int nHour, int nMin, int nSec )
{
	m_StopTime.nHour = nHour;
	m_StopTime.nMin = nMin;
	m_StopTime.nSec = nSec;
}

void CScheduler::SetConfig( CMainFrame *pFrame, SchedTime &start, SchedTime &stop )
{
	this->m_pMyFrame = pFrame;
	memcpy( &m_StartTime, &start, sizeof(SchedTime) );
	memcpy( &m_StopTime, &stop, sizeof(SchedTime) );
}

//////////////////////////////////////////////////////////////////////
// Thread Functions
//////////////////////////////////////////////////////////////////////

DWORD WINAPI SchedProc( LPVOID lpParam )
{
	CScheduler* pSched = (CScheduler *)lpParam;
	SchedTime curtime;

	while( pSched->m_bRunThread )
	{
		pSched->GetCurTime( &curtime );

		// Обрабатываем ВЫКЛючение записи...
		//(Время запуска эквивалетно заданному вр. +2 сек.)
		if( DifferenceInSec(curtime, pSched->m_StopTime) < 3 )
		{
			//pSched->m_pMyFrame->OnBtnSTOP();
			pSched->m_pMyFrame->PostMessage( WM_COMMAND, MAKEWPARAM(IDM_SOUND_STOP, 0),
				NULL );
			break; // выходим из потока (закрываем его)
		}

		// Обрабатываем ВКЛючение записи...
		if( DifferenceInSec(curtime, pSched->m_StartTime) < 3 )
		{
			if( pSched->m_pMyFrame->m_nState != RECORD_STATE )
			{
				//pSched->m_pMyFrame->OnBtnSTOP();
				pSched->m_pMyFrame->PostMessage( WM_COMMAND, MAKEWPARAM(IDM_SOUND_STOP, 0),
					NULL );

				// Проверка на отсутствие файла.
				if( !pSched->m_pMyFrame->m_pSndFile )
				{
					CString strName = pSched->m_pMyFrame->GetAutoName( CString("") );
					CString strPath = pSched->m_pMyFrame->m_conf.GetConfProg()->strLastFilePath;

					if( strPath.IsEmpty() )
						strPath = pSched->m_pMyFrame->m_strDir;
					strPath += "\\";

					strName = strPath + strName;
					pSched->m_pMyFrame->OpenFile( strName );					
				}

				//pSched->m_pMyFrame->OnBtnREC();
				pSched->m_pMyFrame->PostMessage( WM_COMMAND, MAKEWPARAM(IDM_SOUND_REC, 0),
					NULL );
			}
		}

		Sleep(300);
	}

	return 0;
}

void CScheduler::GetCurTime( SchedTime* curtime )
{
	// Получаем текущее время...
	CTime ct = CTime::GetCurrentTime();
	struct tm *newtime = ct.GetLocalTm();

	// Заполняем вых. структуру времени...
	curtime->nHour = newtime->tm_hour;
	curtime->nMin = newtime->tm_min;
	curtime->nSec = newtime->tm_sec;
}

int DifferenceInSec(SchedTime &curtime, SchedTime& needtime)
{
	// Получаем время в сек. ...
	int cursec = curtime.GetTimeInSec();
	int needsec = needtime.GetTimeInSec();

	// Обработка случая curtime=00.00.01, needtime=23.59.59 ...
	if(cursec < needsec)
		cursec += 24*60*60; // ... прибавляем 24 часа

	return (cursec - needsec);
}

//////////////////////////////////////////////////////////////////////
bool CScheduler::IsRinning()
{
	return m_bRunThread;
}

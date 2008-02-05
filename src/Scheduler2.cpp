// Scheduler2.cpp: implementation of the CScheduler2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mp3_recorder.h"
#include "Scheduler2.h"
#include "common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CScheduler2::CScheduler2()
{
	CLEAR_STRUCT(&m_tStart);
	CLEAR_STRUCT(&m_tStop);
	m_bWorkThread = false;
	m_bSchedStart = true;
	m_pSchedFunc = NULL;
	m_nRecTimeSec = 0;
}

CScheduler2::~CScheduler2()
{
	Stop();
}

//////////////////////////////////////////////////////////////////////
// PUBLIC (INTERFACE) FUNCTIONS
//////////////////////////////////////////////////////////////////////
bool CScheduler2::Start()
{
	DWORD  dwThreadID;
	//HANDLE hThread;

	if(m_pSchedFunc == NULL)
		return false;
	
	if(!m_bWorkThread)
	{
		m_hThread = CreateThread(NULL, 0, SchedThreadProc, this, CREATE_SUSPENDED,
			&dwThreadID);
		if(m_hThread != NULL)
		{
			m_bWorkThread = true;
			SetThreadPriority(m_hThread, THREAD_PRIORITY_LOWEST);
			ResumeThread(m_hThread);
			TRACE0("==> CScheduler2::Start: work thread started!\n");
		}
	}

	return m_bWorkThread;
}

//////////////////////////////////////////////////////////////////////
bool CScheduler2::Stop()
{
	if(m_bWorkThread)
	{
		TRACE0("==> CScheduler2::Stop: trying to stop work tread ...\n");
		m_bWorkThread = false;
		WaitForSingleObject(m_hThread, INFINITE);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CScheduler2::IsRunning()
{
	return m_bWorkThread;
}

//////////////////////////////////////////////////////////////////////
void CScheduler2::SetStopTime(SHR_TIME* ptStop, SHR_TIME* ptStart)
{
	m_bSchedStart = false;	// допустим, время старта НЕ определено ...
	GetCurTime(&m_tStart);	// ... берем текущее время за время старта

	if(ptStart != NULL) {	// время старта определено
		m_bSchedStart = true;
		memcpy(&m_tStart, ptStart, sizeof(SHR_TIME));
	}

	// устанавливаем время останова
	memcpy(&m_tStop, ptStop, sizeof(SHR_TIME));

	// обнуляем время записи (если не задано время старта)
	m_nRecTimeSec = 0;
	if(ptStart != NULL)
		m_nRecTimeSec = GetDiffInSec(ptStop, ptStart);
}

//////////////////////////////////////////////////////////////////////
void CScheduler2::SetRecTime(SHR_TIME* ptRec, SHR_TIME* ptStart)
{
	m_bSchedStart = false;	// допустим, время старта НЕ определено ...
	GetCurTime(&m_tStart);	// ... берем текущее время за время старта

	if(ptStart != NULL) {	// время старта определено
		m_bSchedStart = true;
		memcpy(&m_tStart, ptStart, sizeof(SHR_TIME));
	}

	// устанавливаем время останова
	int nStopTimeInSec = GetTimeInSec(&m_tStart) + GetTimeInSec(ptRec) + 1;
	m_tStop.h = nStopTimeInSec / 3600;
	m_tStop.m = (nStopTimeInSec - m_tStop.h*3600) / 60;
	m_tStop.s = nStopTimeInSec - m_tStop.h*3600 - m_tStop.m*60;
	if(m_tStop.h > 23) {
		m_tStop.h = m_tStop.h - 24;
	}

	// запоминаем время записи
	m_nRecTimeSec = GetTimeInSec(ptRec);
}

//////////////////////////////////////////////////////////////////////
void CScheduler2::SetCallbackFunc(SHR2_FUNC pSchedFunc)
{
	if(pSchedFunc)
		m_pSchedFunc = pSchedFunc;
}

//////////////////////////////////////////////////////////////////////
// PRIVATE (IMPLEMENTATION) FUNCTIONS
//////////////////////////////////////////////////////////////////////
void CScheduler2::GetCurTime(SHR_TIME* pCurTime)
{
	// Получаем текущее время
	CTime ct = CTime::GetCurrentTime();
	//struct tm *newtime = ct.GetLocalTm(NULL);
	struct tm newtime;
	ct.GetLocalTm(&newtime);

	pCurTime->h = newtime.tm_hour;
	pCurTime->m = newtime.tm_min;
	pCurTime->s = newtime.tm_sec;
}

//////////////////////////////////////////////////////////////////////
int CScheduler2::GetTimeInSec(SHR_TIME* pTime)
{
	// время в сек = часы*3600с + мин*60с + секунды
	return (pTime->h*3600 + pTime->m*60 + pTime->s);
}

//////////////////////////////////////////////////////////////////////
int CScheduler2::GetDiffInSec(SHR_TIME* pCurTime, SHR_TIME* pNeedTime)
{
	int nCurSec = GetTimeInSec(pCurTime);
	int nNeedSec= GetTimeInSec(pNeedTime);

	// Обработка случая curtime=00.00.01, needtime=23.59.59 ...
	if(nCurSec < nNeedSec)
		nCurSec += 24*60*60;

	return (nCurSec - nNeedSec);
}

//////////////////////////////////////////////////////////////////////
// THREAD FUNCTION
//////////////////////////////////////////////////////////////////////
DWORD WINAPI SchedThreadProc(LPVOID lpParam)
{
	CScheduler2* pSched = (CScheduler2 *)lpParam;
	SHR_TIME  curTime;
	SHR_TIME* pStartTime= &pSched->m_tStart;
	SHR_TIME* pStopTime = &pSched->m_tStop;
	const int WAIT_TIME = 4; // seconds

	bool bStartCalled= false;
	bool bStopCalled = false;

	while(pSched->m_bWorkThread)
	{
		pSched->GetCurTime(&curTime);

		// обрабатываем включение записи
		if(pSched->m_bSchedStart) {
			if(pSched->GetDiffInSec(&curTime, pStartTime) < WAIT_TIME) {
				if(!bStartCalled) {
					TRACE0("==> SchedThreadProc: calling scheduler function (start) ...\n");
					(*pSched->m_pSchedFunc)(0);
					bStartCalled = true;
				}
			}
			else
				bStartCalled = false;
		}

		// обрабатываем выключение записи
		if(pSched->GetDiffInSec(&curTime, pStopTime) < WAIT_TIME) {
			if(!bStopCalled) {
				TRACE0("==> SchedThreadProc: calling scheduler function (stop) ...\n");
				(*pSched->m_pSchedFunc)(1);
				bStopCalled = true;
			}
		}
		else
			bStopCalled = false;

		Sleep(100);
	}//while
	
	TRACE0("==> SchedThreadProc: work tread stopped!\n");
	return 0;
}

//////////////////////////////////////////////////////////////////////
int CScheduler2::GetRecTimeInSec()
{
	SHR_TIME tCurTime;
	GetCurTime(&tCurTime);

	if(m_nRecTimeSec == 0)
		return GetDiffInSec(&tCurTime, &m_tStop);
	else
		return m_nRecTimeSec;
}

//////////////////////////////////////////////////////////////////////

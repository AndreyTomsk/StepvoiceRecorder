// Scheduler.h: interface for the CScheduler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCHEDULER_H__A8F722D9_C552_45A1_BDB9_AB34DCD86E70__INCLUDED_)
#define AFX_SCHEDULER_H__A8F722D9_C552_45A1_BDB9_AB34DCD86E70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct
{	int nHour;
	int nMin;
	int nSec;
	int GetTimeInSec() {return (nHour*60*60 + nMin*60 + nSec);}
} SchedTime;

class CMainFrame;

class CScheduler  
{
	SchedTime m_StartTime;
	SchedTime m_StopTime;
	SchedTime m_RecTime;
	bool m_bRunThread;
	CMainFrame* m_pMyFrame;

public:
	bool IsRinning();
	void SetConfig(CMainFrame* pFrame, SchedTime& start, SchedTime& stop);
	void SetStopTime(int nHour, int nMin, int nSec);
	void SetStartTime(int nHour, int nMin, int nSec);
	bool Stop();
	bool Start();
	CScheduler();
	virtual ~CScheduler();
	friend DWORD WINAPI SchedProc(LPVOID lpParam);
private:
	void GetCurTime(SchedTime* curtime);
};

// ¬ычисление разницы по времени (в секундах)
int DifferenceInSec(SchedTime& curtime, SchedTime& needtime);

#endif // !defined(AFX_SCHEDULER_H__A8F722D9_C552_45A1_BDB9_AB34DCD86E70__INCLUDED_)

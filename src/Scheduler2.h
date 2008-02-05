// Scheduler2.h: interface for the CScheduler2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCHEDULER2_H__68902CF1_52A5_4A3D_ADE1_D15A189A80BA__INCLUDED_)
#define AFX_SCHEDULER2_H__68902CF1_52A5_4A3D_ADE1_D15A189A80BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"

typedef void (*SHR2_FUNC)(int);

class CScheduler2  
{
public:
	CScheduler2();
	virtual ~CScheduler2();
	int GetRecTimeInSec();
	bool IsRunning();
	bool Start();
	bool Stop();
	void SetStopTime(SHR_TIME* ptStop, SHR_TIME* ptStart = NULL);
	void SetRecTime(SHR_TIME* ptRec, SHR_TIME* ptStart = NULL);
	void SetCallbackFunc(SHR2_FUNC);

private:
	friend DWORD WINAPI SchedThreadProc(LPVOID lpParam);
	void GetCurTime(SHR_TIME* pCurTime);
	int  GetTimeInSec(SHR_TIME* pTime);
	int  GetDiffInSec(SHR_TIME* pCurTime, SHR_TIME* pNeedTime);

private:
	SHR_TIME m_tStart;
	SHR_TIME m_tStop;
	int  m_nRecTimeSec;
	bool m_bWorkThread;
	bool m_bSchedStart;
	SHR2_FUNC m_pSchedFunc;
	HANDLE m_hThread;
};

#endif // !defined(AFX_SCHEDULER2_H__68902CF1_52A5_4A3D_ADE1_D15A189A80BA__INCLUDED_)

#pragma once
#include "common.h"

/////////////////////////////////////////////////////////////////////////////

class CStopwatchMS
{
public:
	CStopwatchMS();

	void Reset();
	DWORD GetElapsedTime() const;

private:
	ULONGLONG m_startMS;
};

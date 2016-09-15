#pragma once

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

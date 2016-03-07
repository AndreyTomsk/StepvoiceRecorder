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

void EnableChildWindows(HWND parentWindow, bool enable);

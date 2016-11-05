/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

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

/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#include "stdafx.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CStopwatchMS::CStopwatchMS()
{
	Reset();
}
//---------------------------------------------------------------------------

void CStopwatchMS::Reset()
{
	m_startMS = ::GetTickCount();
}
//---------------------------------------------------------------------------

DWORD CStopwatchMS::GetElapsedTime() const
{
	const ULONGLONG curMS = ::GetTickCount();
	if (m_startMS <= curMS)
		return DWORD(curMS - m_startMS);
	else //DWORD overflow case
		return DWORD(UINT_MAX + curMS - m_startMS);
}
//---------------------------------------------------------------------------

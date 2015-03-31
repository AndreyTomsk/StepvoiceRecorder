#include "stdafx.h"
#include "VASFilter.h"
#include <math.h> //for log10

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

VasFilter::VasFilter(double tresholdDB, int durationMS, bool isEnabled)
	:m_tresholdDB(tresholdDB)
	,m_durationMS(durationMS)
	,m_enabled(isEnabled)
{
}
//---------------------------------------------------------------------------

VasFilter::~VasFilter()
{
}
//---------------------------------------------------------------------------

void VasFilter::Enable(bool isEnabled)
{
	m_enabled = isEnabled;
}
//---------------------------------------------------------------------------

bool VasFilter::ProcessData(void* buffer, DWORD lengthBytes)
{
	ASSERT(lengthBytes%sizeof(float) == 0); //base check we have float data

	//Calculate max level of buffer (or check it is lower than treshold).
	//If lower - get current time stamp.
	//If time stamp already not null - check duration time not exceeded.
	//If exceeded - send notification, don't pass data to next filter.
	//Until max level in input buffer become higher than treshold.

	static DWORD silenceStartMS = 0;
	static bool silenceState = false;

	if (!m_enabled)
	{
		silenceStartMS = 0;
		silenceState = false;
		return Filter::ProcessData(buffer, lengthBytes);
	}

	if (SilenceDetected((float*)buffer, lengthBytes/sizeof(float)))
	{
		const DWORD silenceCurMS = GetTickCount(); //TODO: replace by GetTickCount64

		if (silenceStartMS == 0)
		{
			silenceStartMS = silenceCurMS;
		}
		else if (!silenceState && (silenceCurMS - silenceStartMS > (unsigned)m_durationMS))
		{
			SendNotification(Parameter(_T("VAS.HandleSilence"), 1));
			silenceState = true;
		}
	}
	else
	{
		if (silenceState)
		{
			SendNotification(Parameter(_T("VAS.HandleSilence"), 0));
			silenceState = false;
		}
		//We can also assign silence start, but recording became not silent.
		silenceStartMS = 0;
	}

	if (!silenceState)
		return Filter::ProcessData(buffer, lengthBytes);
	else
		return true; //don't pass data to next filter.
}
//---------------------------------------------------------------------------

bool VasFilter::SilenceDetected(float* buffer, DWORD length) const
{
	for (unsigned i = 0; i < length; i++)
	{
		const float peakDB = 20 * log10(abs(buffer[i]));
		if (peakDB > m_tresholdDB)
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------

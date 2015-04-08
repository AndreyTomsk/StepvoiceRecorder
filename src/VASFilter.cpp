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
	,m_silenceStartMS(0)
	,m_silenceState(false)
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
	ResetDetection();
}
//---------------------------------------------------------------------------

void VasFilter::SetTreshold(double newTresholdDB)
{
	m_tresholdDB = newTresholdDB;
	ResetDetection();
}
//---------------------------------------------------------------------------

void VasFilter::SetDuration(int newDurationMS)
{
	m_durationMS = newDurationMS;
	ResetDetection();
}
//---------------------------------------------------------------------------

void VasFilter::ResetDetection()
{
	m_silenceStartMS = 0;
	m_silenceState = false;
}
//---------------------------------------------------------------------------

bool VasFilter::ProcessData(void* buffer, DWORD lengthBytes)
{
	if (!m_enabled)
		return Filter::ProcessData(buffer, lengthBytes);

	ASSERT(lengthBytes%sizeof(float) == 0); //base check we have float data

	//Calculate max level of buffer (or check it is lower than treshold).
	//If lower - get current time stamp.
	//If time stamp already not null - check duration time not exceeded.
	//If exceeded - send notification, don't pass data to next filter.
	//Until max level in input buffer become higher than treshold.

	if (SilenceDetected((float*)buffer, lengthBytes/sizeof(float)))
	{
		const ULONGLONG silenceCurMS = GetTickCount();
		if (m_silenceStartMS == 0)
		{
			m_silenceStartMS = silenceCurMS;
		}
		else
		{
			ULONGLONG diffMS;
			if (m_silenceStartMS <= silenceCurMS) //normal state
				diffMS = silenceCurMS - m_silenceStartMS;
			else //DWORD overflow (DWORD is result from GetTickCount).
				diffMS = UINT_MAX + silenceCurMS - m_silenceStartMS;

			if ((diffMS >= m_durationMS) && !m_silenceState)
			{
				SendNotification(Parameter(_T("VAS.HandleSilence"), 1));
				m_silenceState = true;
			}
		}
	}
	else //high signal
	{
		if (m_silenceState)
			SendNotification(Parameter(_T("VAS.HandleSilence"), 0));
		m_silenceState = false;
		m_silenceStartMS = 0;
	}

	if (!m_silenceState)
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

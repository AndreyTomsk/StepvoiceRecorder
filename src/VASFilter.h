#ifndef VAS_FILTER_H
#define VAS_FILTER_H
#pragma once

#include "Filter.h"

/////////////////////////////////////////////////////////////////////////////
//Voice Activation System
//Should send notifications when input sound is lower than treshold during
//some time: "VAS.HandleSilence"-1 (entered VAS), "VAS.HandleSilence"-0 (silence is over).

class VasFilter : public Filter
{
public:
	VasFilter(double tresholdDB, int durationMS, bool enabled = true);
	virtual ~VasFilter();

	void Enable(bool isEnabled); //control VAS from GUI

private:
	virtual bool ProcessData(void* buffer, DWORD lengthBytes); //overridden
private:
	bool SilenceDetected(float* buffer, DWORD length) const;
	double m_tresholdDB;
	int m_durationMS;
	bool m_enabled;
};

/////////////////////////////////////////////////////////////////////////////
#endif

#include "stdafx.h"
#include "VAS.h"
#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//===========================================================================
CVAS::CVAS()
{
	StopVAS();
}

//===========================================================================
CVAS::~CVAS()
{
}

//===========================================================================
void CVAS::InitVAS(double fThresholdDB, int nDurationMSec)
{
	// проверяем границы уровня
	if(fThresholdDB < VAS_MIN_THRESHOLD)
		fThresholdDB = VAS_MIN_THRESHOLD;
	if(fThresholdDB > VAS_MAX_THRESHOLD)
		fThresholdDB = VAS_MAX_THRESHOLD;

	// проверяем границы длительности
	if(nDurationMSec < VAS_MIN_DURATION)
		nDurationMSec = VAS_MIN_DURATION;
	if(nDurationMSec > VAS_MAX_DURATION)
		nDurationMSec = VAS_MAX_DURATION;
	
	m_fThresholdDB  = fThresholdDB;
	m_nDurationMSec = nDurationMSec;
	m_bHighSignal	= true;
	m_bVasState		= false;
	m_bIsRunning	= true;
}

//===========================================================================
void CVAS::StopVAS()
{
	m_fThresholdDB  = VAS_MIN_THRESHOLD;
	m_nDurationMSec = VAS_MAX_DURATION;
	m_bHighSignal = true;
	m_bVasState   = false;
	m_bIsRunning  = false;
}

//===========================================================================
void CVAS::ResetVAS()
{	// сбрасываем систему
	InitVAS(m_fThresholdDB, m_nDurationMSec);
}

//===========================================================================
void CVAS::ProcessThreshold(double fNewThreshold)
{
	static DWORD dwStartTimeMSec = 0;

	// выходим, если VAS выключено
	if(!IsRunning())
		return;

	// проверяем границы уровня
	if(fNewThreshold < VAS_MIN_THRESHOLD)
		fNewThreshold = VAS_MIN_THRESHOLD;
	if(fNewThreshold > VAS_MAX_THRESHOLD)
		fNewThreshold = VAS_MAX_THRESHOLD;

	// уровень больше или равен пороговуму, сброс значений и выход
	if(fNewThreshold >= m_fThresholdDB) {
		m_bHighSignal = true;
		m_bVasState	  = false;
		return;
	}
	
	// уровень меньше порогового, фиксация и выход
	if(m_bHighSignal) {
		dwStartTimeMSec = timeGetTime(); // сохр. текущее время для разности
		m_bHighSignal = false;
		return;
	}

	// уровень (повторно) меньше порогового
	if((timeGetTime() - dwStartTimeMSec) >= (DWORD)m_nDurationMSec)
		m_bVasState = true;
}

//===========================================================================
bool CVAS::CheckVAS()
{
	return m_bVasState;
}

//===========================================================================
bool CVAS::IsRunning()
{
	return m_bIsRunning;
}
//===========================================================================
#if !defined(AFX_VAS_H__16ADF521_B733_46EF_B0E5_9ED1E4CC145D__INCLUDED_)
#define AFX_VAS_H__16ADF521_B733_46EF_B0E5_9ED1E4CC145D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*//===========================================================================
enum VASAction	// действие после срабатывания VAS
	{	VAS_OFF		= 0,	// ничего не делать
		VAS_PAUSE	= 1,	// пауза записи
		VAS_STOP	= 2		// останов записи
	};*/

//===========================================================================
const double VAS_MAX_THRESHOLD = -10.3; // 10000/32767
const double VAS_MIN_THRESHOLD = -60.3; //   100/32767
const int VAS_MAX_DURATION = 7800; // 7.8 сек.
const int VAS_MIN_DURATION =  300; // 0.3 сек.

//===========================================================================
class CVAS
{
	double	m_fThresholdDB;	// пороговый уровень (децибелы)
	int		m_nDurationMSec;// длительность до вкл. паузы (милисек.)
	bool	m_bHighSignal;	// флаг уровня сигнала выше порогового
	bool	m_bVasState;	// флаг сработавшей VAS
	bool	m_bIsRunning;	// VAS работает

public:
	CVAS();
	~CVAS();
	void ProcessThreshold(double fNewThresholdDB);
	void InitVAS(double fThresholdDB = -70.3, int nDurationMSec = 7800);
	void StopVAS();
	void ResetVAS();
	bool CheckVAS();
	bool IsRunning();
};

//===========================================================================
#endif // !defined(AFX_VAS_H__16ADF521_B733_46EF_B0E5_9ED1E4CC145D__INCLUDED_)

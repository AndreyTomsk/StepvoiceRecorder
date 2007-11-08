//===========================================================================
#if !defined(AFX_VAS_H__16ADF521_B733_46EF_B0E5_9ED1E4CC145D__INCLUDED_)
#define AFX_VAS_H__16ADF521_B733_46EF_B0E5_9ED1E4CC145D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*//===========================================================================
enum VASAction	// �������� ����� ������������ VAS
	{	VAS_OFF		= 0,	// ������ �� ������
		VAS_PAUSE	= 1,	// ����� ������
		VAS_STOP	= 2		// ������� ������
	};*/

//===========================================================================
const double VAS_MAX_THRESHOLD = -10.3; // 10000/32767
const double VAS_MIN_THRESHOLD = -60.3; //   100/32767
const int VAS_MAX_DURATION = 7800; // 7.8 ���.
const int VAS_MIN_DURATION =  300; // 0.3 ���.

//===========================================================================
class CVAS
{
	double	m_fThresholdDB;	// ��������� ������� (��������)
	int		m_nDurationMSec;// ������������ �� ���. ����� (�������.)
	bool	m_bHighSignal;	// ���� ������ ������� ���� ����������
	bool	m_bVasState;	// ���� ����������� VAS
	bool	m_bIsRunning;	// VAS ��������

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

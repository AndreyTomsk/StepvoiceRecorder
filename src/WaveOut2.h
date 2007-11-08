//////////////////////////////////////////////////////////////////////
#if !defined(AFX_WAVEOUT2_H__07A55090_F9E4_4783_9F20_36E05ECD4161__INCLUDED_)
#define AFX_WAVEOUT2_H__07A55090_F9E4_4783_9F20_36E05ECD4161__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"

//////////////////////////////////////////////////////////////////////
class CWaveOut2  
{
	int	 m_nDevID;			//
	int  m_nDevNum;			//
	int  m_nBufNum;			//
	int  m_nBufSize;		//
	bool m_bIsRunning;		//
	WAVEHDR* m_whdrArray;	//
	HWAVEOUT m_hwo;			//

	WAVE_INFO m_waveinfo;

public:
	CWaveOut2();
	~CWaveOut2();

	bool Open(WAVE_INFO* pwi);
	bool Start();
	bool Pause();
	void Stop();
	void Close();
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_WAVEOUT2_H__07A55090_F9E4_4783_9F20_36E05ECD4161__INCLUDED_)

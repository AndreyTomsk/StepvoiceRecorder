//////////////////////////////////////////////////////////////////////
#ifndef _WAVEIN_H_
#define _WAVEIN_H_

#include "Wave.h"

#if _MSC_VER > 1000
#pragma once
#endif

//////////////////////////////////////////////////////////////////////
class CWaveIn  : public CWave
{
	HWAVEIN			m_hwi;
	WAVEFORMATEX	m_wf;
	IN_DATA_PROC	pDataProc;

	WAVEHDR*		m_whdrCur;

	int	m_nBufIndex;	// указатель на проигрываемый буфер
	int m_nBufAdded;	// число добавленных буферов

public:
	CWaveIn(IN_DATA_PROC pDataProc);
	~CWaveIn();

	// device functions
	void GetDevName(int nDevID, char* szDevName);

	// recording control functions
	int   Open(int nFreq, int nBits, int nStereo);
	bool  Start();
	void  Pause();
	void  Close();
	char* GetBuffer(int& nSamples, DWORD& dwCurSample);
	char* GetBuffer(DWORD& dwBufSize);

private:
	inline void ChangeIndex();

	friend void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg,
		DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};

//////////////////////////////////////////////////////////////////////
#endif

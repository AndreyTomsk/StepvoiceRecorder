//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEOUT_H__47BA3BB1_B8AD_41D0_9871_A109C45441F9__INCLUDED_)
#define AFX_WAVEOUT_H__47BA3BB1_B8AD_41D0_9871_A109C45441F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wave.h"
#include "struct.h"

//////////////////////////////////////////////////////////////////////
class CWaveOut  : public CWave
{
	HWAVEOUT		m_hwo;
	WAVEFORMATEX	m_wf;
	OUT_DATA_PROC	pDataProc;

	int	m_nBufIndex;	// указатель на проигрываемый буфер

	int m_nBufCurSize;

public:
	CWaveOut(OUT_DATA_PROC pDataProc);
	~CWaveOut();

	// device functions
	void GetDevName(int nDevID, char* szDevName);

	// playback control functions
	int  Open(int nFreq, int nBits, int nStereo);
	bool Open(SOUND_INFO* psi);
	bool Start();
	void Pause();
	void Close();
	bool AddBuffer(HWAVEOUT hwo, WAVEHDR* pwhdr);

	char* GetBuffer(int& nSamples, DWORD& dwCurSample);
	char* GetBuffer(DWORD& dwSize);
	char* GetCurBlock();

private:
	void ChangeIndex();

	friend void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg,
		DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_WAVEOUT_H__47BA3BB1_B8AD_41D0_9871_A109C45441F9__INCLUDED_)

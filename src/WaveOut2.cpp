//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mp3_recorder.h"
#include "WaveOut2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//====================================================================
CWaveOut2::CWaveOut2()
{

}

//====================================================================
CWaveOut2::~CWaveOut2()
{

}

bool CWaveOut2::Open(WAVE_INFO* pwi)
{	//
	memcpy(&m_waveinfo, pwi, sizeof(m_waveinfo));

	// заполняем структуру
	WAVEFORMATEX wf;
	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= pwi->nFreq;
	wf.wBitsPerSample	= pwi->nBits;
	wf.nChannels		= pwi->nChannel;
	wf.nBlockAlign		= pwi->nChannel * pwi->nBits / 8;
	wf.nAvgBytesPerSec	= pwi->nFreq * wf.nBlockAlign;
	wf.cbSize			= 0;
	
	//
	Close();
	
	//
	if(waveOutOpen(&m_hwo, pwi->nDevID, &wf, (DWORD)waveOutProc, (DWORD)this,
		CALLBACK_FUNCTION) != MMSYSERR_NOERROR) return ERR_OPEN;
	
	Pause();

	// создаем буферы


	BuffersCreate();	// create buffers
	for(int i=0; i < m_nWBufNum; i++)
		if(!AddBuffer(m_hwo, &m_whdrArray[i]))
		{
			BuffersDelete();
			return ERR_BUFFER;
		}
	return ERR_OK;
}
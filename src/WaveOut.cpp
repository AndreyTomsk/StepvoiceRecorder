//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WaveOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
	DWORD dwParam1, DWORD dwParam2)
{
	if(uMsg == WOM_DONE)
	{
		WAVEHDR*  pwhdr    = (WAVEHDR*)dwParam1;
		CWaveOut* pWaveOut = (CWaveOut*)dwInstance;

		if(pWaveOut->bIsRunning)
		{
			pWaveOut->ChangeIndex();
			pWaveOut->AddBuffer(hwo, pwhdr);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction / Destruction
//////////////////////////////////////////////////////////////////////
CWaveOut::CWaveOut(OUT_DATA_PROC pDataProc)
{
	m_hwo = NULL;
	this->pDataProc = pDataProc;

	m_nDeviceNum = waveOutGetNumDevs();

	m_nBufIndex = 0;

	m_nWBufNum  = 4;
	m_nWBufSize = 8192;
}

CWaveOut::~CWaveOut()
{
	if(m_hwo && m_whdrArray) Close();
}

//////////////////////////////////////////////////////////////////////
// Device Functions
//////////////////////////////////////////////////////////////////////
void CWaveOut::GetDevName(int nDevID, char* szDevName)
{
	WAVEOUTCAPS wc;
	waveOutGetDevCaps(nDevID, &wc, sizeof(WAVEOUTCAPS));
	strcpy(szDevName, wc.szPname);
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::AddBuffer(HWAVEOUT hwo, WAVEHDR* pwhdr)
{
	int  nBytesToPlay = m_nWBufSize;
	bool bResult	  = false;

	if( (*CWaveOut::pDataProc)(pwhdr->lpData, nBytesToPlay, pwhdr->dwBufferLength) )
	{	//
		pwhdr->dwBufferLength = nBytesToPlay;
		m_nBufCurSize = nBytesToPlay;
		if(waveOutWrite(hwo, pwhdr, sizeof(WAVEHDR)) == MMSYSERR_NOERROR)
		{
			bResult = true;
			//ChangeIndex();
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// Recording control functions
//////////////////////////////////////////////////////////////////////
int CWaveOut::Open(int nFreq, int nBits, int nStereo)
{
	m_wf.wFormatTag		= WAVE_FORMAT_PCM;
	m_wf.nSamplesPerSec	= nFreq;
	m_wf.wBitsPerSample	= nBits;
	m_wf.nChannels		= nStereo+1;
	m_wf.nBlockAlign	= nBits * (nStereo+1) / 8;
	m_wf.nAvgBytesPerSec= nFreq * m_wf.nBlockAlign;
	m_wf.cbSize			= 0;

	// close previously opened waveOut
	Close();
	// init playback
	if(waveOutOpen(&m_hwo, m_nDeviceID, &m_wf, (DWORD)waveOutProc, (DWORD)this,
		CALLBACK_FUNCTION) != MMSYSERR_NOERROR) return ERR_OPEN;
	Pause();

	BuffersCreate();	// create buffers
	
	// подгатавливаем буферы в память
	for(int j=0; j<4; j++)
	{
		waveOutPrepareHeader(m_hwo, &m_whdrArray[j], sizeof(WAVEHDR));
	}

	for(int i=0; i < m_nWBufNum; i++)
	{
		if(!AddBuffer(m_hwo, &m_whdrArray[i]))
		{
			BuffersDelete();
			return ERR_BUFFER;
		}
	}

	// Test!
	//waveOutSetVolume(m_hwo, 0x8888);
	//waveOutSetVolume(m_hwo, 0x00FF);

	return ERR_OK;
}

//====================================================================
bool CWaveOut::Open(SOUND_INFO* psi)
{
	m_wf.wFormatTag		= WAVE_FORMAT_PCM;
	m_wf.nSamplesPerSec	= psi->nFreq;
	m_wf.wBitsPerSample	= psi->nBits;
	m_wf.nChannels		= psi->nStereo+1;
	m_wf.nBlockAlign	= psi->nBits * (psi->nStereo+1) / 8;
	m_wf.nAvgBytesPerSec= psi->nFreq * m_wf.nBlockAlign;
	m_wf.cbSize			= 0;
	
	// запускаем систему воспроизведения
	Close();
	if(waveOutOpen(&m_hwo, m_nDeviceID, &m_wf, (DWORD)waveOutProc,
		(DWORD)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		return false;
	}
	Pause();

	// создаем буферы
	BuffersCreate();
	for(int i=0; i < m_nWBufNum; i++)
	{
		if(!AddBuffer(m_hwo, &m_whdrArray[i]))
		{
			BuffersDelete();
			return false;
		}
		waveOutPrepareHeader(m_hwo, &m_whdrArray[i], sizeof(WAVEHDR));
	}

	return true;
}

//====================================================================
void CWaveOut::Close()
{
	if(!bIsRunning)
		return;
	
	bIsRunning = false;
	Sleep(50);

	MMRESULT mr;
	mr = waveOutReset(m_hwo);
	ASSERT(mr == MMSYSERR_NOERROR);

	// освобождаем буферы
	for(int i=0; i<4; i++)
	{
		waveOutUnprepareHeader(m_hwo, &m_whdrArray[i], sizeof(WAVEHDR));
	}

	mr = waveOutClose(m_hwo);
	ASSERT(mr == MMSYSERR_NOERROR);

	m_hwo = NULL;
	BuffersDelete();	// delete all sound buffers
}

//////////////////////////////////////////////////////////////////////
bool CWaveOut::Start()
{
	if(!m_whdrArray)
		return false;
	bIsRunning = true;
	return (waveOutRestart(m_hwo) == MMSYSERR_NOERROR); 
}

void CWaveOut::Pause()
{
	MMRESULT mr = waveOutPause(m_hwo);
	ASSERT(mr == MMSYSERR_NOERROR);
}

//////////////////////////////////////////////////////////////////////
void CWaveOut::ChangeIndex()
{
	m_nBufIndex = (m_nBufIndex < m_nWBufNum-1) ? m_nBufIndex+1 : 0;
}

//====================================================================
char* CWaveOut::GetBuffer(int& nSamples, DWORD& dwCurSample)
{	//
/*	MMTIME mmtPos;
	mmtPos.wType = TIME_SAMPLES;
	waveOutGetPosition(m_hwo, &mmtPos, sizeof(MMTIME));

	// определяем смещение в буфере на начало запиcи
	int nBufSamples = m_nBufCurSize / m_wf.nBlockAlign;
	int nOffSamples = mmtPos.u.sample % nBufSamples;

	if(nCurValue)
	{
		*nCurValue = 0;
		*nCurValue = *((DWORD *)(m_whdrArray[m_nBufIndex].lpData +
			nOffSamples*m_wf.nBlockAlign));
	}

	// определяем какое смещение передавать
	nOffSamples -= (nOffSamples > nSamples) ? nSamples : nOffSamples;
	nOffSamples /= 2;

	// корректируем размер (если надо)
	if(nSamples > nBufSamples-nOffSamples)
	{
		nSamples = nBufSamples-nOffSamples;
	}

	//return(m_whdrArray[m_nBufIndex].lpData);
	return(m_whdrArray[m_nBufIndex].lpData + nOffSamples*m_wf.nBlockAlign);
*/
	DWORD dwBufSize = nSamples * (m_wf.wBitsPerSample/8);
	char* pBuffer   = GetBuffer(dwBufSize);

	dwCurSample = 0;
	nSamples	= dwBufSize / (m_wf.wBitsPerSample/8);

	return pBuffer;
}

//====================================================================
char* CWaveOut::GetBuffer(DWORD& dwBufSize)
{
	char* pBuffer = NULL;
	
	// получаем смещение в байтах от начала записи
	MMTIME	 mmtPos = {TIME_BYTES, 0};
	MMRESULT mr = waveOutGetPosition(m_hwo, &mmtPos, sizeof(MMTIME));

	if((mr == MMSYSERR_NOERROR) && (mmtPos.wType == TIME_BYTES))
	{
		// вычисляем буфер и смещение в нем
		int nBufNum = (mmtPos.u.cb / m_nWBufSize) % m_nWBufNum;
		mmtPos.u.cb = mmtPos.u.cb % m_nWBufSize;

		// выравниваем смещение и размер буфера
		mmtPos.u.cb	 = (mmtPos.u.cb / m_wf.nBlockAlign) * m_wf.nBlockAlign;
		dwBufSize    = (dwBufSize   / m_wf.nBlockAlign) * m_wf.nBlockAlign;

		// корректируем смещение и размер возвращаемого буфера
		DWORD dwSub	 = (mmtPos.u.cb > dwBufSize) ? dwBufSize : mmtPos.u.cb;
		mmtPos.u.cb	-= dwSub;
		dwBufSize	 = dwSub;

		pBuffer = m_whdrArray[nBufNum].lpData + mmtPos.u.cb;
	}

	return pBuffer;
}

//--------------------------------------------------------------------
char* CWaveOut::GetCurBlock()
{
	char* pBuffer = NULL;

	// получаем смещение в байтах от начала записи
	MMTIME	 mmtPos = {TIME_BYTES, 0};
	MMRESULT mr = waveOutGetPosition(m_hwo, &mmtPos, sizeof(MMTIME));

	if((mr == MMSYSERR_NOERROR) && (mmtPos.wType == TIME_BYTES))
	{
		// вычисляем буфер и смещение в нем
		int nBufNum = (mmtPos.u.cb / m_nWBufSize) % m_nWBufNum;
		mmtPos.u.cb = mmtPos.u.cb % m_nWBufSize;

		// выравниваем смещение в буфере
		mmtPos.u.cb	 = (mmtPos.u.cb / m_wf.nBlockAlign) * m_wf.nBlockAlign;

		pBuffer = m_whdrArray[nBufNum].lpData + mmtPos.u.cb;
	}

	return pBuffer;
}

//====================================================================

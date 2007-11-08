//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WaveIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//====================================================================
// Data processing
//====================================================================
void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance,
							DWORD dwParam1, DWORD dwParam2)
{
	if(uMsg == WIM_DATA)
	{
		WAVEHDR* pwhdr	= (WAVEHDR*)dwParam1;
		CWaveIn* pWaveIn= (CWaveIn*)dwInstance;

		// если что-то записано - обрабатываем запись
		if(pwhdr->dwBytesRecorded)
			(*pWaveIn->pDataProc)(pwhdr->lpData, pwhdr->dwBytesRecorded);

		// если не остановлено - добавляем буфер
		if(pWaveIn->bIsRunning)
			waveInAddBuffer(pWaveIn->m_hwi, pwhdr, sizeof(WAVEHDR));
		else
			pWaveIn->m_nBufAdded--;

		// изменяем индекс текущего буфера
		pWaveIn->ChangeIndex();
		pWaveIn->m_whdrCur = (WAVEHDR *)pwhdr->dwUser;
	}
}

//====================================================================
// Construction / Destruction
//====================================================================
CWaveIn::CWaveIn(IN_DATA_PROC pDataProc)
{
	m_hwi = NULL;
	this->pDataProc = pDataProc;

	m_nWBufSize = 2048;
	m_nWBufNum  = 8;
	//m_nWBufSize = 4096;
}

//====================================================================
CWaveIn::~CWaveIn()
{
	if(m_hwi && m_whdrArray)
	{
		Close();
	}
}

//====================================================================
// Device Functions
//====================================================================
void CWaveIn::GetDevName(int nDevID, char* szDevName)
{
	WAVEINCAPS wc;
	waveInGetDevCaps(nDevID, &wc, sizeof(WAVEINCAPS));
	strcpy(szDevName, wc.szPname);
}

//====================================================================
// Recording control functions
//====================================================================
int CWaveIn::Open(int nFreq, int nBits, int nStereo)
{
	m_wf.wFormatTag		= WAVE_FORMAT_PCM;
	m_wf.nSamplesPerSec	= nFreq;
	m_wf.wBitsPerSample	= nBits;
	m_wf.nChannels		= nStereo+1;
	m_wf.nBlockAlign	= nBits * (nStereo+1) / 8;
	m_wf.nAvgBytesPerSec= nFreq * m_wf.nBlockAlign;
	m_wf.cbSize			= 0;

	if(!waveInGetNumDevs()) return ERR_DEVICE;

	MMRESULT mr;
	mr = waveInOpen(&m_hwi, m_nDeviceID, &m_wf, (DWORD)waveInProc,
		(DWORD)this, CALLBACK_FUNCTION);
	if(mr != MMSYSERR_NOERROR)
	{	return ERR_OPEN;
	}

	// в зависимости от freq изменяем размер буфера
	switch(nFreq)
	{
	case 8000:	m_nWBufSize = 512; break;
	case 11025:	m_nWBufSize = 1024; break;
	case 22050: m_nWBufSize = 2048; break;
	default:	m_nWBufSize = 4096; break;
	}
	
	// уменьшим буфер для моно сигнала
	if(m_wf.nChannels == 1)
		m_nWBufSize /= 2;

	// подгатавливаем буферы и добавляем их в очередь
	BuffersCreate();
	for(int i=0; i < m_nWBufNum; i++)
	{
		mr = waveInPrepareHeader(m_hwi, &m_whdrArray[i], sizeof(WAVEHDR));
		ASSERT(mr == MMSYSERR_NOERROR);
		mr = waveInAddBuffer(m_hwi, &m_whdrArray[i], sizeof(WAVEHDR));
		ASSERT(mr == MMSYSERR_NOERROR);
		if(i == m_nWBufNum - 1)
			m_whdrArray[i].dwUser = (DWORD)&m_whdrArray[0];
		else
			m_whdrArray[i].dwUser = (DWORD)&m_whdrArray[i+1];

		/*if(!AddBuffer(m_hwi, &m_whdrArray[i]))
		{
			BuffersDelete();
			return ERR_BUFFER;
		}*/
	}
	
	// инициализируем счетчики
	m_nBufIndex = 0;
	m_whdrCur   = &m_whdrArray[0];

	return ERR_OK;
}

//====================================================================
void CWaveIn::Close()
{
	if(!bIsRunning)
		return;

	bIsRunning = false;
	Sleep(50);
	//

	MMRESULT mr;
	mr = waveInReset(m_hwi);
	ASSERT(mr == MMSYSERR_NOERROR);

	// ожидаем, пока освободятся все буферы
	while(m_nBufAdded > 0)
		Sleep(50);

	// сбрасываем буферы
	for(int i=0; i<m_nWBufNum; i++)
	{	mr = waveInUnprepareHeader(m_hwi, &m_whdrArray[i], sizeof(WAVEHDR));
		ASSERT(mr == MMSYSERR_NOERROR);
	}

	mr = waveInClose(m_hwi);
	ASSERT(mr == MMSYSERR_NOERROR);

	// удаляем звуковые буферы
	BuffersDelete();
}

//====================================================================
bool CWaveIn::Start()
{
	if(!m_whdrArray)
		return false;

	m_nBufAdded = m_nWBufNum;	// инициализируем счетчик

	// очищаем буферы и записываем маленький фрагмент тишины
	// (для устнанения шумов при перезаписи мп3 файла)
	BuffersClear();
	for(int i=0; i < m_nWBufNum; i++)
	{
		pDataProc(m_whdrArray[i].lpData,
			m_whdrArray[i].dwBufferLength);
	}

	bIsRunning = true;
	return (waveInStart(m_hwi) == MMSYSERR_NOERROR); 
}

//====================================================================
void CWaveIn::Pause()
{
	waveInStop(m_hwi);
}

//===========================================================================
/*int CWaveIn::GetLevel()
{
	MMTIME mmtPos;

	// получаем смещение от начала буфера
	mmtPos.wType = TIME_BYTES;
	waveInGetPosition(m_hwi, &mmtPos, sizeof(mmtPos));

	return *(m_whdrArray[m_nBufIndex].lpData + mmtPos.u.cb);
}*/

//===========================================================================
char* CWaveIn::GetBuffer(int& nSamples, DWORD& dwCurSample)
{
/*	// получаем номер проигрываемого семпла
	MMRESULT mr;
	MMTIME   mmtPos;
	mmtPos.wType = TIME_SAMPLES;
	mr = waveInGetPosition(m_hwi, &mmtPos, sizeof(MMTIME));
	ASSERT(mr == MMSYSERR_NOERROR);

	// определяем смещение в проигрываемом буфере на начало записи
	int nBufSamples = m_nWBufSize / (m_wf.wBitsPerSample/8);//m_wf.nBlockAlign;
	int nOffSamples = mmtPos.u.sample % nBufSamples;

	//dwCurSample = *((DWORD *)(m_whdrCur->lpData + nOffSamples*(m_wf.wBitsPerSample/8)));
	dwCurSample = 0;

	// выравниваем смещение и размер буфера


	// корректируем (уменьшаем) смещение
	int nSub = (nOffSamples > nSamples) ? nSamples : nOffSamples;
	nOffSamples -= nSub;
	nSamples     = nSub;

	return (m_whdrCur->lpData + nOffSamples*(m_wf.wBitsPerSample/8));
*/
	DWORD dwBufSize = nSamples * (m_wf.wBitsPerSample/8);
	char* pBuffer   = GetBuffer(dwBufSize);

	dwCurSample = 0;
	nSamples	= dwBufSize / (m_wf.wBitsPerSample/8);

	return pBuffer;
}

//===========================================================================
char* CWaveIn::GetBuffer(DWORD& dwBufSize)
{
	char* pBuffer = NULL;
	
	// получаем смещение в байтах от начала записи
	MMTIME	 mmtPos = {TIME_BYTES, 0};
	MMRESULT mr = waveInGetPosition(m_hwi, &mmtPos, sizeof(MMTIME));

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
		//pBuffer = m_whdrArray[nBufNum].lpData;
		//pBuffer = m_whdrCur->lpData;
	}

	return pBuffer;
}

//===========================================================================
inline void CWaveIn::ChangeIndex()
{
	m_nBufIndex = (m_nBufIndex < m_nWBufNum-1) ? m_nBufIndex+1 : 0;
}

//===========================================================================

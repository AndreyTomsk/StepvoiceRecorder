//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Wave.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWave::CWave()
{
	m_nDeviceID	 = WAVE_MAPPER;
	m_nDeviceNum = 0;

	m_whdrArray	 = NULL;
	m_nWBufNum	 = 4;
	m_nWBufSize	 = 30000;

	bIsRunning	 = false;
}

CWave::~CWave()
{
	BuffersDelete();
}

//////////////////////////////////////////////////////////////////////
// Device functions
//////////////////////////////////////////////////////////////////////
int CWave::GetNumDevs() {
	return m_nDeviceNum;
}

void CWave::SetDevice(int nDevID) {
	m_nDeviceID = nDevID;
}

//////////////////////////////////////////////////////////////////////
// Buffer functions
//////////////////////////////////////////////////////////////////////
void CWave::BuffersCreate()
{
	BuffersDelete();

	m_whdrArray = new WAVEHDR[m_nWBufNum];
	ZeroMemory(m_whdrArray, sizeof(WAVEHDR) * m_nWBufNum);

	for(int i=0; i < m_nWBufNum; i++)
	{
		char* pBuffer	= new char[m_nWBufSize];
		ZeroMemory(pBuffer, m_nWBufSize);
		m_whdrArray[i].dwBufferLength	= m_nWBufSize;
		m_whdrArray[i].lpData			= pBuffer;
	}
}

void CWave::BuffersDelete()
{
	if(m_whdrArray == NULL) return;

	for(int i=0; i<m_nWBufNum; i++)
		if(m_whdrArray[i].lpData)
			delete [] m_whdrArray[i].lpData;
	
	delete [] m_whdrArray;
	m_whdrArray	= NULL;
}

// Функция очистки буферов.
// Обнуляет буфер (для 16-битной записи)
void CWave::BuffersClear()
{
	if(m_whdrArray == NULL) return;

	for(int i=0; i<m_nWBufNum; i++)
	{
		ZeroMemory(m_whdrArray[i].lpData,
			m_whdrArray[i].dwBufferLength);
	}
}

void CWave::GetBufferInfo(int* pNumber, int* pSize)
{
	if(pNumber) pNumber = &m_nWBufNum;
	if(pSize)	pSize	= &m_nWBufSize;
}

void CWave::SetBufferInfo(int  nNumber, int  nSize)
{
	m_nWBufNum	= nNumber;
	m_nWBufSize	= nSize;
}

//////////////////////////////////////////////////////////////////////

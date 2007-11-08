//////////////////////////////////////////////////////////////////////
#ifndef _WAVE_H_
#define _WAVE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////
#define ERR_OK		0
#define ERR_DEVICE	1
#define ERR_OPEN	2
#define ERR_BUFFER	3

//////////////////////////////////////////////////////////////////////
typedef void (*IN_DATA_PROC) (char* pBuffer, int nBufSize);
typedef bool (*OUT_DATA_PROC)(char* pBuffer, int& nBufSize, int nBytesToShow);

//////////////////////////////////////////////////////////////////////
class CWave  
{
protected:
	int	 m_nDeviceID;
	int  m_nDeviceNum;

	int	 m_nWBufNum;
	int	 m_nWBufSize;
	
	WAVEHDR* m_whdrArray;

protected:
	void BuffersClear();
	void BuffersCreate();
	void BuffersDelete();

public:
	bool bIsRunning;

	CWave();		
	virtual ~CWave();

	// device functions
	void	SetDevice(int nDevID);
	int		GetNumDevs();

	// buffer functions
	void GetBufferInfo(int* pNumber, int* pSize);
	void SetBufferInfo(int  nNumber, int  nSize);

};

//////////////////////////////////////////////////////////////////////
#endif

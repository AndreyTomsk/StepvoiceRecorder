
#ifndef _ENCODER_MP3
#define _ENCODER_MP3

#include "struct.h"
#include "encoder.h"
#include <BladeMP3EncDll.h>

///////////////////////////////////////////////////////////////////////////////
class CEncoder_MP3
	: public CEncoder
{
public:
	CEncoder_MP3(CString& strDllPath);
	CEncoder_MP3(CString& strDllPath, void* pSettings);
	virtual ~CEncoder_MP3();

	virtual int  InitEncoder(void* pSettings);
	virtual bool EncodeChunk(char* pBufIn,  int  nBufInSize,
		char* pBufOut, int& nBufOutSize);
	virtual void CloseEncoder();

private:
	void LoadLibrary(CString& strDllName);
	void FreeLibrary();

private:
	HINSTANCE  m_hDll;			// ���������� ���������� lame.dll
	BE_CONFIG  m_beConfig;		// ��������� �������� �����������
	HBE_STREAM m_hbeStream;		// ���������� ������ ����������

	char*	   m_pChunkBuf;			//
	int		   m_nChunkBufOffset;	//
	int		   m_nChunkBufSize;		//
	int		   m_nChunkBufSamples;	//

	char*	   m_pBufIn;		//
	int		   m_nBufInSize;	//

	//char* m_pMp3Buf;
};

///////////////////////////////////////////////////////////////////////////////
#endif // _ENCODER_MP3

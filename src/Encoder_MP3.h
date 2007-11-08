//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ENCODER_MP3_H__EEB8AF0A_A74B_4B03_AA39_F1B8C01461B0__INCLUDED_)
#define AFX_ENCODER_MP3_H__EEB8AF0A_A74B_4B03_AA39_F1B8C01461B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"
#include "encoder.h"
#include "BladeMP3EncDll.h"

//////////////////////////////////////////////////////////////////////
/*typedef struct ENC_FLAGS {
	bool bDllOK;
	bool bEncOK;
	bool bReserved;
} ENC_FLAGS;
*/
//////////////////////////////////////////////////////////////////////

class CEncoder_MP3 : public CEncoder
{
protected:
	HINSTANCE  m_hDll;			// дескриптор библиотеки lame.dll
	BE_CONFIG  m_beConfig;		// стркутура настроек кодировани€
	HBE_STREAM m_hbeStream;		// дескриптор потока кодровани€

	char*	   m_pChunkBuf;			//
	int		   m_nChunkBufOffset;	//
	int		   m_nChunkBufSize;		//
	int		   m_nChunkBufSamples;	//

	char*	   m_pBufIn;		//
	int		   m_nBufInSize;	//

	char* m_pMp3Buf;

protected:
	void LoadLibrary(CString& strDllName);
public:
	CEncoder_MP3(CString& strDllPath);
	CEncoder_MP3(CString& strDllPath, void* pSettings);
	virtual ~CEncoder_MP3();

	virtual int InitEncoder(void* pSettings);
	virtual bool EncodeChunk(char* pBufIn,  int  nBufInSize,
							 char* pBufOut, int& nBufOutSize);
	virtual void CloseEncoder();
};


//////////////////////////////////////////////////////////////////////
/*class CEncoder_MP3
{
	HINSTANCE	m_hDll;
	BE_CONFIG	m_beConfig;		// MP3 config structure
	HBE_STREAM	m_hbeStream;	// MP3 encoding stream

	ENC_FLAGS	m_flags;
	BYTE*		m_pBufferTmp;	// temporary WAV buffer
	DWORD		m_dwBufTmpSize;	// number of bytes to send to each beEncodeChunk()
	DWORD		m_dwBufTmpOffset;

	BYTE*		m_pBufferOut;	 // Output buffer
	DWORD		m_dwBufOutChunk; // min size (in bytes) of output buffer
	DWORD		m_dwBufOutOffset;// = N * m_dwBufOutChunk;

private:
	bool  InitEncoderDll();
	void  Close();

public:
	CEncoder_MP3();
	~CEncoder_MP3();

	int	  Init(CONF_DIAL_MP3* pDialConf);
	bool  EncodeChunk(BYTE* pBufIn, int nBufInSize);
	BYTE* GetBufferOut(int& nBufOutSize);
};
*/
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_ENCODER_MP3_H__EEB8AF0A_A74B_4B03_AA39_F1B8C01461B0__INCLUDED_)

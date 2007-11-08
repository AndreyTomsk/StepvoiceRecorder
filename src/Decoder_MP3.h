//////////////////////////////////////////////////////////////////////
#if !defined(AFX_DECODER_MP3_H__756F060D_0F79_4305_9D0C_D344D63C8BA7__INCLUDED_)
#define AFX_DECODER_MP3_H__756F060D_0F79_4305_9D0C_D344D63C8BA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mpglibdll.h"
#include "decoder.h"

//////////////////////////////////////////////////////////////////////
class CDecoder_MP3 : public CDecoder
{
	// DLL present flags
	HINSTANCE	 m_hDll;
	bool		 m_bDecoderInited;
	struct mpstr m_mp;

	bool		 m_bDllOK;
	//bool		 m_bDecoderInit;

	/*// DLL interface
	INITMP3		InitMP3;
	EXITMP3		ExitMP3;
	DECODEMP3	decodeMP3;*/

	// input (pre) buffer function and variables
	//FILL_PREBUF FillPreBuffer;
	char*		m_pPreBuf;
	int			m_nPreBufSize;
	bool		m_bUpdatePreBuf;

/*	void InitDecoderDll();
	void CloseDecoderDll();*/
protected:
	void ResetVariables();
	void LoadLibrary(CString& strDllName);
public:
	CDecoder_MP3(CString& strDllPath);
	CDecoder_MP3(CString& strDllPath, void* pSettings);
	virtual ~CDecoder_MP3();

	virtual bool InitDecoder(void* pSettings);
	virtual bool DecodeChunk(char* pBufIn,  int  nBufInSize,
							 char* pBufOut, int& nBufOutSize);
	virtual void CloseDecoder();
/*	void SetPreBufProc(FILL_PREBUF pNewProc);
	bool Init();
	bool DecodeChunk(char* pOutBuffer, int& nBytesToDecode);
	void Close();*/
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_DECODER_MP3_H__756F060D_0F79_4305_9D0C_D344D63C8BA7__INCLUDED_)

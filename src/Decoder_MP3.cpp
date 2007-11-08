//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mp3_recorder.h"
#include "Decoder_MP3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//====================================================================
INITMP3	  InitMp3   = NULL;
EXITMP3	  ExitMp3   = NULL;
DECODEMP3 DecodeMp3 = NULL;

//====================================================================
//  ŒÕ—“–” “Œ– / ƒ≈—“–” “Œ–
//====================================================================
CDecoder_MP3::CDecoder_MP3(CString& strDllPath)
: CDecoder(strDllPath)
{
	ResetVariables();
	LoadLibrary(strDllPath);
}

//--------------------------------------------------------------------
CDecoder_MP3::CDecoder_MP3(CString& strDllPath, void* pSettings)
: CDecoder(strDllPath, pSettings)
{
	ResetVariables();
	LoadLibrary(strDllPath);
	InitDecoder(pSettings);
}

//--------------------------------------------------------------------
CDecoder_MP3::~CDecoder_MP3()
{
	CloseDecoder();
	if(m_hDll)
		FreeLibrary(m_hDll);
}

//--------------------------------------------------------------------
void CDecoder_MP3::ResetVariables()
{
	m_bDecoderInited = false;
	ZeroMemory(&m_mp, sizeof(m_mp));
}

//====================================================================
// «¿√–”« ¿ ¡»¡À»Œ“≈ » ƒ≈ Œƒ≈–¿
//====================================================================
void CDecoder_MP3::LoadLibrary(CString& strDllPath)
{
	CString strDllName("\\mpglib.dll");
	strDllName = strDllPath + strDllName;

	m_hDll = ::LoadLibrary(strDllName.GetBuffer(strDllName.GetLength()));
	if(m_hDll != NULL)
	{
		InitMp3   = (INITMP3)	GetProcAddress(m_hDll, TEXT_INITMP3);
		ExitMp3	  = (EXITMP3)	GetProcAddress(m_hDll, TEXT_EXITMP3);
		DecodeMp3 = (DECODEMP3)	GetProcAddress(m_hDll, TEXT_DECODEMP3);

		if(!(InitMp3 && ExitMp3 && DecodeMp3))
		{
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}
}

//====================================================================
// »Õ“≈–‘≈…—Õ€≈ ‘”Õ ÷»»
//====================================================================
bool CDecoder_MP3::InitDecoder(void* pSettings)
{
	//if(CDecoder::InitDecoder(pSettings))
	if(m_hDll)
		m_bDecoderInited = InitMp3(&m_mp) != 0;

	return m_bDecoderInited;
}

//--------------------------------------------------------------------
bool CDecoder_MP3::DecodeChunk(char* pBufIn,  int  nBufInSize,
							   char* pBufOut, int& nBufOutSize)
{
	int nResult = MP3_ERR, nBytesDecoded = 0;

	if(m_bDecoderInited)
	{	// !!!!!! - ‚˚ÎÂÚ‡ÂÚ Ì‡ 1-È ÏËÌÛÚÂ 8.mp3
		try
		{	nResult = DecodeMp3(&m_mp, pBufIn,  nBufInSize,	pBufOut,
				nBufOutSize, &nBytesDecoded);
		}
		catch(...)
		{
		};
		if(nResult == MP3_ERR)
			InitMp3(&m_mp);
		nBufOutSize = nBytesDecoded;
	}

	return (nResult == MP3_OK);
}

//--------------------------------------------------------------------
void CDecoder_MP3::CloseDecoder()
{
	if(m_bDecoderInited)
	{
		ExitMp3(&m_mp);
		ResetVariables();
	}
	CDecoder::CloseDecoder();
}

//====================================================================
/*//////////////////////////////////////////////////////////////////////
void CDecoder_MP3::SetPreBufProc(FILL_PREBUF pNewProc)
{
	FillPreBuffer = pNewProc;
}

//////////////////////////////////////////////////////////////////////
CDecoder_MP3::CDecoder_MP3()
{
	m_bDllOK = false;
	m_bDecoderInited = false;
	InitDecoderDll();
	// init dll structure
	ZeroMemory(&m_mp, sizeof(m_mp));
	// init prebuffer data
	FillPreBuffer   = NULL;
	m_pPreBuf	    = NULL;
	m_nPreBufSize   = 0;
	m_bUpdatePreBuf = true;
}


CDecoder_MP3::~CDecoder_MP3()
{	
	Close(); // close mpeg decoder
	CloseDecoderDll();
}

//////////////////////////////////////////////////////////////////////
void CDecoder_MP3::InitDecoderDll()
{	// init dll
	m_hDll = LoadLibrary("mpglib.dll");
	if(m_hDll == NULL)
		return;
	// check for dll functions
	InitMP3   = (INITMP3)   GetProcAddress(m_hDll, TEXT_INITMP3);
	ExitMP3   = (EXITMP3)   GetProcAddress(m_hDll, TEXT_EXITMP3);
	decodeMP3 = (DECODEMP3) GetProcAddress(m_hDll, TEXT_DECODEMP3);
	if(!InitMP3 || !ExitMP3 || !decodeMP3)
		return;
	m_bDllOK = true;
}


void CDecoder_MP3::CloseDecoderDll()
{
	if(m_bDllOK) FreeLibrary(m_hDll);
}

//////////////////////////////////////////////////////////////////////
bool CDecoder_MP3::Init()
{
	if(m_bDllOK) m_bDecoderInited = InitMP3(&m_mp) != 0;
	return m_bDecoderInited;
}


void CDecoder_MP3::Close()
{
	if(!m_bDecoderInited)
		return;
	ExitMP3(&m_mp);
	m_bDecoderInited = false;
}

//////////////////////////////////////////////////////////////////////
bool CDecoder_MP3::DecodeChunk(char* pOutBuf, int& nBytesToDecode)
{
	//int nRet, nBytesDecoded = 0;

	// check if no library present or it is damaged
	if(!m_hDll || !m_bDecoderInited) return false;
/*
	if(m_bUpdatePreBuf) { // error or end of prebuffer
		m_pPreBuf = FillPreBuffer(m_nPreBufSize); // update pre buffer
		// decode chunk of prebuffer
		nRet = decodeMP3(&m_mp, m_pPreBuf, m_nPreBufSize, pOutBuf,
			nBytesToDecode, &nBytesDecoded);
		if(nRet == MP3_ERR) InitMP3(&m_mp);
		else
			m_bUpdatePreBuf = false;
	}
	else {
		// decode chunk of prebuffer
		nRet = decodeMP3(&m_mp, 0, 0, pOutBuf, nBytesToDecode,
			&nBytesDecoded);
		// prevent program crash
		if(nRet == MP3_ERR) {
			InitMP3(&m_mp);
			m_bUpdatePreBuf = true;
		}
		if(nRet == MP3_NEED_MORE) m_bUpdatePreBuf = true;
	}
	nBytesToDecode = nBytesDecoded; // set really decoded size
*/	// prevent program crash
	//if(nRet == MP3_ERR) InitMP3(&m_mp);
	//m_bUpdatePreBuf = (nRet == MP3_NEED_MORE); // true if was error
	//if(m_bUpdatePreBuf)
	//	goto begin;

	/*int size=0;
	int buff_size=0;
	char out_chunk[8192];
	char* out_buff=pOutBuf;
	int ret;

	m_pPreBuf = FillPreBuffer(m_nPreBufSize);
	if(!m_pPreBuf)
		return false;
	ret = decodeMP3(&m_mp, m_pPreBuf, m_nPreBufSize, out_chunk, 8192, &size);
	if (ret==MP3_ERR) InitMP3(&m_mp);

	while(ret == MP3_OK) {
		CopyMemory (out_buff,out_chunk,size);
		out_buff += size;
		buff_size += size;
		ret = decodeMP3(&m_mp, NULL, 0, out_chunk, 8192, &size);
		if (ret==MP3_ERR) InitMP3(&m_mp);
	}
	nBytesToDecode = buff_size;

	return true;
}
//////////////////////////////////////////////////////////////////////
*/
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mp3_recorder.h"
#include "Encoder_MP3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//====================================================================
BEINITSTREAM		beInitStream	 = NULL;
BEENCODECHUNK		beEncodeChunk	 = NULL;
BEDEINITSTREAM		beDeinitStream	 = NULL;
BECLOSESTREAM		beCloseStream	 = NULL;
BEVERSION			beVersion		 = NULL;
BEWRITEVBRHEADER	beWriteVBRHeader = NULL;

//====================================================================
// КОНСТРУКТОР / ДЕСТРУКТОР
//====================================================================
CEncoder_MP3::CEncoder_MP3(CString& strDllPath)
: CEncoder(strDllPath)
{
	m_hbeStream	= NULL;
	m_pChunkBuf	= NULL;

	m_pMp3Buf = NULL;

	CloseEncoder();
	LoadLibrary(strDllPath);
}

//--------------------------------------------------------------------
CEncoder_MP3::CEncoder_MP3(CString& strDllPath, void* pSettings)
: CEncoder(strDllPath, pSettings)
{
	m_hbeStream	= NULL;
	m_pChunkBuf	= NULL;

	LoadLibrary(strDllPath);
	InitEncoder(pSettings);
}

//--------------------------------------------------------------------
CEncoder_MP3::~CEncoder_MP3()
{
	CloseEncoder();
	if(m_hDll)
		FreeLibrary(m_hDll);
}
  
//====================================================================
// ЗАГРУЗКА БИБЛИОТЕКИ КОДЕРА
//====================================================================
void CEncoder_MP3::LoadLibrary(CString& strDllPath)
{
	CString strDllName("\\lame_enc.dll");
	strDllName = strDllPath + strDllName;

	m_hDll = ::LoadLibrary(strDllName.GetBuffer(strDllName.GetLength()));
	if(m_hDll != NULL)
	{
		beInitStream	= (BEINITSTREAM)    GetProcAddress(m_hDll, TEXT_BEINITSTREAM);
		beEncodeChunk	= (BEENCODECHUNK)   GetProcAddress(m_hDll, TEXT_BEENCODECHUNK);
		beDeinitStream	= (BEDEINITSTREAM)  GetProcAddress(m_hDll, TEXT_BEDEINITSTREAM);
		beCloseStream	= (BECLOSESTREAM)   GetProcAddress(m_hDll, TEXT_BECLOSESTREAM);
		beVersion		= (BEVERSION)       GetProcAddress(m_hDll, TEXT_BEVERSION);
		beWriteVBRHeader= (BEWRITEVBRHEADER)GetProcAddress(m_hDll, TEXT_BEWRITEVBRHEADER);

		bool bLoaded = (beInitStream && beEncodeChunk && beDeinitStream
			&& beCloseStream && beVersion && beWriteVBRHeader);

		if(!bLoaded)
		{
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}
}

//====================================================================
// ИНТЕРФЕЙСНЫЕ ФУНКЦИИ
//====================================================================
int CEncoder_MP3::InitEncoder(void* pSettings)
{
	CloseEncoder();

	if(CEncoder::InitEncoder(pSettings) != ENC_NOERROR)
	{	
		CloseEncoder();
		return ENC_ERR_UNK;
	}

	if(!m_hDll)
	{
		CloseEncoder();
		return ENC_ERR_DLL;
	}

	// получаем указатель на структуру настроек
	CONF_DIAL_MP3* pMp3Conf = (CONF_DIAL_MP3 *)pSettings;

	// инициализируем структуру настроек кодера
	int nModeTbl[] = {BE_MP3_MODE_MONO, BE_MP3_MODE_STEREO};
	m_beConfig.dwConfig						= BE_CONFIG_LAME;
	m_beConfig.format.LHV1.dwStructVersion	= 1;
	m_beConfig.format.LHV1.dwStructSize		= sizeof(m_beConfig);		
	m_beConfig.format.LHV1.dwSampleRate		= pMp3Conf->nFreq;
	m_beConfig.format.LHV1.dwReSampleRate	= pMp3Conf->nFreq;
	m_beConfig.format.LHV1.dwBitrate		= pMp3Conf->nBitrate;
	m_beConfig.format.LHV1.nMode			= nModeTbl[pMp3Conf->nStereo != 0];
	m_beConfig.format.LHV1.nPreset			= LQP_NOPRESET;
	m_beConfig.format.LHV1.bNoRes			= TRUE;					// No Bit resorvoir
	// mpeg version
	bool bMpeg2 = ((pMp3Conf->nBitrate == 32) && (pMp3Conf->nFreq < 22050)) || (pMp3Conf->nBitrate < 32);
	m_beConfig.format.LHV1.dwMpegVersion = (bMpeg2) ? MPEG2 : MPEG1;

	// инициализируем поток кодера
	DWORD dwSamples, dwBufOutMinSize;
	BE_ERR err = beInitStream(&m_beConfig, &dwSamples,
		&dwBufOutMinSize, &m_hbeStream);
	if(err != BE_ERR_SUCCESSFUL)
	{
		CloseEncoder();
		return ENC_ERR_INIT;
	}

	m_pMp3Buf = new char[dwBufOutMinSize];

	// инициализируем внутренний буфер семплов, размером dwSamples
	m_nChunkBufSamples = dwSamples;
	m_nChunkBufSize	   = dwSamples * 2;//pMp3Conf->nBits/8;
	m_pChunkBuf		   = new char[m_nChunkBufSize];
	m_nChunkBufOffset  = 0;

	return ENC_NOERROR;


/*
	do
	{	if(!CEncoder::InitEncoder(pSettings))
			break;
		if(!m_hDll)
			break;

		// получаем указатель на структуру настроек
		CONF_DIAL_MP3* pMp3Conf = (CONF_DIAL_MP3 *)pSettings;

		// инициализируем структуру настроек кодера
		int nModeTbl[] = {BE_MP3_MODE_MONO, BE_MP3_MODE_STEREO};
		m_beConfig.dwConfig						= BE_CONFIG_LAME;
		m_beConfig.format.LHV1.dwStructVersion	= 1;
		m_beConfig.format.LHV1.dwStructSize		= sizeof(m_beConfig);		
		m_beConfig.format.LHV1.dwSampleRate		= pMp3Conf->nFreq;
		//m_beConfig.format.LHV1.dwReSampleRate	= pMp3Conf->nFreq;
		m_beConfig.format.LHV1.dwBitrate		= pMp3Conf->nBitrate;
		m_beConfig.format.LHV1.nMode			= nModeTbl[pMp3Conf->nStereo != 0];
		m_beConfig.format.LHV1.nPreset			= LQP_NOPRESET;
		m_beConfig.format.LHV1.bNoRes			= TRUE;					// No Bit resorvoir
		// mpeg version
		bool bMpeg2 = ((pMp3Conf->nBitrate == 32) && (pMp3Conf->nFreq < 22050)) || (pMp3Conf->nBitrate < 32);
		m_beConfig.format.LHV1.dwMpegVersion = (bMpeg2) ? MPEG2 : MPEG1;

		// инициализируем поток кодера
		DWORD dwSamples, dwBufOutMinSize;
		BE_ERR err = beInitStream(&m_beConfig, &dwSamples,
			&dwBufOutMinSize, &m_hbeStream);
		if(err != BE_ERR_SUCCESSFUL)
			break;

		m_pMp3Buf = new char[dwBufOutMinSize];

		// инициализируем внутренний буфер семплов, размером dwSamples
		m_nChunkBufSamples = dwSamples;
		m_nChunkBufSize	   = dwSamples * 2;//pMp3Conf->nBits/8;
		m_pChunkBuf		   = new char[m_nChunkBufSize];
		m_nChunkBufOffset  = 0;

		return true;
	} while(0);

	CloseEncoder();
	return false;
*/
}

//--------------------------------------------------------------------
bool CEncoder_MP3::EncodeChunk(char* pBufIn,  int  nBufInSize,
							   char* pBufOut, int& nBufOutSize)
{
	nBufOutSize = 0;

	// запоминаем указатель на буфер (для итераций)
	if(pBufIn && nBufInSize)
	{
		m_pBufIn	 = pBufIn;
		m_nBufInSize = nBufInSize;
	}

	// определяем, скока байтов надо для заполнения внутреннего буфера
	int nFillBytes = m_nChunkBufSize - m_nChunkBufOffset;
	nFillBytes = (nFillBytes <= m_nBufInSize) ? nFillBytes : m_nBufInSize;

	// заполняем внутренний буфер и обновляем переменные
	memcpy(m_pChunkBuf + m_nChunkBufOffset, m_pBufIn, nFillBytes);
	m_pBufIn		  += nFillBytes;
	m_nBufInSize	  -= nFillBytes;
	m_nChunkBufOffset += nFillBytes;

	if(m_nChunkBufOffset != m_nChunkBufSize)
		return false; // буфер заполнен не до конца

	m_nChunkBufOffset = 0;

	DWORD dwBytesEncoded = 0;
	BE_ERR err = beEncodeChunk(m_hbeStream, m_nChunkBufSamples,
		(short *)m_pChunkBuf, (BYTE *)pBufOut, &dwBytesEncoded);

	if(err == BE_ERR_SUCCESSFUL)
	{
		nBufOutSize = dwBytesEncoded;
	}

	return (nBufOutSize) ? true : false;
}

//--------------------------------------------------------------------
void CEncoder_MP3::CloseEncoder()
{
	if(m_hbeStream)
	{
		beCloseStream(m_hbeStream);		
		m_hbeStream	= NULL;
	}
	if(m_pChunkBuf)
	{
		delete [] m_pChunkBuf;
		m_pChunkBuf	= NULL;
	}
	ZeroMemory(&m_beConfig, sizeof(m_beConfig));
	m_nChunkBufOffset	= 0;
	m_nChunkBufSamples	= 0;
	m_nChunkBufSize		= 0;
	m_pBufIn			= NULL;
	m_nBufInSize		= 0;

	CEncoder::CloseEncoder();

	if(m_pMp3Buf)
	{
		delete [] m_pMp3Buf;
		m_pMp3Buf = NULL;
	}
}

//====================================================================
/*
CEncoder_MP3::CEncoder_MP3()
{
	ZeroMemory(&m_flags, sizeof(ENC_FLAGS));
	m_flags.bDllOK = InitEncoderDll();

	m_pBufferOut	= new BYTE[15000];
	m_dwBufOutChunk	= 0;
	m_dwBufOutOffset= 0;

	m_pBufferTmp	= NULL;
	m_dwBufTmpSize	= 0;
	m_dwBufTmpOffset= 0;
}


//////////////////////////////////////////////////////////////////////
CEncoder_MP3::~CEncoder_MP3()
{
	Close();
	if(m_flags.bDllOK)
		FreeLibrary(m_hDll);
	if(m_pBufferOut) delete [] m_pBufferOut;
	if(m_pBufferTmp) delete [] m_pBufferTmp;
}


//////////////////////////////////////////////////////////////////////
bool CEncoder_MP3::InitEncoderDll()
{
	m_hDll = LoadLibrary("lame_enc.dll");
	if(m_hDll != NULL) {
		beInitStream	= (BEINITSTREAM)    GetProcAddress(m_hDll, TEXT_BEINITSTREAM);
		beEncodeChunk	= (BEENCODECHUNK)   GetProcAddress(m_hDll, TEXT_BEENCODECHUNK);
		beDeinitStream	= (BEDEINITSTREAM)  GetProcAddress(m_hDll, TEXT_BEDEINITSTREAM);
		beCloseStream	= (BECLOSESTREAM)   GetProcAddress(m_hDll, TEXT_BECLOSESTREAM);
		beVersion		= (BEVERSION)       GetProcAddress(m_hDll, TEXT_BEVERSION);
		beWriteVBRHeader= (BEWRITEVBRHEADER)GetProcAddress(m_hDll, TEXT_BEWRITEVBRHEADER);
		if(beInitStream && beEncodeChunk && beDeinitStream && beCloseStream
			&& beVersion && beWriteVBRHeader)
			return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
void CEncoder_MP3::Close()
{
	if(m_flags.bDllOK && m_flags.bEncOK)
	{
		beCloseStream(m_hbeStream);
		m_dwBufOutOffset= 0;		// zero out buffer offset
		m_dwBufTmpOffset= 0;		// zero tmp buffer offset
		m_flags.bEncOK	= false;
	}
}


//////////////////////////////////////////////////////////////////////
int CEncoder_MP3::Init(CONF_DIAL_MP3* pDialConf)
{
	int   nModeTbl[] = {BE_MP3_MODE_MONO, BE_MP3_MODE_JSTEREO};
	bool  bMpegVersion;
	DWORD dwSamples;

	if(!m_flags.bDllOK) return ENC_ERR_DLL;

	if(pDialConf) {
		Close();
	// struct initializing
		ZeroMemory(&m_beConfig, sizeof(m_beConfig));
		m_beConfig.dwConfig = BE_CONFIG_LAME;
		m_beConfig.format.LHV1.dwStructVersion	= 1;
		m_beConfig.format.LHV1.dwStructSize		= sizeof(m_beConfig);		
		m_beConfig.format.LHV1.dwSampleRate		= pDialConf->nFreq;
		m_beConfig.format.LHV1.dwBitrate		= pDialConf->nBitrate;
		m_beConfig.format.LHV1.nMode			= nModeTbl[pDialConf->nStereo!=0];
		m_beConfig.format.LHV1.nPreset			= LQP_NOPRESET;
		// mpeg version
		bMpegVersion  = (pDialConf->nBitrate == 32);
		bMpegVersion &= (pDialConf->nFreq < 22050);
		m_beConfig.format.LHV1.dwMpegVersion = (bMpegVersion) ? MPEG2 : MPEG1;
	// stream initializing
		BE_ERR err = beInitStream(&m_beConfig, &dwSamples,
			&m_dwBufOutChunk, &m_hbeStream);
		if(err == BE_ERR_SUCCESSFUL) {
			m_flags.bEncOK = true;
		// create temp buffer
			m_dwBufTmpSize	= dwSamples*2;
			m_pBufferTmp	= new BYTE[m_dwBufTmpSize];

			return ENC_ERR_OK;
		}
	} // pDialConf
	return ENC_ERR_INIT;
}


//////////////////////////////////////////////////////////////////////
bool CEncoder_MP3::EncodeChunk(BYTE* pBufIn, int nBufInSize)
{
	DWORD	dwBufInLength, dwBufTmpLength;
	DWORD	dwBufInOffset, dwBytesEncoded;

// OK checking
	if(!m_flags.bDllOK || !m_flags.bEncOK) return false;

// buffers init
	m_dwBufOutOffset= 0;
	dwBufInOffset	= 0;
	dwBufInLength	= nBufInSize - dwBufInOffset;
	dwBufTmpLength	= m_dwBufTmpSize - m_dwBufTmpOffset;

// fill Out buffer
	while (dwBufInLength > dwBufTmpLength)
	{
		memcpy(&m_pBufferTmp[m_dwBufTmpOffset], &pBufIn[dwBufInOffset],
			dwBufTmpLength);
		BE_ERR err = beEncodeChunk(m_hbeStream, m_dwBufTmpSize/2,
			(short*)m_pBufferTmp, &m_pBufferOut[m_dwBufOutOffset],
			&dwBytesEncoded);
		if(err != BE_ERR_SUCCESSFUL)
		{
			m_dwBufTmpOffset = 0;
			return false;
		}
		dwBufInOffset	 += dwBufTmpLength;
		m_dwBufTmpOffset  = 0;
		m_dwBufOutOffset += dwBytesEncoded;

		dwBufInLength	= nBufInSize - dwBufInOffset;
		dwBufTmpLength	= m_dwBufTmpSize;
	}
// store data with length < Tmp buffer length
	memcpy(m_pBufferTmp, &pBufIn[dwBufInOffset], dwBufInLength);
	m_dwBufTmpOffset += dwBufInLength;
	return true;
}


//////////////////////////////////////////////////////////////////////
BYTE* CEncoder_MP3::GetBufferOut(int& nBufOutSize)
{
	nBufOutSize = m_dwBufOutOffset;
	return m_pBufferOut;
}
*/
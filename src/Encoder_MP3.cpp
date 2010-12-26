///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mp3_recorder.h"
#include "Encoder_MP3.h"
#include "common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEINITSTREAM		beInitStream	 = NULL;
BEENCODECHUNK		beEncodeChunk	 = NULL;
BEENCODECHUNKFLOATS16NI beEncodeChunkFloatS16NI = NULL;
BEDEINITSTREAM		beDeinitStream	 = NULL;
BECLOSESTREAM		beCloseStream	 = NULL;
BEVERSION			beVersion		 = NULL;
BEWRITEVBRHEADER	beWriteVBRHeader = NULL;
BEWRITEINFOTAG		beWriteInfoTag   = NULL;

///////////////////////////////////////////////////////////////////////////////
CEncoder_MP3::CEncoder_MP3(int nBitrate, int nFrequency, int nChannels)
	:m_pChunkBuf(NULL)
	,m_hbeStream(NULL)
	,m_hDll(0)
{
	CString l_app_dir = ((CMP3_RecorderApp*)AfxGetApp())->GetProgramDir();
	LoadLibrary(l_app_dir);
	InitEncoder(nBitrate, nFrequency, nChannels);
}

//-----------------------------------------------------------------------------
CEncoder_MP3::~CEncoder_MP3()
{
	CloseEncoder();
	FreeLibrary();
}
  
///////////////////////////////////////////////////////////////////////////////
void CEncoder_MP3::LoadLibrary(CString& strDllPath)
{
	CString strDllName = strDllPath + _T("\\lame_enc.dll");

	m_hDll = ::LoadLibrary(strDllName.GetBuffer(strDllName.GetLength()));
	if (m_hDll != NULL)
	{
		beInitStream	= (BEINITSTREAM)    GetProcAddress(m_hDll, TEXT_BEINITSTREAM);
		beEncodeChunk	= (BEENCODECHUNK)   GetProcAddress(m_hDll, TEXT_BEENCODECHUNK);
		beEncodeChunkFloatS16NI = (BEENCODECHUNKFLOATS16NI) GetProcAddress(m_hDll, TEXT_BEENCODECHUNKFLOATS16NI);
		beDeinitStream	= (BEDEINITSTREAM)  GetProcAddress(m_hDll, TEXT_BEDEINITSTREAM);
		beCloseStream	= (BECLOSESTREAM)   GetProcAddress(m_hDll, TEXT_BECLOSESTREAM);
		beVersion		= (BEVERSION)       GetProcAddress(m_hDll, TEXT_BEVERSION);
		beWriteVBRHeader= (BEWRITEVBRHEADER)GetProcAddress(m_hDll, TEXT_BEWRITEVBRHEADER);
		beWriteInfoTag  = (BEWRITEINFOTAG)  GetProcAddress(m_hDll, TEXT_BEWRITEINFOTAG);

		bool l_all_functions = (beInitStream && beEncodeChunk && beDeinitStream
			&& beCloseStream && beVersion && beWriteVBRHeader);
		if (!l_all_functions)
		{
			FreeLibrary();
			throw CString(_T("Invalid 'lame_enc.dll' library."));
		}
	}
	else
		throw CString(_T("Lame encoder library not found by path:\n") + strDllName);
}

//-----------------------------------------------------------------------------
void CEncoder_MP3::FreeLibrary()
{
	if (m_hDll)
	{
		::FreeLibrary(m_hDll);
		m_hDll = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
void CEncoder_MP3::InitEncoder(int nBitrate, int nFrequency, int nChannels)
{
	CloseEncoder();

	// Adjusting encoder settings.
	m_beConfig.dwConfig						= BE_CONFIG_LAME;
	m_beConfig.format.LHV1.dwStructVersion	= 1;
	m_beConfig.format.LHV1.dwStructSize		= sizeof(m_beConfig);		
	m_beConfig.format.LHV1.dwSampleRate		= nFrequency;
	m_beConfig.format.LHV1.dwReSampleRate	= nFrequency;
	m_beConfig.format.LHV1.dwBitrate		= nBitrate;
	m_beConfig.format.LHV1.nMode			= nChannels == 1 ? BE_MP3_MODE_MONO : BE_MP3_MODE_STEREO;
	m_beConfig.format.LHV1.nPreset			= LQP_NOPRESET;
	m_beConfig.format.LHV1.bNoRes			= TRUE;			// No Bit reservoir
	
	// mpeg version
	bool bMpeg2 = (nBitrate < 32) || (nBitrate == 32 && nFrequency < 22050);
	m_beConfig.format.LHV1.dwMpegVersion = (bMpeg2) ? MPEG2 : MPEG1;

	DWORD dwSamples = 0, dwBufOutMinSize = 0;
	BE_ERR err = beInitStream(&m_beConfig, &dwSamples, &dwBufOutMinSize, &m_hbeStream);
	if (err != BE_ERR_SUCCESSFUL)
		throw CString(_T("Failed to initialize lame encoder."));

	// инициализируем внутренний буфер семплов, размером dwSamples
	m_nChunkBufSamples = dwSamples;
	m_nChunkBufSize	   = dwSamples * 2;
	m_pChunkBuf		   = new char[m_nChunkBufSize];
}

//-----------------------------------------------------------------------------
void CEncoder_MP3::CloseEncoder()
{
	if (m_hbeStream)
	{
		beCloseStream(m_hbeStream);		
		m_hbeStream	= NULL;
	}

	ZeroMemory(&m_beConfig, sizeof(m_beConfig));
	m_nChunkBufOffset	= 0;
	m_nChunkBufSamples	= 0;
	m_nChunkBufSize		= 0;
	m_pBufIn			= NULL;
	m_nBufInSize		= 0;

	SAFE_DELETE_ARRAY(m_pChunkBuf);
}

//-----------------------------------------------------------------------------
bool CEncoder_MP3::EncodeChunk(char* pBufIn,  int  nBufInSize,
							   char* pBufOut, int& nBufOutSize)
{
	// Getting pointer to input buffer for iterations.
	if (pBufIn && nBufInSize)
	{
		m_pBufIn	 = pBufIn;
		m_nBufInSize = nBufInSize;
	}
	// Calculating how much bytes we need for filling internal chunk buffer.
	int nFillBytes = m_nChunkBufSize - m_nChunkBufOffset;
	nFillBytes = (nFillBytes < m_nBufInSize) ? nFillBytes : m_nBufInSize;

	memcpy(m_pChunkBuf + m_nChunkBufOffset, m_pBufIn, nFillBytes);

	m_pBufIn		  += nFillBytes;
	m_nBufInSize	  -= nFillBytes;
	m_nChunkBufOffset += nFillBytes;

	if (m_nChunkBufOffset != m_nChunkBufSize)
	{	
		///@note Chunk buffer is not full, because nBufInSize doesn't have
		///whole number of chunks. We copied all data from input buffer, now
		///we return false and will wait for a new input buffer.
		return false;
	}
	m_nChunkBufOffset = 0;

	DWORD dwBytesEncoded = 0;
	BE_ERR err = beEncodeChunk(m_hbeStream, m_nChunkBufSamples,
		(short *)m_pChunkBuf, (BYTE *)pBufOut, &dwBytesEncoded);

	nBufOutSize = (BE_ERR_SUCCESSFUL == err) ? dwBytesEncoded : 0;
	return (nBufOutSize) ? true : false;
}

/*
///////////////////////////////////////////////////////////////////////////////
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
*/
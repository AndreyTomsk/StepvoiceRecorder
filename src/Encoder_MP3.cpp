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

#ifdef _DEBUG
const TCHAR* g_lameLibraryName = _T("lame_enc_d.dll");
#else
const TCHAR* g_lameLibraryName = _T("lame_enc.dll");
#endif

///////////////////////////////////////////////////////////////////////////////

CEncoder_MP3::CEncoder_MP3(int nBitrate, int nFrequency, int nChannels)
	:m_pChunkBuf(NULL)
	,m_hbeStream(NULL)
	,m_hDll(0)
	,m_chunkBufFloat_l(NULL)
	,m_chunkBufFloat_r(NULL)
	,m_chunkBufFloatSize(0)
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
//-----------------------------------------------------------------------------
  
void CEncoder_MP3::LoadLibrary(CString& strDllPath)
{
	CString strDllName = strDllPath + _T("\\") + g_lameLibraryName;

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
//-----------------------------------------------------------------------------

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
	m_beConfig.format.LHV1.nMode			= nChannels == 1 ? BE_MP3_MODE_MONO : BE_MP3_MODE_JSTEREO;
	m_beConfig.format.LHV1.nPreset			= LQP_NOPRESET;
	m_beConfig.format.LHV1.bNoRes			= TRUE;			// No Bit reservoir
	m_beConfig.format.LHV1.bWriteVBRHeader	= TRUE;
	//m_beConfig.format.LHV1.dwPsyModel		= 0;			// USE DEFAULT PSYCHOACOUSTIC MODEL 
	//m_beConfig.format.LHV1.dwEmphasis		= 0;			// NO EMPHASIS TURNED ON

	// mpeg version
	bool bMpeg2 = (nBitrate < 32) || (nBitrate == 32 && nFrequency < 22050);
	m_beConfig.format.LHV1.dwMpegVersion = (bMpeg2) ? MPEG2 : MPEG1;

	DWORD dwSamples = 0, dwBufOutMinSize = 0;
	BE_ERR err = beInitStream(&m_beConfig, &dwSamples, &dwBufOutMinSize, &m_hbeStream);
	if (err != BE_ERR_SUCCESSFUL)
		throw CString(_T("Failed to initialize lame encoder."));

	// Creating internal dwSamples size buffer for source data.
	m_nChunkBufSize = dwSamples * 2; // Samples has 'short' type.
	m_pChunkBuf = new char[m_nChunkBufSize];

	m_chunkBufFloatSize = dwSamples;
	m_chunkBufFloat_l = new float[m_chunkBufFloatSize];
	m_chunkBufFloat_r = new float[m_chunkBufFloatSize];
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
	SAFE_DELETE_ARRAY(m_pChunkBuf);
	m_nChunkBufSize = 0;

	SAFE_DELETE_ARRAY(m_chunkBufFloat_l);
	SAFE_DELETE_ARRAY(m_chunkBufFloat_r);
	m_chunkBufFloatSize = 0;
}
//-----------------------------------------------------------------------------

bool CEncoder_MP3::EncodeChunk(char* pBufIn,  int  nBufInSize,
							   char* pBufOut, int& nBufOutSize)
{
	int l_input_buffer_offset  = 0;
	int l_output_buffer_offset = 0;

	nBufOutSize = 0;

	// Dividing source buffer into chunks and encoding each.
	while (l_input_buffer_offset < nBufInSize)
	{
		// Taking current chunk.
	    int l_available_count = nBufInSize - l_input_buffer_offset;
		int l_copy_count = min(m_nChunkBufSize, l_available_count);
		memcpy(m_pChunkBuf, pBufIn + l_input_buffer_offset, l_copy_count);
		l_input_buffer_offset += l_copy_count;

		// Encoding samples of type 'short'. Data saved in output buffer.
		DWORD dwBytesEncoded = 0;
		BE_ERR err = beEncodeChunk(m_hbeStream, l_copy_count / sizeof(short), (short *)m_pChunkBuf,
			(BYTE *)pBufOut + l_output_buffer_offset, &dwBytesEncoded);
		
		if (err != BE_ERR_SUCCESSFUL)
			break;

		l_output_buffer_offset += dwBytesEncoded;
		nBufOutSize += dwBytesEncoded;
	}

	return (l_input_buffer_offset == nBufInSize) ? true : false;
}
//-----------------------------------------------------------------------------

bool CEncoder_MP3::EncodeChunkFloat(float* pBufIn, int nBufInSize,
									char* pBufOut, int& nBufOutSize)
{
	int l_input_buffer_offset  = 0;
	int l_output_buffer_offset = 0;

	nBufOutSize = 0;

	int nChannels = (m_beConfig.format.LHV1.nMode == BE_MP3_MODE_MONO) ? 1 : 2;
	ASSERT(nBufInSize % nChannels == 0);

	// Dividing source buffer into chunks and encoding each.
	while (l_input_buffer_offset < nBufInSize)
	{
		// Taking current chunk. Splitting source to left and right sample buffers
		int l_available_count = nBufInSize - l_input_buffer_offset;
		int l_copy_count = min(m_chunkBufFloatSize * nChannels, l_available_count);

		float* l_current_chunk = pBufIn + l_input_buffer_offset;
		for (int i = 0, j = 0; i < l_copy_count; i += nChannels)
		{
			ASSERT (j < m_chunkBufFloatSize);
			m_chunkBufFloat_l[j] = l_current_chunk[i] * 32768;
			if (nChannels > 1)
				m_chunkBufFloat_r[j] = l_current_chunk[i + 1] * 32768;
			j++;
		}
		l_input_buffer_offset += l_copy_count;

		// Encoding float samples. Data saved in output buffer.
		DWORD dwBytesEncoded = 0;
		BE_ERR err = beEncodeChunkFloatS16NI(m_hbeStream, l_copy_count/nChannels, m_chunkBufFloat_l, m_chunkBufFloat_r,
			(BYTE *)pBufOut + l_output_buffer_offset, &dwBytesEncoded);

		if (err != BE_ERR_SUCCESSFUL)
			break;

		l_output_buffer_offset += dwBytesEncoded;
		nBufOutSize += dwBytesEncoded;
	}

	return (l_input_buffer_offset == nBufInSize) ? true : false;
}
//-----------------------------------------------------------------------------

void CEncoder_MP3::WriteVBRHeader(const CString& mp3FileName)
{
	/*
	// Checking file exists (and closed?).
	HANDLE l_file_handle = CreateFile(
		mp3FileName,
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (l_file_handle == INVALID_HANDLE_VALUE)
		throw CString(_T("Unable to write VBR header - file is locked."));

	CloseHandle(l_file_handle);
	*/
	beWriteVBRHeader(mp3FileName);
}
//-----------------------------------------------------------------------------

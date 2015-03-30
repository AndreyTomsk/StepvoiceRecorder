#include "stdafx.h"
#include "mp3_recorder.h" //for GetProgramDir
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

/////////////////////////////////////////////////////////////////////////////

CEncoder_MP3::CEncoder_MP3(int bitrate, int frequency, int channels)
	:m_hbeStream(NULL)
	,m_hDll(0)
	,m_chunkBufFloat_l(NULL)
	,m_chunkBufFloat_r(NULL)
	,m_chunkBufFloatSize(0)
{
	LoadLameLibrary();
	InitEncoder(bitrate, frequency, channels);
}
//---------------------------------------------------------------------------

CEncoder_MP3::~CEncoder_MP3()
{
	CloseEncoder();
	FreeLameLibrary();
}
//---------------------------------------------------------------------------
  
void CEncoder_MP3::LoadLameLibrary()
{
	const CString appDir = ((CMP3_RecorderApp*)AfxGetApp())->GetProgramDir();
	const CString lameFullPath = appDir + _T("\\lame_enc.dll");

	m_hDll = ::LoadLibrary(lameFullPath);
	if (m_hDll == NULL)
		throw CString(_T("'lame_enc.dll' library not found, file path:\n") + lameFullPath);

	beInitStream	= (BEINITSTREAM)    GetProcAddress(m_hDll, TEXT_BEINITSTREAM);
	beEncodeChunk	= (BEENCODECHUNK)   GetProcAddress(m_hDll, TEXT_BEENCODECHUNK);
	beEncodeChunkFloatS16NI = (BEENCODECHUNKFLOATS16NI) GetProcAddress(m_hDll, TEXT_BEENCODECHUNKFLOATS16NI);
	beDeinitStream	= (BEDEINITSTREAM)  GetProcAddress(m_hDll, TEXT_BEDEINITSTREAM);
	beCloseStream	= (BECLOSESTREAM)   GetProcAddress(m_hDll, TEXT_BECLOSESTREAM);
	beVersion		= (BEVERSION)       GetProcAddress(m_hDll, TEXT_BEVERSION);
	beWriteVBRHeader= (BEWRITEVBRHEADER)GetProcAddress(m_hDll, TEXT_BEWRITEVBRHEADER);
	beWriteInfoTag  = (BEWRITEINFOTAG)  GetProcAddress(m_hDll, TEXT_BEWRITEINFOTAG);

	bool allFunctionsFound = (beInitStream && beEncodeChunk && beDeinitStream
		&& beCloseStream && beVersion && beWriteVBRHeader);

	if (!allFunctionsFound)
	{
		FreeLameLibrary();
		throw CString(_T("'lame_enc.dll' file is corrupted."));
	}
}
//---------------------------------------------------------------------------

void CEncoder_MP3::FreeLameLibrary()
{
	if (m_hDll)
	{
		::FreeLibrary(m_hDll);
		m_hDll = 0;
	}
}
//---------------------------------------------------------------------------

void CEncoder_MP3::InitEncoder(int bitrate, int frequency, int channels)
{
	CloseEncoder();

	// Adjusting encoder settings.
	m_beConfig.dwConfig						= BE_CONFIG_LAME;
	m_beConfig.format.LHV1.dwStructVersion	= 1;
	m_beConfig.format.LHV1.dwStructSize		= sizeof(m_beConfig);		
	m_beConfig.format.LHV1.dwSampleRate		= frequency;
	m_beConfig.format.LHV1.dwReSampleRate	= frequency;
	m_beConfig.format.LHV1.dwBitrate		= bitrate;
	m_beConfig.format.LHV1.nMode			= channels == 1 ? BE_MP3_MODE_MONO : BE_MP3_MODE_JSTEREO;
	m_beConfig.format.LHV1.nPreset			= LQP_NOPRESET;
	m_beConfig.format.LHV1.bNoRes			= TRUE;			// No Bit reservoir
	m_beConfig.format.LHV1.bWriteVBRHeader	= TRUE;
	//m_beConfig.format.LHV1.dwPsyModel		= 0;			// USE DEFAULT PSYCHOACOUSTIC MODEL 
	//m_beConfig.format.LHV1.dwEmphasis		= 0;			// NO EMPHASIS TURNED ON

	// mpeg version
	bool bMpeg2 = (bitrate < 32) || (bitrate == 32 && frequency < 22050);
	m_beConfig.format.LHV1.dwMpegVersion = (bMpeg2) ? MPEG2 : MPEG1;

	DWORD dwSamples = 0, dwBufOutMinSize = 0;
	BE_ERR err = beInitStream(&m_beConfig, &dwSamples, &dwBufOutMinSize, &m_hbeStream);
	if (err != BE_ERR_SUCCESSFUL)
		throw CString(_T("Failed to initialize encoder."));

	m_chunkBufFloatSize = dwSamples;
	m_chunkBufFloat_l = new float[m_chunkBufFloatSize];
	m_chunkBufFloat_r = new float[m_chunkBufFloatSize];
}
//---------------------------------------------------------------------------

void CEncoder_MP3::CloseEncoder()
{
	if (m_hbeStream)
	{
		beCloseStream(m_hbeStream);
		m_hbeStream	= NULL;
	}

	ZeroMemory(&m_beConfig, sizeof(m_beConfig));
	SAFE_DELETE_ARRAY(m_chunkBufFloat_l);
	SAFE_DELETE_ARRAY(m_chunkBufFloat_r);
	m_chunkBufFloatSize = 0;
}
//---------------------------------------------------------------------------

bool CEncoder_MP3::EncodeChunkFloat(float* pBufIn, int nBufInSize,
									char* pBufOut, int& nBufOutSize)
{
	int l_input_buffer_offset  = 0;
	int l_output_buffer_offset = 0;

	nBufOutSize = 0;

	int channels = (m_beConfig.format.LHV1.nMode == BE_MP3_MODE_MONO) ? 1 : 2;
	ASSERT(nBufInSize % channels == 0);

	// Dividing source buffer into chunks and encoding each.
	while (l_input_buffer_offset < nBufInSize)
	{
		// Taking current chunk. Splitting source to left and right sample buffers
		int l_available_count = nBufInSize - l_input_buffer_offset;
		int l_copy_count = min(m_chunkBufFloatSize * channels, l_available_count);

		float* l_current_chunk = pBufIn + l_input_buffer_offset;
		for (int i = 0, j = 0; i < l_copy_count; i += channels)
		{
			ASSERT (j < m_chunkBufFloatSize);
			m_chunkBufFloat_l[j] = l_current_chunk[i] * 32768;
			if (channels > 1)
				m_chunkBufFloat_r[j] = l_current_chunk[i + 1] * 32768;
			j++;
		}
		l_input_buffer_offset += l_copy_count;

		// Encoding float samples. Data saved in output buffer.
		DWORD dwBytesEncoded = 0;
		BE_ERR err = beEncodeChunkFloatS16NI(m_hbeStream, l_copy_count/channels, m_chunkBufFloat_l, m_chunkBufFloat_r,
			(BYTE *)pBufOut + l_output_buffer_offset, &dwBytesEncoded);

		if (err != BE_ERR_SUCCESSFUL)
			break;

		l_output_buffer_offset += dwBytesEncoded;
		nBufOutSize += dwBytesEncoded;
	}

	return (l_input_buffer_offset == nBufInSize) ? true : false;
}
//---------------------------------------------------------------------------

void CEncoder_MP3::WriteVBRHeader(const CString& filePath)
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
	beWriteVBRHeader(filePath);
}
//---------------------------------------------------------------------------

bool CEncoder_MP3::ProcessData(void* buffer, DWORD lengthBytes)
{
	//TODO... encode data and send updated buffer.

	return Filter::ProcessData(buffer, lengthBytes);
}
//---------------------------------------------------------------------------


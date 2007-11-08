//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MP3Write.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
#define _ENCODERERR		"MP3 coder"
#define _NOLIBRARY		"Can't find encoder library (lame_enc.dll).\n"
#define _FUNCINITERR	"Can't initialize encoder functions.\n"
#define _STREAMINITERR	"Can't initialize encoding stream.\n"
#define _FILEINITERR	"Can't open file for recording.\n"

//////////////////////////////////////////////////////////////////////
BEINITSTREAM		beInitStream   = NULL;
BEENCODECHUNK		beEncodeChunk  = NULL;
BEDEINITSTREAM		beDeinitStream = NULL;
BECLOSESTREAM		beCloseStream  = NULL;
BEVERSION			beVersion      = NULL;
BEWRITEVBRHEADER	beWriteVBRHeader=NULL;

//////////////////////////////////////////////////////////////////////
CMP3Write::CMP3Write()
{
	hFileOut	= NULL;
	pMP3Buffer	= NULL;
	hbeStream	= 0;
	dwSamples	= 0;
	dwMP3Buffer = 0;
	ZeroMemory(&EF, sizeof(EF));	// zero all flags

	InitLameDLL();
}

CMP3Write::~CMP3Write() {
	CloseFile();
	CloseStream();
	if(hDll)
		FreeLibrary(hDll);
}

//////////////////////////////////////////////////////////////////////
void CMP3Write::InitLameDLL()
{
	hDll = LoadLibrary("lame_enc.dll");
	if(hDll == NULL) {
		AfxMessageBox(_NOLIBRARY, MB_OK|MB_ICONSTOP);
		EF.bIsDLLInited = false;
		return;
	}
	beInitStream	= (BEINITSTREAM)    GetProcAddress(hDll, TEXT_BEINITSTREAM);
	beEncodeChunk	= (BEENCODECHUNK)   GetProcAddress(hDll, TEXT_BEENCODECHUNK);
	beDeinitStream	= (BEDEINITSTREAM)  GetProcAddress(hDll, TEXT_BEDEINITSTREAM);
	beCloseStream	= (BECLOSESTREAM)   GetProcAddress(hDll, TEXT_BECLOSESTREAM);
	beVersion		= (BEVERSION)       GetProcAddress(hDll, TEXT_BEVERSION);
	beWriteVBRHeader= (BEWRITEVBRHEADER)GetProcAddress(hDll, TEXT_BEWRITEVBRHEADER);

	if(!beInitStream || !beEncodeChunk || !beDeinitStream || !beCloseStream || !beVersion || !beWriteVBRHeader)
	{
		AfxMessageBox(_FUNCINITERR, MB_OK|MB_ICONSTOP);
		EF.bIsDLLInited = false;
		return;
	}
	EF.bIsDLLInited = true;
}

//////////////////////////////////////////////////////////////////////
bool CMP3Write::InitStream()
{
	if(!EF.bIsDLLInited)	return false;
	if(EF.bIsStreamInited)	return false;

	BE_ERR err = beInitStream(&beConfig, &dwSamples, &dwMP3Buffer,
		&hbeStream);
	if(err != BE_ERR_SUCCESSFUL) {
		AfxMessageBox(_STREAMINITERR, MB_OK|MB_ICONSTOP);
		EF.bIsDLLInited = false;
		EF.bIsStreamInited = false;
		return false;
	}
	pMP3Buffer = new BYTE[dwMP3Buffer];	// create buffer with mp3 frame size
	EF.bIsStreamInited = true;

	return true;
}

void CMP3Write::CloseStream()
{
	if(!EF.bIsDLLInited)	return;
	if(!EF.bIsStreamInited) return;

	beCloseStream( hbeStream );
	if(pMP3Buffer) {
		delete [] pMP3Buffer;
		pMP3Buffer = NULL;
	}
	EF.bIsStreamInited = false;
}

//////////////////////////////////////////////////////////////////////
void CMP3Write::SetMode(int nBitrate, int nFreq, int nBits, int nStereo)
{
	UINT nMode[2] = {BE_MP3_MODE_MONO, BE_MP3_MODE_JSTEREO};

	if(!EF.bIsDLLInited)
		return;
	
	m_nBits = nBits;

	ZeroMemory(&beConfig, sizeof(beConfig));// clear all fields
	//
	beConfig.dwConfig = BE_CONFIG_LAME;		// use the LAME config structure
	// struct settings
	beConfig.format.LHV1.dwStructVersion	= 1;
	beConfig.format.LHV1.dwStructSize		= sizeof(beConfig);		
	// sampling rate
	beConfig.format.LHV1.dwSampleRate		= nFreq;				// INPUT FREQUENCY
	beConfig.format.LHV1.dwReSampleRate		= 0;					// DON"T RESAMPLE
	// set mode
	beConfig.format.LHV1.nMode				= nMode[nStereo!=0];
	// set bitrate
	beConfig.format.LHV1.dwBitrate			= nBitrate;				// MINIMUM BIT RATE
	beConfig.format.LHV1.nPreset			= LQP_NOPRESET;			// QUALITY PRESET SETTING
	// mpeg version
	if(nBitrate == 32) {
		if(nFreq >= 22050)
			beConfig.format.LHV1.dwMpegVersion = MPEG1;
		else
			beConfig.format.LHV1.dwMpegVersion = MPEG2;
	}
	else {
		if(nBitrate > 32)
			beConfig.format.LHV1.dwMpegVersion = MPEG1;
		else
			beConfig.format.LHV1.dwMpegVersion = MPEG2;
	}
	beConfig.format.LHV1.dwPsyModel			= 0;					// USE DEFAULT PSYCHOACOUSTIC MODEL 
	beConfig.format.LHV1.dwEmphasis			= 0;					// NO EMPHASIS TURNED ON
	beConfig.format.LHV1.bOriginal			= FALSE;				// SET ORIGINAL FLAG

//	beConfig.format.LHV1.dwMaxBitrate		= 64;					// MAXIMUM BIT RATE
//	beConfig.format.LHV1.bCRC				= TRUE;					// INSERT CRC
//	beConfig.format.LHV1.bCopyright			= TRUE;					// SET COPYRIGHT FLAG	
//	beConfig.format.LHV1.bPrivate			= TRUE;					// SET PRIVATE FLAG
//	beConfig.format.LHV1.bWriteVBRHeader	= TRUE;					// YES, WRITE THE XING VBR HEADER
//	beConfig.format.LHV1.bEnableVBR			= TRUE;					// USE VBR
//	beConfig.format.LHV1.nVBRQuality		= 5;					// SET VBR QUALITY
	beConfig.format.LHV1.bNoRes				= TRUE;					// No Bit resorvoir
}

DWORD CMP3Write::GetWIBufferSize()
{
	return dwSamples*m_nBits/8; // buffer size for BYTE array
}

//////////////////////////////////////////////////////////////////////
bool CMP3Write::EncodeChunk(short* pWAVBuffer, DWORD dwBufferSize)
{
	if(EF.bIsDLLInited + EF.bIsStreamInited + EF.bIsFileOpened == 0)
		return false;
	DWORD dwWrite = 0, dwBytesWritten = 0;

	BE_ERR err = beEncodeChunk(hbeStream, dwSamples, pWAVBuffer,
		pMP3Buffer, &dwWrite);
	if(err != BE_ERR_SUCCESSFUL) {
		return false;
	}
	WriteFile(hFileOut, pMP3Buffer, dwWrite, &dwBytesWritten, NULL);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CMP3Write::SetFile(char *pszNewFile)
{
	hFileOut = CreateFile(pszNewFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if(hFileOut == INVALID_HANDLE_VALUE)
	{
		char szErrorText[1000];
		sprintf(szErrorText, "%s\n%s", pszNewFile, _FILEINITERR);
		AfxMessageBox(szErrorText, MB_OK|MB_ICONSTOP);
		EF.bIsFileOpened = false;
		return false;
	}
	EF.bIsFileOpened = true;

	return true;
}

bool CMP3Write::SetFP()
{
	if(!EF.bIsFileOpened)
		return false;

	SetFilePointer(hFileOut, 0, NULL, FILE_END);
	return true;
}

void CMP3Write::CloseFile()
{
	if(EF.bIsFileOpened)
		CloseHandle(hFileOut);
	EF.bIsFileOpened = false;
}

//////////////////////////////////////////////////////////////////////

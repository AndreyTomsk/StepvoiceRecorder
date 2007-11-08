//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MP3WRITE_H__B6387F42_C90E_11D7_BC33_444553540000__INCLUDED_)
#define AFX_MP3WRITE_H__B6387F42_C90E_11D7_BC33_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include "BladeMP3EncDll.h"

//////////////////////////////////////////////////////////////////////
typedef struct tagEncoderFlags {
	bool bIsDLLInited;
	bool bIsStreamInited;
	bool bIsFileOpened;
} EncoderFlags;

//////////////////////////////////////////////////////////////////////
class CMP3Write  
{
private:
	BE_CONFIG	beConfig;	// mp3 encoding structure
	HBE_STREAM	hbeStream;	// encoder stream
	DWORD		dwSamples;	// number of samples to send in beEncodeChunk()

	PBYTE		pMP3Buffer;	// pointer to MP3 Buffer
	DWORD		dwMP3Buffer;// size MP3 Buffer
	int		m_nBits;

	HINSTANCE hDll;

	HANDLE		hFileOut;	// handle of opened file
	EncoderFlags EF;		// encoder state flags

public:
	bool SetFP();
	CMP3Write();

	// main encoding functions
	bool  InitStream();
	bool  EncodeChunk(short* pWAVBuffer, DWORD dwBufferSize);
	void  CloseStream();

	// encoder config
	void  SetMode(int nBitrate, int nFreq, int nBits, int nStereo); // fill beConfig
	DWORD GetWIBufferSize();// return needed WAV buffer size

	// file managment functions
	bool SetFile(char* pszNewFile);
	void CloseFile();

	~CMP3Write();

private:
	void InitLameDLL();
};

#endif // !defined(AFX_MP3WRITE_H__B6387F42_C90E_11D7_BC33_444553540000__INCLUDED_)

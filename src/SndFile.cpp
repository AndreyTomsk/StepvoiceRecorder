//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SndFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////

const int FLASH_PERIOD = 150;

//====================================================================
//  ŒÕ—“–” “Œ–/ƒ≈—“–” “Œ–
//====================================================================
CSndFile::CSndFile()
{
	m_hSndFile    = INVALID_HANDLE_VALUE;
	m_strFileName = "";
	ZeroMemory(&m_sndinfo, sizeof(m_sndinfo));
}

//---------------------------------------------------------------------
CSndFile::~CSndFile()
{	//
	Close();
}

//====================================================================
// Œœ≈–¿÷»» Œ“ –€“»ﬂ/«¿ –€“»ﬂ ‘¿…À¿ :)
//====================================================================
bool CSndFile::Open(CString &str, SOUND_INFO* psi)
{
	// Ô˚Ú‡ÂÏÒˇ Û·‡Ú¸ ‡ÚË·ÛÚ "ÚÓÎ¸ÍÓ ˜ÚÂÌËÂ"
	SetFileAttributes(str.GetBuffer(str.GetLength()), FILE_ATTRIBUTE_NORMAL);

	m_hSndFile = CreateFile(str.GetBuffer(str.GetLength()),
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		0,
		NULL);

	if(IsOpened())
	{	// ËÌËˆË‡ÎËÁËÛÂÏ Ô‡‡ÏÂÚ˚ Ù‡ÈÎ‡
		m_sndinfo.nBitrate = psi->nBitrate;
		m_sndinfo.nFreq	 = psi->nFreq;
		m_sndinfo.nBits	 = psi->nBits;
		m_sndinfo.nStereo	 = psi->nStereo;

		m_strFileName = str;
	}

	return IsOpened();
}

//---------------------------------------------------------------------
void CSndFile::Close()
{
	if(IsOpened())
	{
		CloseHandle(m_hSndFile);
	}

	m_hSndFile	  = INVALID_HANDLE_VALUE;
	m_strFileName = "";
	ZeroMemory(&m_sndinfo, sizeof(m_sndinfo));
}

//---------------------------------------------------------------------
bool CSndFile::IsOpened()
{
	return (m_hSndFile != INVALID_HANDLE_VALUE);
}

//====================================================================
// Œœ≈–¿÷»» ◊“≈Õ»ﬂ/«¿œ»—»
//====================================================================
DWORD CSndFile::Read(char* pBuf, DWORD dwBytesToRead)
{
	DWORD  dwReallyRead = 0;
	ReadFile(m_hSndFile, pBuf, dwBytesToRead, &dwReallyRead, NULL);
	return dwReallyRead;
}

//---------------------------------------------------------------------
DWORD CSndFile::Write(char* pBuf, DWORD dwBytesToWrite)
{
	DWORD  dwReallyWritten = 0;
	static int nCounter = 0;

	WriteFile(m_hSndFile, pBuf, dwBytesToWrite, &dwReallyWritten, NULL);
	// ÒÍË‰˚‚‡ÂÏ ·ÛÙÂ˚ Ì‡ ‰ËÒÍ ˜ÂÂÁ nFlashPeriod ‚˚ÁÓ‚Ó‚
	if(++nCounter % FLASH_PERIOD == 0)
	{
		FlushFileBuffers(m_hSndFile);
		nCounter = 0;
	}

	return dwReallyWritten;
}

//---------------------------------------------------------------------
void CSndFile::Cut()
{
	if(IsOpened())
		SetEndOfFile(m_hSndFile);
}

//====================================================================
// œŒÀ”◊≈Õ»≈ ƒŒœŒÀÕ»“≈À‹ÕŒ… »Õ‘Œ–Ã¿÷»» Œ ‘¿…À≈
//====================================================================
DWORD CSndFile::GetFilePos(int nWhatID)
{
	return IsOpened() ? ::SetFilePointer(m_hSndFile, 0, 0, FILE_CURRENT) : (DWORD)-1;
}

//---------------------------------------------------------------------
DWORD CSndFile::GetFileSize(int nWhatID)
{
	return IsOpened() ? ::GetFileSize(m_hSndFile, NULL) : (DWORD)-1;
}

//---------------------------------------------------------------------
CString& CSndFile::GetFileName()
{
	return m_strFileName;
}

//---------------------------------------------------------------------
void CSndFile::GetSoundInfo(SOUND_INFO &si)
{
	si.nBitrate = m_sndinfo.nBitrate;
	si.nFreq    = m_sndinfo.nFreq;
	si.nBits    = m_sndinfo.nBits;
	si.nStereo  = m_sndinfo.nStereo;
}

//---------------------------------------------------------------------
bool CSndFile::ChangeSoundInfo(SOUND_INFO &si)
{
	m_sndinfo.nBitrate	= si.nBitrate;
	m_sndinfo.nFreq		= si.nFreq;
	m_sndinfo.nBits		= si.nBits;
	m_sndinfo.nStereo	= si.nStereo;

	return true;
}


//====================================================================
// œŒ«»÷»ŒÕ»–Œ¬¿Õ»≈ ¬ ‘¿…À≈
//====================================================================
DWORD CSndFile::SetFilePointer(int nBytes, int nFromType)
{
	switch(nFromType)
	{
	case SND_FILE_END:
		nFromType = FILE_END;
		break;
	case SND_FILE_CURRENT:
		nFromType = FILE_CURRENT;
		break;
	default:
		nFromType = FILE_BEGIN;
	}

	return IsOpened() ? ::SetFilePointer(m_hSndFile, nBytes, 0, nFromType) : (DWORD)-1;
}

//====================================================================

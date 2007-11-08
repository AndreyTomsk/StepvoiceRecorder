/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mp3_recorder.h"
#include "SndFile_MP3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
const int tabsel_123[2][3][16] = {
   { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
     {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
     {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

   { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

const long freqs[] = {44100, 48000, 32000, 22050, 24000,
					  16000, 11025, 12000, 8000 };
/////////////////////////////////////////////////////////////////////////////

//===========================================================================
// �����������/����������
//===========================================================================
CSndFile_MP3::CSndFile_MP3()
{
	m_nAllFrames = 0;
	m_nCurrFrame = 0;
	m_nFrameSize = 0;
}

//---------------------------------------------------------------------------
CSndFile_MP3::~CSndFile_MP3()
{
	Close();
}

//===========================================================================
// ������� ��������/�������� �����
//===========================================================================
bool CSndFile_MP3::Open(CString& str, SOUND_INFO* psi)
{
	Close(); // ??? - ����� �� ��������� ������ ���� ����� ��������� ������
	bool bResult = CSndFile::Open(str, psi);
	if(bResult)
	{	
		GetFileInfo();
		// ��������� ���� �� ��������� �����
		/*if(CSndFile::GetFileSize(0))
		{
			if(!GetFileInfo())
			{
				Close();
				bResult = false;
			}
		}*/
	}
	return bResult;
}

//---------------------------------------------------------------------------
void CSndFile_MP3::Close()
{
	CSndFile::Close();

	m_nAllFrames = 0;
	m_nCurrFrame = 0;
	m_nFrameSize = 0;
}


//===========================================================================
// �������� ������/������
//===========================================================================
DWORD CSndFile_MP3::Read(char* pBuf, DWORD dwBytesToRead)
{
	MPEG_HEADER mpeghdr;
	char*       pOffset = pBuf;

	if(FindNextHeader(&mpeghdr))
	{	// ������, ���� ������ ������ ������ ����� ����, ���������� ��������
		while(mpeghdr.framesize <= (int)dwBytesToRead - (pOffset-pBuf))
		{	//
			pOffset += CSndFile::Read(pOffset, mpeghdr.framesize);
			//
			m_nCurrFrame++;
			//
			if(!FindNextHeader(&mpeghdr))
			{
				break;
			}
		}
	} // if
	return (pOffset-pBuf);
}

//---------------------------------------------------------------------------
DWORD CSndFile_MP3::Write(char* pBuf, DWORD dwBytesToWrite)
{
// 1. ������������� ����� � ���������� ���������� ������� � ���
// 2. ������ ������ = ���. ������/���������� ������� � ���
// 3. �������� ������ ������ � ����� ������, ���������� ����� �� ������
//	    ������ �������� ������
// 4. �������� ������� ����� � ����� ���������� �������
//

	MPEG_HEADER mpeghdr;

	// ���������� ����� ������� �� ����� ��������������
	DWORD dwFramesLength = 0, dwRewriteFrames = 0;
	DWORD dwWritePos     = CSndFile::SetFilePointer(0, SND_FILE_CURRENT);
	while(dwFramesLength < dwBytesToWrite)
	{
		if(FindNextHeader(&mpeghdr))
		{
			dwRewriteFrames++;
			dwFramesLength += mpeghdr.framesize;
			SetFilePointer(mpeghdr.framesize, SND_FILE_CURRENT);
		}
		else
		{
			break;
		}
	}

	// ����� � ����
	SetFilePointer(dwWritePos, SND_FILE_BEGIN);
	DWORD dwBytesWritten = CSndFile::Write(pBuf, dwBytesToWrite);
	
	// ����������� ���������� ������� � ���������� ������
	DWORD dwBufFrames = 0;
	char* pOffset     = pBuf;
	while(DecodeHeader(pOffset, &mpeghdr) && (dwBytesWritten > (pOffset-pBuf)))
	{
		dwBufFrames++;
		pOffset += mpeghdr.framesize;
	}

	// ��������� ����������
	m_nFrameSize  = dwBytesWritten / dwBufFrames;
	m_nCurrFrame += dwBufFrames;
	m_nAllFrames  = m_nAllFrames - dwRewriteFrames + dwBufFrames;

	// �������� ��, ��� �������� ��������
	if(dwFramesLength > dwBytesWritten)
	{
		dwBytesToWrite = dwFramesLength - dwBytesWritten;
		pOffset = new char[dwBytesToWrite];
		ZeroMemory(pOffset, dwBytesToWrite);
		CSndFile::Write(pOffset, dwBytesToWrite);
		delete [] pOffset;
	}

	return dwBytesWritten;
}


//===========================================================================
// ��������� ������� ���������� � ������� � �����
//===========================================================================
DWORD CSndFile_MP3::GetFilePos(int nWhatID)
{
	unsigned long uPos = 0;

	if(IsOpened())
	{
		switch(nWhatID)
		{
		case IN_SECONDS:
			ASSERT(m_sndinfo.nBitrate);
			uPos = (m_nCurrFrame*m_nFrameSize) / (m_sndinfo.nBitrate*125);
			break;
		case IN_KBYTES:
			uPos = CSndFile::GetFilePos(0)/1024;
			break;
		}
	}

	return uPos;
}

//---------------------------------------------------------------------------
DWORD CSndFile_MP3::GetFileSize(int nWhatID)
{
	unsigned long uSize = 0;

	if(IsOpened())
	{
		switch(nWhatID)
		{
		case IN_SECONDS:
			ASSERT(m_sndinfo.nBitrate);
			uSize = (m_nAllFrames*m_nFrameSize) / (m_sndinfo.nBitrate*125);
			break;
		case IN_KBYTES:
			uSize = CSndFile::GetFileSize(0)/1024;
		}
	}

	return uSize;
}

//---------------------------------------------------------------------------
bool CSndFile_MP3::ChangeSoundInfo(SOUND_INFO& si)
{
	bool bRes = false;

	if(m_nAllFrames == 0)
		bRes = CSndFile::ChangeSoundInfo(si);

	return bRes;
}

//===========================================================================
// ���������������� � �����
//===========================================================================
DWORD CSndFile_MP3::Seek(int nSeconds, int nFromType)
{
	if(IsOpened())
	{
		if(m_nFrameSize != 0)
		{	long nFrames = (nSeconds*m_sndinfo.nBitrate*125)/m_nFrameSize;
			SeekFrame(nFrames, nFromType);
		}
	}

	return GetFilePos(IN_SECONDS);
}

//---------------------------------------------------------------------------
void CSndFile_MP3::SeekFrame(int nFrame, int nFromType)
{	//
	switch(nFromType)
	{
	case SND_FILE_BEGIN:
		m_nCurrFrame = nFrame;
		break;
	case SND_FILE_CURRENT:
		m_nCurrFrame += nFrame;
		break;
	case SND_FILE_END:
		m_nCurrFrame = m_nAllFrames - nFrame;
	}
	
	//
	if(m_nCurrFrame <= 0)
	{	// ����������� � ������ �����
		m_nCurrFrame = 0;
		CSndFile::SetFilePointer(0, SND_FILE_BEGIN);
		// ������� ����� ������ �����
		//MPEG_HEADER mpeghdr;
		//FindNextHeader(&mpeghdr);
	}
	else if( m_nCurrFrame >= m_nAllFrames)
	{	// ����������� � ����� �����
		m_nCurrFrame = m_nAllFrames;
		CSndFile::SetFilePointer(0, SND_FILE_END);
	}
	else
	{	// ����������� � ������
		// ������������ ���������������� �� ������ ����� !!!!!!!!!!!
		MPEG_HEADER  mpeghdr;
		CSndFile::SetFilePointer(0, SND_FILE_BEGIN);
		for(int i = m_nCurrFrame; i>0; i--)
		{
			FindNextHeader(&mpeghdr);
			CSndFile::SetFilePointer(mpeghdr.framesize, SND_FILE_CURRENT);
		}
	}
}

//===========================================================================
// ������������� ��������� ������
//===========================================================================
bool CSndFile_MP3::DecodeHeader(char *pHeader, MPEG_HEADER *mpeghdr)
{
	register unsigned long uHeader;
	uHeader = (unsigned long)(((pHeader[0] & 0xFF) << 24) |
							  ((pHeader[1] & 0xFF) << 16) |
							  ((pHeader[2] & 0xFF) <<  8) |
							   (pHeader[3] & 0xFF));

	// ��������� ���� �������������
	if(((uHeader >> 21) & 0x07FF) != 0x07FF)
	{
		goto error;
	}
	
	// ���������� ������ MPEG
	if(uHeader & (1<<20))
	{
		mpeghdr->lsf = (uHeader & (1<<19)) ? 0x0 : 0x1;
		mpeghdr->mpeg25 = 0;
	}
	else
	{
		if(uHeader & (1<<19))
		{	// ���������������
			goto error;
		}
		mpeghdr->lsf = 1;
		mpeghdr->mpeg25 = 1;
	}
	
	// ���������� ������ Layer
	mpeghdr->layer = 4 - ((uHeader>>17) & 3);
	if(mpeghdr->layer == 4)
	{	// ������ - ���������������
		goto error;
	}
	
	mpeghdr->crc = ((uHeader>>16) & 0x1) ^ 0x1; // ���� CRC (0 - ����)
	
	// ���������� �������
	mpeghdr->bitr_index = (uHeader>>12) & 0xF;
	if(!mpeghdr->bitr_index || mpeghdr->bitr_index == 0xF)
	{
		// ������
		goto error;
	}
	
	// ���������� ������� �������������
	if(((uHeader>>10) & 0x3) == 0x3)
	{	// ������ - ���������������
		goto error;
	}
	if(mpeghdr->mpeg25)
	{
		mpeghdr->freq_index = 6 + ((uHeader>>10) & 0x3);
	}
	else
	{
		mpeghdr->freq_index = ((uHeader>>10) & 0x3) + (mpeghdr->lsf * 3);
	}

	mpeghdr->padding   = (uHeader>>9) & 0x1; // ������. ���� � ������ (��� �������� ��������)
	mpeghdr->priv_bit  = (uHeader>>8) & 0x1; // private bit
	mpeghdr->mode      = (uHeader>>6) & 0x3; // stereo mode
	mpeghdr->mode_ext  = (uHeader>>4) & 0x3; // mode extension (for Joint stereo)
	mpeghdr->copyright = (uHeader>>3) & 0x1; // 
	mpeghdr->original  = (uHeader>>2) & 0x1; //
	mpeghdr->emphasis  = uHeader & 0x3;		 //

	mpeghdr->framesize = CountFrameSize(mpeghdr);
	if(!mpeghdr->framesize)
	{
		goto error;
	}

	// ��������� ������� �����������
	return true;

error:
	// ������ � �������������
	return false;
}

//===========================================================================
// ��������� ������� ������
//===========================================================================
int CSndFile_MP3::CountFrameSize(MPEG_HEADER *mpeghdr)
{
	long nSize = 0;

	switch(mpeghdr->layer)
	{
	case 2:
		nSize  = (long)tabsel_123[mpeghdr->lsf][1][mpeghdr->bitr_index] * 144000;
		nSize /= freqs[mpeghdr->freq_index];
		nSize += mpeghdr->padding;
		break;
	case 3:
		nSize  = (long)tabsel_123[mpeghdr->lsf][2][mpeghdr->bitr_index] * 144000;
		nSize /= freqs[mpeghdr->freq_index] << (mpeghdr->lsf);
		nSize += mpeghdr->padding;
		break;
	}

	return (int)nSize;
}

//===========================================================================
// ����� ���������� ������
//===========================================================================
bool CSndFile_MP3::FindNextHeader(MPEG_HEADER *mpeghdr)
{
	const int BUFFER_SIZE = 512;
	char      cBuffer[BUFFER_SIZE], *pFound;

	int nBytesRead = CSndFile::Read(cBuffer, BUFFER_SIZE);
	while(nBytesRead > 3)
	{	// ���� ���������
		pFound = (char *)memchr(cBuffer, 0xFF, nBytesRead);
		while(pFound)
		{
			if(DecodeHeader(pFound, mpeghdr))
			{	// ������������� ��������� �� ������ ���������
				CSndFile::SetFilePointer(-(nBytesRead - (pFound-cBuffer)),
					SND_FILE_CURRENT);
				return true;
			}
			// ���� � ��������� ��������� (����.1 �� 4 ����� - ����� ���������)
			if(pFound - cBuffer >= nBytesRead - 4)
				break;
			
			pFound = (char *)memchr(pFound+4, 0xFF,
				nBytesRead - (pFound+4-cBuffer) - 1);			
		}
		// ������ ����� ������ ������
		CSndFile::SetFilePointer(-3, SND_FILE_CURRENT);
		nBytesRead = CSndFile::Read(cBuffer, BUFFER_SIZE);
	}// while(dwBytesRead > 3)

	return false;
}

//===========================================================================
bool CSndFile_MP3::GetFileInfo()
{
// ���������� ����� � ������ � ������.
// ������������:
//   ������� ���������� �������;
//   ��������� ���������� �� 1-�� ������;
//   ��������� ��������� �� 1-� �����.

	int  nFrameCounter = 0;
	MPEG_HEADER mpeghdr;

	// ���������� ���������� mpeg ������� � �����
	SetFilePointer(0, SND_FILE_BEGIN);
	while(FindNextHeader(&mpeghdr))
	{
		nFrameCounter++;
		CSndFile::SetFilePointer(mpeghdr.framesize, SND_FILE_CURRENT);
	}

	if(nFrameCounter > 0)
	{	// ������������� ��������� �� 1-� �����
		SetFilePointer(0, SND_FILE_BEGIN);
		CSndFile::SetFilePointer(mpeghdr.framesize, SND_FILE_CURRENT);
		FindNextHeader(&mpeghdr);
		
		// ���������� ������������� ����������
		m_nCurrFrame = 0;
		m_nAllFrames = nFrameCounter;
		m_nFrameSize = mpeghdr.framesize;

		m_sndinfo.nBitrate = tabsel_123[mpeghdr.lsf][mpeghdr.layer-1][mpeghdr.bitr_index];
		m_sndinfo.nFreq	   = freqs[mpeghdr.freq_index];
		m_sndinfo.nBits	   = 16;
		m_sndinfo.nStereo  = mpeghdr.mode != 3; // !!!!!!
	}

	return (nFrameCounter > 0);
}

//===========================================================================

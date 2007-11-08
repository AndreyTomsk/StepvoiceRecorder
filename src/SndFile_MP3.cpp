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
// КОНСТРУКТОР/ДЕСТРУКТОР
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
// ФУНКЦИИ ОТКРЫТИЯ/ЗАКРЫТИЯ ФАЙЛА
//===========================================================================
bool CSndFile_MP3::Open(CString& str, SOUND_INFO* psi)
{
	Close(); // ??? - стоит ли закрывать старый файл перед открытием нового
	bool bResult = CSndFile::Open(str, psi);
	if(bResult)
	{	
		GetFileInfo();
		// проверяем файл на ненулевую длину
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
// ОПЕРАЦИИ ЧТЕНИЯ/ЗАПИСИ
//===========================================================================
DWORD CSndFile_MP3::Read(char* pBuf, DWORD dwBytesToRead)
{
	MPEG_HEADER mpeghdr;
	char*       pOffset = pBuf;

	if(FindNextHeader(&mpeghdr))
	{	// читаем, пока размер фрейма больше числа байт, оставшихся дочитать
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
// 1. Раскодировать буфер и определить количество фреймов в нем
// 2. Размер фрейма = общ. размер/количество буферов в нем
// 3. Заменить старые буферы в файле новыми, оставшееся место до нового
//	    буфера заменить нулями
// 4. Изменить текущий фрейм и общее количество фреймов
//

	MPEG_HEADER mpeghdr;

	// определяем скока фреймов мы будем перезаписывать
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

	// пишем в файл
	SetFilePointer(dwWritePos, SND_FILE_BEGIN);
	DWORD dwBytesWritten = CSndFile::Write(pBuf, dwBytesToWrite);
	
	// определение количества фреймов в записанном буфере
	DWORD dwBufFrames = 0;
	char* pOffset     = pBuf;
	while(DecodeHeader(pOffset, &mpeghdr) && (dwBytesWritten > (pOffset-pBuf)))
	{
		dwBufFrames++;
		pOffset += mpeghdr.framesize;
	}

	// обновляем переменные
	m_nFrameSize  = dwBytesWritten / dwBufFrames;
	m_nCurrFrame += dwBufFrames;
	m_nAllFrames  = m_nAllFrames - dwRewriteFrames + dwBufFrames;

	// зануляем то, что осталось дописать
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
// ПОЛУЧЕНИЕ ТЕКУЩЕЙ ИНФОРМАЦИИ О ПОЗИЦИИ И ДЛИНЕ
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
// ПОЗИЦИОНИРОВАНИЕ В ФАЙЛЕ
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
	{	// перемещение в начало файла
		m_nCurrFrame = 0;
		CSndFile::SetFilePointer(0, SND_FILE_BEGIN);
		// находим самый первый фрейм
		//MPEG_HEADER mpeghdr;
		//FindNextHeader(&mpeghdr);
	}
	else if( m_nCurrFrame >= m_nAllFrames)
	{	// перемещение в конец файла
		m_nCurrFrame = m_nAllFrames;
		CSndFile::SetFilePointer(0, SND_FILE_END);
	}
	else
	{	// перемещение к фрейму
		// осуществляем позиционирование от начала файла !!!!!!!!!!!
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
// ДЕКОДИРОВАНИЕ ЗАГОЛОВКА ФРЕЙМА
//===========================================================================
bool CSndFile_MP3::DecodeHeader(char *pHeader, MPEG_HEADER *mpeghdr)
{
	register unsigned long uHeader;
	uHeader = (unsigned long)(((pHeader[0] & 0xFF) << 24) |
							  ((pHeader[1] & 0xFF) << 16) |
							  ((pHeader[2] & 0xFF) <<  8) |
							   (pHeader[3] & 0xFF));

	// проверяем биты синхронизации
	if(((uHeader >> 21) & 0x07FF) != 0x07FF)
	{
		goto error;
	}
	
	// определяем версию MPEG
	if(uHeader & (1<<20))
	{
		mpeghdr->lsf = (uHeader & (1<<19)) ? 0x0 : 0x1;
		mpeghdr->mpeg25 = 0;
	}
	else
	{
		if(uHeader & (1<<19))
		{	// зарезервировано
			goto error;
		}
		mpeghdr->lsf = 1;
		mpeghdr->mpeg25 = 1;
	}
	
	// определяем версию Layer
	mpeghdr->layer = 4 - ((uHeader>>17) & 3);
	if(mpeghdr->layer == 4)
	{	// ошибка - зарезервировано
		goto error;
	}
	
	mpeghdr->crc = ((uHeader>>16) & 0x1) ^ 0x1; // флаг CRC (0 - есть)
	
	// определяем битрейт
	mpeghdr->bitr_index = (uHeader>>12) & 0xF;
	if(!mpeghdr->bitr_index || mpeghdr->bitr_index == 0xF)
	{
		// ошибка
		goto error;
	}
	
	// определяем частоту семплирования
	if(((uHeader>>10) & 0x3) == 0x3)
	{	// ошибка - зарезервировано
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

	mpeghdr->padding   = (uHeader>>9) & 0x1; // дополн. байт в фрейме (для точности битрейта)
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

	// заголовок успешно декодирован
	return true;

error:
	// ошибка в декодировании
	return false;
}

//===========================================================================
// ПОЛУЧЕНИЕ РАЗМЕРА ФРЕЙМА
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
// ПОИСК СЛЕДУЮЩЕГО ФРЕЙМА
//===========================================================================
bool CSndFile_MP3::FindNextHeader(MPEG_HEADER *mpeghdr)
{
	const int BUFFER_SIZE = 512;
	char      cBuffer[BUFFER_SIZE], *pFound;

	int nBytesRead = CSndFile::Read(cBuffer, BUFFER_SIZE);
	while(nBytesRead > 3)
	{	// ищем заголовок
		pFound = (char *)memchr(cBuffer, 0xFF, nBytesRead);
		while(pFound)
		{
			if(DecodeHeader(pFound, mpeghdr))
			{	// устанавливаем указатель на начало заголовка
				CSndFile::SetFilePointer(-(nBytesRead - (pFound-cBuffer)),
					SND_FILE_CURRENT);
				return true;
			}
			// ищем в следующем интервале (смещ.1 на 4 байта - длина заголовка)
			if(pFound - cBuffer >= nBytesRead - 4)
				break;
			
			pFound = (char *)memchr(pFound+4, 0xFF,
				nBytesRead - (pFound+4-cBuffer) - 1);			
		}
		// читаем новую порцию данных
		CSndFile::SetFilePointer(-3, SND_FILE_CURRENT);
		nBytesRead = CSndFile::Read(cBuffer, BUFFER_SIZE);
	}// while(dwBytesRead > 3)

	return false;
}

//===========================================================================
bool CSndFile_MP3::GetFileInfo()
{
// Подготовка файла к чтению и записи.
// Осуществляет:
//   подсчет количества фреймов;
//   получение информации из 1-го фрейма;
//   установка указателя на 1-й фрейм.

	int  nFrameCounter = 0;
	MPEG_HEADER mpeghdr;

	// определяем количество mpeg фреймов в файле
	SetFilePointer(0, SND_FILE_BEGIN);
	while(FindNextHeader(&mpeghdr))
	{
		nFrameCounter++;
		CSndFile::SetFilePointer(mpeghdr.framesize, SND_FILE_CURRENT);
	}

	if(nFrameCounter > 0)
	{	// устанавливаем указатель на 1-й фрейм
		SetFilePointer(0, SND_FILE_BEGIN);
		CSndFile::SetFilePointer(mpeghdr.framesize, SND_FILE_CURRENT);
		FindNextHeader(&mpeghdr);
		
		// производим инициализацию переменных
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

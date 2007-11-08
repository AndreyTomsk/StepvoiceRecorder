//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MappedMP3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
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

const char* sMPEGaudioVersion[2][2] = {
	{"MPEG Version 1 (ISO/IEC 11172-3)", "MPEG Version 2 (ISO/IEC 13818-3)"},
	{"reserver", "MPEG Version 2.5"}
};
const char* sMPEGlayer[] = {
	"Layer I",
	"Layer II",
	"Layer III"
};
const char* sChannelMode[]=
{	"Stereo",
    "Joint stereo (Stereo)",
    "Dual channel (Stereo)",
    "Single channel (Mono)"
};
const char* sEmphasis[] = {
	"none",
	"50/15 ms",
	"reserved",
	"CCIT J.17"
};

//////////////////////////////////////////////////////////////////////
CMappedMP3::CMappedMP3()
{
	InitFileMapping();
}


//////////////////////////////////////////////////////////////////////
CMappedMP3::~CMappedMP3()
{
	CloseFileMapping();
}


//////////////////////////////////////////////////////////////////////
void CMappedMP3::InitFileMapping()
{
	fm.hFile		= INVALID_HANDLE_VALUE;
    fm.hMapFile		= NULL;
    fm.lpMapAddress	= NULL;
    fm.dwSize		= 0xFFFFFFFF;
    *fm.szFileName	= 0;
}


//===========================================================================
bool CMappedMP3::OpenFileMapping(char* szFileName)
{
	CloseFileMapping();
	
    fm.hFile = CreateFile(szFileName,
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		0,
		NULL);
    
	if(fm.hFile == INVALID_HANDLE_VALUE)
		return FALSE;

    // создаем file-mapping объект для нашего файла
    fm.hMapFile = CreateFileMapping(fm.hFile,	// дескриптор файла
		NULL,									// атрибуты безопасности
		PAGE_READWRITE,							// запись/чтение
		0,										// high-order DWORD of size
		0,								// макс. размер объекта, ниж. часть
		NULL);									// имя объекта для маппинга

    if (fm.hMapFile == NULL )
	{
        CloseHandle(fm.hFile);
        return FALSE;
    }

    // отображаем образ файла в адресное пространство нашего процесса
    fm.lpMapAddress = (char *)MapViewOfFile(fm.hMapFile,
		FILE_MAP_WRITE,
		0,
		0,
		0);

    if (fm.lpMapAddress == NULL)
	{
    	CloseHandle(fm.hMapFile);
        CloseHandle(fm.hFile);
        return FALSE;
    }

	// получаем размер файла
    fm.dwSize = GetFileSize(fm.hFile, NULL);
    if (fm.dwSize == 0xFFFFFFFF)
	{
        UnmapViewOfFile(fm.lpMapAddress);
        CloseHandle(fm.hMapFile);
        CloseHandle(fm.hFile);
    	return FALSE;
    }

    strcpy(fm.szFileName, szFileName);

    return InitMp3Stream();
}

//===========================================================================
void CMappedMP3::CloseFileMapping()
{
	if(IsOpened())
	{
		// закрываем файл
		UnmapViewOfFile(fm.lpMapAddress);
		CloseHandle(fm.hMapFile);
		CloseHandle(fm.hFile);

		// инициализируем переменные
		InitFileMapping();
	}
}


//===========================================================================
bool CMappedMP3::IsOpened()
{
	return fm.hMapFile != NULL;
}

//////////////////////////////////////////////////////////////////////
bool CMappedMP3::InitMp3Stream()
{
// INPUT:	FILEMAPPING *fm		- initialized with call to openFileMapping(char *filename);
//          MP3STREAM *mp3s		-
//
//	OUTPUT: MP3STREAM *mp3s		- fill main data needed to play
//
//	RETURN: TRUE	- all OK
//			FALSE	- can't find first header ( error in mp3 file )

	mp3s.lpStreamStart = fm.lpMapAddress;	// copy starting address of the mapped view
	mp3s.uStreamLength = fm.dwSize;			// length of whole stream

	mp3s.spos.lpPos  = mp3s.lpStreamStart;	// set pos to stream start;
	mp3s.spos.uSize  = mp3s.uStreamLength;	// current length of rest of stream
	mp3s.spos.uFrame = 0;					// current frame

	mp3s.uFrameNum = 0;						// count number of frames

	// find first header
	FRAMEHEADER fh;
	if (!FindNextHeader(&fh)) {
		return FALSE;
	}
	mp3s.lpAudioStart = fh.spos.lpPos;	// set first frame pointer
	mp3s.nChannel = fh.stereo;			// number of channels
	mp3s.dwSamplingRate = freqs[fh.sampling_frequency];	// sampling frequency
	mp3s.iLayer = fh.lay;		// layer version
	mp3s.iMPEGversion = fh.lsf;	// mpeg version
	// set frame size
	if (mp3s.iMPEGversion == 1 ) // MPEG 2 & MPEG 2.5
		mp3s.uFrameSize = 576;
	else // MPEG 1
		mp3s.uFrameSize = 1152;

	// MPEG version
	strcpy(mp3s.MPEGversion, sMPEGaudioVersion[fh.mpeg25][fh.lsf]);
	// MPEG layer
	strcpy(mp3s.MPEGlayer, sMPEGlayer[fh.lay-1]);
	// channel mode
	strcpy(mp3s.channelMode, sChannelMode[fh.mode]);
	// emphasis
	strcpy(mp3s.emphasis, sEmphasis[fh.emphasis]);

	mp3s.iMinBitrate = 0xFF;	// default
	mp3s.iMaxBitrate = 0;		// bitrate indexes

	// count all file frames
	mp3s.uFrameNum++; // count this frame ?
	while (ReadNFrames(&fh, 1) == STREAM_OK) {
		mp3s.uFrameNum++;
		mp3s.iMinBitrate = min(mp3s.iMinBitrate, fh.bitrate_index);
		mp3s.iMaxBitrate = max(mp3s.iMaxBitrate, fh.bitrate_index);
	}
	mp3s.uAudioLength = mp3s.spos.lpPos - mp3s.lpAudioStart;

	// is variable birate
	mp3s.bVbr = (mp3s.iMinBitrate != mp3s.iMaxBitrate);

	mp3s.iMinBitrate = tabsel_123[fh.lsf][fh.lay-1][mp3s.iMinBitrate];
	mp3s.iMaxBitrate = tabsel_123[fh.lsf][fh.lay-1][mp3s.iMaxBitrate];

	// set position to first frame
	mp3s.spos.lpPos = mp3s.lpAudioStart;
	mp3s.spos.uSize = mp3s.uAudioLength;
	mp3s.spos.uFrame= 0;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
bool CMappedMP3::Mp3Seek(int iOffset, int iWhence)
{
	// проверяем, что файл открыт
	if(!IsOpened())
		return TRUE;

	int newpos;
	FRAMEHEADER fh;
	switch(iWhence)	{
		case MP3_FILE_BEGIN:
			if (iOffset >= 0 ) {
				mp3s.spos.lpPos  = mp3s.lpAudioStart;
    			mp3s.spos.uSize  = mp3s.uAudioLength;
    			mp3s.spos.uFrame = 0;
    			ReadNFrames(&fh, iOffset);
				return TRUE;
			}
			break;

		case MP3_FILE_CURRENT:
			mp3s.spos.lpPos  = mp3s.lpAudioStart;
			mp3s.spos.uSize  = mp3s.uAudioLength;

			newpos = mp3s.spos.uFrame + iOffset;
			if (newpos < 0) newpos = 0;
      		mp3s.spos.uFrame = 0;
			ReadNFrames(&fh, newpos);
			return TRUE;

		case MP3_FILE_END:
			if (iOffset > 0 ) {
        		mp3s.spos.lpPos  = mp3s.lpAudioStart;
    			mp3s.spos.uSize  = mp3s.uAudioLength;
    			mp3s.spos.uFrame = 0;
        		
				newpos = mp3s.uFrameNum - iOffset;
				ReadNFrames(&fh, newpos);
				return TRUE;
			}
		break;
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
bool CMappedMP3::DecodeFrameHeader(char* header, FRAMEHEADER* fh)
{
// decode first 32 bits in frame ( frame header )
//
// INPUT:	char *header	- pointer to header start
//
// OUTPUT:	FRAMEHEADER *fh	- fill all
//
// RETURN:	TRUE	- if valid header
//			FALSE	- if invalid header

	unsigned long newhead;
	newhead = (unsigned long) ( ( ( header[0] & 0xFF ) << 24 ) |
								( ( header[1] & 0xFF ) << 16 ) |
								( ( header[2] & 0xFF ) <<  8 ) |
								  ( header[3] & 0xFF )
							  );
	// checking sync bits
	if ( ( ( newhead >> 21 ) & 0x07FF ) != 0x07FF )
		return FALSE;

	// MPEG audio version
	if( newhead & (1<<20) ) {
	  fh->lsf = (newhead & (1<<19)) ? 0x0 : 0x1;
	  fh->mpeg25 = 0;
	}
	else {
		if ( newhead & (1<<19))
			return FALSE;
	  fh->lsf = 1;
	  fh->mpeg25 = 1;
	}

	// MPEG Layer version
	fh->lay = 4 - ((newhead>>17)&3);
	if (fh->lay == 4 )
		return FALSE;

	// crc error protection
	fh->error_protection = ((newhead>>16)&0x1)^0x1;

	// bitrate
	fh->bitrate_index = ((newhead>>12)&0xF);
	if (! fh->bitrate_index || fh->bitrate_index == 0xF)
		return FALSE;

	// sampling frequency rate
	if( ((newhead>>10)&0x3) == 0x3) {
	  return FALSE;
	}
	if(fh->mpeg25) {
	  fh->sampling_frequency = 6 + ((newhead>>10)&0x3);
	}
	else
	  fh->sampling_frequency = ((newhead>>10)&0x3) + (fh->lsf*3);

	// padding
	fh->padding   = ((newhead>>9)&0x1);
	// private bit
	fh->extension = ((newhead>>8)&0x1);
	// channel mode
	fh->mode      = ((newhead>>6)&0x3);
	//Mode extension (Only if Joint stereo)
	fh->mode_ext  = ((newhead>>4)&0x3);
	// Copyright
	fh->copyright = ((newhead>>3)&0x1);
	// Original
	fh->original  = ((newhead>>2)&0x1);
	// Emphasis
	fh->emphasis  = newhead & 0x3;
	// stereo
	fh->stereo    = (fh->mode == MPG_MD_MONO) ? 1 : 2;

	// calculate frame size
	switch(fh->lay) {
	case 1:
	#if 0
		fh->framesize  = (long) tabsel_123[fh->lsf][0][fh->bitrate_index] * 12000;
		fh->framesize /= freqs[fhf->sampling_frequency];
		fh->framesize  = ((fh->framesize + fh->padding)<<2);
	#else
		 return FALSE;	// layer=1 not supported!
	#endif
	break;
	
	case 2:
		fh->framesize = (long) tabsel_123[fh->lsf][1][fh->bitrate_index] * 144000;
		fh->framesize /= freqs[fh->sampling_frequency];
		fh->framesize += fh->padding;
	break;

	case 3:
		fh->framesize  = (long) tabsel_123[fh->lsf][2][fh->bitrate_index] * 144000;
		fh->framesize /= freqs[fh->sampling_frequency]<<(fh->lsf);
		fh->framesize = fh->framesize + fh->padding ;
	break;

	default:
		return FALSE;
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
bool CMappedMP3::FindNextHeader(FRAMEHEADER* fh)
{
// search stream for next valid frame header
//
// INPUT:	MP3STREAM *mp3s
//
// OUTPUT:	update STREAMPOS *spos
//			FRAMEHEADER *fh
//
//	RETURN:	TRUE	- if found valid frame header
//			FASLE	- can't find valid frame header 

    char *newhead;
    FRAMEHEADER fhl;
	while( (newhead = (char *)memchr(mp3s.spos.lpPos, 255, mp3s.spos.uSize)) != NULL) { // find 0xFF byte
    	if(DecodeFrameHeader(newhead, fh)) {	// if valid first frame header
        	// go to next header
            if( mp3s.spos.uSize - fh->framesize < 4 ) {	// end of stream
                mp3s.spos.uSize -= (newhead - mp3s.spos.lpPos);
                mp3s.spos.lpPos = newhead;
                fh->spos.lpPos = newhead;
            	return TRUE;
            }
            if (DecodeFrameHeader(newhead + fh->framesize, &fhl)) {	// if valid second frame header
            	if( fhl.lsf == fh->lsf && fhl.mpeg25 == fh->mpeg25 && fhl.lay == fh->lay && fhl.sampling_frequency == fh->sampling_frequency ) {
                	mp3s.spos.uSize -= (newhead - mp3s.spos.lpPos);
                	mp3s.spos.lpPos = newhead;
                    fh->spos.lpPos = newhead;
            		return TRUE;
                }
            }// second decodeFrameHeader
        }// first -||-
        mp3s.spos.lpPos++;
        mp3s.spos.uSize--;
        if ( mp3s.spos.uSize < 4 ) // end of stream
        	return FALSE;
    }// while
    return FALSE;
}


//////////////////////////////////////////////////////////////////////
int CMappedMP3::ReadNFrames(FRAMEHEADER* mp3f, int iNum)
{
// get n frames from stream
//
// INPUT:	MP3STREAM *mp3s			- need  ( mp3s->spos)
//
//	OUTPUT:	MP3STREAM   *mp3s		- update MP3STREAM *mp3s->spos
//          FRAMEHEADER *mp3f		- set mp3f->framesize to length of  num frames
//
//	RETURN: STREAM_OK	 - if found num frames
//			STREAM_END	 - if found  less than num frames , but minimum one frame found
//			STREAM_ERROR - can't find any frame
//
//	REMARK:	function updates mp3s->spos for sequential call to this function

	int frameLength = 0;
	FRAMEHEADER fh;

	mp3f->spos = mp3s.spos;

	for (int i=0; i < iNum; i++) { // count frames
		if(DecodeFrameHeader(mp3s.spos.lpPos, &fh)) {
			if(fh.framesize > mp3s.spos.uSize) { // stream don't contain full frame i.e. end of stream
				mp3f->spos.uSize = frameLength;
				return STREAM_END;
			}

			frameLength += fh.framesize;

			// jump to end of frame ( to start of next frame )
			mp3s.spos.lpPos += fh.framesize;
			mp3s.spos.uSize -= fh.framesize;
			mp3s.spos.uFrame++;

			if (!mp3s.spos.uSize ) { // end of stream
				mp3f->spos.uSize = frameLength;
				return STREAM_END;
			}
		}// if decodeFrameHeader
		else {
			if (frameLength == 0 ) // can't find first frame
				return STREAM_ERROR;
			else {
				mp3f->spos.uSize = frameLength;
				return STREAM_END;
			}
		}// else
	} // for
	mp3f->spos.uSize = frameLength;
	return STREAM_OK;
}

//===========================================================================
int CMappedMP3::Write(char *pBuf, int nBufSize)
{
	// пока заглушка
	return nBufSize;
}

//===========================================================================

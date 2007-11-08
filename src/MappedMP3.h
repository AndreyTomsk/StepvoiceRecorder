//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MAPPEDMP3_H__5FEAAE87_127B_4B3C_AE17_9C15CAACA966__INCLUDED_)
#define AFX_MAPPEDMP3_H__5FEAAE87_127B_4B3C_AE17_9C15CAACA966__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#define MP3_FILE_BEGIN		0
#define MP3_FILE_CURRENT	1
#define	MP3_FILE_END		2

#define STREAM_OK			0
#define STREAM_END			1
#define STREAM_ERROR		2

#define	MPG_MD_STEREO		0
#define MPG_MD_JOINT_STEREO	1
#define MPG_MD_DUAL_CHANNEL	2
#define MPG_MD_MONO			3

//////////////////////////////////////////////////////////////////////
typedef struct {
	char*	lpPos;
	UINT	uSize;
	UINT	uFrame;
} STREAMPOS;

//////////////////////////////////////////////////////////////////////
typedef struct {
	HANDLE	hFile;			// handle of opened file
	HANDLE	hMapFile;		// handle of file-mapping object
	char*	lpMapAddress;	// starting address of the mapped view (! don't modify)
	DWORD	dwSize;
	char	szFileName[MAX_PATH];
} FILEMAPPING;

//////////////////////////////////////////////////////////////////////
typedef struct {
	char*	lpStreamStart;	// start of stream
	UINT	uStreamLength;	// length of stream
	char*	lpAudioStart;	// start of audio data
	UINT	uAudioLength;	// length of audio data
	char*	lpPos;			// current position
	UINT	uLength;		// length of rest of stream (uStreamLength - (lpPos - lpStreamStart))
	UINT	uFrameNum;		// number of frames
	UINT	uCurrFrame;		// current frame
	bool	bVbr;			// variable bitrate
	WORD	nChannel;		// numb. of channels (1 = MONO, 2 = STEREO)
	DWORD	dwSamplingRate;	// sampling frequency (Hz)
	int		iLayer;			// layer
	int		iMPEGversion;	// mpeg version
	int		iInputBufferLength;	// input buffer (number of frames to load per output buffer)
	char	MPEGversion[64];
	char	MPEGlayer[40];
	int		iMinBitrate;	// min bitrate index
	int		iMaxBitrate;	// max bitrate index
	char	channelMode[40];
	char	emphasis[40];
	UINT	uFrameSize;		// number of samples in frame
	STREAMPOS	spos;

} MP3STREAM;

//////////////////////////////////////////////////////////////////////
typedef struct {
	int	stereo;		// channel number (1 = mono, 2 = stereo)
	int	lsf;
	int	mpeg25;
	int	lay;
	int	error_protection;
	int	sampling_frequency;
	int	bitrate_index;
	int padding;
	int	extension;
	int	mode;
	int	mode_ext;
	int	copyright;
	int	original;
	int	emphasis;
	int	framesize;	// computed framesize
	STREAMPOS	spos;

} FRAMEHEADER;


//////////////////////////////////////////////////////////////////////
class CMappedMP3  
{
	FILEMAPPING fm;

	void InitFileMapping();
	bool InitMp3Stream();
	bool FindNextHeader(FRAMEHEADER* fh);
	bool DecodeFrameHeader(char* header, FRAMEHEADER* fh);

public:
	MP3STREAM	mp3s;

	CMappedMP3();
	~CMappedMP3();
	bool OpenFileMapping(char* szFileName);
	void CloseFileMapping();
	bool IsOpened();
	bool Mp3Seek(int iOffset, int iWhence);
	int  ReadNFrames(FRAMEHEADER* fh, int iNum);
	int	 Write(char* pBuf, int nBufSize);
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_MAPPEDMP3_H__5FEAAE87_127B_4B3C_AE17_9C15CAACA966__INCLUDED_)

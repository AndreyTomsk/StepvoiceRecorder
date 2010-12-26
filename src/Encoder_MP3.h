
#ifndef _ENCODER_MP3
#define _ENCODER_MP3

#include <BladeMP3EncDll.h>

///////////////////////////////////////////////////////////////////////////////
class CEncoder_MP3// : public CEncoder
{
public:
	CEncoder_MP3(int nBitrate, int nFrequency, int nChannels);
	virtual ~CEncoder_MP3();

	virtual bool EncodeChunk(char* pBufIn, int nBufInSize, char* pBufOut, int& nBufOutSize);

private:
	void LoadLibrary(CString& strDllPath);
	void FreeLibrary();

	void InitEncoder(int nBitrate, int nFrequency, int nChannels);
	void CloseEncoder();

	HINSTANCE  m_hDll;			// дескриптор библиотеки lame.dll
	BE_CONFIG  m_beConfig;		// стркутура настроек кодировани€
	HBE_STREAM m_hbeStream;		// дескриптор потока кодровани€

	char*	   m_pChunkBuf;			//
	int		   m_nChunkBufOffset;	//
	int		   m_nChunkBufSize;		//
	int		   m_nChunkBufSamples;	//

	char*	   m_pBufIn;		//
	int		   m_nBufInSize;	//
};

///////////////////////////////////////////////////////////////////////////////
#endif // _ENCODER_MP3

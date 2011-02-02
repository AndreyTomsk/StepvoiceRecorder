
#ifndef _ENCODER_MP3
#define _ENCODER_MP3

#include <BladeMP3EncDll.h>

///////////////////////////////////////////////////////////////////////////////
class CEncoder_MP3// : public CEncoder
{
public:
	CEncoder_MP3(int nBitrate, int nFrequency, int nChannels); // throws CString exceptions
	virtual ~CEncoder_MP3();

	bool EncodeChunk(char* pBufIn, int nBufInSize, char* pBufOut, int& nBufOutSize);
	bool EncodeChunkFloat(float* pBufIn, int nBufInSize, char* pBufOut, int& nBufOutSize);
	void WriteVBRHeader(const CString& mp3FileName); // recommended to call after closing.

private:
	void LoadLibrary(CString& strDllPath);
	void FreeLibrary();

	void InitEncoder(int nBitrate, int nFrequency, int nChannels);
	void CloseEncoder();

	HINSTANCE  m_hDll;			// дескриптор библиотеки lame.dll
	BE_CONFIG  m_beConfig;		// стркутура настроек кодировани€
	HBE_STREAM m_hbeStream;		// дескриптор потока кодровани€

	char*	   m_pChunkBuf;		// internal chunk buffer for encoding.
	int		   m_nChunkBufSize;

	float* m_chunkBufFloat_l;
	float* m_chunkBufFloat_r;
	int    m_chunkBufFloatSize;
};

///////////////////////////////////////////////////////////////////////////////
#endif // _ENCODER_MP3

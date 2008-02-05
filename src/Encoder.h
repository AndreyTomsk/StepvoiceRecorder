
#ifndef _COMMON_ENCODER_H
#define _COMMON_ENCODER_H

///////////////////////////////////////////////////////////////////////////////
class CEncoder  
{
public:
	enum ReturnCodes
	{
		ENC_NOERROR,
		ENC_ERR_DLL,
		ENC_ERR_INIT,
		ENC_ERR_UNK
	};
public:
	virtual int  InitEncoder(void* pSettings) = 0;
	virtual bool EncodeChunk(char* pBufIn,  int  nBufInSize,
		                     char* pBufOut, int &nBufOutSize) = 0;
	virtual void CloseEncoder() = 0;
};

///////////////////////////////////////////////////////////////////////////////
#endif // _COMMON_ENCODER_H

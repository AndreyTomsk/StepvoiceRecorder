//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ENCODER_WAV_H__95FDF793_29A3_4F3A_B23E_38DA074B9615__INCLUDED_)
#define AFX_ENCODER_WAV_H__95FDF793_29A3_4F3A_B23E_38DA074B9615__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "encoder.h"

//////////////////////////////////////////////////////////////////////
class CEncoder_WAV : public CEncoder
{
public:
	CEncoder_WAV(CString& strDllPath);
	CEncoder_WAV(CString& strDllPath, void* pSettings);
	virtual ~CEncoder_WAV();

	virtual int InitEncoder(void* pSettings);
	virtual bool EncodeChunk(char* pBufIn,  int  nBufInSize,
							 char* pBufOut, int& nBufOutSize);
	virtual void CloseEncoder();
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_ENCODER_WAV_H__95FDF793_29A3_4F3A_B23E_38DA074B9615__INCLUDED_)

//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mp3_recorder.h"
#include "Encoder_WAV.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//====================================================================
//  ŒÕ—“–” “Œ– / ƒ≈—“–” “Œ–
//====================================================================
CEncoder_WAV::CEncoder_WAV(CString& strDllPath)
: CEncoder(strDllPath)
{
}

//--------------------------------------------------------------------
CEncoder_WAV::CEncoder_WAV(CString& strDllPath, void* pSettings)
: CEncoder(strDllPath, pSettings)
{
}

//--------------------------------------------------------------------
CEncoder_WAV::~CEncoder_WAV()
{
}

//====================================================================
// »Õ“≈–‘≈…—Õ€≈ ‘”Õ ÷»»
//====================================================================
int CEncoder_WAV::InitEncoder(void* pSettings)
{
	bool bError = true;

	if(CEncoder::InitEncoder(pSettings))
	{
		bError = false;
	}

	return bError;
}

//--------------------------------------------------------------------
bool CEncoder_WAV::EncodeChunk(char* pBufIn,  int  nBufInSize,
							   char* pBufOut, int& nBufOutSize)
{
	bool bResult = false;

	if(pBufIn && pBufOut)
	{
		memcpy(pBufOut, pBufIn, nBufInSize);
		nBufOutSize = nBufInSize;
		bResult = true;
	}

	return bResult;
}

//--------------------------------------------------------------------
void CEncoder_WAV::CloseEncoder()
{
	CEncoder::CloseEncoder();
}

//====================================================================

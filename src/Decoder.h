//////////////////////////////////////////////////////////////////////
#if !defined(AFX_DECODER_H__ECA92FCB_E7C8_4BD4_BEB5_448BBA04455D__INCLUDED_)
#define AFX_DECODER_H__ECA92FCB_E7C8_4BD4_BEB5_448BBA04455D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
class CDecoder  
{
public:
	CDecoder(CString& strDllPath);
	CDecoder(CString& strDllPath, void* pSettings);
	virtual ~CDecoder();

	virtual bool InitDecoder(void* pSettings);
	virtual bool DecodeChunk(char* pBufIn,  int  nBufInSize,
							 char* pBufOut, int &nBufOutSize) = 0;
	virtual void CloseDecoder();
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_DECODER_H__ECA92FCB_E7C8_4BD4_BEB5_448BBA04455D__INCLUDED_)

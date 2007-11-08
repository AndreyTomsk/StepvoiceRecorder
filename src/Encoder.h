//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ENCODER_H__805795C2_4D9C_4B0E_B4A2_64EBC0BB0EB7__INCLUDED_)
#define AFX_ENCODER_H__805795C2_4D9C_4B0E_B4A2_64EBC0BB0EB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#define ENC_NOERROR		0
#define ENC_ERR_DLL		1
#define ENC_ERR_INIT	2
#define ENC_ERR_UNK		3

//////////////////////////////////////////////////////////////////////
class CEncoder  
{
public:
	CEncoder(CString& strDllPath);
	CEncoder(CString& strDllPath, void* pSettings);
	virtual ~CEncoder();

	virtual int InitEncoder(void* pSettings);
	virtual bool EncodeChunk(char* pBufIn,  int  nBufInSize,
		                     char* pBufOut, int &nBufOutSize) = 0;
	virtual void CloseEncoder();
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_ENCODER_H__805795C2_4D9C_4B0E_B4A2_64EBC0BB0EB7__INCLUDED_)

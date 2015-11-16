#include "stdafx.h"
#include "MyException.h"
#include <Strsafe.h>

IMPLEMENT_DYNAMIC(CMyException, CException)

CMyException::CMyException(CString message)
	:CSimpleException()
{
	//CSimpleException has 128 characters buffer, limiting message to it.
	StringCchCopy(m_szMessage, 128, message);
	m_bInitialized = TRUE;
	m_bLoaded = TRUE;
}

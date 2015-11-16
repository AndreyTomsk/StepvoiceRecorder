#pragma once

class CMyException : public CSimpleException
{
	DECLARE_DYNAMIC(CMyException)
public:
	CMyException(CString message);
};

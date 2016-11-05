/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once

class CMyException : public CSimpleException
{
	DECLARE_DYNAMIC(CMyException)
public:
	CMyException(CString message);
};

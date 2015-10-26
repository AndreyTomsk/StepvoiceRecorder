#include "stdafx.h"
#include "Debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

WriteDbg::WriteDbg()
{
}
//---------------------------------------------------------------------------

WriteDbg::~WriteDbg()
{
	::OutputDebugStringW(m_stream.str().c_str());
}

/////////////////////////////////////////////////////////////////////////////

WriteDbg& operator <<(WriteDbg& writer, CString str)
{
	writer << str.GetBuffer(str.GetLength());
	return writer;
}
//---------------------------------------------------------------------------

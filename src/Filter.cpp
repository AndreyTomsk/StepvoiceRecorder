#include "stdafx.h"
#include "Filter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

Parameter::Parameter(CString name, int value)
	:m_name(name)
	,m_type(eInteger)
	,m_value_int(value)
{
}
//---------------------------------------------------------------------------

Parameter::Parameter(CString name, float value)
	:m_name(name)
	,m_type(eFloat)
	,m_value_float(value)
{
}
//---------------------------------------------------------------------------

Parameter::Parameter(CString name, CString value)
	:m_name(name)
	,m_type(eString)
	,m_value_string(value)
{
}

/////////////////////////////////////////////////////////////////////////////

Filter::Filter()
	:m_childFilter(NULL)
	,m_notifyCallback(NULL)
	,m_userData(NULL)
{
}
//---------------------------------------------------------------------------

Filter::~Filter()
{
}
//---------------------------------------------------------------------------

void Filter::SetChildFilter(Filter* child)
{
	m_childFilter = child;
}
//---------------------------------------------------------------------------

void Filter::SetNotifyCallback(NOTIFY_CALLBACK func, void* userData)
{
	m_notifyCallback = func;
	m_userData = userData;
}
//---------------------------------------------------------------------------

void Filter::SendNotification(const Parameter& parameter)
{
	if (m_notifyCallback != NULL)
		m_notifyCallback(this, parameter, m_userData);
}
//---------------------------------------------------------------------------

bool Filter::ProcessData(void* buffer, DWORD lengthBytes)
{
	if (m_childFilter != NULL)
		return m_childFilter->ProcessData(buffer, lengthBytes);
	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Filter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

Parameter::Parameter(CString name, int value)
	:name(name)
	,type(eInteger)
	,valueInt(value)
{
}
//---------------------------------------------------------------------------

Parameter::Parameter(CString name, float value)
	:name(name)
	,type(eFloat)
	,valueFloat(value)
{
}
//---------------------------------------------------------------------------

Parameter::Parameter(CString name, CString value)
	:name(name)
	,type(eString)
	,valueString(value)
{
}
//---------------------------------------------------------------------------

WriteDbg& operator <<(WriteDbg& writer, const Parameter& param)
{
	writer << "'" << param.name << "'=";
	if (param.type == Parameter::eInteger)
		writer << param.valueInt;
	if (param.type == Parameter::eFloat)
		writer << param.valueFloat;
	if (param.type == Parameter::eString)
		writer << param.valueString;

	return writer;
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

#include "stdafx.h"
#include "FilterChain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

FilterChain::FilterChain(Filter::NOTIFY_CALLBACK func, void* userData)
	:m_notifyCallback(func)
	,m_userData(userData)
{
}
//---------------------------------------------------------------------------

FilterChain::~FilterChain()
{
	Empty();
}
//---------------------------------------------------------------------------

void FilterChain::AddFilter(Filter* filter)
{
	ASSERT(filter != NULL);
	filter->SetNotifyCallback(m_notifyCallback, m_userData);

	//Making chain
	if (m_filters.size() > 0)
		m_filters.back()->SetChildFilter(filter);

	m_filters.push_back(filter);
}
//---------------------------------------------------------------------------

void FilterChain::Empty()
{
	//Deleting filters in reverse order.
	for (int i = m_filters.size()-1; i >= 0; i--)
	{
		if (i > 0)
			m_filters[i-1]->SetChildFilter(NULL);

		delete m_filters[i];
		m_filters[i] = NULL;
	}
	m_filters.clear();
}
//---------------------------------------------------------------------------

bool FilterChain::IsEmpty() const
{
	return m_filters.empty();
}
//---------------------------------------------------------------------------

/*
unsigned FilterChain::FilterCount() const
{
	return m_filters.size();
}
//---------------------------------------------------------------------------

Filter* FilterChain::GetFilter(unsigned index) const
{
	return (index < m_filters.size()) ? m_filters[index] : NULL;
}
//---------------------------------------------------------------------------
*/
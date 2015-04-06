#ifndef FILTER_CHAIN_H
#define FILTER_CHAIN_H
#pragma once

#include "Filter.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
//Helper class to prepare chain from several filters.
//Filters must be created on heap. Automatically deletes it on destruction.

class FilterChain
{
public:
	FilterChain(Filter::NOTIFY_CALLBACK func, void* userData);
	~FilterChain();

	void AddFilter(Filter* filter);
	void Empty();
	bool IsEmpty() const;
	//unsigned FilterCount() const;
	//Filter* GetFilter(unsigned index) const;

	//Convenient way to get a required filter.
	template<class T>
	T* GetFilter() const
	{
		for (size_t i = 0; i < m_filters.size(); i++)
		{
			if (dynamic_cast<T*>(m_filters[i]))
				return dynamic_cast<T*>(m_filters[i]);
		}
		return NULL;
	}

private:
	std::vector<Filter*> m_filters;
	Filter::NOTIFY_CALLBACK m_notifyCallback;
	void* m_userData;
};

/////////////////////////////////////////////////////////////////////////////
#endif

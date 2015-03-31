#ifndef FILTER_H
#define FILTER_H
#pragma once

/////////////////////////////////////////////////////////////////////////////

struct Parameter
{
	Parameter(CString name, int value);
	Parameter(CString name, float value);
	Parameter(CString name, CString value);

	CString m_name;

	enum Type {eInteger, eFloat, eString};
	Type m_type;

	int m_value_int;
	float m_value_float;
	CString m_value_string;
};

/////////////////////////////////////////////////////////////////////////////
// Provides functionality for chain processing. Base class.
// All other useful info like stereo mode, etc. suggested to store in child
// classes.

class Filter
{
public:
	typedef void (*NOTIFY_CALLBACK)(
		const Filter* fromFilter, const Parameter& parameter, void* userData);

	void SetChildFilter(Filter* child);
	void SetNotifyCallback(NOTIFY_CALLBACK func, void* userData);

protected:
	Filter();

	//Helper function for derived classes - notifying via callback.
	//If callback not set - does nothing.
	void SendNotification(const Parameter& parameter);

	//Override method for custom buffer processing.
	//Use this method for passing data to next filter in chain.
	virtual bool ProcessData(void* buffer, DWORD lengthBytes);

private:
	Filter* m_childFilter;
	NOTIFY_CALLBACK m_notifyCallback;
	void* m_userData;
};

/////////////////////////////////////////////////////////////////////////////
#endif
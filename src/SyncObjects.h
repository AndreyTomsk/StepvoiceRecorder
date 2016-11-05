/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#ifndef _SYNC_OBJECTS_H
#define _SYNC_OBJECTS_H

////////////////////////////////////////////////////////////////////////////////
class CMyCriticalSection
{
public:
	CMyCriticalSection()
	{
		InitializeCriticalSection(&m_crit_object);
	}
	~CMyCriticalSection()
	{
		DeleteCriticalSection(&m_crit_object);
	}

private:
	friend class CMyLock;
	CRITICAL_SECTION m_crit_object;
};

////////////////////////////////////////////////////////////////////////////////
class CMyLock
{
public:
	CMyLock(CMyCriticalSection& a_crit_section)
		:m_crit_section(a_crit_section)
	{
		EnterCriticalSection(&m_crit_section.m_crit_object);
	}
	~CMyLock()
	{
		LeaveCriticalSection(&m_crit_section.m_crit_object);
	}

private:
	CMyLock(const CMyLock &);
	const CMyLock& operator=(const CMyLock &);
private:
	CMyCriticalSection& m_crit_section;
};

////////////////////////////////////////////////////////////////////////////////

#endif // _SYNC_OBJECTS_H

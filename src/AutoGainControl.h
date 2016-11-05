/*
Stepvoice Recorder
Copyright (C) 2004-2016 Andrey Firsov
*/

#pragma once
#include "WasapiHelpers.h"
#include "SyncObjects.h"

//Controlling recording volume, based on actual recording level.

class CAutoGainControl
{
public:
	CAutoGainControl();
	~CAutoGainControl();

	void Start(IWasapiRecorder* recorder);
	void Stop();

private:
	IWasapiRecorder* m_recorder;

	CMyCriticalSection m_syncObject;
	static DWORD WINAPI AdjustmentProc(LPVOID lpParam);
	HANDLE m_thread;
	bool m_exitThread;
};

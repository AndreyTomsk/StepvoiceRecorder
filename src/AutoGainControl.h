#pragma once
#include "WasapiHelpers.h"
#include "SyncObjects.h"

//Controlling recording volume, based on actual recording level.

class CAutoGainControl
{
public:
	CAutoGainControl();
	~CAutoGainControl();

	void Start(IWasapiRecorder* recorder, CSliderCtrl* volumeSlider);
	void Stop();

private:
	IWasapiRecorder* m_recorder;
	CSliderCtrl* m_volumeSlider;

	CMyCriticalSection m_syncObject;
	static DWORD WINAPI AdjustmentProc(LPVOID lpParam);
	HANDLE m_thread;
	bool m_exitThread;
};

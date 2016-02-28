#include "stdafx.h"
#include "AutoGainControl.h"
#include "Utility.h" //for CStopwatchMS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CAutoGainControl::CAutoGainControl()
	:m_recorder(NULL)
	,m_volumeSlider(NULL)
	,m_exitThread(false)
	,m_thread(NULL)
{
	m_thread = ::CreateThread(NULL, 0, AdjustmentProc, this, 0, 0);
	LOG_DEBUG() << __FUNCTION__ <<", m_thread=" << int(m_thread);
}
//---------------------------------------------------------------------------

CAutoGainControl::~CAutoGainControl()
{
	LOG_DEBUG() << __FUNCTION__ << "::1";

	if (m_thread != NULL) {
		m_exitThread = true;
		::WaitForSingleObject(m_thread, INFINITE);
	}

	LOG_DEBUG() << __FUNCTION__ << "::2";
}
//---------------------------------------------------------------------------

void CAutoGainControl::Start(IWasapiRecorder* recorder, CSliderCtrl* volumeSlider)
{
	CMyLock lock(m_syncObject);

	if (recorder != NULL && recorder->VolumeControlAvailable()) {
		m_recorder = recorder;
		m_volumeSlider = volumeSlider;
	}
	else {
		m_recorder = NULL;
		m_volumeSlider = NULL;
	}

	if (recorder != NULL && !recorder->VolumeControlAvailable())
		LOG_WARNING() << __FUNCTION__ <<", volume control not available.";
}
//---------------------------------------------------------------------------

void CAutoGainControl::Stop()
{
	CMyLock lock(m_syncObject);
	m_recorder = NULL;
	m_volumeSlider = NULL;
}
//---------------------------------------------------------------------------

DWORD WINAPI CAutoGainControl::AdjustmentProc(LPVOID lpParam)
{
	CStopwatchMS stopWatchMS;

	//check also recorder not paused.

	CAutoGainControl* agc = static_cast<CAutoGainControl*>(lpParam);
	while (!agc->m_exitThread)
	{
		::Sleep(10);
		//Adjusting volume every 1 second.

		CMyLock lock(agc->m_syncObject);

		IWasapiRecorder* recorder = agc->m_recorder;
		if (recorder == NULL || !recorder->IsStarted())
			continue;

		if (stopWatchMS.GetElapsedTime() > 1000)
		{
			stopWatchMS.Reset();
			LOG_DEBUG() << __FUNCTION__ << "::adjusting";
		}
	}
	return 0;
}
//---------------------------------------------------------------------------

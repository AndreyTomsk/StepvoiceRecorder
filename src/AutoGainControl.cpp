#include "stdafx.h"
#include "AutoGainControl.h"
#include "Utility.h" //for CStopwatchMS
#include <list>
#include <algorithm>
#include <cmath> //for float abs

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CAutoGainControl::CAutoGainControl()
	:m_recorder(NULL)
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

void CAutoGainControl::Start(IWasapiRecorder* recorder)
{
	CMyLock lock(m_syncObject);

	if (recorder != NULL && recorder->VolumeControlAvailable()) {
		m_recorder = recorder;
	}
	else {
		m_recorder = NULL;
	}

	if (recorder != NULL && !recorder->VolumeControlAvailable())
		LOG_WARNING() << __FUNCTION__ <<", volume control not available.";
}
//---------------------------------------------------------------------------

void CAutoGainControl::Stop()
{
	CMyLock lock(m_syncObject);
	m_recorder = NULL;
}
//---------------------------------------------------------------------------

struct TooLoud {
	TooLoud(float loudValue) : m_loudValue(loudValue) {}
	float m_loudValue;
	bool operator() (const float& value) { return value >= m_loudValue; }
};
//---------------------------------------------------------------------------

DWORD WINAPI CAutoGainControl::AdjustmentProc(LPVOID lpParam)
{
	CStopwatchMS stopWatchMS;

	//Last 5 seconds of peak levels. Each measure goes after 25msec, thus
	//200 elements required in list.
	std::list<float> maxLevels;
	maxLevels.resize(200);

	CAutoGainControl* agc = static_cast<CAutoGainControl*>(lpParam);
	while (!agc->m_exitThread)
	{
		::Sleep(25);
		CMyLock lock(agc->m_syncObject);

		//checking recorder state

		IWasapiRecorder* recorder = agc->m_recorder;
		if (recorder == NULL || recorder->IsPaused()) {
			maxLevels.clear();
			stopWatchMS.Reset();
			continue;
		}

		//updating maxLevels list

		if (maxLevels.size() == 200)
			maxLevels.pop_front();
		maxLevels.push_back(std::abs(recorder->GetPeakLevel(-1))); //both channels peak

		//each 2 seconds - updating volume slider.

		if (stopWatchMS.GetElapsedTime() < 1000)
			continue;

		stopWatchMS.Reset();
		const float curMaxLevel = *std::max_element(maxLevels.begin(), maxLevels.end());
		//LOG_DEBUG() << "curMaxLevel=" << curMaxLevel;

		//if (curMaxLevel >= 0.9) - decrease volume, if possible.
		//if (curMexLevel <  0.8) - increase volume, if possible.
		//else - do nothing.

		if (curMaxLevel >= 0.9f) {
			recorder->SetVolume(max(recorder->GetVolume() - 0.1f, 0.0f)); //-10%
			//normalizing value to exclude from next check (testing).
			std::replace_if(maxLevels.begin(), maxLevels.end(), TooLoud(0.9f), 0.85f);
		}
		else
		if (curMaxLevel < 0.8) {
			recorder->SetVolume(min(recorder->GetVolume() + 0.1f, 1.0f)); //+10%
		}
	}
	return 0;
}
//---------------------------------------------------------------------------

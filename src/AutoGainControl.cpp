#include "stdafx.h"
#include "AutoGainControl.h"
#include "Utility.h" //for CStopwatchMS
#include <list>
#include <algorithm>

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

struct TooLoud {
	TooLoud(float loudValue) : m_loudValue(loudValue) {}
	float m_loudValue;
	bool operator() (const float& value) { return value >= m_loudValue; }
};
//---------------------------------------------------------------------------

DWORD WINAPI CAutoGainControl::AdjustmentProc(LPVOID lpParam)
{
	CStopwatchMS stopWatchMS;

	//Last 10 seconds of peak levels. Each measure goes after 10msec, thus
	//1000 elements required in list.
	std::list<float> maxLevels;
	maxLevels.resize(1000);

	CAutoGainControl* agc = static_cast<CAutoGainControl*>(lpParam);
	while (!agc->m_exitThread)
	{
		::Sleep(10);
		CMyLock lock(agc->m_syncObject);

		//checking recorder state

		IWasapiRecorder* recorder = agc->m_recorder;
		if (recorder == NULL) {
			maxLevels.clear();
			continue;
		}
		if (recorder->IsPaused())
			continue;

		//updating maxLevels list

		if (maxLevels.size() == 1000)
			maxLevels.pop_front();
		maxLevels.push_back(abs(recorder->GetPeakLevel(-1))); //both channels peak

		//each 2 seconds - updating volume slider.

		if (stopWatchMS.GetElapsedTime() < 1000)
			continue;

		stopWatchMS.Reset();
		const float curMaxLevel = *std::max_element(maxLevels.begin(), maxLevels.end());

		//if (curMaxLevel >= 0.9) - decrease volume, if possible.
		//if (curMexLevel <  0.8) - increase volume, if possible.
		//else - do nothing.

		CSliderCtrl* vs = agc->m_volumeSlider;
		if (curMaxLevel >= 0.9 && (vs->GetPos() > vs->GetRangeMin())) {
			LOG_DEBUG() << __FUNCTION__ << "::volume down, maxLevel=" << curMaxLevel;

			int prevPos = max(vs->GetPos() - 2*vs->GetPageSize(), vs->GetRangeMin());
			vs->SetPos(prevPos);

			//normalizing value to exclude from next check (test).
			//*itMaxLevel = 0.85f;
			//maxLevels.remove_if(too_loud());
			std::replace_if(maxLevels.begin(), maxLevels.end(), TooLoud(0.9f), 0.85f);
		}
		else
		if (curMaxLevel < 0.8 && (vs->GetPos() < vs->GetRangeMax())) {
			LOG_DEBUG() << __FUNCTION__ << "::volume up, maxLevel=" << curMaxLevel;

			int nextPos = min(vs->GetPos() + 2*vs->GetPageSize(), vs->GetRangeMax());
			vs->SetPos(nextPos);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------

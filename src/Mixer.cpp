//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Mixer.h"
#include "common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
CMixer::CMixer()
	: m_hMixer(NULL)//, m_volArray(NULL)
{
	CAfList<int> l;
	l.Push(NULL);
	l.Pop();
}

CMixer::~CMixer()
{
	Close();
}



//////////////////////////////////////////////////////////////////////
void CMixer::Close()
{
	if(!m_hMixer)
		return;

	mixerClose(m_hMixer);
	m_hMixer = NULL;

/*	while(!m_volVector.empty())
	{
		//!!!
		CControlVolume *pLastVol = &m_volVector[m_volVector.size()-1];
		SAFE_DELETE(pLastVol);
		m_volVector.pop_back();
	}
*/
	while(m_volList.Size() != 0)
	{
		CControlVolume *pLastVol = m_volList.GetTopItem();
		SAFE_DELETE(pLastVol);
		m_volList.Pop();
	}
}

//////////////////////////////////////////////////////////////////////
int CMixer::GetLinesNum() const
{
	//return m_volVector.size();
	return m_volList.Size();
}

/*
//////////////////////////////////////////////////////////////////////
bool CMixer::Open(HWND hWnd, int nMixerID)
{
	MMRESULT Ret;

	m_hMixer = NULL;
	// open mixer
	Ret = mixerOpen(&m_hMixer, nMixerID, DWORD(hWnd), 0, CALLBACK_WINDOW);
	if(Ret != MMSYSERR_NOERROR)
		return false;
	// trying to get mixer parameters
	do {
		// get WaveOut line parameters
		m_DinLine.cbStruct = sizeof(m_DinLine);
		m_DinLine.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
		//m_DinLine.dwDestination = 0;
		Ret = mixerGetLineInfo(HMIXEROBJ(m_hMixer), &m_DinLine,
			MIXER_GETLINEINFOF_COMPONENTTYPE);
		if(Ret != MMSYSERR_NOERROR)
			break;	// unable to get WaveOut line parameters

		// get volume regulator parameters (type - VOLUME)
		m_DinCtls.cbStruct = sizeof(m_DinCtls);
		m_DinCtls.dwLineID = m_DinLine.dwLineID;
		m_DinCtls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		m_DinCtls.cControls= 1;
		m_DinCtls.cbmxctrl = sizeof(m_DinCtl);
		m_DinCtls.pamxctrl = &m_DinCtl;
		Ret = mixerGetLineControls(HMIXEROBJ(m_hMixer), &m_DinCtls,
			MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if(Ret != MMSYSERR_NOERROR)
			break;	// unable to get volume regulator parameters

		// init volume control description
		m_DinDet.cbStruct = sizeof(m_DinDet);
		m_DinDet.dwControlID = m_DinCtl.dwControlID;
		m_DinDet.cChannels = 1;
		m_DinDet.cMultipleItems = 0;
		m_DinDet.cbDetails = sizeof(m_DinVal);
		m_DinDet.paDetails = &m_DinVal;

		return true;
	} while(0);
	// close mixer if operations fail
	Close();
	return false;
}


//////////////////////////////////////////////////////////////////////
void CMixer::SetLine(int nLineID)
{
	m_nLineID = nLineID;
}

//////////////////////////////////////////////////////////////////////
void CMixer::SetLevel(int nPercent)
{
	int nRangeMax, nRangeMin; 
	
	m_nLevel = nPercent;
	// count mixer level
	nRangeMax = m_DinCtl.Bounds.dwMaximum;
	nRangeMin = m_DinCtl.Bounds.dwMinimum;
	m_DinVal.dwValue = (nRangeMax - nRangeMin)*nPercent/100 - nRangeMin;
	// set new level
	mixerSetControlDetails(HMIXEROBJ(m_hMixer), &m_DinDet,
		MIXER_GETCONTROLDETAILSF_VALUE);
}

//////////////////////////////////////////////////////////////////////
int CMixer::GetLine(DWORD lParam)
{
	// get line from mixer

	return m_nLineID;
}

//////////////////////////////////////////////////////////////////////
int CMixer::GetLevel(DWORD lParam)
{
	int nRangeMax, nRangeMin;
	
	if(lParam == 0) lParam = m_DinCtl.dwControlID;
	// check if WaveOut level changed
	if(lParam != m_DinCtl.dwControlID) return m_nLevel;
	// get mixer level
	mixerGetControlDetails(HMIXEROBJ(m_hMixer), &m_DinDet,
		MIXER_GETCONTROLDETAILSF_VALUE);
	// count level in percent
	nRangeMax = m_DinCtl.Bounds.dwMaximum;
	nRangeMin = m_DinCtl.Bounds.dwMinimum;
	m_nLevel = 100*(m_DinVal.dwValue + nRangeMin)/(nRangeMax-nRangeMin);  
	return m_nLevel; // in percent
}


//////////////////////////////////////////////////////////////////////
// Ќовый код
//////////////////////////////////////////////////////////////////////
BOOL CMixer::OpenMixer(HWND hWnd, BOOL bRecMixer)
{
	//if(!mixerGetNumDevs())
	//	return false;

	MMRESULT mmRes;
	UINT uDeviceID;
	HWAVEIN	hwi;
	HWAVEOUT hwo;

	WAVEFORMATEX wf;
	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= 8000;
	wf.wBitsPerSample	= 8;
	wf.nChannels		= 1;
	wf.nBlockAlign		= 1;
	wf.nAvgBytesPerSec	= 8000;
	wf.cbSize			= 0;

	CloseMixer();

	// открываем микшер звуковой карты по умолчанию
	if(bRecMixer)
	{
		if(MMERROR(waveInOpen(&hwi, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL |
			WAVE_FORMAT_DIRECT)))
		{	// открываем микшер по номеру зан€той (а значит WAVE_MAPPER) карты
			for(uDeviceID = 0; uDeviceID < waveInGetNumDevs()-1; uDeviceID++)
			{
				if(MMERROR(waveInOpen(&hwi, uDeviceID, &wf, 0, 0, CALLBACK_NULL |
					WAVE_FORMAT_DIRECT)))
					break;
				waveInClose(hwi);
			}	
			mmRes = mixerOpen(&m_hMixer, uDeviceID, (DWORD)hWnd, 0, CALLBACK_WINDOW |
				MIXER_OBJECTF_WAVEIN);
		}
		else
		{	// открываем микшер по дескриптору WAVE_MAPPER карты
			mmRes = mixerOpen(&m_hMixer, (UINT)hwi, (DWORD)hWnd, 0, CALLBACK_WINDOW |
				MIXER_OBJECTF_HWAVEIN);
			waveInClose(hwi);
		}
	}//end if(bRecMixer)
	else
	{	// то же самое, что и дл€ записи
		if(MMERROR(waveOutOpen(&hwo, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL |
			WAVE_FORMAT_DIRECT)))
		{
			for(uDeviceID = 0; uDeviceID < waveOutGetNumDevs()-1; uDeviceID++)
			{
				if(MMERROR(waveOutOpen(&hwo, uDeviceID, &wf, 0, 0, CALLBACK_NULL |
					WAVE_FORMAT_DIRECT)))
					break;
				waveOutClose(hwo);
			}
			mmRes = mixerOpen(&m_hMixer, uDeviceID, (DWORD)hWnd, 0, CALLBACK_WINDOW |
				MIXER_OBJECTF_WAVEOUT);
		}
		else
		{	mmRes = mixerOpen(&m_hMixer, (UINT)hwo, (DWORD)hWnd, 0, CALLBACK_WINDOW |
				MIXER_OBJECTF_HWAVEOUT);
			waveOutClose(hwo);
		}
	}// end elseif(bRecMixer)
	if(MMERROR(mmRes))
		return false;

	// получаем количество SRC линий
	MIXERLINE mLine;
	ZeroMemory(&mLine, sizeof(MIXERLINE));
	mLine.cbStruct = sizeof(MIXERLINE);
	mLine.dwDestination = (bRecMixer) ? 1 : 0;
	mLine.dwComponentType = (bRecMixer) ? MIXERLINE_COMPONENTTYPE_DST_WAVEIN :
		MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	
	mmRes = mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mLine, MIXER_OBJECTF_HMIXER |
		MIXER_GETLINEINFOF_COMPONENTTYPE);
	if(MMERROR(mmRes))
	{
		CloseMixer();
		return false;
	}
	m_nLineNum = mLine.cConnections;

	return true;
}

void CMixer::CloseMixer()
{
	if(m_hMixer)
	{
		mixerClose(m_hMixer);
		m_hMixer = NULL;
		m_nLineID = -1;
		m_nVolume = -1;
	}
}

int CMixer::GetLinesNum() const
{

	return -1;
}

CString CMixer::GetLineName(int nNum)
{
	return "";
}

void CMixer::SetLine(int nLineNum)
{
}

void CMixer::SetVol(int nPercent)
{
}

int CMixer::GetCurLine() const
{
	return -1;
}

int CMixer::GetCurVol() const
{
	return -1;
}

//====================================================================
// получить идентификатор звуковой карты по-умолчанию (дл€ записи)
int CMixer::GetMixerIn()
{
	HWAVEIN hwi;
	UINT nDeviceID;
	WAVEFORMATEX wf;
	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= 8000;
	wf.wBitsPerSample	= 8;
	wf.nChannels		= 1;
	wf.nBlockAlign		= 1;
	wf.nAvgBytesPerSec	= 8000;
	wf.cbSize			= 0;

	if(MMERROR(waveInOpen(&hwi, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL)))
	{	// отбрасываем устройства, которые открываютс€
		for (nDeviceID = 0; nDeviceID < waveInGetNumDevs()-1; nDeviceID++)
		{
			if(MMERROR(waveInOpen(&hwi, nDeviceID, &wf, 0, 0, CALLBACK_NULL)))
				break;
			waveInClose(hwi);
		}
	}
	else
	{	// получаем идентификатор устройства
		waveInGetID(hwi, &nDeviceID);
		waveInClose(hwi);
	}

	return nDeviceID;
}

//====================================================================
// получить идентификатор звуковой карты по-умолчанию (дл€ воспроизв.)
int CMixer::GetMixerOut()
{
	HWAVEOUT hwo;
	UINT nDeviceID;
	WAVEFORMATEX wf;
	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= 8000;
	wf.wBitsPerSample	= 8;
	wf.nChannels		= 1;
	wf.nBlockAlign		= 1;
	wf.nAvgBytesPerSec	= 8000;
	wf.cbSize			= 0;

	if(MMERROR(waveOutOpen(&hwo, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL)))
	{	// отбрасываем устройства, которые открываютс€
		for (nDeviceID = 0; nDeviceID < waveOutGetNumDevs()-1; nDeviceID++)
		{
			if(MMERROR(waveOutOpen(&hwo, nDeviceID, &wf, 0, 0, CALLBACK_NULL)))
				break;
			waveOutClose(hwo);
		}
	}
	else
	{	// получаем идентификатор устройства
		waveOutGetID(hwo, &nDeviceID);
		waveOutClose(hwo);
	}

	return nDeviceID;
}
*/
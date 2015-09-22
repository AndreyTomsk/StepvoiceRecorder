// MixerPlay.cpp: implementation of the CMixerPlay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MixerPlay.h"
#include "common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMixerPlay::CMixerPlay() : m_nCurLine(0), m_hWnd(NULL)
{
}

CMixerPlay::~CMixerPlay()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CMixerPlay::Open(int nDeviceID, HWND hWnd)
{
	MMRESULT mmRes;
	HWAVEOUT hwo;

	WAVEFORMATEX wf;
	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= 8000;
	wf.wBitsPerSample	= 8;
	wf.nChannels		= 1;
	wf.nBlockAlign		= 1;
	wf.nAvgBytesPerSec	= 8000;
	wf.cbSize			= 0;

	m_hWnd = hWnd;
	Close();

	WAVEOUTCAPS caps;
	waveOutGetDevCaps(nDeviceID, &caps, sizeof(WAVEOUTCAPS));

	// открываем микшер (WAVE_MAPPER - тоже работает)
	if(MMERROR(waveOutOpen(&hwo, nDeviceID, &wf, 0, 0, CALLBACK_NULL |
		WAVE_FORMAT_DIRECT)))
	{	// открываем микшер по номеру занятой (а значит WAVE_MAPPER) карты
		for(nDeviceID = 0; nDeviceID < (int)waveOutGetNumDevs(); nDeviceID++)
		{
			if(MMERROR(waveOutOpen(&hwo, nDeviceID, &wf, 0, 0, CALLBACK_NULL |
				WAVE_FORMAT_DIRECT)))
				break;
			waveOutClose(hwo);
		}	
		mmRes = mixerOpen(&m_hMixer, nDeviceID, (DWORD)hWnd, 0, CALLBACK_WINDOW |
			MIXER_OBJECTF_WAVEOUT);
	}
	else
	{	// открываем микшер по дескриптору WAVE_MAPPER карты
		mmRes = mixerOpen(&m_hMixer, (UINT)hwo, (DWORD)hWnd, 0, CALLBACK_WINDOW |
			MIXER_OBJECTF_HWAVEOUT);
		waveOutClose(hwo);
	}
	if(MMERROR(mmRes))
		return false;

	// добавляем линии громкости в список
	CControlVolume *pWaveVol = new CControlVolume;
	//if(pMainVol->Init(m_hMixer, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
	//	NO_SOURCE, INIT_BYTYPE))
	//{
	//	m_volList.Push(pMainVol);
	//}

	if(pWaveVol->Init(m_hMixer, MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
		MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT, INIT_BYTYPE))
	{
		m_volumeControls.push_back(pWaveVol);
	}
		

	if(GetLinesNum() == 0)
	{
		Close();
		SAFE_DELETE(pWaveVol);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CMixerPlay::Close()
{
	CMixer::Close();
}

//////////////////////////////////////////////////////////////////////
CString CMixerPlay::GetLineName(int Num)
{
	if(Num < 0 || Num >= GetLinesNum())
		return CString("");

	CControlVolume *pCV = m_volumeControls[Num];
	return pCV->GetLineName();
}

//////////////////////////////////////////////////////////////////////
int CMixerPlay::GetLineType(int /*nLineNum*/)
{
	return -1;
}

//////////////////////////////////////////////////////////////////////
void CMixerPlay::SetLine(int nLineNum)
{
	if((nLineNum < 0) || (nLineNum >= GetLinesNum()))
		return;

	m_nCurLine = nLineNum;
	// посылаем сообщение (для отображения подсказки пользователю)
	if(m_hWnd)
		PostMessage(m_hWnd, MM_MIXM_LINE_CHANGE, (WPARAM)m_hMixer, 0);
}

//////////////////////////////////////////////////////////////////////
int CMixerPlay::GetCurLine()
{
	if(GetLinesNum() == 0)
		m_nCurLine = -1;

	return m_nCurLine;
}

//////////////////////////////////////////////////////////////////////
void CMixerPlay::SetVol(int nPercent)
{
	if (GetLinesNum() > 0)
	{
		CControlVolume *pCV = m_volumeControls[GetCurLine()];
		pCV->SetVolume(nPercent);
	}
}

//////////////////////////////////////////////////////////////////////
int CMixerPlay::GetVol(int nLineNum)
{
	if((nLineNum < 0) || (nLineNum >= GetLinesNum()))
		return -1;

	CControlVolume *pCV = m_volumeControls[nLineNum];
	return pCV->GetVolume();
}

//////////////////////////////////////////////////////////////////////

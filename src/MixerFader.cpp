// MixerFader.cpp: implementation of the CControlVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "mp3_recorder.h"
#include "MixerFader.h"
#include "common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MMOK(M) ((M) == MMSYSERR_NOERROR)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CControlVolume::CControlVolume() : m_hMixer(NULL)
{
	CLEAR_STRUCT(&m_mxl);
	m_mxl.cbStruct = sizeof(MIXERLINE);

	CLEAR_STRUCT(&m_Control);
	m_Control.cbStruct = sizeof(MIXERCONTROL);
}

CControlVolume::~CControlVolume()
{
}

//////////////////////////////////////////////////////////////////////
bool CControlVolume::Init(HMIXER hMixer, DWORD dwDstLine, DWORD dwSrcLine, int nInitMethod)
{
	m_hMixer = hMixer;

	if (nInitMethod == INIT_BYTYPE)
	{
		if(GetDstLineIndex(m_mxl, dwDstLine) == -1)
			return false;
		if(dwSrcLine != NO_SOURCE)
		{
			if(GetSrcLineIndex(m_mxl, dwSrcLine) == -1)
				return false;
		}
	}
	else if(nInitMethod == INIT_BYPOSITION)
	{
		m_mxl.dwDestination = dwDstLine;
		m_mxl.dwSource = dwSrcLine;

		if(!MMOK(mixerGetLineInfo((HMIXEROBJ)hMixer, &m_mxl, MIXER_GETLINEINFOF_SOURCE |
			MIXER_OBJECTF_HMIXER)))
			return false;
	}

	// ИЩЕМ VOLUME КОНТРОЛ
	MIXERLINECONTROLS mixerLineControls;
	mixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
	mixerLineControls.dwLineID = m_mxl.dwLineID;
	mixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mixerLineControls.cControls = 1;
	mixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	mixerLineControls.pamxctrl = &m_Control;

	if(!MMOK(mixerGetLineControls((HMIXEROBJ)hMixer, &mixerLineControls,
		MIXER_GETLINECONTROLSF_ONEBYTYPE | MIXER_OBJECTF_HMIXER)))
		return false;

	 // !!!!! m_mxl.dwComponentType содержит тип линии

	return true;
}

//////////////////////////////////////////////////////////////////////
int CControlVolume::GetVolume()
{
	int nPercent = -1;

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_Control.dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;

	if(MMOK(mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mxcd,
		MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER)))
	{
		nPercent = m_Control.Bounds.dwMaximum - m_Control.Bounds.dwMinimum;
		nPercent = int(100 * mxcdVolume.dwValue / (float)nPercent);
	}

	return nPercent;
}

//////////////////////////////////////////////////////////////////////
void CControlVolume::SetVolume(int nPercent)
{
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_Control.dwControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;

	mxcdVolume.dwValue = m_Control.Bounds.dwMaximum - m_Control.Bounds.dwMinimum;
	mxcdVolume.dwValue = int((float)mxcdVolume.dwValue / 100 * (nPercent+1)); // коррекция

	mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mxcd,
		MIXER_SETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);
}

//////////////////////////////////////////////////////////////////////
CString CControlVolume::GetLineName() const
{
	return CString(m_mxl.szName);
}

//////////////////////////////////////////////////////////////////////
DWORD CControlVolume::GetLineID()
{
	return m_mxl.dwLineID;
}

DWORD CControlVolume::GetControlID()
{
	return m_Control.dwControlID;
}

//////////////////////////////////////////////////////////////////////
int CControlVolume::GetLineType()
{
	return (m_mxl.dwComponentType - MIXERLINE_COMPONENTTYPE_SRC_FIRST);
}

//////////////////////////////////////////////////////////////////////
int CControlVolume::GetDstLineIndex(MIXERLINE &mxl, DWORD dwLineType)
{
	MIXERCAPS caps;
	if(!MMOK(mixerGetDevCaps((UINT)m_hMixer, &caps, sizeof(MIXERCAPS))))
		return -1;

	for(int nCurID = 0; nCurID < (int)caps.cDestinations; nCurID++)
	{
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwSource = 0;
		mxl.dwDestination = nCurID;
		if(!MMOK(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mxl,
			MIXER_GETLINEINFOF_DESTINATION)))
			return -1;
		if(mxl.dwComponentType == dwLineType)
			return nCurID;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
int CControlVolume::GetSrcLineIndex(MIXERLINE &mxl, DWORD dwLineType)
{
 	UINT  nLines = mxl.cConnections;
	DWORD dwDstIndex = mxl.dwDestination;

	for(UINT nCurID = 0; nCurID < nLines; nCurID++)
	{
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwSource = nCurID;
		mxl.dwDestination = dwDstIndex;
		if(!MMOK(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mxl,
			MIXER_GETLINEINFOF_SOURCE)))
			return -1;
		if(mxl.dwComponentType == dwLineType)
			return nCurID;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////

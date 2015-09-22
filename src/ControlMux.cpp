// ControlMux.cpp: implementation of the CControlMux class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ControlMux.h"
#include "common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CControlMux::CControlMux() : m_hMixer(NULL), m_pmxcdNames(NULL)
{
	CLEAR_STRUCT(&m_Control);
	m_Control.cbStruct = sizeof(m_Control);
}

CControlMux::~CControlMux()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
void CControlMux::Close()
{
	SAFE_DELETE(m_pmxcdNames);
}

//////////////////////////////////////////////////////////////////////
bool CControlMux::Init(HMIXER hMixer, DWORD dwDstLine, DWORD dwSrcLine, int nInitMethod)
{
	if(nInitMethod != INIT_BYPOSITION)
		return false;

	// получаем необходимую линию
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(mxl);
	mxl.dwDestination = dwDstLine;
	mxl.dwSource = dwSrcLine;
	
	if(!MMOK(mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_DESTINATION |
		MIXER_OBJECTF_HMIXER)))
	{
		Close();
		return false;
	}

	// ищем MUX или MIXER контрол
	MIXERLINECONTROLS mixerLineControls;
	mixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
	mixerLineControls.dwLineID = mxl.dwLineID;
	mixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
	mixerLineControls.cControls = 1;
	mixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);
	mixerLineControls.pamxctrl = &m_Control;

	if(!MMOK(mixerGetLineControls((HMIXEROBJ)hMixer, &mixerLineControls,
		MIXER_GETLINECONTROLSF_ONEBYTYPE | MIXER_OBJECTF_HMIXER)))
	{
		mixerLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
		if(!MMOK(mixerGetLineControls((HMIXEROBJ)hMixer, &mixerLineControls,
			MIXER_GETLINECONTROLSF_ONEBYTYPE | MIXER_OBJECTF_HMIXER)))
		{
			Close();
			return false;
		}

	}

	// заполняем массив имен линий
	m_pmxcdNames = new MIXERCONTROLDETAILS_LISTTEXT[m_Control.cMultipleItems];

	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = m_Control.cMultipleItems;
	mxcd.dwControlID = m_Control.dwControlID;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
	mxcd.paDetails = m_pmxcdNames;

	if(!MMOK(mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd,
		MIXER_GETCONTROLDETAILSF_LISTTEXT | MIXER_OBJECTF_HMIXER)))
	{
		Close();
		return false;
	}

	m_hMixer = hMixer;
	return true;
}

//////////////////////////////////////////////////////////////////////
void CControlMux::SetCurListItem(CString strLineName)
{
	// получаем индекс линии по имени
	int i = 0;
	while(i < m_Control.cMultipleItems && strLineName != m_pmxcdNames[i].szName)
		i++;
	if(i == m_Control.cMultipleItems)
		return;

	// заполняем массив состояния линий (вкл./выкл.)
	MIXERCONTROLDETAILS_BOOLEAN *pmxcdValue = 
		new MIXERCONTROLDETAILS_BOOLEAN[m_Control.cMultipleItems];
	for(int j = 0; j < m_Control.cMultipleItems; j++)
		pmxcdValue[j].fValue = (j == i) ? 1 : 0;

	// изменяем состояние контрола
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = m_Control.cMultipleItems;
	mxcd.dwControlID = m_Control.dwControlID;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxcd.paDetails = pmxcdValue;

	mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mxcd,
		MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER);
	
	SAFE_DELETE(pmxcdValue);
}

//////////////////////////////////////////////////////////////////////
CString CControlMux::GetCurListItem()
{
	MIXERCONTROLDETAILS_BOOLEAN *pmxcdValue = 
		new MIXERCONTROLDETAILS_BOOLEAN[m_Control.cMultipleItems];

	// получаем массив состояния линий (вкл./выкл.)
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = m_Control.cMultipleItems;
	mxcd.dwControlID = m_Control.dwControlID;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxcd.paDetails = pmxcdValue;

	if(!MMOK(mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mxcd,
		MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_HMIXER)))
	{
		SAFE_DELETE(pmxcdValue);
		return CString("");
	}

	// возвращаем имя линии по индексу
	int i = 0;
	while(i < m_Control.cMultipleItems && pmxcdValue[i].fValue == 0)
		i++;

	SAFE_DELETE(pmxcdValue);

	return (i < m_Control.cMultipleItems) ? CString(m_pmxcdNames[i].szName) : CString("");
}

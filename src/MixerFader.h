// MixerFader.h: interface for the CControlVolume class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MIXERFADER_H__D0C59C4C_B177_4DC7_960C_E072A7833A36__INCLUDED_)
#define AFX_MIXERFADER_H__D0C59C4C_B177_4DC7_960C_E072A7833A36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////
enum {INIT_BYTYPE, INIT_BYPOSITION};
const DWORD NO_SOURCE = MIXERLINE_COMPONENTTYPE_SRC_LAST+1;

enum {
	SRCLINETYPE_UNDEFINED	= 0,
	SRCLINETYPE_DIGITAL		= 1,
	SRCLINETYPE_LINE		= 2,
	SRCLINETYPE_MICROPHONE	= 3,
	SRCLINETYPE_SYNTHESIZER	= 4,
	SRCLINETYPE_COMPACTDISC	= 5,
	SRCLINETYPE_TELEPHONE	= 6,
	SRCLINETYPE_PCSPEAKER	= 7,
	SRCLINETYPE_WAVEOUT		= 8,
	SRCLINETYPE_AUXILIARY	= 9,
	SRCLINETYPE_ANALOG		= 10
};

//////////////////////////////////////////////////////////////////////
class CControlVolume
{
	HMIXER m_hMixer;
	MIXERLINE m_mxl;
	MIXERCONTROL m_Control;
	
public:
	int GetLineType();
	DWORD GetLineID();
	CControlVolume();
	virtual ~CControlVolume();

	bool Init(HMIXER hMixer, DWORD dwDstLine, DWORD dwSrcLine, int nInitMethod = INIT_BYTYPE);
	void SetVolume(int nPercent);
	int GetVolume();
	CString GetLineName() const;
protected:
	int GetSrcLineIndex(MIXERLINE &mxl, DWORD dwLineType);
	int GetDstLineIndex(MIXERLINE &mxl, DWORD dwLineType);
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_MIXERFADER_H__D0C59C4C_B177_4DC7_960C_E072A7833A36__INCLUDED_)

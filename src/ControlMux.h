// ControlMux.h: interface for the CControlMux class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLMUX_H__8D66C453_760B_4FB5_BC0C_AF3EE97079AF__INCLUDED_)
#define AFX_CONTROLMUX_H__8D66C453_760B_4FB5_BC0C_AF3EE97079AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include "mixer.h"

class CControlMux  
{
	HMIXER m_hMixer;
	MIXERCONTROL m_Control;
	MIXERCONTROLDETAILS_LISTTEXT *m_pmxcdNames;

public:
	void Close();
	CControlMux();
	virtual ~CControlMux();

	enum {INIT_BYTYPE, INIT_BYPOSITION};

	bool Init(HMIXER hMixer, DWORD dwDstLine, DWORD dwSrcLine, int nInitMethod = INIT_BYTYPE);
	void SetCurListItem(CString strLineName);
	CString GetCurListItem();
};

#endif // !defined(AFX_CONTROLMUX_H__8D66C453_760B_4FB5_BC0C_AF3EE97079AF__INCLUDED_)

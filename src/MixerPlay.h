// MixerPlay.h: interface for the CMixerPlay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXERPLAY_H__ECA7ECEE_AAC3_4C49_96EB_F36116820BE2__INCLUDED_)
#define AFX_MIXERPLAY_H__ECA7ECEE_AAC3_4C49_96EB_F36116820BE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mixer.h"

class CMixerPlay : public CMixer  
{
protected:
	int m_nCurLine;
	HWND m_hWnd;
public:
	int GetLineType(int nLineNum);
	CMixerPlay();
	virtual ~CMixerPlay();

	virtual BOOL Open(int nDeviceID, HWND hWnd);
	virtual void Close();

	virtual CString GetLineName(int Num);	// получить название линии по номеру

	virtual void SetLine(int nLineNum);		// установить линию по номеру
	virtual int GetCurLine();				// получить номер текущей линии

	virtual void SetVol(int nPercent);		// установить громкость линии
	virtual int GetVol(int nLineNum);		// получить громкость текущ. линии в процентах
};

#endif // !defined(AFX_MIXERPLAY_H__ECA7ECEE_AAC3_4C49_96EB_F36116820BE2__INCLUDED_)

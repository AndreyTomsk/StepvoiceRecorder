//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MIXER_H__E563DA37_D5B3_443E_A2C4_A874DF1DF29E__INCLUDED_)
#define AFX_MIXER_H__E563DA37_D5B3_443E_A2C4_A874DF1DF29E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include <vector>
#include "MixerFader.h"
#include "AfStack.h"

//////////////////////////////////////////////////////////////////////
#define MMERROR(M) ((M) != MMSYSERR_NOERROR)
#define MMOK(M) ((M) == MMSYSERR_NOERROR)

//////////////////////////////////////////////////////////////////////
class CMixer  
{
protected:
	HMIXER	m_hMixer;			// дескриптор микшера
	//CControlVolume* m_volArray;	// массив линий (контролов громкости)
	//std::vector<CControlVolume> m_volVector;
	CAfList<CControlVolume> m_volList;

public:
	CMixer();
	~CMixer();

	virtual BOOL Open(int nDeviceID, HWND hWnd) = 0;
	virtual void Close();						// закрыть микшер

	int GetLinesNum()  const;					// получить количество SRC линий
	virtual int GetCurLine() = 0;				// получить номер текущей линии

	virtual void SetLine(int nLineNum) = 0;		// установить линию по номеру

	virtual CString GetLineName(int nLineNum) = 0; // получить название линии по номеру
	virtual int GetLineType(int nLineNum) = 0;	// получить тип

	virtual void SetVol(int nPercent) = 0;		// установить громкость линии
	virtual int  GetVol(int nLineNum) = 0;		// получить громкость текущ. линии в процентах
};

//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_MIXER_H__E563DA37_D5B3_443E_A2C4_A874DF1DF29E__INCLUDED_)

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

	virtual CString GetLineName(int Num);	// �������� �������� ����� �� ������

	virtual void SetLine(int nLineNum);		// ���������� ����� �� ������
	virtual int GetCurLine();				// �������� ����� ������� �����

	virtual void SetVol(int nPercent);		// ���������� ��������� �����
	virtual int GetVol(int nLineNum);		// �������� ��������� �����. ����� � ���������
};

#endif // !defined(AFX_MIXERPLAY_H__ECA7ECEE_AAC3_4C49_96EB_F36116820BE2__INCLUDED_)

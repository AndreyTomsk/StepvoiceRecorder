////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_MIXER_H__E563DA37_D5B3_443E_A2C4_A874DF1DF29E__INCLUDED_)
#define AFX_MIXER_H__E563DA37_D5B3_443E_A2C4_A874DF1DF29E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include <vector>

#include "MixerFader.h"

////////////////////////////////////////////////////////////////////////////////
#define MMERROR(M) ((M) != MMSYSERR_NOERROR)
#define MMOK(M) ((M) == MMSYSERR_NOERROR)

////////////////////////////////////////////////////////////////////////////////
class CMixer  
{
public:
	CMixer();
	~CMixer();

	const HMIXER GetMixerHandle() const { return m_hMixer; }

	virtual BOOL Open(int nDeviceID, HWND hWnd) = 0;
	virtual void Close();						// ������� ������

	int GetLinesNum()  const;					// �������� ���������� SRC �����
	virtual int GetCurLine() = 0;				// �������� ����� ������� �����

	virtual void SetLine(int nLineNum) = 0;		// ���������� ����� �� ������

	virtual CString GetLineName(int nLineNum) = 0; // �������� �������� ����� �� ������
	virtual int GetLineType(int nLineNum) = 0;	// �������� ���

	virtual void SetVol(int nPercent) = 0;		// ���������� ��������� �����
	virtual int  GetVol(int nLineNum) = 0;		// �������� ��������� �����. ����� � ���������

protected:
	HMIXER m_hMixer;							// ���������� �������
	std::vector<CControlVolume*> m_volumeControls;
};

////////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_MIXER_H__E563DA37_D5B3_443E_A2C4_A874DF1DF29E__INCLUDED_)

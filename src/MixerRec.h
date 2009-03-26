// MixerRec.h: interface for the CMixerRec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXERREC_H__3BD6C5EA_1479_4F4B_91FC_6DEE0F184AF2__INCLUDED_)
#define AFX_MIXERREC_H__3BD6C5EA_1479_4F4B_91FC_6DEE0F184AF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Mixer.h"
#include "ControlMux.h"

class CMixerRec : public CMixer  
{
protected:
	CControlMux	m_RecMux;

public:
	int GetLineType(int nLineNum);
	CMixerRec();
	virtual ~CMixerRec();

	virtual BOOL Open(int nDeviceID, HWND hWnd);
	virtual void Close();

	virtual CString GetLineName(int nNum);	// �������� �������� ����� �� ������

	virtual void SetLine(int nLineNum);		// ���������� ����� �� ������
	virtual void SetVol(int nPercent);		// ���������� ��������� �����

	virtual int GetCurLine();				// �������� ����� ������� �����
	virtual int GetVol(int nLineNum);		// �������� ��������� �����. ����� � ���������

	CControlVolume* GetCurrentVolControl();
};

#endif // !defined(AFX_MIXERREC_H__3BD6C5EA_1479_4F4B_91FC_6DEE0F184AF2__INCLUDED_)

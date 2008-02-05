// MixerRec.cpp: implementation of the CMixerRec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mp3_recorder.h"
#include "MixerRec.h"
#include "common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMixerRec::CMixerRec()
{
}

//////////////////////////////////////////////////////////////////////
CMixerRec::~CMixerRec()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CMixerRec::Open(int nDeviceID, HWND hWnd)
{
	MMRESULT mmRes;
	HWAVEIN	hwi;

	WAVEFORMATEX wf;
	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= 8000;
	wf.wBitsPerSample	= 8;
	wf.nChannels		= 1;
	wf.nBlockAlign		= 1;
	wf.nAvgBytesPerSec	= 8000;
	wf.cbSize			= 0;

	Close();

	// �������� ������� ������ ������� �������� �����

	if(MMERROR(waveInOpen(&hwi, nDeviceID, &wf, 0, 0, CALLBACK_NULL |
		WAVE_FORMAT_DIRECT)))
	{	// ��������� ������ �� ������ ������� (� ������ WAVE_MAPPER) �����
		for(nDeviceID = 0; nDeviceID < (int)waveInGetNumDevs(); nDeviceID++)
		{
			if(MMERROR(waveInOpen(&hwi, nDeviceID, &wf, 0, 0, CALLBACK_NULL |
				WAVE_FORMAT_DIRECT)))
				break;
			waveInClose(hwi);
		}	
		mmRes = mixerOpen(&m_hMixer, nDeviceID, (DWORD)hWnd, 0, CALLBACK_WINDOW |
			MIXER_OBJECTF_WAVEIN);
	}
	else
	{	// ��������� ������ �� ����������� WAVE_MAPPER �����
		mmRes = mixerOpen(&m_hMixer, (UINT)hwi, (DWORD)hWnd, 0, CALLBACK_WINDOW |
			MIXER_OBJECTF_HWAVEIN);
		waveInClose(hwi);
	}

	if(MMERROR(mmRes))
		return false;

	// �������� ���������� SRC �����

	MIXERLINE mLine;
	ZeroMemory(&mLine, sizeof(MIXERLINE));
	mLine.cbStruct = sizeof(MIXERLINE);
	mLine.dwDestination = 1;
	mLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	
	if(MMERROR(mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mLine, MIXER_OBJECTF_HMIXER |
		MIXER_GETLINEINFOF_COMPONENTTYPE)))
	{
		Close();
		return false;
	}

	if (mLine.cConnections > 1)
	{	// �������������� ������������� ����� ...
		if (!m_RecMux.Init(m_hMixer, mLine.dwDestination, 0, INIT_BYPOSITION))
		{
			Close();
			return false;
		}
	}
	for (int i=0; i < (int)mLine.cConnections; i++)
	{
		CControlVolume *pCV = new CControlVolume;
		if (pCV->Init(m_hMixer, mLine.dwDestination, i, INIT_BYPOSITION))
		{
			m_volList.Push(pCV);
		}
		else
		{
			SAFE_DELETE(pCV);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
void CMixerRec::Close()
{
	CMixer::Close();
	m_RecMux.Close();
}

//////////////////////////////////////////////////////////////////////
CString CMixerRec::GetLineName(int Num)
{
	if((Num < 0) || (Num >= GetLinesNum()))
		return CString("");
	//CControlVolume *pCV = &m_volVector[Num];
	CControlVolume *pCV = m_volList[Num];
	return pCV->GetLineName();
}

//////////////////////////////////////////////////////////////////////
int CMixerRec::GetLineType(int nLineNum)
{
	if((nLineNum < 0) || (nLineNum >= GetLinesNum()))
		return 3; // ����� ���������� ��������

	CControlVolume *pCV = m_volList[nLineNum];
	return pCV->GetLineType();
}

//////////////////////////////////////////////////////////////////////
void CMixerRec::SetLine(int nLineNum)
{
	if((nLineNum < 0) || (nLineNum >= GetLinesNum()))
		return;
	//CControlVolume *pCV = &m_volVector[nLineNum];
	CControlVolume *pCV = m_volList[nLineNum];
	
	if (GetLinesNum() > 1)
		m_RecMux.SetCurListItem(pCV->GetLineName());
}

//////////////////////////////////////////////////////////////////////
int CMixerRec::GetCurLine()
{
	if(GetLinesNum() == 0)
		return -1;
	if(GetLinesNum() == 1)
		return 0;

	CString strCurLineName = m_RecMux.GetCurListItem();

	// �������� ������ ����� �� �� �����
	int i = 0;
	CControlVolume *pCV;
	for(; i < GetLinesNum(); i++)
	{
		//pCV = &m_volVector[i];
		pCV = m_volList[i];
		if(pCV->GetLineName() == strCurLineName)
			break;
	}

	return i;
}

//////////////////////////////////////////////////////////////////////
void CMixerRec::SetVol(int nPercent)
{
	//CControlVolume *pCV = &m_volVector[GetCurLine()];
	CControlVolume *pCV = m_volList[GetCurLine()];
	pCV->SetVolume(nPercent);
}

//////////////////////////////////////////////////////////////////////
int CMixerRec::GetVol(int nLineNum)
{
	if((nLineNum < 0) || (nLineNum >= GetLinesNum()))
		return -1;
	//CControlVolume *pCV = &m_volVector[nLineNum];
	CControlVolume *pCV = m_volList[nLineNum];
	return pCV->GetVolume();
}

//////////////////////////////////////////////////////////////////////

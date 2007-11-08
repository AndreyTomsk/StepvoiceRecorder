/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SNDFILE_MP3_H__3C28F4E5_E074_490A_84B6_EC4FF697D355__INCLUDED_)
#define AFX_SNDFILE_MP3_H__3C28F4E5_E074_490A_84B6_EC4FF697D355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sndfile.h"
#include "struct.h"	// �������� ��������� WAVEINFO,
					// ��. ��������� �������,
					// ��. ����������������.

/////////////////////////////////////////////////////////////////////////////
// ����� CSndFile_MP3 ������������ ��� ������ � ������� ������� MP3. ��������
// (�����) �������� ������������ ������ CSndFile. 
/////////////////////////////////////////////////////////////////////////////
class CSndFile_MP3 : public CSndFile
{
protected:
	int m_nAllFrames;
	int m_nCurrFrame;
	int m_nFrameSize;

protected:
	bool GetFileInfo();
	bool DecodeHeader(char* pHeader, MPEG_HEADER *mpeghdr);
	bool FindNextHeader(MPEG_HEADER *mpeghdr);
	int  CountFrameSize(MPEG_HEADER *mpeghdr);
	void SeekFrame(int nFrame, int nFromType);
public:
	CSndFile_MP3();
	virtual ~CSndFile_MP3();

	// �������� ��������/��������
	virtual bool Open(CString& str, SOUND_INFO* psi);
	virtual void Close();

	// �������� �����/������
	virtual DWORD Read( char *pBuf, DWORD dwBytesToRead);
	virtual DWORD Write(char *pBuf, DWORD dwBytesToWrite);

	// ��������� �������������� ����������
	virtual DWORD GetFilePos( int nWhatID);
	virtual DWORD GetFileSize(int nWhatID);
	virtual	bool  ChangeSoundInfo(SOUND_INFO& si);

	// ���������������� � �����
	virtual DWORD Seek(int nSeconds, int nFromType);
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_SNDFILE_MP3_H__3C28F4E5_E074_490A_84B6_EC4FF697D355__INCLUDED_)

/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SNDFILE_H__7A3D53AF_692D_4E42_BD6A_06418D454968__INCLUDED_)
#define AFX_SNDFILE_H__7A3D53AF_692D_4E42_BD6A_06418D454968__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "struct.h"	// описание структуры SOUNDINFO,
					// идентиф. позиционирования.

/////////////////////////////////////////////////////////////////////////////
// Класс инкапсулирует общие методы работы сo звуковыми файлами.
class CSndFile  
{
private:
	HANDLE     m_hSndFile;
protected:
	SOUND_INFO m_sndinfo;
	CString    m_strFileName;

protected:
	DWORD SetFilePointer(int nBytes, int nFromType);
public:
	CSndFile();
	virtual ~CSndFile();

	// операции открытия/закрытия
	virtual bool Open(CString& str, SOUND_INFO* psi);
	virtual void Close();
			bool IsOpened();

	// операции чтения/записи
	virtual DWORD Read( char *pBuf, DWORD dwBytesToRead);
	virtual DWORD Write(char *pBuf, DWORD dwBytesToWrite);
			void  Cut();

	// получение дополнительной информации
	virtual DWORD    GetFilePos( int nWhatID);
	virtual DWORD    GetFileSize(int nWhatID);
			CString& GetFileName();
	virtual	void     GetSoundInfo(SOUND_INFO& si);
	virtual bool	 ChangeSoundInfo(SOUND_INFO& si);

	// позиционирование в файле
	virtual DWORD Seek(int nSeconds, int nFromType) = 0;
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_SNDFILE_H__7A3D53AF_692D_4E42_BD6A_06418D454968__INCLUDED_)

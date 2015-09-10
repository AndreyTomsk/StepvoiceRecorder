#ifndef _MAINFRM_HELPERS_H
#define _MAINFRM_HELPERS_H
#pragma once


////////////////////////////////////////////////////////////////////////////////

namespace Helpers
{
	CString ToString_HMMSS(unsigned int seconds);
	CString ToString_HMMSS(double seconds);

	//Check if a file is suitable for recording (not exist or length = 0).
	bool IsSuitableForRecording(const CString& filePath, DWORD* outErrorCode = NULL);

	//strTemplate - string template, passed to CTime.Format method.
	CString GetAutoname(CString strTemplate, CString fileExtension);
	CString GetMp3AutonameFromConfig();
	//CString GetAutonameRegEx(CString strTemplate, CString fileExtension);

/*
CString GetAutoName(CString strTemplate, CString fileExtension);

//��� �� ����� ��������:
//�� ������ ������� �����, �������� ������ �������������� ����
//� ������ ����������, ����� ����� ���� ���������� ������.

//��������:
//�� ������� ����� ��������� ����� ��� ������;
//���� ����� �� �����;
//
//������ ������������ ��������� ������;
//������� ��������� ��������� ����;
//����������� ��� �����;
//�����: ���������� ��� ���������� ����� - �� ��������� �������;


CString GetNonExistingFileName(CString folder, CString fileTemplate);
*/
};

////////////////////////////////////////////////////////////////////////////////

#endif // _MAINFRM_HELPERS_H

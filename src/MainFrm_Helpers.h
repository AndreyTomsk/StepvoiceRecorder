#pragma once

////////////////////////////////////////////////////////////////////////////////

namespace Helpers
{
	CString ToString_HMMSS(unsigned int seconds);
	CString ToString_HMMSS(double seconds);

	//Check if a file is suitable for recording (not exist or length = 0).
	bool IsSuitableForRecording(const CString& filePath, DWORD* outErrorCode = NULL);

	//Provides a new file path, suitable for recording - which doesn't exist
	//in output folder. If current autoname exist - generate next one for checking.
	//Adds "_02", "_03", etc. suffixes to name.
	CString GetNewRecordingFilePath();

	//strTemplate - string template, passed to CTime.Format method.
	CString FilterTemplate(CString strTemplate);
	CString GetAutoname(CString strTemplate, CString fileExtension);
	CString GetMp3AutonameFromConfig();
	
	//Returns output Folder from options (if set) or from a last used file.
	//If both folders empty - returns path to desktop folder.
	CString GetOutputFolder();
	
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
}; //namespace Helpers

#ifndef _AUTONAME_H
#define _AUTONAME_H
#pragma once

//strTemplate - string template, passed to CTime.Format method.
CString GetAutoName(CString strTemplate, CString fileExtension);

CString GetAutonameRegEx(CString strTemplate, CString fileExtension);

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



#endif // _AUTONAME_H

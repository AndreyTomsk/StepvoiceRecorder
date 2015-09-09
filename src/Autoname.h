#ifndef _AUTONAME_H
#define _AUTONAME_H
#pragma once

//strTemplate - string template, passed to CTime.Format method.
CString GetAutoName(CString strTemplate, CString fileExtension);

CString GetAutonameRegEx(CString strTemplate, CString fileExtension);

/*
CString GetAutoName(CString strTemplate, CString fileExtension);

//что мы хотим получить:
//на основе шаблона файла, получить первый несуществующий файл
//в данной директории, чтобы можно было продолжить запись.

//Алгоритм:
//из шаблона файла формируем маску для поиска;
//ищем файлы по маске;
//
//шаблон предполагает нумерацию файлов;
//находим последний найденный файл;
//вытаскиваем его номер;
//Финиш: возвращаем имя свободного файла - со следующим номером;


CString GetNonExistingFileName(CString folder, CString fileTemplate);
*/



#endif // _AUTONAME_H

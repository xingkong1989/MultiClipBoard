
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// MultiClipBoard.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"



void OutPutString(wchar_t* wszFormat, ...)
{
	va_list vList;
	wchar_t wsOutString[MAX_PATH] = { 0 };
	ZeroMemory(wsOutString, MAX_PATH * sizeof(WCHAR));

	va_start(vList, wszFormat);
	_vsntprintf_s(wsOutString, MAX_PATH, wszFormat, vList);
	va_end(vList);
	wcscat_s(wsOutString, MAX_PATH, L"\r\n");

	OutputDebugString(wsOutString);
}


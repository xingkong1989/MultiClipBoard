
// stdafx.cpp : 只包括标准包含文件的源文件
// MultiClipBoard.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

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


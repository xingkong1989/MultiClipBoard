
// MultiClipBoard.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

// CMultiClipBoardApp: 
// �йش����ʵ�֣������ MultiClipBoard.cpp
//

class CMultiClipBoardApp : public CWinApp
{
public:
	CMultiClipBoardApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMultiClipBoardApp theApp;
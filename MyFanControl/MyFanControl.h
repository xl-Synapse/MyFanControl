
// MyFanControl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyFanControlApp: 
// �йش����ʵ�֣������ MyFanControl.cpp
//

class CMyFanControlApp : public CWinApp
{
public:
	CMyFanControlApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

};

extern CMyFanControlApp theApp;
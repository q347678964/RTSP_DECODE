
// BaseVersion.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBaseVersionApp:
// �йش����ʵ�֣������ BaseVersion.cpp
//

class CBaseVersionApp : public CWinApp
{
public:
	CBaseVersionApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBaseVersionApp theApp;
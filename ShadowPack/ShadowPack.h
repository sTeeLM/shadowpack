
// ShadowPack.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CShadowPackApp:
// �йش����ʵ�֣������ ShadowPack.cpp
//

class CShadowPackApp : public CWinAppEx
{
public:
	CShadowPackApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CShadowPackApp theApp;
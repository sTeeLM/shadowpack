﻿// ProgressControl.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "ProgressControl.h"


// CProgressControl

IMPLEMENT_DYNAMIC(CProgressControl, CProgressCtrl)

CProgressControl::CProgressControl()
{

}

CProgressControl::~CProgressControl()
{
}

void CProgressControl::Reset()
{
}

void CProgressControl::Initialize(CWnd* pParent, UINT nID)
{
	SubclassDlgItem(nID, pParent);
}

void CProgressControl::SetFullScale(UINT nVal)
{
}

void CProgressControl::Increase(UINT nVal)
{
}

void CProgressControl::Cancel(void)
{
}

BOOL CProgressControl::IsCanceled(void)
{
	return 0;
}

BEGIN_MESSAGE_MAP(CProgressControl, CProgressCtrl)
END_MESSAGE_MAP()



// CProgressControl 消息处理程序


#pragma once
#include "MediaBase.h"
class CBMPFileMedia :
    public CMediaBase
{
public:
    CBMPFileMedia();
    virtual ~CBMPFileMedia();
public:



public:
	// ʵ��CMediaFactory�Ľӿ�
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMediaBase* Factory();
protected:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;
};


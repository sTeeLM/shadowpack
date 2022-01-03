#pragma once

#include "framework.h"
#include "MediaBase.h"

class CMediaFactory
{
public:
	CMediaFactory(void);
	virtual ~CMediaFactory(void);

public:

	typedef CMediaBase* (*FN_MEDIA_FACTORY)();
	typedef LPCTSTR(*FN_MEDIA_GET_NAME)();
	typedef LPCTSTR* (*FN_MEDIA_GET_EXTTABLE)();
	typedef struct _MEDIA_INFO_T
	{
		FN_MEDIA_GET_NAME fnGetName;
		FN_MEDIA_GET_EXTTABLE fnGetExtTable;
		FN_MEDIA_FACTORY fnFatory;
	}MEDIA_INFO_T;

	typedef struct _MEDIA_DESC_T {
		UINT Catagory;
		MEDIA_INFO_T* Infos;
	}MEDIA_DESC_T;

public:
	static CMediaBase* CreateMediaFromExt(LPCTSTR szExt);
	static CString CreateExtTable();
private:
	static MEDIA_INFO_T m_MediaInfoImageFile[];
	static MEDIA_DESC_T m_MediaDescTable[];
};

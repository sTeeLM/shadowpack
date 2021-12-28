#pragma once

#include "PackErrors.h"

class CFileCache
{
public:
	CFileCache();
	virtual ~CFileCache();

public:
	LPVOID Alloc(ULONGLONG nSize, CPackErrors& Errors);
	void Free();
protected:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPVOID m_pMap;
	CString m_strCacheFile;
};


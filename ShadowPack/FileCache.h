#pragma once

#include "PackErrors.h"

class CFileCache
{
public:
	CFileCache();
	virtual ~CFileCache();

public:
	LPVOID Alloc(ULONGLONG nSize, CPackErrors& Errors);
	void Free(LPVOID pAddress);
protected:
	class CFileCacheEntry 
	{
	public:
		CFileCacheEntry(
			HANDLE hFile = INVALID_HANDLE_VALUE, 
			HANDLE hFileMap = NULL, 
			LPVOID pMap = NULL, 
			LPCTSTR szCacheFile = NULL) :
			m_hFile(hFile),
			m_hFileMap(hFileMap),
			m_pMap(pMap),
			m_strCacheFile(szCacheFile)
		{}
		virtual ~CFileCacheEntry() {}
	public:
		HANDLE m_hFile;
		HANDLE m_hFileMap;
		LPVOID m_pMap;
		CString m_strCacheFile;
	};
	CMap<ULONGLONG, ULONGLONG, CFileCacheEntry, CFileCacheEntry> m_pCacheEntry;
};


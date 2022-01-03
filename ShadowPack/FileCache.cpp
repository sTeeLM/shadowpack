#include "pch.h"
#include "FileCache.h"
#include "ConfigManager.h"
#include "ShadowPack.h"
#include "PackUtils.h"

/*
实在懒了，直接用mmap实现内存cache，可能有4G问题？不过什么图片会大于4G呢。。。
哈勃望远镜拍的么？
*/

CFileCache::CFileCache()
{

}

CFileCache::~CFileCache()
{
}

LPVOID CFileCache::Alloc(ULONGLONG nSize, CPackErrors& Errors)
{
	CConfigManager::CONFIG_VALUE_T val;
	BOOL bUseSystemCache = FALSE;
	LARGE_INTEGER li;
	CString strCacheFile;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hFileMap = NULL;
	LPVOID pMap = NULL;

	if (!theApp.m_Config.GetConfig(_T("media"), _T("media_use_system_cache"), val)) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto error;
	}
	bUseSystemCache = val.n8;

	if (!theApp.m_Config.GetConfig(_T("media"), _T("media_custom_cache_path"), val)) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto error;
	}
	strCacheFile = val.str;
	free(val.str);

	if (bUseSystemCache) {
		strCacheFile = CPackUtils::GetTempPath();
	}

	strCacheFile = CPackUtils::GetTempFileName(strCacheFile, _T("FileCache_"));

	TRACE(_T("use file cache %s\n"), strCacheFile);

	// create file
	if ((hFile = ::CreateFile((LPCTSTR)strCacheFile,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
		NULL)) == INVALID_HANDLE_VALUE) {
		Errors.SetError(CPackErrors::PE_IO, strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	// set size
	li.QuadPart = nSize;
	if (INVALID_SET_FILE_POINTER == ::SetFilePointer(hFile, li.LowPart, &li.HighPart, FILE_BEGIN)
		&& ::GetLastError() != NO_ERROR) {
		Errors.SetError(CPackErrors::PE_IO, strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	// create map
	li.QuadPart = nSize;
	if (NULL == (hFileMap = ::CreateFileMapping(hFile, NULL, PAGE_READWRITE, li.HighPart, li.LowPart, NULL))) {
		Errors.SetError(CPackErrors::PE_IO, strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	if (NULL == (pMap = ::MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0))) {
		Errors.SetError(CPackErrors::PE_IO, strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	m_pCacheEntry.SetAt((ULONGLONG)pMap, CFileCacheEntry(hFile, hFileMap, pMap, strCacheFile));

	return pMap;

error:
	if (hFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	if (hFileMap != NULL) {
		::CloseHandle(hFileMap);
		hFileMap = NULL;
	}
	return NULL;
}

void CFileCache::Free(LPVOID pAddress)
{
	CFileCacheEntry Entry;
	if (m_pCacheEntry.Lookup((ULONGLONG)pAddress, Entry)){
		if (Entry.m_pMap) {
			::UnmapViewOfFile(Entry.m_pMap);
		}

		if (Entry.m_hFileMap != NULL) {
			::CloseHandle(Entry.m_hFileMap);
		}

		if (Entry.m_hFile != INVALID_HANDLE_VALUE) {
			::CloseHandle(Entry.m_hFile);
		}

		if (Entry.m_strCacheFile.GetLength()) {
			Entry.m_strCacheFile = _T("");
		}
		m_pCacheEntry.RemoveKey((ULONGLONG)Entry.m_pMap);
	}
}

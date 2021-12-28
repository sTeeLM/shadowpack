#include "pch.h"
#include "FileCache.h"
#include "ConfigManager.h"
#include "ShadowPack.h"
#include "PackUtils.h"

/*
实在懒了，直接用mmap实现内存cache，可能有4G问题？不过什么图片会大于4G呢。。。
哈勃望远镜拍的么？
*/

CFileCache::CFileCache() :
	m_hFile(INVALID_HANDLE_VALUE),
	m_hFileMap(NULL),
	m_pMap(NULL),
	m_strCacheFile(_T(""))
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

	if (!theApp.m_Config.GetConfig(_T("media"), _T("media_use_system_cache"), val)) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto error;
	}
	bUseSystemCache = val.n8;

	if (!theApp.m_Config.GetConfig(_T("media"), _T("media_custom_cache_path"), val)) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto error;
	}
	m_strCacheFile = val.str;
	free(val.str);

	if (bUseSystemCache) {
		m_strCacheFile = CPackUtils::GetTempPath() + _T("FileCache.bin");
	} else {
		m_strCacheFile += _T("FileCache.bin");
	}

	// create file
	if ((m_hFile = ::CreateFile((LPCTSTR)m_strCacheFile,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
		NULL)) == INVALID_HANDLE_VALUE) {
		Errors.SetError(CPackErrors::PE_IO, m_strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	// set size
	li.QuadPart = nSize;
	if (INVALID_SET_FILE_POINTER == ::SetFilePointer(m_hFile, li.LowPart, &li.HighPart, FILE_BEGIN)
		&& ::GetLastError() != NO_ERROR) {
		Errors.SetError(CPackErrors::PE_IO, m_strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	// create map
	li.QuadPart = nSize;
	if (NULL == (m_hFileMap = ::CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, li.HighPart, li.LowPart, NULL))) {
		Errors.SetError(CPackErrors::PE_IO, m_strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	if (NULL == (m_pMap = ::MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0))) {
		Errors.SetError(CPackErrors::PE_IO, m_strCacheFile, CPackUtils::GetLastError());
		goto error;
	}

	return m_pMap;

error:
	if (m_hFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
	if (m_hFileMap != NULL) {
		::CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
	return NULL;
}

void CFileCache::Free()
{
	if (m_pMap) {
		::UnmapViewOfFile(m_pMap);
		m_pMap = NULL;
	}

	if (m_hFileMap != NULL) {
		::CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}

	if (m_hFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	if (m_strCacheFile.GetLength()) {
		m_strCacheFile = _T("");
	}
}

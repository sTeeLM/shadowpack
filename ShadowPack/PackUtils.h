#pragma once

typedef BOOL(*CB_WALK_DIR)(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam);

class CPackUtils
{
private:
	CPackUtils(void);
	virtual ~CPackUtils(void);
public:

	static void TranslateSize(LONGLONG nSize, CString & strOut);
	static BOOL GetFileSize(LPCTSTR szPath,ULONGLONG & size);
	static BOOL WalkDir(LPCTSTR szPath, CB_WALK_DIR cb, LPVOID pParam, BOOL bSkipError = TRUE, INT nLevel = -1);
	static size_t ST2SA(LPCTSTR szSrc, CStringA & szDst);
	static LPCTSTR StringRFind(LPCTSTR szSrc, LPCTSTR szFrom, TCHAR c);
	static LPCTSTR GetPathName(LPCTSTR szPath);
	static CString GetPathPath(LPCTSTR szPath);
	static BOOL IsPathExist(LPCTSTR szPath, LPWIN32_FIND_DATA pffd = NULL);
	static CString CreateTempPath();
	static BOOL RemoveDir(LPCTSTR szPath);
	static CString GetLastError(DWORD dwError);
	static CString GetLastStdError(int err);
};


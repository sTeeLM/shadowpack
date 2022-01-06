#pragma once

typedef BOOL(*CB_WALK_DIR)(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam);

class CPackUtils
{
private:
	CPackUtils(void);
	virtual ~CPackUtils(void);
public:

	static void TranslateSize(ULONGLONG nSize, CString & strOut);
	static BOOL GetFileSize(LPCTSTR szPath,ULONGLONG & size);
	static BOOL WalkDir(LPCTSTR szPath, CB_WALK_DIR cb, LPVOID pParam, BOOL bSkipError = TRUE, INT nLevel = -1);
	static size_t ST2SA(LPCTSTR szSrc, CStringA & szDst);
	static LPCTSTR StringRFind(LPCTSTR szSrc, LPCTSTR szFrom, TCHAR c);
	static LPCTSTR GetPathName(LPCTSTR szPath);
	static CString GetPathPath(LPCTSTR szPath);
	static BOOL IsPathExist(LPCTSTR szPath, LPWIN32_FIND_DATA pffd = NULL);
	static CString CreateTempPath();
	static HANDLE CreateTempFile(LPCTSTR szTempDir, CString & strTempFileName);
	static CString GetTempPath();
	static BOOL RemoveDir(LPCTSTR szPath);
	static CString GetLastError(DWORD dwError);
	static CString GetLastError();
	static CString GetLastError(CException * e);
	static CString GetLastStdError(int err);
	static CString GetWorkingPath();
	static CString GetTempFileName(LPCTSTR szPathName, LPCTSTR szPrefix);
	static BOOL ShowLocationDirDlg(CWnd* pOwner, CString& strDir);
	static void FillBufferRand(LPBYTE pBuffer, ULONGLONG nSize);
	static BOOL SetWindowTop(CWnd* pWnd);
	static BOOL CancelWindowTop(CWnd* pWnd);
	static CString FormatSecond(DOUBLE dSeconds);
	static const BYTE F5LookupTable[4][8];
	static const BYTE F5RevLookupTable[8];
};


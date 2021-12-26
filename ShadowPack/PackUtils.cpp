#include "pch.h"
#include "PackUtils.h"
#include <stdlib.h>


CPackUtils::CPackUtils(void)
{
}

CPackUtils::~CPackUtils(void)
{
}


void CPackUtils::TranslateSize(LONGLONG nSize, CString & strOut)
{
	double dSize;
	LONGLONG r;
	if(nSize > 1024 * 1024 * 1024) {
		r = nSize / (1024 * 1024 * 1024);
		nSize %= (1024 * 1024 * 1024);
		dSize = (double)r + (double)nSize / (1024 * 1024 * 1024);
		strOut.Format(_T("%.1f G"), dSize);
	} else if(nSize > 1024 * 1024) {
		r = nSize / (1024 * 1024);
		nSize %= (1024 * 1024);
		dSize = (double)r + (double)nSize / (1024 * 1024);
		strOut.Format(_T("%.1f M"), dSize);
	} else if(nSize > 1024) {
		r = nSize / (1024);
		nSize %= (1024);
		dSize = (double)r + (double)nSize / (1024);
		strOut.Format(_T("%.1f K"), dSize);
	} else {
		dSize = (double) nSize;
		strOut.Format(_T("%.1f"), dSize);
	}
}

BOOL CPackUtils::ShowLocationDirDlg(CWnd *pOwner, CString& strDir)
{
	// get dir location
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = pOwner->GetSafeHwnd();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH * 2];
	szFolder[0] = 0;
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
			bRet = TRUE;
		IMalloc* pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
	strDir = szFolder;
	return bRet;
}

CString CPackUtils::GetWorkingPath()
{
	TCHAR Buffer[1024];
	CString strRet = _T("");
	if (_tgetcwd(Buffer, _countof(Buffer))) {
		strRet = Buffer;
	}
	if (strRet.Right(1).Compare(_T("\\"))) {
		strRet += _T("\\");
	}

	return strRet;
}

BOOL CPackUtils::GetFileSize(LPCTSTR szPath, ULONGLONG & size)
{
	CFile file;
	CFileException ex;
	BOOL bRet = FALSE;

	size = 0L;

	bRet = file.Open(szPath, CFile::modeRead | CFile::shareDenyWrite,&ex);

	if(!bRet) {
		TRACE(_T("File could not be opened %d\n"), ex.m_cause);
	} else {
		size = file.GetLength();
		bRet = TRUE;
	}

	return bRet;
}

BOOL CPackUtils::WalkDir(LPCTSTR szPath, CB_WALK_DIR cb, LPVOID pParam, BOOL bSkipError /* = TRUE */, INT nLevel /* = -1*/)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	LARGE_INTEGER filesize;
	CString strPath = szPath ;
	strPath += _T("\\");
	CString strPath1 = strPath + _T("*");

	if( nLevel-- == 0) return TRUE;

	hFind = ::FindFirstFile(strPath1, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		TRACE(_T("Dir %s could not be opened %d\n"), szPath, ::GetLastError());
		return bSkipError ? TRUE : FALSE;
	} 

	do {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(!lstrcmp(ffd.cFileName, _T(".")) || !lstrcmp(ffd.cFileName, _T(".."))) {
				continue;
			}
			TRACE(_T("dir  %s\n"),  strPath + ffd.cFileName);
			if(!cb(szPath, &ffd, pParam)) {
				::FindClose(hFind);
				return FALSE;
			}
			if(!WalkDir(strPath + ffd.cFileName, cb, pParam, bSkipError, nLevel)) {
				::FindClose(hFind);
				return FALSE;
			}
		} else {
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			TRACE(_T("file  %s   %ld bytes\n"), strPath + ffd.cFileName, filesize.QuadPart);
			if(!cb(szPath, &ffd, pParam)) {
				::FindClose(hFind);
				return FALSE;
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	::FindClose(hFind);
	if(!cb(szPath, NULL, pParam)) {
		return FALSE;
	}
	return TRUE;
}

size_t CPackUtils::ST2SA(LPCTSTR szSrc, CStringA & szDst)
{
	CT2CA szOut(szSrc);
	szDst = szOut;
	return strlen(szOut);
}

LPCTSTR CPackUtils::StringRFind(LPCTSTR szSrc, LPCTSTR szFrom, TCHAR c)
{
	LPCTSTR p = szFrom;

	if(NULL == szSrc ||( p!= NULL && p < szSrc)) return NULL;

	if(p == NULL) {
		p = szSrc + lstrlen(szSrc);
	}

	while(p >= szSrc) {
		if(*p == c) return p;
		p--;
	}
	return NULL;
}

LPCTSTR CPackUtils::GetPathName(LPCTSTR szPath)
{
	LPCTSTR p = CPackUtils::StringRFind(szPath, NULL, _T('\\'));
	if(p != NULL) {
		return ++p;
	}
	return szPath;
}

CString CPackUtils::GetPathPath(LPCTSTR szPath)
{
	CString strRet = szPath;
	LPCTSTR p = GetPathName(szPath);
	return strRet.Left(strRet.GetLength() - lstrlen(p) - 1);
}

BOOL CPackUtils::IsPathExist(LPCTSTR szPath, LPWIN32_FIND_DATA pffd /*= NULL*/)
{
	WIN32_FIND_DATA ffd;

	HANDLE hFind = ::FindFirstFile(szPath, &ffd);
	if(hFind == INVALID_HANDLE_VALUE) {
		return FALSE;
	} else {
		if(pffd != NULL)
			::CopyMemory(pffd, &ffd, sizeof(ffd));
		::FindClose(hFind);
		return TRUE;
	}
}

CString CPackUtils::GetTempPath()
{
	CString strPath, strRet;
	INT nSize = (MAX_PATH + 1) * 10;
	LPTSTR pBuffer = strPath.GetBuffer(nSize);
	if (::GetTempPath(nSize, pBuffer) != 0) {
		strPath.ReleaseBuffer();
		srand((UINT)time(NULL));
		strRet.Format(_T("%s"), strPath);
		if (strRet.Right(1).Compare(_T("\\"))) {
			strRet += _T("\\");
		}
		return strRet;
	}
	strPath.ReleaseBuffer();
	strPath = _T("");
	return strPath;
}

CString CPackUtils::CreateTempPath()
{
	CString strPath, strRet;
	INT nSize = (MAX_PATH+1) * 10;
	LPTSTR pBuffer = strPath.GetBuffer(nSize);
	if(::GetTempPath(nSize, pBuffer) != 0) {
		strPath.ReleaseBuffer();
		srand((UINT)time(NULL));
		strRet.Format(_T("%sFUCK.%4d"), strPath, rand());
		return strRet;
	}
	strPath.ReleaseBuffer();
	strPath = _T("");
	return strPath;
}

BOOL CPackUtils::RemoveDir(LPCTSTR szPath)
{
	SHFILEOPSTRUCT fop;
	CString strPath = szPath;
	strPath += _T('\0'); // 诡异
	::ZeroMemory(&fop, sizeof(fop));
	fop.wFunc = FO_DELETE;
	fop.pFrom = strPath;
	fop.pTo = NULL;
	fop.fFlags = FOF_NOCONFIRMATION ;
	INT ret = SHFileOperation(&fop);

	return ret == 0;
}

CString CPackUtils::GetLastError()
{
	return GetLastError(::GetLastError());
}

CString CPackUtils::GetLastError(DWORD dwError)
{
	LPTSTR pBuffer = NULL;
	CString strMessage;
	::FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_IGNORE_INSERTS| 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError, // 此乃错误代码，通常在程序中可由 GetLastError()得之
		LANG_NEUTRAL,
		(LPTSTR)&pBuffer,
		0  ,
		NULL );
	if(pBuffer) {
		strMessage =  pBuffer;
		::LocalFree(pBuffer);
	} else {
		strMessage.Format( _T("UNKNOWN Error Code %d"), dwError);
	}
	return strMessage;
}

CString CPackUtils::GetLastStdError(int err)
{
	TCHAR Buffer[1024] = {0};
	INT n = _tcserror_s(Buffer, _countof(Buffer), err);
	CString strMessage;
	if(!n) {
		strMessage = Buffer;
	} else {
		strMessage.Format( _T("UNKNOWN Std Error Code %d"), err);
	}
	return strMessage;
}

void CPackUtils::FillBufferRand(LPBYTE pBuffer, UINT nSize)
{
	int* p = NULL;
	LPBYTE p1 = NULL;
	srand((unsigned)time(NULL));
	p = (int*)pBuffer;
	for (INT i = 0; i < nSize / sizeof(int); i++) {
		*p = rand();
		p++;
	}
	p1 = (LPBYTE)p;
	for (INT i = 0; i < nSize % sizeof(int); i++) {
		p1[i] = rand();
	}
}

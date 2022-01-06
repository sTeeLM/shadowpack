#include "pch.h"
#include "PackUtils.h"
#include <stdlib.h>


CPackUtils::CPackUtils(void)
{
}

CPackUtils::~CPackUtils(void)
{
}

#define P_BYTES 1099511627776L
#define G_BYTES 1073741824
#define M_BYTES 1048576
#define K_BYTES 1024

void CPackUtils::TranslateSize(ULONGLONG nSize, CString & strOut)
{
	double dSize;
	ULONGLONG r;
	if (nSize > P_BYTES) {
		r = nSize / (P_BYTES);
		nSize %= (P_BYTES);
		dSize = (double)r + (double)nSize / (P_BYTES);
		strOut.Format(_T("%.1f PB"), dSize);
	} else if (nSize > G_BYTES) {
		r = nSize / (G_BYTES);
		nSize %= (G_BYTES);
		dSize = (double)r + (double)nSize / (G_BYTES);
		strOut.Format(_T("%.1f GB"), dSize);
	} else if(nSize > M_BYTES) {
		r = nSize / (M_BYTES);
		nSize %= (M_BYTES);
		dSize = (double)r + (double)nSize / (M_BYTES);
		strOut.Format(_T("%.1f MB"), dSize);
	} else if(nSize > K_BYTES) {
		r = nSize / (K_BYTES);
		nSize %= (K_BYTES);
		dSize = (double)r + (double)nSize / (K_BYTES);
		strOut.Format(_T("%.1f KB"), dSize);
	} else {
		dSize = (double) nSize;
		strOut.Format(_T("%.1f B"), dSize);
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

CString CPackUtils::GetTempFileName(LPCTSTR szPathName, LPCTSTR szPrefix)
{
	CString strRet = _T("");
	TCHAR szTempFileName[MAX_PATH];
	if (::GetTempFileName(szPathName, szPrefix, 0, szTempFileName)) {
		szTempFileName[_countof(szTempFileName) - 1] = 0;
		strRet = szTempFileName;
	}
	return strRet;
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

HANDLE CPackUtils::CreateTempFile(LPCTSTR szTempDir, CString & strTempFileName)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;

	strTempFileName = GetTempFileName(szTempDir, NULL);
	// create file
	hFile = ::CreateFile((LPCTSTR)strTempFileName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);
	return hFile;
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

CString CPackUtils::GetLastError(CException* e)
{
	TCHAR Buffer[4096];
	CString strRet;
	if (e->GetErrorMessage(Buffer, _countof(Buffer))) {
		Buffer[_countof(Buffer) - 1] = 0;
		strRet = Buffer;
	}
	return strRet;
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

void CPackUtils::FillBufferRand(LPBYTE pBuffer, ULONGLONG nSize)
{
	int* p = NULL;
	LPBYTE p1 = NULL;
	srand((unsigned)time(NULL));
	p = (int*)pBuffer;
	for (ULONGLONG i = 0; i < nSize / sizeof(int); i++) {
		*p = rand();
		p++;
	}
	p1 = (LPBYTE)p;
	for (INT i = 0; i < nSize % sizeof(int); i++) {
		p1[i] = rand();
	}
}
//设置窗口置顶
BOOL CPackUtils::SetWindowTop(CWnd* pWnd)
{
	if (pWnd) {
		if (pWnd->GetExStyle() & WS_EX_TOPMOST)
		{
			return TRUE;
		}
		else {
			return pWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	return FALSE;
}

//取消窗口置顶
BOOL CPackUtils::CancelWindowTop(CWnd* pWnd)
{
	if (pWnd) {
		return pWnd->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	} else {
		return FALSE;
	}
}

CString CPackUtils::FormatSecond(DOUBLE dSeconds)
{
	INT nHrs, nMin, nDay;
	DOUBLE dSec;
	CString strRet;

	nDay = (INT)(dSeconds / 86400.0);
	nHrs = (INT)((dSeconds - (nDay * 86400.0)) / 3600.0);
	nMin = (INT)((dSeconds - (nHrs * 3600.0)) / 60.0);
	dSec = dSeconds - (nHrs * 3600.0) - (nMin * 60.0);

	strRet.Format(_T("%02d[D]:%02d[H]:%02d[M]:%06.3f[S]"), nDay, nHrs, nMin, dSec);

	return strRet;
}

/*
// embed 2bit data into 3 bit target
// x2,x1                     a3,a2,a1
BYTE CPixelImageMedia::CPixelBlock::F5Embed(BYTE nData, BYTE nTarget)
{
	BYTE s = 0, dig, mask;
	// cal s

	nData &= 0x3;
	nTarget &= 0x7;

	mask = nTarget;
	for (INT i = 0; i < 3; i++) {
		s ^= (mask & 0x1) * (i + 1);
		mask >>= 1;
	}

	dig = s ^ nData;

	nTarget = nTarget ^ (1 << (dig - 1));

	return nTarget;

}
// extract 2bit data from 3 bit target
BYTE CPixelImageMedia::CPixelBlock::F5Extract(BYTE nTarget)
{
	BYTE s = 0;
	for (INT i = 0; i < 3; i++) {
		s ^= (nTarget & 0x1) * (i + 1);
		nTarget >>= 1;
	}

	return s & 0x3;
}
*/

/*

data = 0, target = 0, res = 0, res ^ target = 0, ext = 0
data = 0, target = 1, res = 0, res ^ target = 1, ext = 0
data = 0, target = 2, res = 0, res ^ target = 2, ext = 0
data = 0, target = 3, res = 7, res ^ target = 4, ext = 0
data = 0, target = 4, res = 0, res ^ target = 4, ext = 0
data = 0, target = 5, res = 7, res ^ target = 2, ext = 0
data = 0, target = 6, res = 7, res ^ target = 1, ext = 0
data = 0, target = 7, res = 7, res ^ target = 0, ext = 0

data = 1, target = 0, res = 1, res ^ target = 1, ext = 1
data = 1, target = 1, res = 1, res ^ target = 0, ext = 1
data = 1, target = 2, res = 6, res ^ target = 4, ext = 1
data = 1, target = 3, res = 1, res ^ target = 2, ext = 1
data = 1, target = 4, res = 6, res ^ target = 2, ext = 1
data = 1, target = 5, res = 1, res ^ target = 4, ext = 1
data = 1, target = 6, res = 6, res ^ target = 0, ext = 1
data = 1, target = 7, res = 6, res ^ target = 1, ext = 1

data = 2, target = 0, res = 2, res ^ target = 2, ext = 2
data = 2, target = 1, res = 5, res ^ target = 4, ext = 2
data = 2, target = 2, res = 2, res ^ target = 0, ext = 2
data = 2, target = 3, res = 2, res ^ target = 1, ext = 2
data = 2, target = 4, res = 5, res ^ target = 1, ext = 2
data = 2, target = 5, res = 5, res ^ target = 0, ext = 2
data = 2, target = 6, res = 2, res ^ target = 4, ext = 2
data = 2, target = 7, res = 5, res ^ target = 2, ext = 2

data = 3, target = 0, res = 4, res ^ target = 4, ext = 3
data = 3, target = 1, res = 3, res ^ target = 2, ext = 3
data = 3, target = 2, res = 3, res ^ target = 1, ext = 3
data = 3, target = 3, res = 3, res ^ target = 0, ext = 3
data = 3, target = 4, res = 4, res ^ target = 0, ext = 3
data = 3, target = 5, res = 4, res ^ target = 1, ext = 3
data = 3, target = 6, res = 4, res ^ target = 2, ext = 3
data = 3, target = 7, res = 3, res ^ target = 4, ext = 3

*/

const BYTE CPackUtils::F5LookupTable[4][8] = {
	{0,1,2,4,4,2,1,0},
	{1,0,4,2,2,4,0,1},
	{2,4,0,1,1,0,4,2},
	{4,2,1,0,0,1,2,4},
};

const BYTE CPackUtils::F5RevLookupTable[8] = {
	/*0,1,2,3,4,5,6,7 */
	  0,1,2,3,3,2,1,0
};
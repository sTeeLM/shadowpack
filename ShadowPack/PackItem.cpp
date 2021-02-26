#include "StdAfx.h"
#include "PackItem.h"
#include "PackUtils.h"

CPackItem::CPackItem(CPackItem * pParent = NULL):
	m_pParent(pParent),
	m_eRef(REF_NONE),
	m_nOffset(0),
	m_strName(_T(""))
{
	::ZeroMemory(&m_ItemHeader, sizeof(m_ItemHeader));
	m_ItemHeader.dwType = TYPE_NONE;
	m_ItemHeader.dwSignature = PACKAGE_ITEM_SIG;
}

CPackItem::~CPackItem(void)
{
	RemoveAllChildren();
	TRACE(_T("item %s %d freed\n"), m_strName, GetType());
}

BOOL CPackItem::IsValidItemHeader(const pack_item_header_t & header)
{
	if(header.dwSignature != PACKAGE_ITEM_SIG)
		return FALSE;

	if(header.dwNameSize > MAX_PATH || header.dwNameSize == 0)
		return FALSE;

	if(header.dwType == TYPE_DIR || header.dwType == TYPE_EOD) {
		if(header.dwDataSize != 0)
			return FALSE;
	} else if(header.dwType == TYPE_FILE) {
//		if(header.dwDataSize == 0)
//			return FALSE;
	} else {
		return FALSE;
	}

	return TRUE;
}

CPackItem * CPackItem::CreatePackItemFromDir(CPackItem * pParent, LPCTSTR szPath, LPCTSTR szName,CPackErrors & Error)
{
	CPackItem * pItem = new(std::nothrow) CPackItem(pParent);
	if(pItem) {
		pItem->SetType(TYPE_DIR);
		pItem->SetRef(REF_DISK);
		pItem->SetName(szName);
	} else {
		Error.SetError(CPackErrors::PE_NOMEM);
	}
	return pItem;
}

CPackItem * CPackItem::CreatePackItemFromFile(CPackItem * pParent, LPCTSTR szPath, LPCTSTR szName, LONGLONG nDataSize, CPackErrors & Error)
{
	CPackItem * pItem = new(std::nothrow) CPackItem(pParent);
	if(pItem) {
		CString strPath = szPath;
		strPath += _T("\\");
		strPath += szName;
		pItem->SetType(TYPE_FILE);
		pItem->SetRef(REF_DISK);
		pItem->SetDataSize(nDataSize);
		pItem->SetName(szName);
		pItem->SetDiskLocation(strPath);
	} else {
		Error.SetError(CPackErrors::PE_NOMEM);
	}
	return pItem;
}

CPackItem * CPackItem::CreatePackItemFromMediaFile(CPackItem * pParent, pack_item_header_t & Header, 
		CStream * pInput, BOOL & bCancel, CProgress &Progress, CPackErrors & Error)
{
	CPackItem * pItem = NULL;
	CHAR szNameA[MAX_PATH + 1] = {0};
	ULONGLONG nSize = Header.dwNameSize;

	pItem = new(std::nothrow) CPackItem(pParent);
	if(pItem) {
		memcpy(&pItem->m_ItemHeader, &Header, sizeof(Header));
		if(nSize > MAX_PATH || !pInput->Read(szNameA, nSize, Progress, Error)) {
			goto err;
		}
		szNameA[nSize] = 0;
		CA2CT szName(szNameA);
		pItem->SetName(szName);
		pItem->SetRef(REF_MEDIA);
		pItem->SetOffset(pInput->GetOffset() - pItem->GetNameSize() - pItem->GetHeaderSize());

		if(!pInput->Seek(Header.dwDataSize, SEEK_CUR, Error)) {
			goto err;
		}
		Progress.IncScale(Header.dwDataSize);
	} else {
		Error.SetError(CPackErrors::PE_NOMEM);
	}

	return pItem;

err:
	if(NULL != pItem) {
		delete pItem;
		pItem = NULL;
	}
	return NULL;
}

CPackItem * CPackItem::CreatePackItemFromMediaDir(CPackItem * pParent, pack_item_header_t & Header, 
		CStream * pInput, BOOL & bCancel, CProgress &Progress, CPackErrors & Error)
{
	CPackItem * pItem = NULL;
	CHAR szNameA[MAX_PATH + 1] = {0};
	ULONGLONG nSize = Header.dwNameSize;

	pItem = new(std::nothrow) CPackItem(pParent);
	if(pItem) {
		memcpy(&pItem->m_ItemHeader, &Header, sizeof(Header));
		if(nSize > MAX_PATH || !pInput->Read(szNameA, nSize, Progress, Error)) {
			goto err;
		}

		szNameA[nSize] = 0;
		CA2CT szName(szNameA);
		pItem->SetName(szName);
		pItem->SetOffset(pInput->GetOffset() - pItem->GetNameSize() - pItem->GetHeaderSize());
		pItem->SetRef(REF_MEDIA);
	} else {
		Error.SetError(CPackErrors::PE_NOMEM);
	}
	return pItem;
err:
	if(NULL != pItem) {
		delete pItem;
		pItem = NULL;
	}
	return NULL;
}

BOOL CPackItem::HasChild(LPCTSTR szName, pack_item_type_t type)
{
	INT nSize = GetChildrenCount();
	while( -- nSize >= 0) {
		if(!lstrcmp(m_Children[nSize]->GetName(), szName)) {
			return TRUE;
		}
	}
	return FALSE;
}

LONGLONG CPackItem::EvalSize(pack_item_type_t type, LPCTSTR szName, LONGLONG nDataSize)
{
	CStringA strA;
	LPCTSTR p = NULL;
	LONGLONG nNameLen;
 
	if(type == TYPE_FILE) { // szPath is name
		nNameLen = CPackUtils::ST2SA(szName, strA);
		return nNameLen + nDataSize + sizeof(pack_item_header_t);
	} else if(type == TYPE_DIR){
		nNameLen = CPackUtils::ST2SA(szName, strA);
		return 2 * (nNameLen + sizeof(pack_item_header_t));
	}
	return 0;
}

void CPackItem::AddChild(CPackItem * pItem)
{
	if(IsDir()) {
		m_Children.Add(pItem);
	}
}

BOOL CPackItem::RemoveAllChildren(CB_REMOVE cb, LPVOID pParam)
{
	if(IsDir()) {

		INT nSize = GetChildrenCount();

		while( -- nSize >= 0) {
			if(m_Children[nSize]->IsDir()) {
				if(!m_Children[nSize]->RemoveAllChildren(cb, pParam)) {
					return FALSE;
				}
			}
			if(NULL != cb) {
				if(!cb(m_Children[nSize], pParam)){
					return FALSE;;
				}
			}
			delete m_Children[nSize];
			m_Children.RemoveAt(nSize);
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CPackItem::RemoveChild(CPackItem* pItem, CB_REMOVE cb, LPVOID pParam)
{
	if(IsDir()) {
		INT nSize = GetChildrenCount();
		while( -- nSize >= 0) {
			if(m_Children[nSize] == pItem) {
				if(m_Children[nSize]->IsDir()) {
					if(!pItem->RemoveAllChildren(cb, pParam)) {
						return FALSE;
					}
				}
				if(NULL != cb) {
					if(!cb(pItem, pParam)){
						return FALSE;
					}
				}
				m_Children.RemoveAt(nSize);
				delete pItem;
				return TRUE;
			}
		}
	}
	return FALSE;
}


CString CPackItem::GetExt()
{
	CPath path = m_strName;
	return path.GetExtension();
}
void CPackItem::SetName(LPCTSTR szName) 
{
	CStringA strA;
	m_strName = szName == NULL ? _T("") : szName;
	m_ItemHeader.dwNameSize = CPackUtils::ST2SA(szName, strA);
}

 
BOOL CPackItem::WalkItemTree(CB_WALK cb, LPVOID pParam)
{

	if(cb != NULL) { // self
		if(!cb(this, FALSE, pParam)) return FALSE;
	}

	// children
    INT nSize = GetChildrenCount();
	for(INT i = 0 ; i < nSize ; i ++) {
		if(m_Children[i]->IsDir()) {
			if(!m_Children[i]->WalkItemTree(cb, pParam)) {
				return FALSE;
			}
		} else if(m_Children[i]->IsFile()){
			if(!cb(m_Children[i], FALSE, pParam)) return FALSE;
		}
	}

	if(cb != NULL && IsDir()) { // eod
		if(!cb(this, TRUE, pParam)) return FALSE;
	}
	return TRUE;
}

BOOL CPackItem::SaveToMediaEOD(CStream * pInput, CStream * pOutput, CProgress & Progress, CPackErrors & Error)
{
	
	ULONGLONG nOffset = pOutput->GetOffset();

	TRACE(_T("save eod of %s, offset %lld\n"), GetName(), nOffset);


	CT2CA szNameA(m_strName);
	pack_item_header_t tmpHeader;

	memcpy(&tmpHeader, &m_ItemHeader, sizeof(m_ItemHeader));
	tmpHeader.dwType = TYPE_EOD;

	if(!pOutput->Write(&tmpHeader, sizeof(m_ItemHeader), Progress, Error)) {
		return FALSE;
	}

	if(!pOutput->Write(szNameA,strlen(szNameA), Progress, Error)) {
		return FALSE;
	}

	return TRUE;
}

BOOL CPackItem::SaveToMedia(CStream * pInput, CStream * pOutput, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress)
{

	ULONGLONG nOffset = pOutput->GetOffset();

	TRACE(_T("save item to media : %s, type %d, ref %d offset %lld\n"), GetName(),GetType() , GetRef(), nOffset);

	CT2CA szNameA(m_strName);

	if(!pOutput->Write(&m_ItemHeader, sizeof(m_ItemHeader), Progress, Error)) {
		return FALSE;
	}

	if(!pOutput->Write(szNameA,strlen(szNameA), Progress, Error)) {
		return FALSE;
	}

	if(IsFile()) {
		if(IsRefDisk()) {
			if(!CopyFile(GetDiskLocation() ,pOutput, bCancel, Error, Progress)) {
				return FALSE;
			}
		} else if(IsRefMedia()){
			if(!CopyFile(pInput ,pOutput, bCancel, Error, Progress)) {
				return FALSE;
			}
		} else {

		}
	}



	// 设置 ref为media，更新offset！
	SetRef(REF_MEDIA);
	SetOffset(nOffset);
	SetDiskLocation(_T(""));

	return TRUE;
}
// stream to stream
BOOL CPackItem::CopyFile(CStream * pInput, CStream * pOutput, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress)
{
	ULONGLONG nLength, nReaded;
	BYTE buffer[4096];
	BOOL bRet = FALSE;

	nLength = GetDataSize();
	while(nLength) {
		if(bCancel) {
			Error.SetError(CPackErrors::PE_CANCELED);
			goto err;
		}
		nReaded = sizeof(buffer) < nLength ? sizeof(buffer) : nLength;
		if(!pInput->Read(buffer, nReaded, Progress, Error)) {
			goto err;
		}
		if(!pOutput->Write(buffer, nReaded, Progress, Error)) {
			goto err;
		}
		nLength -= nReaded;
	}
	bRet = TRUE;
err:
	return bRet;
}

// file to file
BOOL CPackItem::CopyFile(LPCTSTR szSrc, LPCTSTR szDst, BOOL & bCancel, BOOL bAllowOverwrite, CPackErrors & Error, CProgress & Progress)
{
	CFile fileSrc, fileDst;
	CFileException exSrc, exDst;
	ULONGLONG nLength, nReaded;
	BYTE buffer[4096];
	BOOL bRet = FALSE;
	DWORD dwErr;

	if(!fileSrc.Open(szSrc, CFile::modeRead | CFile::shareDenyWrite, &exSrc)) {
		dwErr = ::GetLastError();
		Error.SetError(CPackErrors::PE_IO,szSrc,CPackUtils::GetLastError(::GetLastError()));
		goto err;
	}

	if(!fileDst.Open(szDst, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, &exDst)) {
		dwErr = ::GetLastError();
		Error.SetError(CPackErrors::PE_IO,szDst, CPackUtils::GetLastError(::GetLastError()));
		goto err;
	}

	nLength = fileSrc.GetLength();
	if(nLength > GetDataSize()) {
		nLength = GetDataSize();
	}

	try {
		while(nLength) {
			if(bCancel) {
				Error.SetError(CPackErrors::PE_CANCELED);
				goto err;
			}
			nReaded = fileSrc.Read(buffer, sizeof(buffer));
			if(nLength >= nReaded) {
				nLength -= nReaded;
			} else {
				nLength = 0;
				nReaded = nLength;
			}
			fileDst.Write(buffer, (INT)nReaded);
			Progress.IncScale(nReaded);
		}
	} catch (CFileException * ex) {
		ex = NULL;
		Error.SetError(CPackErrors::PE_IO, szSrc,CPackUtils::GetLastError(::GetLastError()));
		goto err;
	}

	bRet = TRUE;
err:
	if(fileSrc.m_hFile != INVALID_HANDLE_VALUE)
		fileSrc.Close();
	if(fileDst.m_hFile != INVALID_HANDLE_VALUE)
		fileDst.Close();
	if(!bRet)
		DeleteFile(szDst);
	return bRet;
}

// file to stream
BOOL CPackItem::CopyFile(LPCTSTR szSrcPath, CStream * pOutput, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress)
{
	CFile fileSrc;
	CFileException exSrc;
	ULONGLONG nLength, nReaded;
	BYTE buffer[4096];
	BOOL bRet = FALSE;
	DWORD dwErr;

	if(!fileSrc.Open(szSrcPath, CFile::modeRead | CFile::shareDenyWrite, &exSrc)) {
		dwErr = ::GetLastError();
		Error.SetError(CPackErrors::PE_IO, szSrcPath, CPackUtils::GetLastError(::GetLastError()));
		goto err;
	}

	nLength = fileSrc.GetLength();

	if(nLength > GetDataSize()) {
		nLength = GetDataSize();
	}

	try {
		while(nLength) {
			if(bCancel) {
				Error.SetError(CPackErrors::PE_CANCELED);
				goto err;
			}
			nReaded = fileSrc.Read(buffer, sizeof(buffer));
			if(nLength >= nReaded) {
				nLength -= nReaded;
			} else {
				nLength = 0;
				nReaded = nLength;
			}

			if(!pOutput->Write(buffer, nReaded, Progress, Error)) {
				goto err;
			}
			
		}
	} catch (CFileException * ex) {
		ex = NULL;
		Error.SetError(CPackErrors::PE_IO,szSrcPath, CPackUtils::GetLastError(::GetLastError()));
		goto err;
	}

	bRet = TRUE;
err:
	if(fileSrc.m_hFile != INVALID_HANDLE_VALUE)
		fileSrc.Close();
	return bRet;
}

// stream to file
BOOL CPackItem::CopyFile(CStream * pInput, LPCTSTR szDstPath, BOOL & bCancel, 
						 BOOL bAllowOverwrite, CPackErrors & Error, CProgress & Progress)
{
	CFile fileDst;
	CFileException exDst;
	ULONGLONG nLength, nReaded;
	BYTE buffer[4096];
	BOOL bRet = FALSE;

	if(!fileDst.Open(szDstPath, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, &exDst)) {
		Error.SetError(CPackErrors::PE_IO,szDstPath, CPackUtils::GetLastError(::GetLastError()));
		goto err;
	}

	nLength = GetDataSize();

	try {
		while(nLength) {
			if(bCancel) {
				Error.SetError(CPackErrors::PE_CANCELED);
				goto err;
			}
			nReaded = sizeof(buffer) < nLength ? sizeof(buffer) : nLength;
			if(!pInput->Read(buffer, nReaded, Progress, Error)) {
				goto err;
			}
			nLength -= nReaded;
			fileDst.Write(buffer, (INT)nReaded);
		}
	} catch (CFileException * ex) {
		ex = NULL;
		Error.SetError(CPackErrors::PE_IO, szDstPath,CPackUtils::GetLastError(::GetLastError()));
		goto err;
	}

	bRet = TRUE;
err:
	if(fileDst.m_hFile != INVALID_HANDLE_VALUE)
		fileDst.Close();
	if(!bRet)
		DeleteFile(szDstPath);
	return bRet;
}

BOOL CPackItem::ExportToDiskPath(CStream * pInput, LPCTSTR szDstPath, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress, BOOL bChangeRefToDisk/* = FALSE */, BOOL bAllowOverwrite /* = FALSE*/)
{	
	BOOL bRet = FALSE;

	TRACE(_T("ExportToDiskPath: [%s][%s] %s->%s\n"), 
		IsDir() ? _T("DIR"):_T("FILE"), 
		IsRefDisk() ? _T("DISK"):_T("MEDIA"),
		GetName(),
		szDstPath);

	if(IsFile()) {
		if(IsRefMedia()) {
			if(pInput->Seek(GetOffset() + GetNameSize() + GetHeaderSize(), SEEK_SET, Error)) {
				bRet =  CopyFile(pInput, szDstPath, bCancel, bAllowOverwrite, Error, Progress);
			}
		} else if(IsRefDisk()) {
			bRet =  CopyFile(GetDiskLocation(), szDstPath, bCancel, bAllowOverwrite, Error, Progress);
		}
	} else if(IsDir()) {
		bRet = ::CreateDirectory(szDstPath, NULL);
		DWORD dwLastError = ::GetLastError();
		if(!bRet && dwLastError == ERROR_ALREADY_EXISTS) {
			if(!bAllowOverwrite)
				Error.SetError(CPackErrors::PE_EXISTED, szDstPath);
			else
				bRet = TRUE;
		} else if(!bRet) {
			Error.SetError(CPackErrors::PE_IO, szDstPath, CPackUtils::GetLastError(dwLastError));
		} else {
			Progress.IncScale(GetDataSize());
		}
	}

	if(bRet & bChangeRefToDisk) {
		SetOffset(0);
		SetRef(REF_DISK);
		SetDiskLocation(szDstPath);
	}

	return bRet;
}
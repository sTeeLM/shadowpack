#include "StdAfx.h"
#include "Pack.h"
#include "Resource.h"

#include "Media.h"
#include "MediaFactory.h"
#include "Stream.h"


CPack::CPack(void): 
	CPackItem(NULL),
	m_nTotalDataSize(0),
	m_isDirty(FALSE),
	m_pCurrentDir(NULL),
	m_pInputMedia(NULL),
	m_pOutputMedia(NULL)
{
	SetName(ROOT_NAME);
	SetType(CPackItem::TYPE_DIR);
	SetRef(CPackItem::REF_NONE);
	m_pCurrentDir = this;
}

CPack::~CPack(void)
{
	
}


void CPack::SetCurrentDir(CPackItem * pItem)
{
	m_pCurrentDir = pItem;
}

CPackItem * CPack::GetCurrentDir()
{
	return m_pCurrentDir;
}

void CPack::SetDirty(BOOL bDirty /*= TRUE*/)
{
	m_isDirty = bDirty;
}

BOOL CPack::IsDirty()
{
	return m_isDirty;
}

BOOL CPack::IsEmpty()
{
	return GetChildrenCount() == 0;
}


void CPack::SetTotalDataSize(LONGLONG n)
{
	TRACE(_T("root SetTotalDataSize %lld\n"), n);
	m_nTotalDataSize = n;
	if(m_pInputMedia) {
		m_pInputMedia->SetDataSize(m_nTotalDataSize);
	}
	if(m_pOutputMedia) {
		m_pOutputMedia->SetDataSize(m_nTotalDataSize);
	}
}
void CPack::DecTotalDataSize(LONGLONG n)
{
	TRACE(_T("root DecTotalDataSize %lld -> %lld\n"),m_nTotalDataSize, (m_nTotalDataSize - n));
	m_nTotalDataSize -= n;
	if(m_pInputMedia) {
		m_pInputMedia->SetDataSize(m_nTotalDataSize);
	}
	if(m_pOutputMedia) {
		m_pOutputMedia->SetDataSize(m_nTotalDataSize);
	}
}
void CPack::IncTotalDataSize(LONGLONG n)
{
	TRACE(_T("root IncTotalDataSize %lld -> %lld\n"),m_nTotalDataSize, (m_nTotalDataSize + n));
	m_nTotalDataSize += n;
	if(m_pInputMedia) {
		m_pInputMedia->SetDataSize(m_nTotalDataSize);
	}
	if(m_pOutputMedia) {
		m_pOutputMedia->SetDataSize(m_nTotalDataSize);
	}
}

BOOL CPack::AddItemDirGetSizeCB(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam)
{
	CAddItemCBParam * p = (CAddItemCBParam *)pParam;
	LARGE_INTEGER nSize;

	CString strInfo;

	if(*p->pbCancel) {
		p->pError->SetError(CPackErrors::PE_CANCELED);
		return FALSE;
	}

	if(NULL == pffd) { // is EOD
		
	} else if(pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		p->nTotalSize.QuadPart += CPackItem::EvalSize(CPackItem::TYPE_DIR, pffd->cFileName, 0);
		strInfo.Format(IDS_SCAN_DIR, szPath, pffd->cFileName);
	} else {
		strInfo.Format(IDS_SCAN_FILE, szPath, pffd->cFileName);
		nSize.HighPart = pffd->nFileSizeHigh;
		nSize.LowPart = pffd->nFileSizeLow;
		p->nTotalSize.QuadPart += CPackItem::EvalSize(CPackItem::TYPE_FILE, pffd->cFileName, nSize.QuadPart);
	}

	p->pProgress->SetInfo(strInfo);
	
	return TRUE;
}
BOOL CPack::AddItemDirCreateItemCB(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam)
{
	CAddItemCBParam * p = (CAddItemCBParam *)pParam; 
	CPackItem * pItem = NULL;
	LARGE_INTEGER nSize;

	if(*p->pbCancel) {
		p->pError->SetError(CPackErrors::PE_CANCELED);
		return FALSE;
	}

	if(NULL == pffd) { // is EOD
		p->pItem = p->pItem->GetParent();
	} else if(pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		pItem = CPackItem::CreatePackItemFromDir(p->pItem, szPath, pffd->cFileName, *p->pError);
		if(pItem) {
			p->pItem->AddChild(pItem);
			p->pItem = pItem;
			p->pProgress->IncScale(pItem->GetTotalSize());
		}
	} else {
		nSize.HighPart = pffd->nFileSizeHigh;
		nSize.LowPart = pffd->nFileSizeLow;
		pItem = CPackItem::CreatePackItemFromFile(p->pItem, szPath, pffd->cFileName, nSize.QuadPart, *p->pError);
		if(pItem) {
			p->pItem->AddChild(pItem);
			p->pProgress->IncScale(pItem->GetTotalSize());
		}
	}
	
	return (pffd != NULL && pItem != NULL) || (pffd == NULL && pItem == NULL);
}

BOOL CPack::AddItemDirInternal(LPCTSTR szPathToAdd, CPackItem * pRoot, CPackErrors & Error, 
					   BOOL & bCancel, CProgress & Progress)
{
	BOOL bRet = FALSE;
	CString strDirName;
	CPackItem * pItem = NULL;

	CAddItemCBParam Param;
	Param.pItem = NULL;
	Param.pbCancel = &bCancel;
	Param.nTotalSize.QuadPart = 0;
	Param.pError = &Error;
	Param.pProgress = &Progress;

	TRACE(_T("AddItemDir %s, root is %s\n"), szPathToAdd, pRoot->GetName());

	strDirName = CPackUtils::GetPathName(szPathToAdd);

	if((pItem = CPackItem::CreatePackItemFromDir(pRoot,szPathToAdd, strDirName, Error)) == NULL) {
		goto error;
	}
	
	Param.pItem = pItem;

	// get total size, and test each dir and file ok, see if > cap of media
	
	if(!CPackUtils::WalkDir(szPathToAdd, AddItemDirGetSizeCB, &Param)) {
		if(bCancel) {
			Error.SetError(CPackErrors::PE_CANCELED);
		}
		goto error;
	}

	Progress.SetFullScale(Param.nTotalSize.QuadPart);
	Progress.ShowProgressBar();
	Progress.ShowInfoBar(FALSE);

	Param.nTotalSize.QuadPart += Param.pItem->GetTotalSize();

	if(Param.nTotalSize.QuadPart + GetTotalDataSize() > GetOutputMedia()->GetCapicity()) {
		Error.SetError(CPackErrors::PE_OVER_CAPICITY);
		goto error;
	}

	// real work!
	if(!CPackUtils::WalkDir(szPathToAdd, AddItemDirCreateItemCB, &Param)) {
		if(bCancel) {
			Error.SetError(CPackErrors::PE_CANCELED);
		}
		goto error;
	}

	pRoot->AddChild(pItem);
	IncTotalDataSize(Param.nTotalSize.QuadPart);

	SetDirty();

	bRet = TRUE;
	return bRet;

error:

	CDeleteItemCBParam ParamRemove;
	ParamRemove.pError = & Error;
	ParamRemove.pProgress = &Progress;
	ParamRemove.pbCancel = &bCancel;
	ParamRemove.pThis = this;
	ParamRemove.bIsRollBack = TRUE;
	
	Progress.ShowProgressBar(FALSE);
	Progress.ShowInfoBar(TRUE);

	if(pItem != NULL) {
		pItem->RemoveAllChildren(DeleteItemCB, &ParamRemove);
		delete pItem;
		pItem = NULL;
	}
	return bRet;
}

BOOL CPack::AddItemDirListItemCB(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam)
{
	CAddItemListDirCBParam * p = (CAddItemListDirCBParam *) pParam;
	if(NULL != pffd) {
		if(!p->pRoot->HasChild(pffd->cFileName, 
			(pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? CPackItem::TYPE_DIR :
			CPackItem::TYPE_FILE)) {
			CString strPath = szPath;
			CString strInfo;
			strPath += _T("\\");
			strPath += pffd->cFileName;
			p->aPathToAdd.Add(strPath);
			p->aType.Add((pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? CPackItem::TYPE_DIR : 
				CPackItem::TYPE_FILE);
			TRACE(_T("AddItemDirListItemCB: add item %s , type %d\n"), strPath, 
				(pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? CPackItem::TYPE_DIR : 
				CPackItem::TYPE_FILE);
			if(pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				strInfo.Format(IDS_SCAN_DIR, szPath, pffd->cFileName);
			} else {
				strInfo.Format(IDS_SCAN_FILE, szPath, pffd->cFileName);
			}
			p->pProgress->SetInfo(strInfo);
		} else {
			p->pError->SetError(CPackErrors::PE_DUP_ITEM, pffd->cFileName);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CPack::AddItemDir(LPCTSTR szPathToAdd, CPackItem * pRoot, CPackErrors & Error, 
				   BOOL & bCancel, CProgress & Progress)
{
	CAddItemListDirCBParam * pParam = new(std::nothrow) CAddItemListDirCBParam();
	pParam->pRoot = pRoot;
	pParam->pError = &Error;
	pParam->pProgress = &Progress;
	INT nSize = 0;
	BOOL bRet = FALSE;

	CString strNameToAdd = CPackUtils::GetPathName(szPathToAdd);
	if(strNameToAdd[0] == 0) { // is end with \ (C:\), no name part
	// 添加内容
		if(!CPackUtils::WalkDir(szPathToAdd, &AddItemDirListItemCB, pParam, TRUE, 1)){
			goto error;
		}
	} else {
	// 添加目录，首先检查是否有重复项目
		if(pRoot->HasChild(strNameToAdd, CPackItem::TYPE_DIR)) {
			pParam->pError->SetError(CPackErrors::PE_DUP_ITEM, strNameToAdd);
			goto error;
		} else {
			pParam->aPathToAdd.Add(szPathToAdd);
			pParam->aType.Add(CPackItem::TYPE_DIR);
		}
	}

	nSize = pParam->aPathToAdd.GetCount();
	for(INT i = 0 ; i < nSize; i ++) {
		if(pParam->aType[i] == CPackItem::TYPE_DIR) {
			if(!AddItemDirInternal(pParam->aPathToAdd[i], pRoot, Error, bCancel, Progress)) {
				goto error;
			}
		} else {
			if(!AddItemFile(pParam->aPathToAdd[i], pRoot, Error)) {
				goto error;
			}
		}
	}
	SetDirty();
	bRet = TRUE;
error:
	if(NULL != pParam) {
		delete pParam;
		pParam = NULL;
	}
	return bRet;
}

BOOL CPack::AddItemFile(LPCTSTR szPathToAdd, CPackItem * pRoot, CPackErrors & Error)
{
	BOOL bRet = FALSE;
	CString strFileName, strParentPath;
	CPackItem * pItem = NULL;
	ULONGLONG nSize;

	TRACE(_T("AddItemFile %s, root is %s\n"), szPathToAdd, pRoot->GetName());

	if(!CPackUtils::GetFileSize(szPathToAdd, nSize)) {
		Error.SetError(CPackErrors::PE_IO, szPathToAdd, CPackUtils::GetLastError(::GetLastError()));
		goto error;
	}

	strFileName = CPackUtils::GetPathName(szPathToAdd);

	if(pRoot->HasChild(strFileName, CPackItem::TYPE_FILE)) {
		Error.SetError(CPackErrors::PE_DUP_ITEM, strFileName);
		goto error;
	}

	strParentPath = CPackUtils::GetPathPath(szPathToAdd);
	

	if((pItem = CPackItem::CreatePackItemFromFile(pRoot,strParentPath, strFileName, nSize, Error)) == NULL) {
		goto error;
	}

	if(pItem->GetTotalSize() + GetTotalDataSize() > GetOutputMedia()->GetCapicity()) {
		Error.SetError(CPackErrors::PE_OVER_CAPICITY);
		goto error;
	}

	pRoot->AddChild(pItem);
	IncTotalDataSize(pItem->GetTotalSize());
	SetDirty();

	bRet = TRUE;
	return bRet;
error:

	if(pItem != NULL) {
		pItem->RemoveAllChildren();
		delete pItem;
		pItem = NULL;
	}
	return bRet;
}

BOOL CPack::DeleteItemCB(CPackItem * pItem, LPVOID pParam)
{
	CString strInfo;
	CDeleteItemCBParam * p = (CDeleteItemCBParam *)pParam;

	if(*p->pbCancel && !p->bIsRollBack) {
		p->pError->SetError(CPackErrors::PE_CANCELED);
		return FALSE;
	}

	if(!p->bIsRollBack) {
		p->pThis->DecTotalDataSize(pItem->GetTotalSize());
	}
	strInfo.Format(IDS_DELETE_ITEM, pItem->GetName());
	p->pProgress->SetInfo(strInfo);
	TRACE(_T("deleteing %s\n"), pItem->GetName());
	return TRUE;
}

BOOL CPack::DeleteItem(CPackItem * pItem, CPackErrors & Error, 
				   BOOL & bCancel, CProgress & Progress)
{
	CDeleteItemCBParam Param;
	Param.pError = & Error;
	Param.pProgress = &Progress;
	Param.pbCancel = &bCancel;
	Param.pThis = this;
	Param.bIsRollBack = FALSE;
	CPackItem *pParent = NULL;
	BOOL bRet = FALSE;

	pParent = pItem->GetParent();

	if(pParent) {
		bRet = pParent->RemoveChild(pItem, DeleteItemCB, &Param);
		SetDirty();
	}

	return bRet;
}

BOOL CPack::ExportItemCB(CPackItem * pItem, BOOL bIsEOD, LPVOID pParam)
{
	CExportItemCBParam * p = (CExportItemCBParam * )pParam;
	CString strInfo;
	CString strPath;

	if(*p->pbCancel) {
		p->pError->SetError(CPackErrors::PE_CANCELED);
		return FALSE;
	}

	if(p->bIsGetSize) {

		if(!bIsEOD && pItem->IsFile()) {
			p->nTotalSize.QuadPart += pItem->GetDataSize();
		}
		p->nTotalItemCnt ++;
		
		if(!bIsEOD) {
			strPath = p->strExportRoot + _T("\\") + pItem->GetName();
			if(CPackUtils::IsPathExist(strPath)) {
				p->pError->SetError(CPackErrors::PE_EXISTED, strPath);
				return FALSE;
			}
		} else {
			strPath = p->strExportRoot;
		}

		if(!bIsEOD && pItem->IsFile()) {
			strInfo.Format(IDS_SCAN_FILE, p->strExportRoot, pItem->GetName());
			TRACE(_T("check file %s\\%s\n"),p->strExportRoot, pItem->GetName());
			p->pProgress->SetInfo(strInfo);
		} else if(!bIsEOD  && pItem->IsDir()){
			strInfo.Format(IDS_SCAN_DIR, p->strExportRoot, pItem->GetName());
			p->strExportRoot = strPath;
			TRACE(_T("into dir %s\n"), p->strExportRoot);
			p->pProgress->SetInfo(strInfo);
		} else{
			strPath = CPackUtils::GetPathPath(p->strExportRoot);
			TRACE(_T("up to dir %s -> %s\n"), p->strExportRoot, strPath);
			p->strExportRoot = strPath;
		}
		return TRUE;
	} else {
		if(!bIsEOD) {
			strPath = p->strExportRoot + _T("\\") + pItem->GetName();
			if(CPackUtils::IsPathExist(strPath)) {
				p->pError->SetError(CPackErrors::PE_EXISTED, strPath);
				return FALSE;
			}
		} else {
			strPath = p->strExportRoot;
		}
		if(!bIsEOD) {
			if(p->bOnMediaOnly && !pItem->IsRefMedia()) {
				TRACE(_T("skip item %s\\%s\n"), p->strExportRoot, pItem->GetName());
				if(pItem->IsDir()) {
					p->strExportRoot = strPath;
				}
			} else {
				if(!pItem->ExportToDiskPath(p->pThis->GetInputMedia()->GetStream(),
					strPath, *p->pbCancel, *p->pError, *p->pProgress, p->bChangeRefToDisk)) {
					return FALSE;
				}
		
				if(pItem->IsDir()) {
					p->strExportRoot = strPath;
				}
			}
		} else {
			strPath = CPackUtils::GetPathPath(p->strExportRoot);
			TRACE(_T("up to dir %s -> %s\n"), p->strExportRoot, strPath);
			p->strExportRoot = strPath;
		}
		return TRUE;
	}

}

BOOL CPack::ExportItemDirToDiskPath(CPackItem * pItem, CExportItemCBParam & Param)
{
	return pItem->WalkItemTree(ExportItemCB, &Param);
}

BOOL CPack::ExportItemFileToDiskPath(CPackItem * pItem, LPCTSTR szDstPath, LPCTSTR szDstName, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress, BOOL bMediaOnly, BOOL bChangeRefToDisk, BOOL bAllowOverwrite)
{
	CString strPath;
	if(bMediaOnly && !pItem->IsRefMedia()) {
		return TRUE;
	}
	strPath.Format(_T("%s\\%s"), szDstPath, szDstName);
	if(!bAllowOverwrite) {
		if(CPackUtils::IsPathExist(strPath)) {
			Error.SetError(CPackErrors::PE_EXISTED, strPath);
			return FALSE;
		}
	}
	return pItem->ExportToDiskPath(GetInputMedia()->GetStream(),strPath, bCancel, Error, Progress, bChangeRefToDisk);
}

BOOL CPack::ExportItemFileToDiskPath(CPackItem * pItem, CExportItemCBParam & Param)
{
	CString strPath;

	if(Param.bIsGetSize) {
		Param.nTotalSize.QuadPart += pItem->GetTotalSize();
		return TRUE;
	} else {
		return ExportItemFileToDiskPath(pItem, Param.strExportRoot, pItem->GetName(), 
			*Param.pbCancel, *Param.pError, *Param.pProgress, Param.bOnMediaOnly, Param.bChangeRefToDisk, FALSE);
	}
	return TRUE;
}

BOOL CPack::ExportItemToDiskPath(CArray<CPackItem *, CPackItem *> & aExport, LPCTSTR szDstDir, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress, BOOL bMediaOnly, BOOL bChangeRefToDisk)
{
	INT nSize = aExport.GetCount();

	CExportItemCBParam Param;
	Param.pError = &Error;
	Param.pProgress = &Progress;
	Param.pbCancel = &bCancel;
	Param.pThis = this;
	Param.bIsGetSize = TRUE;
	Param.nTotalSize.QuadPart = 0;
	Param.nTotalItemCnt = 0;
	Param.strExportRoot = szDstDir;
	Param.bOnMediaOnly = bMediaOnly;
	Param.bChangeRefToDisk = bChangeRefToDisk;

	Progress.ShowInfoBar();
	Progress.ShowProgressBar(FALSE);

	// get size;
	for(INT i = 0 ; i < nSize ; i ++) {
		if(aExport[i]->IsDir()) {
			if(!ExportItemDirToDiskPath(aExport[i], Param)) {
				return FALSE;
			}
		} else if(aExport[i]->IsFile()){
			if(!ExportItemFileToDiskPath(aExport[i], Param)) {
				return FALSE;
			}
		}
	}

	// real work

	Progress.ShowInfoBar(FALSE);
	Progress.ShowProgressBar();
	Progress.SetFullScale(Param.nTotalSize.QuadPart);
	Param.bIsGetSize = FALSE;

	for(INT i = 0 ; i < nSize ; i ++) {
		if(aExport[i]->IsDir()) {
			if(!ExportItemDirToDiskPath(aExport[i], Param)) {
				return FALSE;
			}
		} else if(aExport[i]->IsFile()){
			if(!ExportItemFileToDiskPath(aExport[i], Param)) {
				return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CPack::Clear( CPackErrors & Error, 
				   BOOL & bCancel, CProgress & Progress)
{
	CDeleteItemCBParam Param;
	Param.pError = & Error;
	Param.pProgress = &Progress;
	Param.pbCancel = &bCancel;
	Param.pThis = this;
	CPackItem *pParent = NULL;
	BOOL bRet = FALSE;
	Param.bIsRollBack = FALSE;

	bRet = RemoveAllChildren(DeleteItemCB, &Param);

	SetCurrentDir(this);

	SetDirty();

	return bRet;
}

BOOL CPack::SaveItemCB(CPackItem * pItem, BOOL bIsEOD, LPVOID pParam)
{
	CSaveItemCBParam * p = (CSaveItemCBParam * )pParam;

	if(pItem->IsRoot()) {
		TRACE(_T("skip root\n"));
	} else if(bIsEOD && pItem->IsDir()){
		if(!pItem->SaveToMediaEOD(p->pThis->GetInputMedia()->GetStream(),
			p->pThis->GetOutputMedia()->GetStream(), 
			*p->pProgress, *p->pError)) {
			return FALSE;
		}
	} else{
		if(!pItem->SaveToMedia(p->pThis->GetInputMedia()->GetStream(),
			p->pThis->GetOutputMedia()->GetStream(),
			*p->pbCancel, *p->pError, *p->pProgress)) {
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CPack::SavePack(LPCTSTR szPathName, CPasswordGetter & PasswordGetter, BOOL & bCancel,  CPackErrors & Error, CProgress & Progress)
{

	CSaveItemCBParam Param;
	CMedia * pNewInputMedia = NULL, * pNewOutputMedia = NULL;

	Param.pError = &Error;
	Param.pProgress = &Progress;
	Param.pbCancel = &bCancel;
	Param.pThis = this;

	
	Progress.ShowInfoBar(FALSE);
	Progress.ShowProgressBar();


	GetOutputMedia()->SetDataSize(GetTotalDataSize());

	if(!GetOutputMedia()->OpenMedia(CMedia::MODE_WRITE, szPathName, PasswordGetter, bCancel, Error, Progress)) {
		goto err;
	}

	// 开始保存
	Progress.Reset();
	Progress.SetFullScale(GetTotalDataSize());
	if(!GetOutputMedia()->GetStream()->Seek(0, SEEK_SET, Error)) {
		goto err;
	}
	if(!WalkItemTree(SaveItemCB, &Param)) {
		goto err;
	}

	Progress.Reset();
	// 需要SaveMedia自己设置Progress的FullScale
	if(!GetOutputMedia()->SaveMedia(bCancel, Error, Progress)) {
		goto err;
	}

	SetDirty(FALSE);
	GetOutputMedia()->SetFormatChanged(FALSE);

	// close input
	GetInputMedia()->CloseMedia();
	
	// make output media new input
	pNewInputMedia = GetOutputMedia();
	if(!pNewInputMedia->SetToRead(Error)) {
		pNewInputMedia->CloseMedia();
		pNewInputMedia = NULL;
		AttachInputMedia(NULL);
		goto err;
	}

	if(!(pNewOutputMedia = pNewInputMedia->CloneMedia(Error))) {
		AttachOutputMedia(NULL);
		goto err;
	}

	AttachOutputMedia(pNewInputMedia);
	AttachInputMedia(pNewOutputMedia);

	return TRUE;
err:
	return FALSE;
}

BOOL CPack::LoadPack(LPCTSTR szPathName, LPCTSTR szExt, CPasswordGetter & PasswordGetter, 
					 BOOL & bCancel, CPackErrors & Error, CProgress & Progress)
{

	ULONGLONG nOffset = 0;
	CStream * pInput = NULL;
	CPackItem * pItem = NULL, *pRoot = NULL;
	ULONGLONG nSize, nReaded;
	pack_item_header_t header;
	CMedia * pInputMedia = NULL;
	CMedia *pOutputMedia = NULL;

	pRoot = this;


	if(!(pInputMedia = CMediaFactory::CreateMediaFromExt(szExt))) {
		Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
		goto err;
	}

	if(!pInputMedia->OpenMedia(CMedia::MODE_READ, szPathName, PasswordGetter, bCancel, Error, Progress)) {
		pInputMedia->CloseMedia();
		pInputMedia = NULL;
		goto err;
	}

	if(!(pOutputMedia = pInputMedia->CloneMedia(Error))) {
		pInputMedia->CloseMedia();
		pInputMedia = NULL;
		goto err;
	}

	pInput = pInputMedia->GetStream();

	if(!pInput->Seek(0, SEEK_SET, Error)) {
		goto err;
	}

	Progress.Reset();
	Progress.SetFullScale(pInputMedia->GetDataSize());
	nReaded = 0;

	while(nReaded < pInputMedia->GetDataSize()) {
		nSize = sizeof(header);
		if(!pInput->Read(&header,nSize, Progress, Error)) {
			goto err;
		}

		if(bCancel) {
			Error.SetError(CPackErrors::PE_CANCELED);
			goto err;
		}


		if(!CPackItem::IsValidItemHeader(header)) {
			Error.SetError(CPackErrors::PE_CORRUPT_DATA);
			goto err;
		}

		if(header.dwType == CPackItem::TYPE_EOD) {
			nSize = (DWORD)header.dwNameSize;
			if(!pInput->Seek(header.dwNameSize, SEEK_CUR, Error)) {
				goto err;
			}
			Progress.IncScale(header.dwNameSize);
			nReaded += header.dwNameSize + sizeof(header);
			if(pRoot) {
				TRACE(_T("load dir EOD %s->%s\n"), pRoot->GetName(), pRoot->GetParent()->GetName());
				pRoot = pRoot->GetParent();
			}
			else {
				Error.SetError(CPackErrors::PE_CORRUPT_DATA);
				goto err;
			}
		} else if(header.dwType == CPackItem::TYPE_FILE) {
			if(!(pItem = CPackItem::CreatePackItemFromMediaFile(pRoot, header, pInput, bCancel, Progress, Error))) {
				goto err;
			}
			nReaded += pItem->GetTotalSize();
			TRACE(_T("load file %s, offset %lld\n"), pItem->GetName(), pItem->GetOffset());
			pRoot->AddChild(pItem);
			SetDirty();
			IncTotalDataSize(pItem->GetTotalSize());
		} else if(header.dwType == CPackItem::TYPE_DIR) {
			if(!(pItem = CPackItem::CreatePackItemFromMediaDir(pRoot, header, pInput, bCancel, Progress, Error))) {
				goto err;
			}
			nReaded += pItem->GetTotalSize() - pItem->GetEODSize();
			TRACE(_T("load dir %s, %s->%s, offset %lld\n"), pItem->GetName(), pRoot->GetName(), pItem->GetName(), pItem->GetOffset());
			pRoot->AddChild(pItem);
			SetDirty();
			pRoot = pItem;
			IncTotalDataSize(pItem->GetTotalSize());
		}
	}

	if(pRoot != this) {
		Error.SetError(CPackErrors::PE_CORRUPT_DATA);
		goto err;
	}

	
	AttachInputMedia(pInputMedia);
	AttachOutputMedia(pOutputMedia);
	SetDirty(FALSE);

	return TRUE;

err:
	AttachOutputMedia(pOutputMedia);
	AttachInputMedia(pInputMedia);
	SetDirty(FALSE);
	return FALSE;

}
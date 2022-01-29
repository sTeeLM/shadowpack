// FileManager.cpp: 实现文件
//

#include "pch.h"
#include "resource.h"
#include "ShadowPack.h"
#include "PackManager.h"
#include "PackUtils.h"
#include "ShadowPack.h"
#include "ShadowPackDlg.h"
// CPackManager

IMPLEMENT_DYNAMIC(CPackManager, CListCtrl)

CPackManager::CPackManager():
	m_pMedia(NULL),
	m_nTotalSize(0),
	m_bDirty(FALSE),
	m_bUseDiskCache(FALSE),
	m_strCachePath(_T("")),
	m_pParent(NULL),
	m_DragDropTarget(this)
{

}

CPackManager::~CPackManager()
{
}

void CPackManager::Initialize(CWnd* pParent, UINT nID)
{
	CRect Rect;
	CString strTemp;
	INT nColInterval;
	DWORD dwStyle;
	CConfigManager::CONFIG_VALUE_T val;
	BOOL bUseSystemCache = FALSE;

	m_pParent = pParent;

	if (theApp.m_Config.GetConfig(_T("pack"), _T("pack_use_hd_cache"), val)) {
		m_bUseDiskCache = val.n8;
		if (theApp.m_Config.GetConfig(_T("pack"), _T("pack_use_system_cache"), val)) {
			bUseSystemCache = val.n8;
		}
		if (theApp.m_Config.GetConfig(_T("pack"), _T("pack_custom_cache_path"), val)) {
			m_strCachePath = val.str;
			free(val.str);
		}
	}

	if (bUseSystemCache) {
		m_strCachePath = CPackUtils::GetTempPath();
	}

	SubclassDlgItem(nID, pParent);

	GetClientRect(&Rect);
	nColInterval = Rect.Width() / 8;


	strTemp.LoadString(IDS_COLUMN_NAME);
	InsertColumn(0, (LPCTSTR)strTemp, LVCFMT_LEFT, nColInterval);
	strTemp.LoadString(IDS_COLUMN_TIME);
	InsertColumn(1, (LPCTSTR)strTemp, LVCFMT_LEFT, nColInterval * 2);
	strTemp.LoadString(IDS_COLUMN_SIZE);
	InsertColumn(2, (LPCTSTR)strTemp, LVCFMT_LEFT, nColInterval * 2);

	dwStyle = GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	SetExtendedStyle(dwStyle);
	m_DragDropTarget.Register(this);
	m_DragDropTarget.SetParent(pParent);
}

BOOL CPackManager::MediaAttached()
{
	return m_pMedia != NULL;
}

void CPackManager::AttachMedia(CMediaBase* pMedia)
{
	m_pMedia = pMedia;
}

BOOL CPackManager::LoadMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPasswordGetterBase& Password, CPackErrors& Errors)
{
	CStreamBase* pStream = NULL;
	CPackItem* pPackItem = NULL;

	// load media meta
	Errors.SetError(CPackErrors::PE_OK);

	if (!m_pMedia || !m_pMedia->LoadMedia(szFilePath, Password, Progress, Errors)) {
		if (!m_pMedia)
			Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}

	m_strMediaPath = szFilePath;

	// getstream
	pStream = m_pMedia->GetStream();
	if (!pStream) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}
	if (!pStream->Seek(0, CStreamBase::STREAM_SEEK_SET, Errors)) {
		goto err;
	}

	 // load item into list
	Progress.Reset(IDS_LOAD_DATA);
	Progress.SetFullScale(m_pMedia->GetMediaUsedBytes());

	while ((m_pMedia->GetMediaUsedBytes() - m_nTotalSize) > 0) {
		if (Progress.IsCanceled(Errors)) {
			goto err;
		}
		if ((pPackItem = CPackItem::CreateItemFromStream(pStream, m_strCachePath, m_bUseDiskCache, Progress, Errors)) != NULL) {
			if (!InsertPackItem(pPackItem, Errors)) {
				CPackItem::Free(pPackItem);
				goto err;
			}
			m_nTotalSize += pPackItem->GetTotalSize();
		}
		else {
			goto err;
		}
	}

	ASSERT(m_nTotalSize == m_pMedia->GetMediaUsedBytes());

	return TRUE;
 err:
	ClearAllItems();
	return FALSE;
 }

BOOL CPackManager::SaveMedia(CProgressBase& Progress, CPackErrors& Errors)
{
	CStreamBase* pStream = NULL;
	CPackItem* pItem = NULL;
	INT nItems;

	Errors.SetError(CPackErrors::PE_OK);

	// write media
	if (!m_pMedia) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}

	pStream = m_pMedia->GetStream();
	if (!pStream) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}
	if (!pStream->Seek(0, CStreamBase::STREAM_SEEK_SET, Errors)) {
		goto err;
	}

	Progress.Reset(IDS_SAVE_DATA);
	Progress.SetFullScale(GetTotalSize());

	nItems = CListCtrl::GetItemCount();
	for (INT i = 0; i < nItems; i++) {
		if (Progress.IsCanceled(Errors)) {
			goto err;
		}
		pItem = (CPackItem*)CListCtrl::GetItemData(i);
		if (!pItem) {
			Errors.SetError(CPackErrors::PE_INTERNAL);
			goto err;
		}
		if (!pItem->WriteItemToStream(pStream, Progress, Errors)) {
			goto err;
		}
	}

	// save media
	if (!m_pMedia->SaveMedia(m_strMediaPath, Progress, Errors)) {
		goto err;
	}
	m_bDirty = FALSE;
	return TRUE;
err:
	return FALSE;
}

 void CPackManager::DettachMedia()
{
	 ClearAllItems();
	 m_pMedia->CloseMedia();
	 delete m_pMedia;
	 m_pMedia = NULL;
	 m_bDirty = FALSE;
}

UINT CPackManager::GetSelectedItemCnt()
{
	return CListCtrl::GetSelectedCount();
}

CString CPackManager::GetFirstSelectedItemName()
{
	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	CPackItem* pItem = NULL;
	INT nItem;
	if (pos != NULL) {
		nItem = CListCtrl::GetNextSelectedItem(pos);
		pItem = (CPackItem*)CListCtrl::GetItemData(nItem);
		if (pItem)
			return pItem->GetName();
	}
	return _T("");
}

BOOL CPackManager::ExportFirstSelectedItemToFile(LPCTSTR szItemPath, CProgressBase& Progress, CPackErrors& Errors)
{
	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	CPackItem* pItem = NULL;
	INT nItem;
	if (pos != NULL) {
		nItem = CListCtrl::GetNextSelectedItem(pos);
		pItem = (CPackItem*)CListCtrl::GetItemData(nItem);
		if (pItem) {
			Progress.Reset(IDS_EXPORT_DATA);
			Progress.SetFullScale(pItem->GetTotalSize());
			return pItem->WriteItemToFile(szItemPath, Progress, Errors);
		}
	}
	return FALSE;
}

BOOL CPackManager::ExportSelectedItemsToDir(LPCTSTR szItemPath, CProgressBase& Progress, CPackErrors& Errors)
{
	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	CPackItem* pItem = NULL;
	CString strItemPath;
	INT nItem;
	ULONGLONG nSize = 0;

	// cal total data size!
	while (pos != NULL) {
		nItem = CListCtrl::GetNextSelectedItem(pos);
		pItem = (CPackItem*)CListCtrl::GetItemData(nItem);
		if (pItem) {
			nSize += pItem->GetDataSize();
		} else {
			ASSERT(FALSE);
		}
	}
	Progress.Reset(IDS_EXPORT_DATA);
	Progress.SetFullScale(nSize);

	pos = CListCtrl::GetFirstSelectedItemPosition();

	while (pos != NULL) {
		nItem = CListCtrl::GetNextSelectedItem(pos);
		pItem = (CPackItem*)CListCtrl::GetItemData(nItem);
		if (pItem) {
			strItemPath = szItemPath;
			if (strItemPath.Right(1).Compare(_T("\\")))
				strItemPath += _T("\\");
			strItemPath += pItem->GetName();
			if (!pItem->WriteItemToFile(strItemPath, Progress, Errors)) {
				AfxMessageBox(Errors.ToString());
				return FALSE;
			}
		} else {
			ASSERT(FALSE);
		}
	}
	return TRUE;
}

BOOL CPackManager::AddItemFromFileMulti(const CArray<CString> & aryFileNames, CProgressBase& Progress, CPackErrors& Errors)
{
	ULONGLONG nSize, nTotalDataSize = 0;;
	for (INT i = 0; i < aryFileNames.GetCount(); i++) {
		if (!CPackUtils::IsFile(aryFileNames[i])) {
			Errors.SetError(CPackErrors::PE_NOT_FILE, CPackUtils::GetPathName(aryFileNames[i]));
			return FALSE;
		}
		else if (ItemExist(CPackUtils::GetPathName(aryFileNames[i]))) {
			Errors.SetError(CPackErrors::PE_EXISTED, CPackUtils::GetPathName(aryFileNames[i]));
			return FALSE;
		}
		else if (!CPackUtils::GetFileSize(aryFileNames[i], nSize)) {
			Errors.SetError(CPackErrors::PE_IO, aryFileNames[i], CPackUtils::GetLastError());
		}
		else {
			nTotalDataSize += nSize;
		}
	}

	if (nTotalDataSize > m_pMedia->GetMediaTotalBytes() - m_pMedia->GetMediaUsedBytes()) {
		Errors.SetError(CPackErrors::PE_OVER_CAPICITY);
		return FALSE;
	}

	Progress.Reset(IDS_READ_FILE);
	Progress.SetFullScale(nTotalDataSize);
	for (INT i = 0; i < aryFileNames.GetCount(); i++) {
		if (!AddItemFromFile(aryFileNames[i], Progress, Errors)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CPackManager::AddItemFromFile(LPCTSTR szItemPath, CProgressBase& Progress, CPackErrors& Errors)
{
	CPackItem* pPackItem = NULL;
	ULONGLONG nSize;
	if (ItemExist(CPackUtils::GetPathName(szItemPath))) {
		Errors.SetError(CPackErrors::PE_EXISTED, CPackUtils::GetPathName(szItemPath));
	} else if (CPackUtils::GetFileSize(szItemPath, nSize)) {
		if (nSize <= m_pMedia->GetMediaTotalBytes() - m_pMedia->GetMediaUsedBytes()) {
			pPackItem = CPackItem::CreateItemFromFile(szItemPath, m_strCachePath, m_bUseDiskCache, Progress, Errors);
			if (pPackItem) {
				if (pPackItem->GetTotalSize() > (m_pMedia->GetMediaTotalBytes() - m_nTotalSize)) {
					Errors.SetError(CPackErrors::PE_OVER_CAPICITY);
					CPackItem::Free(pPackItem);
				}
				else if (!InsertPackItem(pPackItem, Errors)) {
					CPackItem::Free(pPackItem);
				}
				else {
					m_nTotalSize += pPackItem->GetTotalSize();
					m_bDirty = TRUE;
					return m_pMedia->SetMediaUsedBytes(m_nTotalSize, Errors);
				}
			}
		} else {
			Errors.SetError(CPackErrors::PE_OVER_CAPICITY);
		}
	} else {
		Errors.SetError(CPackErrors::PE_IO, szItemPath, CPackUtils::GetLastError());
	}
	return FALSE;
}

void CPackManager::DeleteSelectedItems()
{
	POSITION pos = NULL;
	CPackItem* pItem = NULL;
	INT nItem;
	CPackErrors Errors;

again:
	pos = CListCtrl::GetFirstSelectedItemPosition();

	while (pos != NULL) {
		nItem = CListCtrl::GetNextSelectedItem(pos);
		pItem = (CPackItem*)CListCtrl::GetItemData(nItem);
		if (pItem) {
			m_nTotalSize -= pItem->GetTotalSize();
			CPackItem::Free(pItem);
		} else {
			ASSERT(FALSE);
		}
		m_bDirty = TRUE;
		m_pMedia->SetMediaUsedBytes(m_nTotalSize, Errors);
		CListCtrl::DeleteItem(nItem);
		goto again;
	}

}

void CPackManager::ClearAllItems()
{
	CPackItem* pItem = NULL;
	INT nItem = CListCtrl::GetItemCount();
	CPackErrors Errors;
	while (nItem) {
		pItem = (CPackItem*)CListCtrl::GetItemData(nItem - 1);
		if (pItem) {
			CPackItem::Free(pItem);
		}
		nItem--;
	}
	m_nTotalSize = 0;
	m_bDirty = TRUE;
	m_pMedia->SetMediaUsedBytes(m_nTotalSize, Errors);
	CListCtrl::DeleteAllItems();
}

CMediaBase* CPackManager::GetMedia()
{
	return m_pMedia;
}

BOOL CPackManager::IsDirty()
{
	return m_bDirty || (m_pMedia && m_pMedia->IsMediaDirty());
}

ULONGLONG CPackManager::GetTotalSize()
{
	return m_nTotalSize;
}

BOOL CPackManager::InsertPackItem(CPackItem* pItem, CPackErrors& Errors)
{
	LVITEM lvi;
	INT nIndex;
	CString strItem;

	nIndex = CListCtrl::GetItemCount();
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = nIndex;
	lvi.iSubItem = 0;
	strItem = pItem->GetName();
	lvi.pszText = (LPTSTR)(LPCTSTR)strItem;
	if (InsertItem(&lvi) < 0)
		goto err;

	strItem = pItem->FormatDateItme();
	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)(LPCTSTR)strItem;
	if(!SetItem(&lvi)) {
		DeleteItem(nIndex);
		goto err;
	}

	strItem.Format(_T("%d"), pItem->GetDataSize());
	lvi.iSubItem = 2;
	lvi.pszText = (LPTSTR)(LPCTSTR)strItem;
	if (!SetItem(&lvi)) {
		DeleteItem(nIndex);
		goto err;
	}

	if (!SetItemData(nIndex, (DWORD_PTR)pItem)) {
		DeleteItem(nIndex);
		goto err;
	}

	return TRUE;
err:
	return FALSE;
}

BOOL CPackManager::ItemExist(LPCTSTR szItemName)
{
	CPackItem* pItem = NULL;
	INT nItem = CListCtrl::GetItemCount();
	for (INT i = 0; i < nItem; i++) {
		pItem = (CPackItem*)CListCtrl::GetItemData(i);
		if (pItem) {
			if (!pItem->GetName().CompareNoCase(szItemName)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}



CPackManager::CPackItem* CPackManager::CPackItem::CreateItemFromStream(CStreamBase* pStream, LPCTSTR szCacheDir, BOOL bUseFileCache, CProgressBase& Progress, CPackErrors& Errors)
{
	PACK_ITEM_T Header;
	CPackItem* pPackItem = NULL;
	LPBYTE* pData = NULL;
	CHAR NameBuffer[MAX_PATH];
	CString strTempFileName;
	// read header
	if (pStream->Read((LPBYTE)&Header, sizeof(Header), Progress, Errors)) {
		if (Header.dwSign == PACK_ITEM_SIGN && Header.nNameSize < MAX_PATH) {
			pPackItem = new (std::nothrow) CPackItem();
			if(pPackItem) {
				// read name
				if (pStream->Read((LPBYTE)NameBuffer, Header.nNameSize, Progress, Errors)) {
					NameBuffer[Header.nNameSize] = 0;
					CA2CT szr(NameBuffer);
					pPackItem->m_strFileName = szr;
					pPackItem->m_nSize = Header.nDataSize;
					pPackItem->m_Time = Header.nTime;
					// read data
					if (pPackItem->m_nSize) {
						if (bUseFileCache) {
							if ((pPackItem->m_hFile = CPackUtils::CreateTempFile(szCacheDir, strTempFileName)) != INVALID_HANDLE_VALUE) {
								if (Stream2Handle(pStream, pPackItem->m_hFile, strTempFileName, Header.nDataSize, Progress, Errors)) {
									return pPackItem;
								}
							} else {
								Errors.SetError(CPackErrors::PE_IO, CPackUtils::GetLastError());
							}
						} else {
							if ((pPackItem->m_pData = new (std::nothrow) BYTE[Header.nDataSize]) != NULL) {
								if (pStream->Read((LPBYTE)pPackItem->m_pData, Header.nDataSize, Progress, Errors)) {
									return pPackItem;
								}
							}
							else {
								Errors.SetError(CPackErrors::PE_NOMEM);
							}
						}
					} else {
						return pPackItem;
					}
				} else {
					Errors.SetError(CPackErrors::PE_CORRUPT_DATA);
				}
				CPackItem::Free(pPackItem);
			} else {
				Errors.SetError(CPackErrors::PE_NOMEM);
			}
		} else {
			Errors.SetError(CPackErrors::PE_CORRUPT_DATA);
		}
	}
	return NULL;
}

CPackManager::CPackItem* CPackManager::CPackItem::CreateItemFromFile(LPCTSTR szFilePath, LPCTSTR szCacheDir, BOOL bUseFileCache, CProgressBase& Progress, CPackErrors& Errors)
{
	CFileException exFile;
	CFile file;
	ULONGLONG nLength;
	CPackItem* pPackItem = NULL;
	CString strTempFileName;
	if (file.Open(szFilePath, CFile::modeRead | CFile::shareDenyWrite, &exFile)) {
		nLength = file.GetLength();
		if (nLength > 4294967296L) {
			file.Close();
			Errors.SetError(CPackErrors::PE_TOO_LARGE_FILE, szFilePath);
		}
		else {
			pPackItem = new (std::nothrow) CPackItem();
			if (pPackItem) {
				pPackItem->m_strFileName = file.GetFileName();
				pPackItem->m_nSize = (ULONG)nLength;
				pPackItem->m_Time = CTime::GetCurrentTime();
				if (pPackItem->m_nSize) {
					// read data
					if (bUseFileCache) {
						if ((pPackItem->m_hFile = CPackUtils::CreateTempFile(szCacheDir, strTempFileName)) != INVALID_HANDLE_VALUE) {
							if (CFile2Handle(&file, pPackItem->m_hFile, strTempFileName, (UINT)nLength, Progress, Errors)) {
								return pPackItem;
							}
							else {
								Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
							}
						}
						else {
							Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
						}
					}
					else {
						if ((pPackItem->m_pData = new (std::nothrow) BYTE[pPackItem->m_nSize])) {
							if (CFile2Memory(&file, pPackItem->m_pData, (UINT)nLength, Progress, Errors)) {
								return pPackItem;
							}
						}
						else {
							Errors.SetError(CPackErrors::PE_NOMEM);
						}
						CPackItem::Free(pPackItem);
					}
				}
				else {
					return pPackItem;
				}
			}
			else {
				Errors.SetError(CPackErrors::PE_NOMEM);
			}
		}
		file.Close();
	} else {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
	}
	return NULL;
}

void CPackManager::CPackItem::Free(CPackItem* pItem)
{
	if (pItem->m_pData) {
		delete[] pItem->m_pData;
		pItem->m_pData = NULL;
	}
	if (pItem->m_hFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(pItem->m_hFile);
		pItem->m_hFile = INVALID_HANDLE_VALUE;
	}
	delete pItem;
}

BOOL CPackManager::CPackItem::WriteItemToStream(CStreamBase* pStream, CProgressBase& Progress, CPackErrors& Errors)
{
	PACK_ITEM_T Header;
	struct tm Tm;
	CT2CA sza((LPCTSTR)m_strFileName);
	Header.dwSign = PACK_ITEM_SIGN;
	Header.nDataSize = m_nSize;
	Header.nNameSize = (UINT)strlen((LPSTR)sza);
	Header.nTime = mktime(m_Time.GetLocalTm(&Tm));
	if (pStream->Write(&Header, sizeof(Header), Progress, Errors)) {
		if (pStream->Write((LPSTR)sza, Header.nNameSize, Progress, Errors)) {
			if (m_nSize) {
				if (m_hFile != INVALID_HANDLE_VALUE) {
					if (INVALID_SET_FILE_POINTER != ::SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN)) {
						return Handle2Stream(m_hFile, m_strFileName, pStream, Header.nDataSize, Progress, Errors);
					}
					else {
						Errors.SetError(CPackErrors::PE_IO, m_strFileName, CPackUtils::GetLastError());
					}
				}
				else if (pStream->Write(m_pData, Header.nDataSize, Progress, Errors)) {
					return TRUE;
				}
			}
			else {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CPackManager::CPackItem::WriteItemToFile(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	CFileException exFile;
	CFile file;
	BOOL bRet = FALSE;
	if (file.Open(szFilePath, CFile::modeWrite | CFile::modeCreate, &exFile)) {
		if (m_nSize) {
			if (m_hFile != INVALID_HANDLE_VALUE) {
				if (INVALID_SET_FILE_POINTER != ::SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN)) {
					bRet = Handle2CFile(m_hFile, m_strFileName, &file, m_nSize, Progress, Errors);
				} else {
					Errors.SetError(CPackErrors::PE_IO, m_strFileName, CPackUtils::GetLastError());
				}
			}
			else if (m_pData) {
				bRet = Memory2CFile(m_pData, &file, m_nSize, Progress, Errors);
			}
		}
		else {
			bRet = TRUE;
		}
		file.Close();
	} else {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
	}
	
	return bRet;
}

BYTE CPackManager::CPackItem::ReadWriteBuffer[READ_WRITE_BUFFER_SIZE];

BOOL CPackManager::CPackItem::Stream2Handle(CStreamBase* pStream, HANDLE hFile, LPCTSTR szHandleFileName, UINT nSize, CProgressBase& Progress, CPackErrors& Errors)
{	
	UINT nWrite;
	DWORD dwWrited;
	
	while (nSize) {
		nWrite = nSize > READ_WRITE_BUFFER_SIZE ? READ_WRITE_BUFFER_SIZE : nSize;
		if (!pStream->Read(ReadWriteBuffer, nWrite, Progress, Errors)) {
			break;
		}
		dwWrited = 0;
		if (!::WriteFile(hFile, ReadWriteBuffer, nWrite, &dwWrited, NULL) || dwWrited != nWrite) {
			Errors.SetError(CPackErrors::PE_IO, szHandleFileName, CPackUtils::GetLastError());
			break;
		}
		nSize -= nWrite;
		if (Progress.IsCanceled(Errors)) {
			break;
		}
	}
	return nSize == 0;
}

BOOL CPackManager::CPackItem::Handle2Stream(HANDLE hFile, LPCTSTR szHandleFileName, CStreamBase* pStream, UINT nSize, CProgressBase& Progress, CPackErrors& Errors)
{
	UINT nWrite;
	DWORD dwWrited;
	while (nSize) {
		nWrite = nSize > READ_WRITE_BUFFER_SIZE ? READ_WRITE_BUFFER_SIZE : nSize;
		dwWrited = 0;
		if (!::ReadFile(hFile, ReadWriteBuffer, nWrite, &dwWrited, NULL) || dwWrited != nWrite) {
			Errors.SetError(CPackErrors::PE_IO, szHandleFileName, CPackUtils::GetLastError());
			break;
		}
		if (!pStream->Write(ReadWriteBuffer, nWrite, Progress, Errors)) {
			break;
		}
		nSize -= nWrite;
		if (Progress.IsCanceled(Errors)) {
			break;
		}
	}
	return nSize == 0;
}

BOOL CPackManager::CPackItem::CFile2Handle(CFile* file, HANDLE hFile, LPCTSTR szHandleFileName, UINT nSize, CProgressBase& Progress, CPackErrors& Errors)
{
	UINT nWrite;
	DWORD dwWrited;
	while (nSize) {
		nWrite = nSize > READ_WRITE_BUFFER_SIZE ? READ_WRITE_BUFFER_SIZE : nSize;
		dwWrited = file->Read(ReadWriteBuffer, nWrite);
		if (dwWrited != nWrite) {
			Errors.SetError(CPackErrors::PE_IO, file->GetFileName(), CPackUtils::GetLastError());
			break;
		}
		dwWrited = 0;
		if (!::WriteFile(hFile, ReadWriteBuffer, nWrite, &dwWrited, NULL) || dwWrited != nWrite) {
			Errors.SetError(CPackErrors::PE_IO, szHandleFileName, CPackUtils::GetLastError());
			break;
		}
		nSize -= nWrite;
		if (Progress.IsCanceled(Errors)) {
			break;
		}
		Progress.Increase(nWrite);
	}
	return nSize == 0;
}

BOOL CPackManager::CPackItem::Handle2CFile(HANDLE hFile, LPCTSTR szHandleFileName, CFile* file, UINT nSize, CProgressBase& Progress, CPackErrors& Errors)
{
	UINT nWrite;
	DWORD dwWrited;
	while (nSize) {
		nWrite = nSize > READ_WRITE_BUFFER_SIZE ? READ_WRITE_BUFFER_SIZE : nSize;
		dwWrited = 0;
		if (!::ReadFile(hFile, ReadWriteBuffer, nWrite, &dwWrited, NULL) || dwWrited != nWrite) {
			Errors.SetError(CPackErrors::PE_IO, szHandleFileName, CPackUtils::GetLastError());
			break;
		}
		try {
			file->Write(ReadWriteBuffer, nWrite);
		} catch (CFileException* e) {
			Errors.SetError(CPackErrors::PE_IO, file->GetFileName(), CPackUtils::GetLastError(e));
		}
		nSize -= nWrite;
		if (Progress.IsCanceled(Errors)) {
			break;
		}
		Progress.Increase(nWrite);
	}
	return nSize == 0;
}

BOOL CPackManager::CPackItem::CFile2Memory(CFile* file, LPBYTE pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Errors)
{
	UINT nWrite;
	DWORD dwWrited;
	LPBYTE p = pBuffer;
	while (nSize) {
		nWrite = nSize > READ_WRITE_BUFFER_SIZE ? READ_WRITE_BUFFER_SIZE : nSize;
		dwWrited = file->Read(p, nWrite);
		if (dwWrited != nWrite) {
			Errors.SetError(CPackErrors::PE_IO, file->GetFileName(), CPackUtils::GetLastError());
			break;
		}
		nSize -= nWrite;
		p += nWrite;
		if (Progress.IsCanceled(Errors)) {
			break;
		}
		Progress.Increase(nWrite);
	}
	return nSize == 0;
}

BOOL CPackManager::CPackItem::Memory2CFile(LPBYTE pBuffer, CFile* file, UINT nSize, CProgressBase& Progress, CPackErrors& Errors)
{
	UINT nWrite;
	LPBYTE p = pBuffer;
	while (nSize) {
		nWrite = nSize > READ_WRITE_BUFFER_SIZE ? READ_WRITE_BUFFER_SIZE : nSize;
		try {
			file->Write(p, nWrite);
		}
		catch (CFileException* e) {
			Errors.SetError(CPackErrors::PE_IO, file->GetFileName(), CPackUtils::GetLastError(e));
		}
		nSize -= nWrite;
		p += nWrite;
		if (Progress.IsCanceled(Errors)) {
			break;
		}
		Progress.Increase(nWrite);
	}
	return nSize == 0;
}

CString CPackManager::CPackItem::GetName()
{
	return m_strFileName;
}

ULONGLONG CPackManager::CPackItem::GetDataSize()
{
	return m_nSize;
}

ULONGLONG CPackManager::CPackItem::GetTotalSize()
{
	CT2CA sza((LPCTSTR)m_strFileName);

	return strlen((LPSTR)sza) + sizeof(PACK_ITEM_T) + m_nSize;
}

CString CPackManager::CPackItem::FormatDateItme()
{
	return m_Time.Format(_T("%Y-%m-%d %H:%M:%S"));;
}



BEGIN_MESSAGE_MAP(CPackManager, CListCtrl)
END_MESSAGE_MAP()



// CPackManager 消息处理程序

DROPEFFECT CPackManager::CPackItemDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TRACE(_T("OnDragEnter\n"));
	m_bCanDrop = TestDropOK(pDataObject);
	return m_bCanDrop ? DROPEFFECT_COPY : DROPEFFECT_NONE;;
}

BOOL CPackManager::CPackItemDropTarget::TestDropOK(COleDataObject* pDataObject)
{
	HGLOBAL hGlobal = NULL;
	HDROP   hDrop = NULL;
	UINT nDropCount;
	TCHAR szFullFileName[MAX_PATH];
	BOOL bOK = TRUE;

	if (!pDataObject) {
		bOK = FALSE;
		goto finish;
	}

	hGlobal = pDataObject->GetGlobalData(CF_HDROP);
	TRACE(_T("TestDropOK hGlobal = %p\n"), hGlobal);
	if (hGlobal == NULL) {
		bOK = FALSE;
		goto finish;
	}

	hDrop = (HDROP)GlobalLock(hGlobal);
	TRACE(_T("TestDropOK hDrop = %p\n"), hDrop);
	if (hDrop == NULL) {
		bOK = FALSE;
		goto finish;
	}

	nDropCount = DragQueryFile(hDrop, -1, NULL, 0);
	for (int i = 0; i < nDropCount; i++)
	{
		DragQueryFile(hDrop, i, szFullFileName, MAX_PATH);
		TRACE(_T("TestDropOK %s\n"), szFullFileName);
		if (!CPackUtils::IsFile(szFullFileName)) {
			bOK = FALSE;
			break;
		}
	}
finish:
	if (hDrop) {
		DragFinish(hDrop);
	}
	if (hGlobal) {
		GlobalUnlock(hGlobal);
	}
	return bOK;
}

DROPEFFECT CPackManager::CPackItemDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	TRACE(_T("OnDragOver\n"));
	return m_bCanDrop ? DROPEFFECT_COPY : DROPEFFECT_NONE;
}

BOOL CPackManager::CPackItemDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	TRACE(_T("OnDrop\n"));
	HGLOBAL hGlobal;
	HDROP   hDrop;
	UINT nDropCount;
	TCHAR szFullFileName[MAX_PATH];

	hGlobal = pDataObject->GetGlobalData(CF_HDROP);
	TRACE(_T("OnDrop hGlobal = %p\n"), hGlobal);
	if (hGlobal == NULL) {
		return FALSE;
	}

	hDrop = (HDROP)GlobalLock(hGlobal);
	TRACE(_T("OnDrop hDrop = %p\n"), hDrop);
	if (hDrop == NULL) {
		GlobalUnlock(hGlobal);
		return FALSE;
	}

	nDropCount = DragQueryFile(hDrop, -1, NULL, 0);
	dynamic_cast<CShadowPackDlg*>(m_pParent)->m_aryItemPathNames.RemoveAll();
	for (int i = 0; i < nDropCount; i++)
	{
		DragQueryFile(hDrop, i, szFullFileName, MAX_PATH);
		TRACE(_T("Drop %s\n"), szFullFileName);
		dynamic_cast<CShadowPackDlg*>(m_pParent)->m_aryItemPathNames.Add(szFullFileName);
	}
	DragFinish(hDrop);

	if (dynamic_cast<CShadowPackDlg*>(m_pParent)->m_aryItemPathNames.GetCount()) {
		dynamic_cast<CShadowPackDlg*>(m_pParent)->StartThread(&CShadowPackDlg::ThreadAddItem);
	}

	GlobalUnlock(hGlobal);
	return TRUE;
}

void CPackManager::CPackItemDropTarget::OnDragLeave(CWnd* pWnd)
{
	TRACE(_T("OnDragLeave\n"));
	m_bCanDrop = FALSE;
}

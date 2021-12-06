// FileManager.cpp: 实现文件
//

#include "pch.h"
#include "resource.h"
#include "ShadowPack.h"
#include "PackManager.h"
#include "PackUtils.h"

// CPackManager

IMPLEMENT_DYNAMIC(CPackManager, CListCtrl)

CPackManager::CPackManager():
	m_pMedia(NULL),
	m_nTotalSize(0),
	m_bDirty(FALSE)
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
		if (Progress.IsCanceled()) {
			Errors.SetError(CPackErrors::PE_CANCELED);
			goto err;
		}
		if ((pPackItem = CPackItem::CreateItemFromStream(pStream, Progress, Errors)) != NULL) {
			if (!InsertPackItem(pPackItem, Errors)) {
				CPackItem::Free(pPackItem);
				goto err;
			}
			m_nTotalSize += pPackItem->GetTotalSize();
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
		if (Progress.IsCanceled()) {
			Errors.SetError(CPackErrors::PE_CANCELED);
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
	UINT nSize = 0;

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

BOOL CPackManager::AddItemFromFile(LPCTSTR szItemPath, CProgressBase& Progress, CPackErrors& Errors)
{
	CPackItem* pPackItem = NULL;
	ULONGLONG nSize;
	Progress.Reset(IDS_READ_FILE);

	if (ItemExist(CPackUtils::GetPathName(szItemPath))) {
		Errors.SetError(CPackErrors::PE_EXISTED, CPackUtils::GetPathName(szItemPath));
	} else if (CPackUtils::GetFileSize(szItemPath, nSize)) {
		if (nSize <= m_pMedia->GetMediaTotalBytes() - m_pMedia->GetMediaUsedBytes()) {
			Progress.SetFullScale(nSize);
			pPackItem = CPackItem::CreateItemFromFile(szItemPath, Progress, Errors);
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
	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	CPackItem* pItem = NULL;
	INT nItem;
	CPackErrors Errors;

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

UINT CPackManager::GetTotalSize()
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



CPackManager::CPackItem* CPackManager::CPackItem::CreateItemFromStream(CStreamBase* pStream, CProgressBase& Progress, CPackErrors& Errors)
{
	PACK_ITEM_T Header;
	CPackItem* pPackItem = NULL;
	LPBYTE* pData = NULL;
	CHAR NameBuffer[MAX_PATH];

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
						if ((pPackItem->m_pData = new (std::nothrow) BYTE[Header.nDataSize]) != NULL) {
							if (pStream->Read((LPBYTE)pPackItem->m_pData, Header.nDataSize, Progress, Errors)) {
								return pPackItem;
							}
						} else {
							Errors.SetError(CPackErrors::PE_NOMEM);
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

CPackManager::CPackItem* CPackManager::CPackItem::CreateItemFromFile(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	CFileException exFile;
	CFile file;
	ULONGLONG nLength;
	ULONG nRead;
	CPackItem* pPackItem = NULL;
	LPBYTE pBuffer = NULL;
	if (file.Open(szFilePath, CFile::modeRead | CFile::shareDenyWrite, &exFile)) {
		nLength = file.GetLength();
		pPackItem = new (std::nothrow) CPackItem();
		if (pPackItem) {
			pPackItem->m_strFileName = file.GetFileName();
			pPackItem->m_nSize = (ULONG)nLength;
			pPackItem->m_Time = CTime::GetCurrentTime();
			if (pPackItem->m_nSize) {
				// read data
				pPackItem->m_pData = new (std::nothrow) BYTE[pPackItem->m_nSize];
				if (pPackItem->m_pData) {
					pBuffer = pPackItem->m_pData;
					while (nLength) {
						if (Progress.IsCanceled()) {
							Errors.SetError(CPackErrors::PE_CANCELED);
							break;
						}
						nRead = READ_WRITE_BUFFER_SIZE;
						nRead = nRead > nLength ? (ULONG)nLength : nRead;
						if (file.Read(pBuffer, nRead) != nRead) {
							Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
							break;
						}
						nLength -= nRead;
						pBuffer += nRead;
						Progress.Increase(nRead);
					}
					if (nLength == 0) {
						return pPackItem;
					}
				} else {
					Errors.SetError(CPackErrors::PE_NOMEM);
				}
			} else {
				return pPackItem;
			}
			CPackItem::Free(pPackItem);
		} else {
			Errors.SetError(CPackErrors::PE_NOMEM);
		}
		file.Close();
	} else {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
	}
	return NULL;
}

void CPackManager::CPackItem::Free(CPackItem* pItem)
{
	delete pItem;
}

BOOL CPackManager::CPackItem::WriteItemToStream(CStreamBase* pStream, CProgressBase& Progress, CPackErrors& Errors)
{
	PACK_ITEM_T Header;
	struct tm Tm;
	CT2CA sza((LPCTSTR)m_strFileName);
	Header.dwSign = PACK_ITEM_SIGN;
	Header.nDataSize = m_nSize;
	Header.nNameSize = strlen((LPSTR)sza);
	Header.nTime = mktime(m_Time.GetLocalTm(&Tm));
	if (pStream->Write(&Header, sizeof(Header), Progress, Errors)) {
		if (pStream->Write((LPSTR)sza, Header.nNameSize, Progress, Errors)) {
			if (pStream->Write(m_pData, Header.nDataSize, Progress, Errors)) {
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
	ULONGLONG nLength;
	ULONG nWrite;
	LPBYTE pBuffer = NULL;
	if (file.Open(szFilePath, CFile::modeWrite | CFile::modeCreate, &exFile)) {
		nLength = m_nSize;
		pBuffer = m_pData;
		while (nLength) {
			nWrite = READ_WRITE_BUFFER_SIZE;
			nWrite = nWrite > (ULONG) nLength ? (ULONG)nLength : nWrite;

			try {
				file.Write(pBuffer, nWrite);
			} catch (CFileException* e) {
				Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
				break;
			}
			nLength -= nWrite;
			pBuffer += nWrite;
			Progress.Increase(nWrite);
		}
		file.Close();
		if (nLength == 0)
			return TRUE;
	} else {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastError());
	}

	return FALSE;
}

CString CPackManager::CPackItem::GetName()
{
	return m_strFileName;
}

UINT CPackManager::CPackItem::GetDataSize()
{
	return m_nSize;
}

UINT CPackManager::CPackItem::GetTotalSize()
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


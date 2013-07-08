#include "StdAfx.h"
#include "PackItem.h"
#include "Resource.h"
#include <string>

#define PACKAGE_ITEM_SIG 0xCCDDEEFF
#define MAX_PACK_FILE_LENGTH (200*1024*1024) // 200M

CPackItem::CPackItem(void)
{
	memset(&m_Header, 0, sizeof(m_Header));
	m_Header.dwSignature = PACKAGE_ITEM_SIG;
	m_szName = _T("");
	m_pData  = NULL;
}

CPackItem::~CPackItem(void)
{
	if(NULL != m_pData) {
		free(m_pData);
		m_pData = NULL;
	}
}

DWORD CPackItem::GetTotalSize() const
{
	return m_Header.dwNameSize + m_Header.dwDataSize + sizeof(m_Header) + 1;
}

const CString & CPackItem::GetName() const
{
	return m_szName;
}

BOOL CPackItem::IsValidHeader(const PackItemHeader & header)
{
	if(header.dwSignature != PACKAGE_ITEM_SIG)
		return FALSE;

	if(header.dwNameSize < 1) // null name not allowed!
		return FALSE;

	return TRUE;
}

CPackItem * CPackItem::CreatePackItemFromFile(const CString & szItemName, const CString & szFilePath, CString & szError)
{
	CPackItem * pRet = NULL;
	CFile fIn;
	ULONGLONG uLength = 0;
	BOOL bRet = FALSE;
	TCHAR szMessage[1024];

	pRet = new CPackItem();

	pRet->m_szName = szItemName;
	{ // set name size
		CT2CA szName(szItemName);
		TRACE("item name is %s\n", szName);
		std::string strName = szName;
		pRet->m_Header.dwNameSize = strName.size();
	}

	TRY {
		fIn.Open(szFilePath,CFile::modeRead);
		uLength = fIn.GetLength();
		if(uLength > MAX_PACK_FILE_LENGTH) {
			//szError = _T("too large to pack!");
			szError.LoadString(IDS_ERROR_TOO_LARGE_DATA);
		} else {
			if(uLength > 0) {
				pRet->m_pData = (PBYTE)malloc((size_t)uLength);
				if(NULL != pRet->m_pData) {
					if(uLength == fIn.Read(pRet->m_pData, uLength)) {
						pRet->m_Header.dwDataSize = uLength;
						bRet = TRUE;
					} else {
						//szError = _T("read file error!");
						szError.LoadString(IDS_ERROR_READ_FILE);
					}
				} else {
					//szError = _T("no memory!");
					szError.LoadString(IDS_ERROR_INTERNAL);
				}
			} else {
				bRet = TRUE;
			}
		}
		fIn.Close();
	} CATCH (CFileException, pex) {
		pex->GetErrorMessage(szMessage, 1024);
		szError = szMessage;
		fIn.Close();
	} END_CATCH

	if(!bRet) {
		if(NULL != pRet) {
			delete pRet;
			pRet = NULL;
		}
	}
	return pRet;
}

CPackItem * CPackItem::CreatePackItemFromMemory(const PackItemHeader * pData, size_t size, CString & szError)
{
	CPackItem * pRet = NULL;
	size_t name_len = 0;
	if(size < sizeof(PackItemHeader) + 1 || NULL == pData) {
		//szError = _T("invalid header!");
		szError.LoadString(IDS_ERROR_CORRUPT_DATA);
		goto err_invalid_header;
	}

	if(!IsValidHeader(*pData)) {
		//szError = _T("invalid header!");
		szError.LoadString(IDS_ERROR_CORRUPT_DATA);
		goto err_invalid_header;
	}

	if(pData->dwDataSize + pData->dwNameSize + 1 > size) {
		//szError = _T("invalid header!");
		szError.LoadString(IDS_ERROR_CORRUPT_DATA);
		goto err_invalid_header;
	}

	if(*((PBYTE)pData + sizeof(PackItemHeader) + pData->dwNameSize) != '\0') {
		//szError = _T("invalid header!");
		szError.LoadString(IDS_ERROR_CORRUPT_DATA);
		goto err_invalid_header;
	}

	pRet = new CPackItem();

	memcpy(&pRet->m_Header, pData, sizeof(pRet->m_Header));

	// read name
	{
		CA2CT szName((char *)( (PBYTE)pData + sizeof(PackItemHeader)) );
		TRACE(_T("item name is %s\n"), szName);
		pRet->m_szName = szName;
		name_len = strlen((char *)( (PBYTE)pData + sizeof(PackItemHeader)) );
	}

	if(name_len != pData->dwNameSize) {
		//szError = _T("invalid data!");
		szError.LoadString(IDS_ERROR_CORRUPT_DATA);
		goto err_invalid_data;
	}

	if(pData->dwNameSize != 0) {
		pRet->m_pData = (PBYTE)malloc(pData->dwDataSize);
		if(pRet->m_pData == 0) {
			//szError = _T("out of memory!");
			szError.LoadString(IDS_ERROR_INTERNAL);
			goto err_out_mem;
		}
		memcpy(pRet->m_pData, (PBYTE)pData + sizeof(PackItemHeader) + pData->dwNameSize + 1, pData->dwDataSize);
	}

	return pRet;

err_invalid_data:
err_out_mem:
err_invalid_header:
	if(NULL != pRet) {
		delete pRet;
		pRet = NULL;
	}
	return pRet;
}

BOOL CPackItem::ExportDataToFile(const CString & szItemName, const CString & szFilePath, CString & szError)
{
	CFile fOut;
	TCHAR szMessage[1024];
	BOOL bRet = FALSE;
	TRY {
		fOut.Open(szFilePath,CFile::modeWrite|CFile::modeCreate);
		if(m_pData != NULL && m_Header.dwDataSize > 0) {
			fOut.Write(m_pData, m_Header.dwDataSize);
		}
		fOut.Close();
		bRet = TRUE;
	} CATCH (CFileException, pex) {
		pex->GetErrorMessage(szMessage, 1024);
		szError = szMessage;
		fOut.Close();
	} END_CATCH

	return bRet;
}

BOOL CPackItem::ExportDataToMemory(PBYTE pBuffer, size_t size, CString & szError)
{
	
	if(size < sizeof(PackItemHeader) + m_Header.dwDataSize + m_Header.dwNameSize + 1 || NULL == pBuffer) {
		//szError = _T("invalid header!");
		szError.LoadString(IDS_ERROR_CORRUPT_DATA);
		goto err_invalid_header;
	}

	// copy header
	memcpy(pBuffer, &m_Header, sizeof(m_Header));
	
	// copy name
	{
		CT2CA szName(m_szName);
		TRACE("item name is %s\n", szName);
		std::string strName = szName;
		memcpy(pBuffer + sizeof(m_Header), strName.c_str(), m_Header.dwNameSize);
		pBuffer[sizeof(m_Header) + m_Header.dwNameSize] = 0;
	}

	//copy data, null data allowed
	if(m_Header.dwDataSize != 0 && m_pData != NULL) {
		memcpy(pBuffer + sizeof(m_Header) + m_Header.dwNameSize + 1, m_pData, m_Header.dwDataSize);
	}

	return TRUE;

err_invalid_header:
	return FALSE;
}
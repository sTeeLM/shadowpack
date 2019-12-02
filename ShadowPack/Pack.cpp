#include "StdAfx.h"
#include "Pack.h"
#include "Resource.h"

#include "openssl\aes.h"
#include "openssl\cast.h"
#include "openssl\blowfish.h"
#include "openssl\md5.h"

#include <list>

TCHAR *NoneExts[] = {
	_T("*"),
	NULL
};

TCHAR *RawPPExts[] = {
	_T("BMP"),
	_T("PNG"),
	_T("TIFF"),
	_T("TGA"),
	NULL
};

TCHAR *JStegExts[] = {
	_T("JPEG"),
	_T("JPG"),
	NULL
};

CPack::PackHandler CPack::m_Handler[3] = 
{
	{
		_T("Default"),
		PF_NONE,
		NoneExts,
		DefaultReadImage, /* default */
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	},
	{
		_T("RawPP"),
		PF_RAWPP,
		RawPPExts,
		RawPPReadImage,
		RawPPWriteImage,
		RawPPCanSetFormat,
		RawPPSetFormat,
		RawPPGetSaveFilter,
		RawPPSaveDefaultExt
	},
	{
		_T("JSteg"),
		PF_JSTEG,
		JStegExts,
		JStegReadImage,
		JStegWriteImage,
		JStegCanSetFormat,
		JStegSetFormat,
		JStegGetSaveFilter,
		JStegSaveDefaultExt
	}
};

CPack::CPack(void)
{
	m_Header.dwFormat = PF_NONE;
	m_Header.dwEncryptType = EM_NONE;
	m_Header.dwSignature = PACKAGE_HEADER_SIG;
	m_Header.dwCapicity = 0;
	m_Header.dwCount = 0;
	m_PackItemList.RemoveAll();
	m_szPassword = _T("");
	m_bIsDirty = FALSE;
	m_szFilter = _T("");
	m_szExt = _T("");
}

CPack::~CPack(void)
{
	Clear();
}



BOOL CPack::IsValidHeader(const PackHeader & header)
{
	if(header.dwSignature == PACKAGE_HEADER_SIG) {
		if(header.dwEncryptType >= EM_NONE && header.dwEncryptType <= EM_CAST) {
			if(header.dwFormat == PF_RAWPP ||  header.dwFormat == PF_JSTEG) {
				if(header.dwFormatParam <= 3 && header.dwFormatParam >= 1) {
					if(header.dwDataSize != 0 && header.dwCount != 0) {
						if(header.dwDataSize > sizeof(CPackItem::PackItemHeader)) {
							return TRUE;
						}
					} else if(header.dwDataSize == 0 && header.dwCount == 0) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

void CPack::GenerateKey(BYTE * key, LPCTSTR szPassword)
{
	MD5_CTX ctx;

	MD5_Init(&ctx);

	MD5_Update(&ctx, szPassword, _tcslen(szPassword) * sizeof(TCHAR));

	MD5_Final(key, &ctx);
}

BOOL CPack::DecryptData(PBYTE pBuffer, size_t size, EncryptMethod eEmethod, LPCTSTR szPassword)
{
	BYTE key[16];
	BYTE iv[16];

	AES_KEY aes_key;
	CAST_KEY cast_key;
	BF_KEY bf_key;

	int num = 0;

	if(eEmethod == EM_NONE)
		return TRUE;

	if(NULL == szPassword || _tcslen(szPassword) == 0)
		return FALSE;

	// generate key
	GenerateKey(key, szPassword);

	memcpy(iv, key, sizeof(iv));

	if(eEmethod == EM_AES) {
		AES_set_encrypt_key(key, 128, &aes_key);
		AES_cfb8_encrypt(pBuffer, pBuffer, size, &aes_key, iv, &num, AES_ENCRYPT);
	} else if(eEmethod == EM_BLOWFISH){
		BF_set_key(&bf_key, 16, key);
		BF_cfb64_encrypt(pBuffer, pBuffer, size, &bf_key, iv, &num, BF_ENCRYPT);
	} else if(eEmethod == EM_CAST){
		CAST_set_key(&cast_key, 16, key);
		CAST_cfb64_encrypt(pBuffer, pBuffer, size, &cast_key, iv, &num, CAST_ENCRYPT);
	}

	return TRUE;
}

BOOL CPack::EncryptData(PBYTE pBuffer, size_t size, EncryptMethod eEmethod, LPCTSTR szPassword)
{
	BYTE key[16];
	BYTE iv[16];

	AES_KEY aes_key;
	CAST_KEY cast_key;
	BF_KEY bf_key;

	int num = 0;

	if(eEmethod == EM_NONE)
		return TRUE;

	if(NULL == szPassword || _tcslen(szPassword) == 0)
		return FALSE;

	// generate key
	GenerateKey(key, szPassword);

	memcpy(iv, key, sizeof(iv));

	if(eEmethod == EM_AES) {
		AES_set_encrypt_key(key, 128, &aes_key);
		AES_cfb8_encrypt(pBuffer, pBuffer, size, &aes_key, iv, &num, AES_DECRYPT);
	} else if(eEmethod == EM_BLOWFISH){
		BF_set_key(&bf_key, 16, key);
		BF_cfb64_encrypt(pBuffer, pBuffer, size, &bf_key, iv, &num, BF_DECRYPT);
	} else if(eEmethod == EM_CAST){
		CAST_set_key(&cast_key, 16, key);
		CAST_cfb64_encrypt(pBuffer, pBuffer, size, &cast_key, iv, &num, CAST_DECRYPT);
	}

	return TRUE;
}


BOOL  CPack::SetPackFormat(PackFormat eFormat, DWORD dwParam)
{
	PackHandler * handle = NULL;
	BOOL bRet = TRUE;

	handle = GetHandlerByFormat(eFormat);

	if(eFormat != m_Header.dwFormat || dwParam != m_Header.dwFormatParam) {
		bRet = handle->fnSetFormat(dwParam, &m_Header, m_Bmp);
		if(bRet) {
			m_bIsDirty = TRUE;
		}
	}

	return bRet;
}

CPack * CPack::LoadFromImageByHandle(LPCTSTR szSrc, LPCTSTR szExt, PackHandler * handle, CPackErrors & Error, BOOL * bCancel,
							 CB_GET_PASSWORD fnGetPass, CB_SET_PROGRESS fnSetProgress)
{
	CPack * pPack = NULL;
	PackHeader * pBuffer = NULL;
	size_t offset = 0;
	size_t itemSize = 0;
	UINT nIndex;

	pPack = new CPack();
	pBuffer = handle->fnReadImage(szSrc, szExt, pPack->m_Bmp, Error, bCancel, fnSetProgress);
	if(NULL == pBuffer) {
		/*
		if(bCancel && *bCancel) {
			eErrorCode = CPackErrors::PE_CANCELED;
			Error.SetError( CPackErrors::PE_CANCELED);
		}
		*/
		goto load_image_error;
	}

	memcpy(&pPack->m_Header, pBuffer, sizeof(pPack->m_Header));
	if(pPack->m_Header.dwEncryptType != EM_NONE) {
		pPack->m_szPassword = fnGetPass();
	}

	if(pPack->m_szPassword.IsEmpty() && pPack->m_Header.dwEncryptType != EM_NONE) {
		Error.SetError(CPackErrors::PE_NEED_PASSWORD);
		goto load_pack_error;
	}
	if(pPack->m_Header.dwCount != 0 && pPack->m_Header.dwDataSize != 0) {
		if(!DecryptData(((PBYTE)pBuffer) + sizeof(pPack->m_Header), 
			pPack->m_Header.dwDataSize, 
			(EncryptMethod)pPack->m_Header.dwEncryptType, (LPCTSTR)pPack->m_szPassword)) {
			Error.SetError(CPackErrors::PE_DECRYPT);
			goto decrypt_pack_error;
		}
	}
	for( DWORD i = 0 ; i < pPack->m_Header.dwCount ; i++) {
		CPackItem * pItem = CPackItem::CreatePackItemFromMemory(
			(const CPackItem::PackItemHeader *)(((PBYTE)pBuffer) + sizeof(pPack->m_Header) + offset), 
			pPack->m_Header.dwDataSize - itemSize,
			Error);
		if(NULL == pItem) {
			goto load_pack_item_error;
		}
		nIndex = pPack->m_PackItemList.Add(pItem);
		offset += pItem->GetTotalSize();
		itemSize += pItem->GetTotalSize();
		if(itemSize > pPack->m_Header.dwDataSize) {
			Error.SetError(CPackErrors::PE_CORRUPT_DATA);
			goto load_pack_item_error;
		}
	}
	if(itemSize != pPack->m_Header.dwDataSize) {
		Error.SetError(CPackErrors::PE_CORRUPT_DATA);
		goto load_pack_item_error;
	}

	if(NULL != pBuffer) {
		free(pBuffer);
		pBuffer = NULL;		
	}	
	return pPack;

load_pack_item_error:
decrypt_pack_error:
load_pack_error:
load_image_error:
	if(NULL != pPack) {
		delete pPack;
		pPack = NULL;
	}
	if(NULL != pBuffer) {
		free(pBuffer);
		pBuffer = NULL;		
	}
	return pPack;
}

CPack * CPack::LoadFromImage(LPCTSTR szSrc, LPCTSTR szExt, CString & szError, BOOL * bCancel,
							 CB_GET_PASSWORD fnGetPass, CB_SET_PROGRESS fnSetProgress)
{

	std::list<PackHandler *> handles;
	std::list<PackHandler *>::iterator it;
	CPack * ret = NULL;
	CPackErrors Error;

	handles = GetHandlersByExt(szExt);

	for(it = handles.begin(); it!= handles.end(); it++) {
		TRACE(_T("load image try handler %s\n"), (*it)->szName);
		ret = LoadFromImageByHandle(szSrc, szExt, *it, Error, bCancel,
							 fnGetPass, fnSetProgress);
		if(NULL != ret) {// OK!
			break;
		}

		if(Error.GetError() != CPackErrors::PE_UNSUPPORT_PACK) { // some thing wrong, do not continue
			break;
		}
	}

	szError = Error.ToString();
	return ret;
}

BOOL CPack::SaveToImage(LPCTSTR szDst, LPCTSTR szExt, CString & szError, BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	PackHeader * pBuffer = NULL;
	DWORD offset = 0;
	UINT nIndex;
	PackHandler * handle = NULL;
	CPackErrors Error;

	handle = GetHandlerByFormat((PackFormat)m_Header.dwFormat);

	pBuffer = (PackHeader *)malloc(sizeof(PackHeader) + m_Header.dwDataSize);

	if(NULL == pBuffer) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		goto malloc_error;
	}

	memcpy(pBuffer, &m_Header, sizeof(m_Header));

	for(nIndex = 0 ; nIndex < m_PackItemList.GetCount() ; nIndex ++) {
		CPackItem * pItem = m_PackItemList[nIndex];
		if(!pItem->ExportDataToMemory((PBYTE)pBuffer + sizeof(m_Header) + offset,m_Header.dwDataSize - offset ,Error)) {
			goto save_pack_item_error;
		}
		offset += pItem->GetTotalSize();
	}

	// encrypt it;
	if(!EncryptData((PBYTE)pBuffer + sizeof(m_Header), m_Header.dwDataSize, (EncryptMethod)m_Header.dwEncryptType, (LPCTSTR)m_szPassword)) {
		Error.SetError(CPackErrors::PE_ENCRYPT);
		goto encrypt_error;
	}

	if(!handle->fnWriteImage(pBuffer, m_Bmp, szDst, szExt, Error, bCancel, fnSetProgress)) {
		/*
			if(bCancel && *bCancel) {
				Error.SetError(CPackErrors::PE_CANCELED);
			} else {
				//szError = _T("pack data error!");
				szError.LoadString(IDS_ERROR_INTERNAL);
			}
		*/
			goto pack_data_error;
	}

	if(NULL != pBuffer) {
		free(pBuffer);
		pBuffer = NULL;
	}

	m_bIsDirty = FALSE;
	szError = Error.ToString();
	return TRUE;

pack_data_error:
encrypt_error:
save_pack_item_error:
malloc_error:
	if(NULL != pBuffer) {
		free(pBuffer);
		pBuffer = NULL;
	}
	szError = Error.ToString();
	return FALSE;
}

DWORD CPack::GetPackItemCount() const
{
	return m_Header.dwCount;
}


CPackItem * CPack::GetPackItem(UINT nIndex)
{
	UINT nSize = m_PackItemList.GetCount();
	if( nIndex < nSize) {
		return m_PackItemList[nIndex];
	}
	return NULL;
}


BOOL CPack::AddPackItem(CPackItem * pItem, 	UINT & nIndex, CPackErrors & Error)
{
	if(m_Header.dwDataSize + pItem->GetTotalSize() > m_Header.dwCapicity) {
		Error.SetError(CPackErrors::PE_OVER_CAPICITY);
		return FALSE;
	}
	nIndex = m_PackItemList.Add(pItem);
	m_Header.dwCount ++;
	m_Header.dwDataSize += pItem->GetTotalSize();

	m_bIsDirty = TRUE;

	return TRUE;
}



BOOL CPack::IsDirty()
{
	return m_bIsDirty;
}

BOOL CPack::IsEmpty()
{
	return m_Header.dwCount == 0;
}

BOOL CPack::Clear()
{
	if(m_Header.dwCount != 0) {
		for(UINT i = 0; i < m_PackItemList.GetCount() ; i++) {
			CPackItem * pItem = m_PackItemList[i];
			delete pItem;
		}	
		m_PackItemList.RemoveAll();
		m_Header.dwDataSize = 0;
		m_Header.dwCount = 0;

		m_bIsDirty = TRUE;
	}
	return TRUE;
}

BOOL CPack::RemovePackItem(UINT nIndex, UINT nCount/* = 1 */)
{
	UINT nSize = m_PackItemList.GetCount();
	if(nCount > 0 && nIndex < nSize && nIndex + nCount - 1 < nSize) {
		for(UINT i = nIndex; i < nIndex + nCount ; i++) {
			CPackItem * pItem = m_PackItemList[i];
			m_Header.dwDataSize -= pItem->GetTotalSize();
			m_Header.dwCount --;
			delete pItem;
		}
		m_PackItemList.RemoveAt(nIndex, nCount);
		m_bIsDirty = TRUE;
		return TRUE;
	}
	return FALSE;
}


BOOL CPack::CanSetFormat(PackFormat eFormat, DWORD dwParam) const
{
	PackHandler * handle = NULL;

	handle = GetHandlerByFormat(eFormat);

	return handle->fnCanSetFormat(dwParam, &m_Header, m_Bmp);
}

void CPack::SetPassword(const CString & szPassword) 
{
	if(m_szPassword.Compare(szPassword) != 0){
		m_szPassword = szPassword; 
		m_bIsDirty = TRUE;
	}
}


std::list<CPack::PackHandler *> CPack::GetHandlersByExt(LPCTSTR szExt)
{
	std::list<PackHandler *> ret;
	PackHandler * pHandler;
	for(INT i = 0 ; i < sizeof(m_Handler)/sizeof(PackHandler) ; i++) {
		pHandler = &m_Handler[i];
		for(INT j = 0 ; pHandler->szExts[j] != NULL; j++) {
			if(lstrcmpi(szExt, pHandler->szExts[j]) == 0) {
				ret.push_back(pHandler);
				break;
			}
		}
	}

	ret.push_back(&m_Handler[0]); /* default handler */

	return ret;
}


CPack::PackHandler * CPack::GetHandlerByFormat(PackFormat eFormat)
{
	if(eFormat <= PF_JSTEG && eFormat >= PF_NONE) {
		return &m_Handler[eFormat];
	}
	return NULL;
}

const CString & CPack::GetFilter()
{
	PackHandler * pHandler = GetHandlerByFormat((PackFormat)m_Header.dwFormat);
	pHandler->fnGetSaveFilter(m_szFilter);
	return m_szFilter;
}

const CString & CPack::GetDefaultExt()
{
	PackHandler * pHandler = GetHandlerByFormat((PackFormat)m_Header.dwFormat);
	pHandler->fnGetSaveDefaultExt(m_szExt);
	return m_szExt;
}

void CPack::PadMemory(LPBYTE buffer, size_t size)
{
	size_t n = 0, r = 0;
	INT b;

	if(size == 0)
		return;

	n = size / sizeof(INT);
	r = size % sizeof(INT);

	srand(time(NULL));

	for(size_t i = 0 ; i < n ; i ++) {
		b = rand();
		memcpy(buffer, &b, sizeof(INT));
		buffer += sizeof(INT);
	}

	for(size_t i = 0 ; i < r ; i ++) {
		b = rand();
		buffer[i] = (BYTE)rand();
	}	
}
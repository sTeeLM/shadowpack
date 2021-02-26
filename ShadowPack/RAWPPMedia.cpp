#include "StdAfx.h"
#include "RAWPPMedia.h"
#include "CipherStream.h"
#include "RAWPPOptionDialog.h"


CRAWPPMedia::CRAWPPMedia(void):
m_pBlockBuffer(NULL),
m_pStream(NULL),
m_strMediaFilePath(_T("")),
m_bFormatChanged(FALSE),
m_Mode(CMedia::MODE_READ),
m_nOffset(0L),
m_strPassword(_T("")),
m_pOrgMedia(NULL)
{
	::ZeroMemory(&m_Header, sizeof(m_Header));
}

CRAWPPMedia::~CRAWPPMedia(void)
{

}

BOOL CRAWPPMedia::IsValidMediaHeader()
{
	if(m_Header.dwSignature != MEDIA_HEADER_SIG) {
		return FALSE;
	}

	if(m_Header.dwCipherType >= CPackCipher::CIPHER_CNT) {
		return FALSE;
	}

	if(m_Header.uFormat.wFormat == 0 
		|| m_Header.uFormat.wFormat >= PF_COUNT) {
		return FALSE;
	}

	if(m_Header.uDataSize > m_Header.uCapicity) {
		return FALSE;
	}

	if(m_Header.uFormat.wParam == 0 || m_Header.uFormat.wParam >= PFP_COUNT)
		return FALSE;

	ULONGLONG nCap = GetFormatParamCap((rawpp_media_format_param_t)m_Header.uFormat.wParam);

	if(m_Header.uCapicity != nCap) {
		return FALSE;
	}

	return TRUE;
}

BOOL CRAWPPMedia::SaveMedia(BOOL &bCancel, CPackErrors &Error, CProgress &Progress)
{
	if(!RandomFillFreeSpace(bCancel, Error, Progress)) {
		return FALSE;
	}

	if(!SaveFile(bCancel, Error, Progress)) {
		return FALSE;
	}

	return TRUE;

}

BOOL CRAWPPMedia::OpenMedia(media_mode_t mode, LPCTSTR szPath, CPasswordGetter &PasswordGetter, 
							BOOL &bCancel, CPackErrors &Error, CProgress &Progress)
{

	ULONGLONG nHeaderSize = GetMediaHeaderSize();
	ULONGLONG nBlockSize = 0,i = 0;
	BOOL bRet = FALSE;
	INT nFormat = (INT)PFP_1PP;
	CCipherStream * pStream = NULL;
	CString strPassword = _T("");

	SetMediaFilePath(szPath);
	SetMediaMode(mode);

	if(!OpenFile(mode, szPath, bCancel, Error, Progress)) {
		goto err;
	}

	if(!(m_pBlockBuffer = AllocBlockBuffer(Error))) {
		goto err;
	}

	if(mode == MODE_READ) {

		if(GetBlockCount() < nHeaderSize) {
			Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
			goto err;
		}

		i = 1;
	try_next_format:
	//  try PFP_1PP / PFP_2PP / PFP_3PP
		nBlockSize = i * nHeaderSize;
		if(!ReadBlock(m_pBlockBuffer, 0, nBlockSize, Error)) {
			goto no_header;
		}

		if(!m_pBlockBuffer->ReadByteFromBlock((LPBYTE)&GetMediaHeader(), nHeaderSize, 
			nBlockSize, (rawpp_media_format_param_t)i, Error)) {
			goto err;
		}

		if(!(IsValidMediaHeader() && i == GetFormatParam()) && i < 3) {
			i ++;
			nFormat ++;
			goto try_next_format;
		} else if(IsValidMediaHeader() && i == GetFormatParam()) {
			goto valid_header;
		} 
	no_header:
		// 格式化成默认的media, PF_RAWPP & PFP_1PP
		GetMediaHeader().dwSignature = MEDIA_HEADER_SIG;
		GetMediaHeader().dwCipherType = CPackCipher::CIPHER_NONE;
		GetMediaHeader().uFormat.wFormat = PF_RAWPP;
		GetMediaHeader().uFormat.wParam = PFP_1PP;
		GetMediaHeader().uDataSize = 0;	
		GetMediaHeader().uCapicity = GetBlockCount() - GetMediaHeaderSize();	

	valid_header:
		// 生成inputstream 和outputstream

		if(IsEncryptMedia()) {
			strPassword = PasswordGetter.GetPassword();
			if(!strPassword.GetLength()) {
				Error.SetError(CPackErrors::PE_NEED_PASSWORD);
				goto err;
			}
		}
		SetPassword(strPassword);
	} else {
		// write header
		if(GetFormatParam() == PFP_1PP) {
			nBlockSize = nHeaderSize;
		} else if(GetFormatParam() == PFP_2PP) {
			nBlockSize = 2 * nHeaderSize;
		} else if(GetFormatParam() == PFP_3PP) {
			nBlockSize = 3 * nHeaderSize;
		} else {
			Error.SetError(CPackErrors::PE_INTERNAL);
			goto err;
		}

		if(m_pBlockBuffer->GetCapicity() < nBlockSize) {
			if(!m_pBlockBuffer->ReAlloc(nBlockSize, Error)) {
				goto err;
			}
		}

		if(!GetOrgMedia()->ReadBlock(m_pBlockBuffer, 0, nBlockSize, Error)) {
			goto err;
		}

		if(!m_pBlockBuffer->WriteByteToBlock((const LPBYTE)&GetMediaHeader(), nHeaderSize, nBlockSize, 
			(rawpp_media_format_param_t)GetFormatParam(), Error)) {
			goto err;
		}

		if(!WriteBlock(m_pBlockBuffer, 0, nBlockSize, Error)) {
			goto err;
		}

	}

	pStream = new(std::nothrow) CCipherStream((CStream *)this);
	if(pStream) {
		pStream->SetCipher(GetCipherType(), GetPassword());
		m_pStream = pStream;
	} else {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	return TRUE;
err:
	CloseFile();
	if(m_pStream != NULL) {
		delete m_pStream;
		m_pStream = NULL;
	}

	return FALSE;
}

BOOL CRAWPPMedia::SetToRead(CPackErrors &Error)
{
	m_Mode = MODE_READ;
	return ReopenFileToRead(Error);
}

void CRAWPPMedia::CloneCore(CRAWPPMedia * pFrom)
{
	memcpy(&GetMediaHeader(), &pFrom->GetMediaHeader(), (size_t)pFrom->GetMediaHeaderSize());
	SetPassword(pFrom->GetPassword());
	SetMediaFilePath(pFrom->GetMediaFilePath());
	SetOrgMedia(pFrom);
}

BOOL CRAWPPMedia::RandomFillFreeSpace(BOOL & bCancel, CPackErrors &Error, CProgress &Progress)
{

	ULONGLONG nFree = GetFreeSize(), nWrite, nBlockOffset;
	CStream * pOutput = GetStream();
	BYTE Buffer[4096];

	srand((unsigned int)time(NULL));

	if(nFree == 0) {
		return TRUE;
	}

	if(pOutput == NULL) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(!pOutput->Seek(GetDataSize(), SEEK_SET, Error)) {
		return FALSE;
	}

	TRACE(_T("Fill free space with rand: Cap %lld, DataSize %lld, Free %lld, Offset %lld, Total Block %lld\n"),
		GetCapicity(), GetDataSize(), nFree, m_pStream->GetOffset(), GetBlockCount());

	Progress.Reset();
	Progress.SetFullScale(nFree);

	while(nFree) {

		if(bCancel) {
			Error.SetError(CPackErrors::PE_CANCELED);
			return FALSE;
		}

		for(size_t i = 0 ; i < sizeof(Buffer) / sizeof(INT) ; i ++) {
			INT b = rand();
			memcpy(Buffer + i * sizeof(INT), &b, sizeof(INT));
		}		
		nWrite = nFree > sizeof(Buffer) ? sizeof(Buffer) : nFree;
		if(!pOutput->Write(Buffer ,nWrite,Progress, Error)) {
			return FALSE;
		}
		nFree -= nWrite;

	}


	// 由于block count 不一定能被3整除，可能还最多预留2个block
	if(GetFormatParam() == PFP_1PP) {
		nBlockOffset = pOutput->GetOffset() + GetMediaHeaderSize();
	} else if(GetFormatParam() == PFP_2PP) {
		nBlockOffset = 2 * (pOutput->GetOffset() + GetMediaHeaderSize());
	} else if(GetFormatParam() == PFP_3PP) {
		nBlockOffset = 3 * (pOutput->GetOffset() + GetMediaHeaderSize());
	} else {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(GetBlockCount() <  nBlockOffset) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	nWrite = GetBlockCount() - nBlockOffset;
	
	TRACE(_T("Left %lld Block to Write at BlockOffset %lld\n"), nWrite, nBlockOffset);
	Progress.Reset();
	Progress.SetFullScale(nWrite);

	if(nWrite > 0) {
		if(m_pBlockBuffer->GetCapicity() < nWrite) {
			if(!m_pBlockBuffer->ReAlloc(nWrite, Error)) {
				return FALSE;
			}
		}

		if(!GetOrgMedia()->ReadBlock(m_pBlockBuffer, nBlockOffset, nWrite, Error)) {
			return FALSE;
		}

		if(!WriteBlock(m_pBlockBuffer, nBlockOffset, nWrite, Error)) {
			return FALSE;
		}
	}

	return TRUE;
}

ULONGLONG CRAWPPMedia::GetFormatParamCap(rawpp_media_format_param_t t)
{
	ULONGLONG nNewCapicity  = 0;
	if(t == PFP_1PP) {
		nNewCapicity = GetBlockCount() - GetMediaHeaderSize();
	} else if(t == PFP_2PP) {
		nNewCapicity = GetBlockCount() / 2 - GetMediaHeaderSize();
	} else if(t == PFP_3PP) {
		nNewCapicity = GetBlockCount() / 3 - GetMediaHeaderSize();
	} else {
		return 0;
	}
	return nNewCapicity;
}

BOOL CRAWPPMedia::CanSetNewFormatParam(CRAWPPMedia::rawpp_media_format_param_t t)
{
	ULONGLONG nNewCapicity  = GetFormatParamCap(t);
	return nNewCapicity >= GetDataSize();
}

BOOL CRAWPPMedia::ShowOptionDlg()
{

	CRAWPPOptionDialog dlg;
	INT nRet;

	dlg.m_bEncrypt = IsEncryptMedia();
	dlg.m_szPassword = GetPassword();
	if(dlg.m_bEncrypt) {
		dlg.m_nEncryptMethod =(INT)(GetMediaHeader().dwCipherType ) - 1;
	}
	dlg.m_bEnable1PP = CanSetNewFormatParam(CRAWPPMedia::PFP_1PP);
	dlg.m_bEnable2PP = CanSetNewFormatParam(CRAWPPMedia::PFP_2PP);
	dlg.m_bEnable3PP = CanSetNewFormatParam(CRAWPPMedia::PFP_3PP);

	dlg.m_nFormat = (INT) GetMediaHeader().uFormat.wParam - 1;

	nRet = dlg.DoModal();

	if(nRet == IDOK) {
		if(dlg.m_bEncrypt &&
			(dlg.m_nEncryptMethod + 1) > CPackCipher::CIPHER_NONE && (dlg.m_nEncryptMethod + 1) < CPackCipher::CIPHER_CNT) {
				GetMediaHeader().dwCipherType = dlg.m_nEncryptMethod + 1;
				SetPassword(dlg.m_szPassword);
		} else {
			GetMediaHeader().dwCipherType = CPackCipher::CIPHER_NONE;
			SetPassword(_T(""));
		}

		if((dlg.m_nFormat + 1) > CRAWPPMedia::PFP_INVALID && (dlg.m_nFormat + 1) < CRAWPPMedia::PFP_COUNT) {
			GetMediaHeader().uFormat.wParam = dlg.m_nFormat + 1;
			GetMediaHeader().uCapicity = 
				GetFormatParamCap((CRAWPPMedia::rawpp_media_format_param_t)GetFormatParam());
		}

		SetFormatChanged();
	}

	return nRet == IDOK;
}

void CRAWPPMedia::CloseMedia()
{
	CPackErrors Error;

	if(GetStream() != NULL)
		GetStream()->Close(Error);
	if(m_pStream != NULL) {
		delete m_pStream;
		m_pStream = NULL;
	}

	CloseFile();

	if(NULL != m_pBlockBuffer) {
		FreeBlockBuffer(m_pBlockBuffer);
		m_pBlockBuffer = NULL;
	}

	::ZeroMemory(&m_Header, sizeof(m_Header));

	delete this;
}


BOOL CRAWPPMedia::Read(LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error)
{
	// 根据offset，格式，size，读取底层block并提取数据
	ULONGLONG nBlockOffset;
	ULONGLONG nBlockSize;

	if(m_nOffset + nSize > GetCapicity()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(GetFormatParam() == PFP_1PP) {
		nBlockOffset = (m_nOffset + GetMediaHeaderSize());
		nBlockSize = nSize;
	} else if(GetFormatParam() == PFP_2PP) {
		nBlockOffset = 2 * (m_nOffset + GetMediaHeaderSize());
		nBlockSize = 2 * nSize;
	} else if(GetFormatParam() == PFP_3PP) {
		nBlockOffset = 3 * (m_nOffset + GetMediaHeaderSize());
		nBlockSize = 3 * nSize;
	} else {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(!ReadBlock(m_pBlockBuffer, nBlockOffset, nBlockSize, Error)) {
		return FALSE;
	}

	if(!m_pBlockBuffer->ReadByteFromBlock((LPBYTE)pBuffer, nSize, nBlockSize, 
		(rawpp_media_format_param_t)GetFormatParam(), Error)) {
		return FALSE;
	}

	m_nOffset += nSize;
	Progress.IncScale(nSize);

	return TRUE;
}

BOOL CRAWPPMedia::Write(const LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error)
{
	// 根据offset，格式，size，将数据写入block并写入底层
	ULONGLONG nBlockOffset;
	ULONGLONG nBlockSize;

	if(m_nOffset + nSize > GetCapicity()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
	
	if(GetFormatParam() == PFP_1PP) {
		nBlockOffset = (m_nOffset + GetMediaHeaderSize());
		nBlockSize = nSize;
	} else if(GetFormatParam() == PFP_2PP) {
		nBlockOffset = 2 * (m_nOffset + GetMediaHeaderSize());
		nBlockSize = 2 * nSize;
	} else if(GetFormatParam() == PFP_3PP) {
		nBlockOffset = 3 * (m_nOffset + GetMediaHeaderSize());
		nBlockSize = 3 * nSize;
	} else {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(!GetOrgMedia()->ReadBlock(m_pBlockBuffer, nBlockOffset, nBlockSize, Error)) {
		return FALSE;
	}

	if(!m_pBlockBuffer->WriteByteToBlock((const LPBYTE)pBuffer, nSize, nBlockSize, 
		(rawpp_media_format_param_t)GetFormatParam(), Error)) {
		return FALSE;
	}

	if(!WriteBlock(m_pBlockBuffer, nBlockOffset, nBlockSize, Error)) {
		return FALSE;
	}

	m_nOffset += nSize;
	Progress.IncScale(nSize);

	return TRUE;
}


BOOL CRAWPPMedia::Seek(LONGLONG nOffset, INT nOrg, CPackErrors & Error)
{
	ULONGLONG nCap = GetCapicity();
	BOOL bRet = FALSE;
	switch(nOrg) {
	case SEEK_CUR:
		if(nOffset + m_nOffset <= nCap && nOffset + m_nOffset >= 0) {
			m_nOffset += nOffset;
			bRet = TRUE;
		}break;
	case SEEK_END:
		if(nOffset <= 0 && nCap + nOffset >= 0) {
			m_nOffset = nCap + nOffset;
			bRet = TRUE;
		}break;
	case SEEK_SET:
		if(nOffset >= 0 && (ULONGLONG)nOffset <= nCap) {
			m_nOffset = nOffset;
			bRet = TRUE;
		}
	default:
		Error.SetError(CPackErrors::PE_INTERNAL);
	}

	return bRet;
}

BOOL CRAWPPMedia::Close(CPackErrors & Error)
{
	return TRUE;
}

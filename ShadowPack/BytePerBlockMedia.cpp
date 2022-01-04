#include "pch.h"
#include "BytePerBlockMedia.h"
#include "PackUtils.h"
#include "resource.h"

CBytePerBlockMedia::CBytePerBlockMedia() :
	m_bIsDirty(FALSE)
{
	ZeroMemory(&m_Header, sizeof(m_Header));
	m_Header.dwBPBBlockPerByte = 1;
}

CBytePerBlockMedia::~CBytePerBlockMedia()
{

}

BOOL CBytePerBlockMedia::FillEmptySpace(CProgressBase& Progress, CPackErrors& Errors)
{
	ULONGLONG nTotal = GetMediaTotalBytes() - GetMediaUsedBytes();
	ULONGLONG nOffset = GetMediaUsedBytes() + sizeof(m_Header);
	UINT nWrite = 0;
	TRACE(_T("FillEmptySpace %I64u Bytes @ %I64u\n"), nTotal, nOffset);
	while (nTotal > 0) {
		nWrite = (UINT)(nTotal > BPB_STREAM_BATCH_SIZE ? BPB_STREAM_BATCH_SIZE : nTotal);
		CPackUtils::FillBufferRand(ReadWriteBuffer, nWrite);
		m_Cipher.EncryptBlock(ReadWriteBuffer, nWrite, nOffset);
		if (!RawWriteData(ReadWriteBuffer, nOffset, nWrite, m_Header.dwBPBBlockPerByte, Errors)) {
			return FALSE;
		}
		nTotal -= nWrite;
		nOffset += nWrite;
		Progress.Increase(nWrite);
	}
	return TRUE;
}

BOOL CBytePerBlockMedia::TestHeaderValid(const MEDIA_HEADER_T* pH)
{
	const BPB_MEDIA_HEADER_T* pHeader = (const BPB_MEDIA_HEADER_T*)pH;
	LARGE_INTEGER li;
	if (CMediaBase::TestHeaderValid(pH)) {
		if (pHeader->dwBPBMediaSign == BPB_MEDIA_HEADER_SIGN) {
			if (pHeader->dwBPBBlockPerByte <= MAX_BPB_MEDIA_BPB_SIZE && pHeader->dwBPBBlockPerByte >= MIN_BPB_MEDIA_BPB_SIZE) {
				if (pHeader->BPBHeader.dwHeaderSize == sizeof(BPB_MEDIA_HEADER_T)) {
					li.HighPart = pHeader->BPBHeader.dwDataSizeHi;
					li.LowPart  = pHeader->BPBHeader.dwDataSizeLow;
					if ((ULONGLONG)li.QuadPart <= GetTotalBlocks() / pHeader->dwBPBBlockPerByte) {
						if (pHeader->dwBPBCipher < CPackCipher::GetCipherCount()) {
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CBytePerBlockMedia::RawReadData(LPVOID pBuffer, ULONGLONG nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors)
{
	LPBYTE p = (LPBYTE)pBuffer;
	ASSERT((nOffset + nSize) * nBPBBlockPerByte <= GetTotalBlocks());
	if ((nOffset + nSize) * nBPBBlockPerByte <= GetTotalBlocks()) {
		for (UINT i = 0; i < nSize; i++) {
			p[i] = GetByteFromBlocks(nOffset + i,nBPBBlockPerByte);
		}
		return TRUE;
	} else {
		Errors.SetError(CPackErrors::PE_INTERNAL);
	}
	return FALSE;
}

BOOL CBytePerBlockMedia::RawWriteData(LPVOID pBuffer, ULONGLONG nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors)
{
	LPBYTE p = (LPBYTE)pBuffer;
	ASSERT((nOffset + nSize) * nBPBBlockPerByte <= GetTotalBlocks());
	if ((nOffset + nSize) * nBPBBlockPerByte <= GetTotalBlocks()) {
		for (UINT i = 0; i < nSize; i++) {
			SetByteToBlocks(p[i], nOffset + i, nBPBBlockPerByte);
		}
		return TRUE;
	}
	else {
		Errors.SetError(CPackErrors::PE_INTERNAL);
	}
	return FALSE;
	
}

BOOL CBytePerBlockMedia::LoadMeta(CPasswordGetterBase& PasswordGetter, CPackErrors& Errors)
{
	BPB_MEDIA_HEADER_T Header, HeaderPlain;
	CString strPassword;
	BOOL bRet = FALSE;

	// try header with crypt
	
test_encrypt:
	for (INT i = MIN_BPB_MEDIA_BPB_SIZE; i <= MAX_BPB_MEDIA_BPB_SIZE; i++) {
		ZeroMemory(&Header, sizeof(Header));
		ZeroMemory(&HeaderPlain, sizeof(HeaderPlain));
		if (RawReadData(&Header, 0, sizeof(Header), i, Errors)) {
#ifdef DEBUG
			TRACE(_T("m_Header.dwBPBBlockPerByte = %d\n"), i);
			DumpHeader(_T("LoadMeta"), Header);
#endif
			if (!strPassword.GetLength()) {
				if (m_Cipher.SetKeyType(CPackCipher::CIPHER_NONE, NULL)) {
					m_Cipher.DecryptBlock(&Header, &HeaderPlain, sizeof(Header), 0);
					if (TestHeaderValid((const MEDIA_HEADER_T *)&HeaderPlain)) {
						bRet = TRUE;
						goto success;
					}
				}
				else {
					Errors.SetError(CPackErrors::PE_CIPHER);
					goto err;
				}
				continue;
			}

			for (UINT i = 1; i < CPackCipher::GetCipherCount(); i++) {
				if (m_Cipher.SetKeyType((CPackCipher::PACK_CIPHER_TYPE_T)i, (LPCTSTR)strPassword)) {
					m_Cipher.DecryptBlock(&Header, &HeaderPlain, sizeof(Header), 0);
					if (TestHeaderValid((const MEDIA_HEADER_T*)&HeaderPlain)) {
						bRet = TRUE;
						goto success;
					}
				}
				else {
					Errors.SetError(CPackErrors::PE_CIPHER);
					goto err;
				}
			}
		}
	}

// 尝试获取密码
	if (!strPassword.GetLength()) {
		strPassword = PasswordGetter.GetPassword();
		if (strPassword.GetLength())
			goto test_encrypt;
	}

// 没有发现任何有效的头，作为空白文件处理
	if (GetTotalBlocks() >= sizeof(Header)) {
		HeaderPlain.dwBPBMediaSign = BPB_MEDIA_HEADER_SIGN;
		HeaderPlain.dwBPBBlockPerByte = 1;
		HeaderPlain.BPBHeader.dwSign = MEDIA_HEADER_SIGN;
		HeaderPlain.BPBHeader.dwDataSizeHi = 0;
		HeaderPlain.BPBHeader.dwDataSizeLow = 0;
		HeaderPlain.BPBHeader.dwHeaderSize = sizeof(BPB_MEDIA_HEADER_T);
		HeaderPlain.dwBPBCipher = CPackCipher::CIPHER_NONE;
		m_Cipher.SetKeyType(CPackCipher::CIPHER_NONE, NULL);
		AfxMessageBox(IDS_EMPTY_PACK, MB_ICONINFORMATION);
		goto success;
	} else {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
	}

err:
	return bRet;

success:
	bRet = TRUE;
	memcpy(&m_Header, &HeaderPlain, sizeof(HeaderPlain));
	return bRet;
}
#ifdef DEBUG
void CBytePerBlockMedia::DumpHeader(CString strWhen, BPB_MEDIA_HEADER_T& Header)
{
	LPBYTE p = (LPBYTE)&Header;
	TRACE(_T("%s :\n"), (LPCTSTR)strWhen);
	for (INT i = 0; i < sizeof(Header); i++) {
		TRACE(_T("%02x \n"), p[i]);
	}
	TRACE(_T("\n"));
}
#endif
BOOL CBytePerBlockMedia::SaveMeta(CPackErrors& Errors)
{
	BPB_MEDIA_HEADER_T Header;
	// save header
	m_Cipher.EncryptBlock(&m_Header, &Header, sizeof(Header), 0);
#ifdef DEBUG
	TRACE(_T("m_Header.dwBPBBlockPerByte = %d\n"), m_Header.dwBPBBlockPerByte);
	DumpHeader(_T("SaveMeta"), Header);
#endif
	if (!RawWriteData(&Header, 0, sizeof(Header), m_Header.dwBPBBlockPerByte, Errors)) {
		return FALSE;
	}

	return TRUE;
}

BYTE CBytePerBlockMedia::ReadWriteBuffer[BPB_STREAM_BATCH_SIZE];

BOOL CBytePerBlockMedia::Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error)
{
	UINT nRead;
	BOOL bRet = FALSE;
	LPBYTE p = (LPBYTE)pBuffer;
	ASSERT(nSize + GetOffset() <= GetMediaTotalBytes() && pBuffer != NULL);
	if (nSize + GetOffset() <= GetMediaTotalBytes()) {
		while (nSize > 0) {
			nRead = nSize > BPB_STREAM_BATCH_SIZE ? BPB_STREAM_BATCH_SIZE : nSize;
			if (!RawReadData(ReadWriteBuffer, sizeof(m_Header) + GetOffset(), nRead, m_Header.dwBPBBlockPerByte, Error)) {
				break;
			}
			m_Cipher.DecryptBlock(ReadWriteBuffer, p, nRead, GetOffset() + sizeof(m_Header));
			if (!Seek(nRead, STREAM_SEEK_CUR, Error)) {
				break;
			}
			nSize -= nRead;
			p += nRead;
			Progress.Increase(nRead);
		}
		if (nSize == 0) {
			bRet = TRUE;
		}
	} else {
		Error.SetError(CPackErrors::PE_EOF);
	}
	return bRet;
}

BOOL CBytePerBlockMedia::Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error)
{
	UINT nWrite;
	BOOL bRet = FALSE;
	LPBYTE p = (LPBYTE)pBuffer;
	ASSERT(nSize + GetOffset() <= GetMediaTotalBytes() && pBuffer != NULL);
	if (nSize + GetOffset() <= GetMediaTotalBytes()) {
		while (nSize > 0) {
			nWrite = nSize > BPB_STREAM_BATCH_SIZE ? BPB_STREAM_BATCH_SIZE : nSize;
			m_Cipher.EncryptBlock(p, ReadWriteBuffer, nWrite, GetOffset() + sizeof(m_Header));
			if (!RawWriteData(ReadWriteBuffer, sizeof(m_Header) + GetOffset(), nWrite, m_Header.dwBPBBlockPerByte, Error)) {
				break;
			}
			if (!Seek(nWrite, STREAM_SEEK_CUR, Error)) {
				break;
			}
			p += nWrite;
			nSize -= nWrite;
			Progress.Increase(nWrite);
		}
		if (nSize == 0) {
			bRet = TRUE;
		}
	}
	else {
		Error.SetError(CPackErrors::PE_EOF);
	}
	return bRet;
}

BOOL CBytePerBlockMedia::Seek(LONGLONG nOffset, CStreamBase::SEEK_TYPE_T Org, CPackErrors& Error)
{
	if (Org == STREAM_SEEK_SET && nOffset <= GetMediaTotalBytes() && nOffset >= 0) {
		m_nOffset = nOffset;
	} else if (Org == STREAM_SEEK_CUR && (nOffset + m_nOffset) <= GetMediaTotalBytes() && (nOffset + m_nOffset) >= 0) {
		m_nOffset += nOffset;
	} else if (Org == STREAM_SEEK_END && nOffset <= 0 && (0 - nOffset) <= GetMediaTotalBytes()) {
		m_nOffset = 0 - nOffset;
	} else {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
	return TRUE;
}

CStreamBase* CBytePerBlockMedia::GetStream()
{
	return this;
}

BOOL CBytePerBlockMedia::SetMediaUsedBytes(ULONGLONG nSize, CPackErrors& Error)
{
	ASSERT(nSize <= GetMediaTotalBytes());
	LARGE_INTEGER li;
	li.QuadPart = nSize;
	if (nSize <= GetMediaTotalBytes()) {
		m_Header.BPBHeader.dwDataSizeHi = li.HighPart;
		m_Header.BPBHeader.dwDataSizeLow = li.LowPart;
		return TRUE;
	} else {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
}

ULONGLONG CBytePerBlockMedia::GetMediaUsedBytes()
{
	LARGE_INTEGER li;
	li.HighPart = m_Header.BPBHeader.dwDataSizeHi;
	li.LowPart = m_Header.BPBHeader.dwDataSizeLow;
	return li.QuadPart;
}

ULONGLONG CBytePerBlockMedia::GetMediaTotalBytes()
{
	if(GetTotalBlocks() / m_Header.dwBPBBlockPerByte >= sizeof(m_Header))
		return GetTotalBlocks() / m_Header.dwBPBBlockPerByte - sizeof(m_Header);
	return 0;
}


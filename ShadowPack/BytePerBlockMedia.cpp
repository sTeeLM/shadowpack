#include "pch.h"
#include "BytePerBlockMedia.h"

CBytePerBlockMedia::CBytePerBlockMedia() :
	m_bIsDirty(FALSE)
{
	memset(&m_Header, 0, sizeof(m_Header));
	m_Header.dwBPBBlockPerByte = 1;
}

CBytePerBlockMedia::~CBytePerBlockMedia()
{

}

BOOL CBytePerBlockMedia::TestHeaderValid(const BPB_MEDIA_HEADER_T* pHeader)
{
	if (pHeader->dwBPBMediaSign == BPB_MEDIA_HEADER_SIGN && pHeader->BPBHeader.dwSign == MEDIA_HEADER_SIGN) {
		if (pHeader->dwBPBBlockPerByte <= MAX_BPB_MEDIA_BPB_SIZE && pHeader->dwBPBBlockPerByte >= MIN_BPB_MEDIA_BPB_SIZE) {
			if (pHeader->BPBHeader.dwDataSize <= GetTotalBlocks() / pHeader->dwBPBBlockPerByte) {
				if (pHeader->dwBPBCipher == m_Cipher.GetCipherType()) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CBytePerBlockMedia::RawReadData(LPVOID pBuffer, UINT nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors)
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

BOOL CBytePerBlockMedia::RawWriteData(LPVOID pBuffer, UINT nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors)
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
	for (INT i = 1; i <= 4; i++) {
		if (RawReadData(&Header, 0, sizeof(Header), i, Errors)) {

			if (!strPassword.GetLength()) {
				if (m_Cipher.SetKeyType(CPackCipher::CIPHER_NONE, NULL)) {
					m_Cipher.DecryptBlock(&Header, &HeaderPlain, sizeof(Header), 0);
					if (TestHeaderValid(&HeaderPlain)) {
						bRet = TRUE;
						goto success;
					}
				}
				else {
					Errors.SetError(CPackErrors::PE_ENCRYPT);
					goto err;
				}
				continue;
			}

			if (m_Cipher.SetKeyType(CPackCipher::CIPHER_AES, (LPCTSTR)strPassword)) {
				m_Cipher.DecryptBlock(&Header, &HeaderPlain, sizeof(Header), 0);
				if (TestHeaderValid(&HeaderPlain)) {
					bRet = TRUE;
					goto success;
				}
			} else {
				Errors.SetError(CPackErrors::PE_ENCRYPT);
				goto err;
			}

			if (m_Cipher.SetKeyType(CPackCipher::CIPHER_SEED, (LPCTSTR)strPassword)) {
				m_Cipher.DecryptBlock(&Header, &HeaderPlain, sizeof(Header), 0);
				if (TestHeaderValid(&HeaderPlain)) {
					bRet = TRUE;
					goto success;
				}
			} else {
				Errors.SetError(CPackErrors::PE_ENCRYPT);
				goto err;
			}

			if (m_Cipher.SetKeyType(CPackCipher::CIPHER_CAMELLIA, (LPCTSTR)strPassword)) {
				m_Cipher.DecryptBlock(&Header, &HeaderPlain, sizeof(Header), 0);
				if (TestHeaderValid(&HeaderPlain)) {
					bRet = TRUE;
					goto success;
				}
			} else {
				Errors.SetError(CPackErrors::PE_ENCRYPT);
				goto err;
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
		HeaderPlain.BPBHeader.dwDataSize = 0;
		HeaderPlain.dwBPBCipher = CPackCipher::CIPHER_NONE;
		m_Cipher.SetKeyType(CPackCipher::CIPHER_NONE, NULL);
		goto success;
	} else {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_PACK);
	}

err:
	return bRet;

success:
	bRet = TRUE;
	memcpy(&m_Header, &HeaderPlain, sizeof(HeaderPlain));
	return bRet;
}

BOOL CBytePerBlockMedia::SaveMeta(CPackErrors& Errors)
{
	BPB_MEDIA_HEADER_T Header;
	// save header
	m_Cipher.EncryptBlock(&m_Header, &Header, sizeof(Header), 0);
	if (!RawWriteData(&Header, 0, sizeof(Header), m_Header.dwBPBBlockPerByte, Errors)) {
		return FALSE;
	}

	return TRUE;
}

BOOL CBytePerBlockMedia::Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error)
{
	UINT nRead;
	BOOL bRet = FALSE;
	BYTE ReadBuffer[BPB_STREAM_BATCH_SIZE];
	LPBYTE p = (LPBYTE)pBuffer;
	ASSERT(nSize + GetOffset() <= GetMediaTotalBytes() && pBuffer != NULL);
	if (nSize + GetOffset() <= GetMediaTotalBytes() && pBuffer != NULL) {
		while (nSize > 0) {
			nRead = nSize > BPB_STREAM_BATCH_SIZE ? BPB_STREAM_BATCH_SIZE : nSize;
			if (!RawReadData(ReadBuffer, sizeof(m_Header) + GetOffset(), nRead, m_Header.dwBPBBlockPerByte, Error)) {
				break;
			}
			m_Cipher.DecryptBlock(ReadBuffer, p, nRead, GetOffset() + sizeof(m_Header));
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
	BYTE WriteBuffer[BPB_STREAM_BATCH_SIZE];
	LPBYTE p = (LPBYTE)pBuffer;
	ASSERT(nSize + GetOffset() <= GetMediaTotalBytes() && pBuffer != NULL);
	if (nSize + GetOffset() <= GetMediaTotalBytes() && pBuffer != NULL) {
		while (nSize > 0) {
			nWrite = nSize > BPB_STREAM_BATCH_SIZE ? BPB_STREAM_BATCH_SIZE : nSize;
			m_Cipher.EncryptBlock(p, WriteBuffer, nWrite, GetOffset() + sizeof(m_Header));
			if (!RawWriteData(WriteBuffer, sizeof(m_Header) + GetOffset(), nWrite, m_Header.dwBPBBlockPerByte, Error)) {
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

BOOL CBytePerBlockMedia::Seek(INT nOffset, CStreamBase::SEEK_TYPE_T Org, CPackErrors& Error)
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

BOOL CBytePerBlockMedia::SetMediaUsedBytes(UINT nSize, CPackErrors& Error)
{
	ASSERT(nSize <= GetMediaTotalBytes());
	if (nSize <= GetMediaTotalBytes()) {
		m_Header.BPBHeader.dwDataSize = nSize;
		return TRUE;
	} else {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
}

UINT CBytePerBlockMedia::GetMediaUsedBytes()
{
	return m_Header.BPBHeader.dwDataSize;
}

UINT CBytePerBlockMedia::GetMediaTotalBytes()
{
	ASSERT(GetTotalBlocks() / m_Header.dwBPBBlockPerByte >= sizeof(m_Header));
	if(GetTotalBlocks() / m_Header.dwBPBBlockPerByte >= sizeof(m_Header))
		return GetTotalBlocks() / m_Header.dwBPBBlockPerByte - sizeof(m_Header);
	return 0;
}


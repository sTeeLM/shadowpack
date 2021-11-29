#include "StdAfx.h"
#include "CipherStream.h"

CCipherStream::CCipherStream(CStreamBase * pLower):
m_pLower(pLower)
{

}

CCipherStream::~CCipherStream(void)
{

}

BOOL CCipherStream::Read(LPVOID pBuffer, UINT nSize, CProgress & Progress, CPackErrors & Error)
{
	UINT nOffset = m_pLower->GetOffset();
	BOOL bRet = m_pLower->Read(pBuffer, nSize, Progress, Error);

	if(bRet) {
		m_Cipher.DecryptBlock(pBuffer, (size_t)nSize, nOffset);
	}
	return bRet;
}

BOOL CCipherStream::Write(const LPVOID pBuffer, UINT nSize, CProgress & Progress, CPackErrors & Error)
{
	BOOL bRet = FALSE;
	BYTE Buffer[4096], *p = (LPBYTE)pBuffer;
	UINT nLeft = nSize, nBatch;
	UINT nOffset = 0L;

	if(m_Cipher.GetCipherType() == CPackCipher::CIPHER_NONE) {
		bRet =  m_pLower->Write(pBuffer, nSize, Progress, Error);
	} else {
		bRet = TRUE;
		while(nLeft) {
			nBatch =  nLeft > sizeof(Buffer) ? sizeof(Buffer) : nLeft;
			memcpy(Buffer, p + (nSize - nLeft), (size_t)nBatch);
			nOffset = m_pLower->GetOffset();
			m_Cipher.EncryptBlock(Buffer, (size_t)nBatch, nOffset);
			if(!m_pLower->Write(Buffer, nBatch, Progress, Error)) {
				bRet = FALSE;
				break;
			}
			nLeft -= nBatch;
		}
	}

	return bRet;
}


BOOL CCipherStream::Close(CPackErrors & Error)
{
	return m_pLower->Close(Error);
}

BOOL CCipherStream::Seek(LONGLONG nOffset, INT nOrg, CPackErrors & Error)
{
	return m_pLower->Seek(nOffset, nOrg, Error);
}

UINT CCipherStream::GetOffset()
{
	return m_pLower->GetOffset();
}

BOOL CCipherStream::SetCipher(CPackCipher::pack_cipher_type_t nType, LPCTSTR strPassword)
{
	return m_Cipher.SetKeyType(nType, strPassword);
}
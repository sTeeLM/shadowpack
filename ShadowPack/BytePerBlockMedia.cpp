#include "pch.h"
#include "BytePerBlockMedia.h"

CBytePerBlockMedia::CBytePerBlockMedia():
	m_pBlockBuffer(NULL),
	m_nBlockBufferSize(0)
{
}

CBytePerBlockMedia::~CBytePerBlockMedia()
{

}

void CBytePerBlockMedia::Free()
{
	if (m_pBlockBuffer) {
		delete[]m_pBlockBuffer;
		m_pBlockBuffer = NULL;
		m_nBlockBufferSize = 0;
	}
}

void CBytePerBlockMedia::SetBlocks(UINT nBlockOffset, CBlockBase* pBlock, UINT nBlockCnt)
{
	ASSERT(nBlockOffset + nBlockCnt <= m_nBlockBufferSize);
	for (INT i = 0; i < nBlockCnt; i++) {
		m_pBlockBuffer[nBlockOffset + i].CopyFrom(pBlock + i);
	}
}

void CBytePerBlockMedia::GetBlocks(UINT nBlockOffset, CBlockBase* pBlock, UINT nBlockCnt)
{
	ASSERT(nBlockOffset + nBlockCnt <= m_nBlockBufferSize);
	for (INT i = 0; i < nBlockCnt; i++) {
		pBlock[i].CopyFrom(m_pBlockBuffer + nBlockOffset + i);
	}
}


BOOL CBytePerBlockMedia::LoadMeta(CPasswordGetterBase& PasswordGetter, CPackErrors& Errors)
{
	return 0;
}

BOOL CBytePerBlockMedia::SaveMeta(CPackErrors& Errors)
{
	return 0;
}

BOOL CBytePerBlockMedia::Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error)
{
	return 0;
}

BOOL CBytePerBlockMedia::Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error)
{
	return 0;
}

BOOL CBytePerBlockMedia::Seek(INT nOffset, INT nOrg, CPackErrors& Error)
{
	return 0;
}

UINT CBytePerBlockMedia::GetOffset()
{
	return 0;
}

CStreamBase* CBytePerBlockMedia::GetStream()
{
	return nullptr;
}

UINT CBytePerBlockMedia::GetMediaUsedBytes()
{
	return 0;
}

UINT CBytePerBlockMedia::GetMediaTotalBytes()
{
	return 0;
}

BOOL CBytePerBlockMedia::IsMediaDirty()
{
	return 0;
}

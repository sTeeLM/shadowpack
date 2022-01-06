#include "pch.h"
#include "PCMAudioMedia.h"
#include "PackUtils.h"
#include "resource.h"

CPCMAudioMedia::CPCMAudioMedia() :
	m_bUseFileCache(FALSE),
	m_pSampleBuffer(NULL),
	m_nBitsPerSample(0),
	m_nSampleCnt(0),
	m_nChannels(0),
	m_OptPCMAudioMedia(NULL, IDS_OPT_PCM_MEDIA)
{
}

CPCMAudioMedia::~CPCMAudioMedia()
{

}

void CPCMAudioMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
	LARGE_INTEGER li;
	m_OptPCMAudioMedia.m_nCrypto = m_Header.dwBPBCipher;
	m_OptPCMAudioMedia.m_nBytePerBlock = m_Header.dwBPBBlockPerByte - 1;
	m_OptPCMAudioMedia.m_strPasswd1 = m_OptPCMAudioMedia.m_strPasswd2
		= m_Cipher.GetPassword();
	m_OptPCMAudioMedia.m_nTotalBlocks = GetTotalBlocks();
	m_OptPCMAudioMedia.m_nHeaderSize = sizeof(m_Header);
	li.HighPart = m_Header.BPBHeader.dwDataSizeHi;
	li.LowPart = m_Header.BPBHeader.dwDataSizeLow;
	m_OptPCMAudioMedia.m_nUsedBytes = li.QuadPart;
	m_OptPCMAudioMedia.m_strBlockUnit.LoadString(IDS_OPT_PCM_MEDIA_BLOCK_UNIT);
	pPropertySheet->AddPage(&m_OptPCMAudioMedia);
}

BOOL CPCMAudioMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	BOOL bDirty = FALSE;
	if (m_OptPCMAudioMedia.m_nCrypto != m_Header.dwBPBCipher) {
		TRACE(_T("m_nCrypto change from %d to %d\n"), m_Header.dwBPBCipher, m_OptPCMAudioMedia.m_nCrypto);
		m_Header.dwBPBCipher = m_OptPCMAudioMedia.m_nCrypto;
		m_Cipher.SetKeyType((CPackCipher::PACK_CIPHER_TYPE_T)m_Header.dwBPBCipher, m_OptPCMAudioMedia.m_strPasswd1);
		bDirty = TRUE;
	}

	if (m_OptPCMAudioMedia.m_nBytePerBlock >= 0 && m_OptPCMAudioMedia.m_nBytePerBlock <= 3 &&
		m_OptPCMAudioMedia.m_nBytePerBlock != m_Header.dwBPBBlockPerByte - 1) {
		TRACE(_T("m_nBytePerPixel change from %d to %d\n"), m_Header.dwBPBBlockPerByte, m_OptPCMAudioMedia.m_nBytePerBlock + 1);
		m_Header.dwBPBBlockPerByte = m_OptPCMAudioMedia.m_nBytePerBlock + 1;
		bDirty = TRUE;
	}

	return bDirty;
}

BOOL CPCMAudioMedia::Alloc(ULONGLONG nFrames, UINT nChannels, UINT nBitsPerSample, CPackErrors& Error)
{
	ULONGLONG nSize = nBitsPerSample;
	BOOL bRet;

	if (nSize < 32)
		nSize = 4;
	else
		nSize = nBitsPerSample / 8;

	nSize *= (nFrames * nChannels);

	if (nSize >= 4294967296L) {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if ((m_pSampleBuffer = (LPBYTE)malloc(nSize)) == NULL) {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}
	m_nSampleCnt = nFrames * nChannels;
	m_nBitsPerSample = nBitsPerSample;
	m_nChannels = nChannels;
	bRet = TRUE;
err:
	return bRet;
}

void CPCMAudioMedia::Free()
{
	if (m_pSampleBuffer) {
		free(m_pSampleBuffer);
		m_pSampleBuffer = NULL;
	}
	m_nBitsPerSample = 0;
	m_nSampleCnt = 0;
	m_nChannels = 0;
}

void CPCMAudioMedia::SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt)
{
	ULONGLONG nOffset;
	UINT nSize;

	if (m_nBitsPerSample <= 32) {
		nOffset = nFrameOffset * m_nChannels * 4;
		nSize = nFrameCnt * m_nChannels * 4;
	}
	else {
		nOffset = nFrameOffset * m_nChannels * (m_nBitsPerSample / 8);
		nSize = nFrameCnt * m_nChannels * (m_nBitsPerSample / 8);
	}

	memcpy(m_pSampleBuffer + nOffset, pBuffer, nSize);
}

void CPCMAudioMedia::GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt)
{
	ULONGLONG nOffset;
	UINT nSize;

	if (m_nBitsPerSample <= 32) {
		nOffset = nFrameOffset * m_nChannels * 4;
		nSize = nFrameCnt * m_nChannels * 4;
	}
	else {
		nOffset = nFrameOffset * m_nChannels * (m_nBitsPerSample / 8);
		nSize = nFrameCnt * m_nChannels * (m_nBitsPerSample / 8);
	}

	memcpy(pBuffer, m_pSampleBuffer + nOffset, nSize);
}

/*
m_nBitsPerSample:
8  : m_nSampleCnt / 3;
16 : m_nSampleCnt / 2;
20 : m_nSampleCnt / 2;
24 : m_nSampleCnt
32 : m_nSampleCnt
64 : m_nSampleCnt
*/

ULONGLONG CPCMAudioMedia::GetTotalBlocks()
{
	if (m_nBitsPerSample == 8) {
		return m_nSampleCnt / 3;
	}
	else if (m_nBitsPerSample == 16 || m_nBitsPerSample == 20) {
		return m_nSampleCnt / 2;
	}
	else {
		return m_nSampleCnt;
	}
}

BYTE CPCMAudioMedia::GetByteFromBlocks(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	switch(m_nBitsPerSample) {
		case 8:
			return GetByteFromBlocks8(nBlockOffset, nBlockPerByte);
		case 16:
			return GetByteFromBlocks16(nBlockOffset, nBlockPerByte);
		case 20:
			return GetByteFromBlocks20(nBlockOffset, nBlockPerByte);
		case 24:
			return GetByteFromBlocks24(nBlockOffset, nBlockPerByte);
		case 32:
			return GetByteFromBlocks32(nBlockOffset, nBlockPerByte);
		case 64:
			return GetByteFromBlocks64(nBlockOffset, nBlockPerByte);
	}
	return 0;
}

// 每个sample 4 字节，最高字节有数据，其他为0，一个block 3 个 sample
BYTE CPCMAudioMedia::GetByteFromBlocks8(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	ULONGLONG nByteOffset = nBlockOffset  * nBlockPerByte * 4 * 3;
	LPBYTE p = m_pSampleBuffer + nByteOffset;
	BYTE nRet = 0;
	BYTE nTarget = 0;
	if (nBlockPerByte == 1) { /* 0: 3, 1: 2, 2: 3 */
		ASSERT(nByteOffset + 12 < m_nSampleCnt * 4);
		nRet = p[3] & 0x7;
		nRet <<= 2;
		nRet |= p[7] & 0x3;
		nRet <<= 3;
		nRet |= p[11] & 0x7;
	} else if (nBlockPerByte == 2) { /* 0：2，1: 1， 2: 1| 2：2， 3: 1， 4: 1*/
		nRet = p[3] & 0x3;
		nRet <<= 1;
		nRet |= p[7] & 0x1;
		nRet <<= 1;
		nRet |= p[11] & 0x1;
		nRet <<= 2;

		nRet |= p[15] & 0x3;
		nRet <<= 1;
		nRet |= p[19] & 0x1;
		nRet <<= 1;
		nRet |= p[23] & 0x1;
	} else if (nBlockPerByte == 3) { /* 0：1，1: 1， 2: 1|2：1，3: 1， 4: 1|5：1，6：0，7：1*/
		nRet = p[3] & 0x1;
		nRet <<= 1;
		nRet |= p[7] & 0x1;
		nRet <<= 1;
		nRet |= p[11] & 0x1;
		nRet <<= 1;

		nRet |= p[15] & 0x1;
		nRet <<= 1;
		nRet |= p[19] & 0x1;
		nRet <<= 1;
		nRet |= p[23] & 0x1;
		nRet <<= 1;

		nRet |= p[27] & 0x1;
		nRet <<= 1;
		nRet |= p[35] & 0x1;
	} else if (nBlockPerByte == 4) {
		/* F5 algo , every pixel offer 3 bits as target hide 2 bits data, max 1 bit of target changed */
		for (INT i = 0; i < 4; i++) {
			nRet <<= 2;
			nTarget = p[3 + i * 12] & 0x1;
			nTarget <<= 1;
			nTarget |= p[7 + i * 12] & 0x1;
			nTarget <<= 1;
			nTarget |= p[11 + i * 12] & 0x1;
			nRet |= CPackUtils::F5RevLookupTable[nTarget];
		}
	}
	return nRet;
}

void CPCMAudioMedia::SetByteFromBlocks8(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	ULONGLONG nByteOffset = nBlockOffset * nBlockPerByte * 4 * 3;
	LPBYTE p = m_pSampleBuffer + nByteOffset;
	BYTE nTarget = 0;
	BYTE nRet = 0;
	BYTE nRes;
	if (nBlockPerByte == 1) { /* 0: 3, 1: 2, 2: 3 */
		ASSERT(nByteOffset + 12 < m_nSampleCnt * 4);
		p[3] &= ~0x7;
		p[3] |= ((nData >> 5) & 0x7);

		p[7] &= ~0x3;
		p[7] |= ((nData >> 3) & 0x3);

		p[11] &= ~0x7;
		p[11] |= (nData & 0x7);
	}
	else if (nBlockPerByte == 2) { /* 0：2，1: 1， 2: 1| 2：2， 3: 1， 4: 1 */
		p[3] &= ~0x3;
		p[3] |= ((nData >> 6) & 0x3);

		p[7] &= ~0x1;
		p[7] |= ((nData >> 5) & 0x1);

		p[11] &= ~0x1;
		p[11] |= ((nData >> 4) & 0x1);


		p[15] &= ~0x3;
		p[15] |= ((nData >> 2) & 0x3);

		p[19] &= ~0x1;
		p[19] |= ((nData >> 1) & 0x1);

		p[23] &= ~0x1;
		p[23] |= (nData & 0x1);
	}
	else if (nBlockPerByte == 3) {
		p[3] &= ~0x1;
		p[3] |= ((nData >> 7) & 0x1);

		p[7] &= ~0x1;
		p[7] |= ((nData >> 6) & 0x1);

		p[11] &= ~0x1;
		p[11] |= ((nData >> 5) & 0x1);

		p[15] &= ~0x1;
		p[15] |= ((nData >> 4) & 0x1);

		p[19] &= ~0x1;
		p[19] |= ((nData >> 3) & 0x1);

		p[23] &= ~0x1;
		p[23] |= ((nData >> 2) & 0x1);

		p[27] &= ~0x1;
		p[27] |= ((nData >> 1) & 0x1);

		p[35] &= ~0x1;
		p[35] |= (nData & 0x1);
	}
	else if (nBlockPerByte == 4) {
		nRet = 0xC0;
		for (INT i = 0; i < 4; i++) {
			nTarget = p[3 + i * 12] & 0x1;
			nTarget <<= 1;
			nTarget |= p[7 + i * 12] & 0x1;
			nTarget <<= 1;
			nTarget |= p[11 + i * 12] & 0x1;
			nRes = CPackUtils::F5LookupTable[((nData & nRet) >> ((3 - i) * 2))][nTarget];
			if (nRes == 4) {
				p[3 + i * 12] ^= 0x1;
			}
			else if (nRes == 2) {
				p[7 + i * 12] ^= 0x1;
			}
			else if (nRes == 1) {
				p[11 + i * 12] ^= 0x1;
			}
			nRet >>= 2;
		}
	}
}

BYTE CPCMAudioMedia::GetByteFromBlocks16(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	return 0;
}

BYTE CPCMAudioMedia::GetByteFromBlocks20(ULONGLONG nOffset, UINT nBlockPerByte)
{
	return 0;
}

BYTE CPCMAudioMedia::GetByteFromBlocks24(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	return 0;
}

BYTE CPCMAudioMedia::GetByteFromBlocks32(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	return 0;
}

BYTE CPCMAudioMedia::GetByteFromBlocks64(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	return 0;
}

void CPCMAudioMedia::SetByteToBlocks(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	switch (m_nBitsPerSample) {
	case 8:
		return SetByteFromBlocks8(nData, nBlockOffset, nBlockPerByte);
	case 16:
		return SetByteFromBlocks16(nData, nBlockOffset, nBlockPerByte);
	case 20:
		return SetByteFromBlocks20(nData, nBlockOffset, nBlockPerByte);
	case 24:
		return SetByteFromBlocks24(nData, nBlockOffset, nBlockPerByte);
	case 32:
		return SetByteFromBlocks32(nData, nBlockOffset, nBlockPerByte);
	case 64:
		return SetByteFromBlocks64(nData, nBlockOffset, nBlockPerByte);
	}
}




void CPCMAudioMedia::SetByteFromBlocks16(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte)
{
}

void CPCMAudioMedia::SetByteFromBlocks20(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte)
{
}

void CPCMAudioMedia::SetByteFromBlocks24(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte)
{
}

void CPCMAudioMedia::SetByteFromBlocks32(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte)
{
}

void CPCMAudioMedia::SetByteFromBlocks64(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte)
{
}


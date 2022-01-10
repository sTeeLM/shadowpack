#include "pch.h"
#include "PCMAudioMedia.h"
#include "PackUtils.h"
#include "ConfigManager.h"
#include "ShadowPack.h"
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
	BOOL bRet = FALSE;

	CConfigManager::CONFIG_VALUE_T val;

	if (theApp.m_Config.GetConfig(_T("media"), _T("media_use_hd_cache"), val)) {
		m_bUseFileCache = val.n8;
	}

	if (m_pSampleBuffer) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if (nSize < 32)
		nSize = 4;
	else
		nSize = nBitsPerSample / 8;

	nSize *= (nFrames * nChannels);

	if (m_bUseFileCache) {
		m_pSampleBuffer = (LPBYTE)m_FileCache.Alloc(nSize, Error);
	} else {
		if (nSize >= 4294967296L) {
			Error.SetError(CPackErrors::PE_NOMEM);
			goto err;
		}
		m_pSampleBuffer = new (std::nothrow) BYTE[nSize];
		if (!m_pSampleBuffer) {
			Error.SetError(CPackErrors::PE_NOMEM);
		}
	}

	if (!m_pSampleBuffer) {
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
		if (m_bUseFileCache) {
			m_FileCache.Free(m_pSampleBuffer);
		}
		else {
			free(m_pSampleBuffer);
		}
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

void CPCMAudioMedia::SetSample(LPBYTE* pBuffer, UINT nLineSize, SAMPLE_FORMAT_T Format, INT& nSamples, INT nChannels)
{
	switch (Format) {
	SAMPLE_FMT_U8:          ///< unsigned 8 bits
	SAMPLE_FMT_S16:         ///< signed 16 bits
	SAMPLE_FMT_S32:         ///< signed 32 bits
	SAMPLE_FMT_FLT:         ///< float
	SAMPLE_FMT_DBL:         ///< double
	SAMPLE_FMT_S64:         ///< signed 64 bits

	SAMPLE_FMT_U8P:         ///< unsigned 8 bits, planar
	SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
	SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
	SAMPLE_FMT_FLTP:        ///< float, planar
	SAMPLE_FMT_DBLP:        ///< double, planar
	SAMPLE_FMT_S64P:        ///< signed 64 bits, planar
		break;
	}

}

void CPCMAudioMedia::GetSample(LPBYTE* pBuffer, UINT nLineSize, SAMPLE_FORMAT_T Format, INT& nSamples, INT nChannels)
{
	switch (Format) {
	SAMPLE_FMT_U8:          ///< unsigned 8 bits
	SAMPLE_FMT_S16:         ///< signed 16 bits
	SAMPLE_FMT_S32:         ///< signed 32 bits
	SAMPLE_FMT_FLT:         ///< float
	SAMPLE_FMT_DBL:         ///< double
	SAMPLE_FMT_U8P:         ///< unsigned 8 bits, planar
	SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
	SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
	SAMPLE_FMT_FLTP:        ///< float, planar
	SAMPLE_FMT_DBLP:        ///< double, planar
	SAMPLE_FMT_S64:         ///< signed 64 bits
	SAMPLE_FMT_S64P:        ///< signed 64 bits, planar
		break;
	}
}

/*
m_nBitsPerSample:
8  : m_nSampleCnt / 3;
16 : m_nSampleCnt / 2;
20 : m_nSampleCnt / 2;
24 : m_nSampleCnt / 2
32 : m_nSampleCnt
64 : m_nSampleCnt
*/

ULONGLONG CPCMAudioMedia::GetTotalBlocks()
{
	if (m_nBitsPerSample == 8) {
		return m_nSampleCnt / 3;
	}
	else if (m_nBitsPerSample == 16 || m_nBitsPerSample == 20 || m_nBitsPerSample == 24) {
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
		case 20:
		case 24:
			return GetByteFromBlocks162024(nBlockOffset, nBlockPerByte);
		case 32:
		case 64:
			return GetByteFromBlocks3264(nBlockOffset, nBlockPerByte);
	}
	return 0;
}

void CPCMAudioMedia::SetByteToBlocks(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	switch (m_nBitsPerSample) {
	case 8:
		return SetByteFromBlocks8(nData, nBlockOffset, nBlockPerByte);
	case 16:
	case 20:
	case 24:
		return SetByteFromBlocks162024(nData, nBlockOffset, nBlockPerByte);
	case 32:
	case 64:
		return SetByteFromBlocks3264(nData, nBlockOffset, nBlockPerByte);
	}
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
	BYTE nMask = 0;
	BYTE nRes;
	if (nBlockPerByte == 1) { /* 0: 3, 1: 2, 2: 3 */
		ASSERT(nByteOffset + 12 < m_nSampleCnt * 4);
		p[3] &= ~0x7;
		p[3] |= ((nData >> 5) & 0x7);

		p[7] &= ~0x3;
		p[7] |= ((nData >> 3) & 0x3);

		p[11] &= ~0x7;
		p[11] |= (nData & 0x7);
	} else if (nBlockPerByte == 2) { /* 0：2，1: 1， 2: 1| 2：2， 3: 1， 4: 1 */
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
	} else if (nBlockPerByte == 3) {
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
	} else if (nBlockPerByte == 4) {
		nMask = 0xC0;
		for (INT i = 0; i < 4; i++) {
			nTarget = p[3 + i * 12] & 0x1;
			nTarget <<= 1;
			nTarget |= p[7 + i * 12] & 0x1;
			nTarget <<= 1;
			nTarget |= p[11 + i * 12] & 0x1;
			nRes = CPackUtils::F5LookupTable[((nData & nMask) >> ((3 - i) * 2))][nTarget];
			if (nRes == 4) {
				p[3 + i * 12] ^= 0x1;
			}
			else if (nRes == 2) {
				p[7 + i * 12] ^= 0x1;
			}
			else if (nRes == 1) {
				p[11 + i * 12] ^= 0x1;
			}
			nMask >>= 2;
		}
	}
}

// 16： 每个sample 4 字节，最高2字节有数据，其他为0，一个block 2 个 sample
// 20： 每个sample 4 字节，最高20位有数据，其他为0，一个block 2 个 sample
// 24： 每个sample 4 字节，最高3字节有数据，其他为0，一个block 2 个 sample
BYTE CPCMAudioMedia::GetByteFromBlocks162024(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	ULONGLONG nByteOffset = nBlockOffset * nBlockPerByte * 4 * 2;
	LPBYTE p = m_pSampleBuffer + nByteOffset;
	BYTE nRet = 0;
	ULONGLONG nTarget = 0;
	ULONGLONG nMask = 0;
	UINT nIndexShift = (m_nBitsPerSample == 16) ? 2 : 1;
	UINT nMaskShift = (m_nBitsPerSample == 20) ? 4 : 0;
	if (nBlockPerByte == 1) {
		for (UINT i = 0; i < 2; i++) { /* 1 byte in 2 bits block, 4 | 4 */
			nRet <<= 4;
			nRet |= (p[nIndexShift + i * 4] & (0xF << nMaskShift)) >> nMaskShift;
		}
	} else if (nBlockPerByte == 2) { /* 1 byte in 4 16 bits , 2 | 2 | 2 | 2 */
		for (INT i = 0; i < 4; i++) {
			nRet <<= 2;
			nRet |= (p[nIndexShift + i * 4] & (0x3 << nMaskShift)) >> nMaskShift;
		}
	} else if (nBlockPerByte == 3) { /* F5算法，每一个16bis 的最后两位作为target */
		for (UINT i = 0; i < 6; i++) {
			nTarget <<= 2;
			nTarget |= (p[nIndexShift + i * 4] & (0x3 << nMaskShift)) >> nMaskShift;
		}
		nMask = 0xE00L;
		for (UINT i = 0; i < 4; i++) {
			nRet <<= 2;
			nRet |= CPackUtils::F5RevLookupTable[(BYTE)((nTarget & (nMask >> (i * 3))) >> (9 - i * 3))];
		}
	} else if (nBlockPerByte == 4) { /* every 16bit use last 1 bits */
		for (UINT i = 0; i < 8; i++) {
			nRet <<= 1;
			nRet |= (p[nIndexShift + i * 4] & (0x1 << nMaskShift)) >> nMaskShift;
		}
	}
	return nRet;
}

void CPCMAudioMedia::SetByteFromBlocks162024(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	ULONGLONG nByteOffset = nBlockOffset * nBlockPerByte * 4 * 2;
	LPBYTE p = m_pSampleBuffer + nByteOffset;
	ULONGLONG nTarget = 0;
	ULONGLONG nMask = 0;
	BYTE nMask1 = 0;
	UINT nIndexShift = (m_nBitsPerSample == 16) ? 2 : 1;
	UINT nMaskShift = (m_nBitsPerSample == 20) ? 4 : 0;
	BYTE nRes;
	if (nBlockPerByte == 1) {
		for (UINT i = 0; i < 2; i++) {  /* 1 byte in 2 bits block, 4 | 4 */
			p[nIndexShift + i * 4] &= ~(0xF << nMaskShift);
			p[nIndexShift + i * 4] |= ((nData >> (4 - i * 4)) & 0xF) << nMaskShift;
		}
	}
	else if (nBlockPerByte == 2) {
		for (INT i = 0; i < 4; i++) { /* 1 byte in 4 16 bits , 2 | 2 | 2 | 2 */
			p[nIndexShift + i * 4] &= ~(0x3 << nMaskShift);
			p[nIndexShift + i * 4] |= ((nData >> (6 - i * 2)) & 0x3) << nMaskShift;
		}
	} else if (nBlockPerByte == 3) { 
		for (UINT i = 0; i < 6; i++) {
			nTarget <<= 2;
			nTarget |= (p[nIndexShift + i * 4] & (0x3 << nMaskShift)) >> nMaskShift;
		}
		nMask = 0xE00L;
		nMask1 = 0xC0;
		for (UINT i = 0; i < 4; i+=2) {
			nRes = CPackUtils::F5LookupTable[((nData & (nMask1 >> (2 * i))) >> ((3 - i) * 2))]
				[(BYTE)((nTarget & (nMask >> (i * 3))) >> (9 - i * 3))];
			if (nRes == 4) {
				p[nIndexShift + i * 6] ^= (0x2 << nMaskShift);
			}
			else if (nRes == 2) {
				p[nIndexShift + i * 6] ^= (0x1 << nMaskShift);
			}
			else if (nRes == 1) {
				p[nIndexShift + 4 + i * 6] ^= (0x2 << nMaskShift);
			}
		}
		for (UINT i = 1; i < 4; i+=2) {
			nRes = CPackUtils::F5LookupTable[((nData & (nMask1 >> (2 * i) )) >> ((3 - i) * 2))]
				[(BYTE)((nTarget & (nMask >> (i * 3))) >> (9 - i * 3))];
			if (nRes == 4) {
				p[nIndexShift - 2 + i * 6] ^= (0x1 << nMaskShift);
			}
			else if (nRes == 2) {
				p[nIndexShift + 2 + i * 6] ^= (0x2 << nMaskShift); // 2:4, 1:3
			}
			else if (nRes == 1) {
				p[nIndexShift + 2 + i * 6] ^= (0x1 << nMaskShift);
			}
		}
	} else if (nBlockPerByte == 4) { /* every 16bit use last 1 bits */
		for (INT i = 0; i < 8; i++) {
			p[nIndexShift + i * 4] &= ~(0x1 << nMaskShift);
			p[nIndexShift + i * 4] |= ((nData >> (7 - i)) & 0x1) << nMaskShift;
		}
	}
}

// 32： 每个sample 4 字节, 一个block 1 个 sample
// 64： 每个sample 8 字节, 一个block 1 个 sample
BYTE CPCMAudioMedia::GetByteFromBlocks3264(ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	UINT nSampleSizeByte = m_nBitsPerSample / 8;
	ULONGLONG nByteOffset = nBlockOffset * nBlockPerByte * nSampleSizeByte;
	LPBYTE p = m_pSampleBuffer + nByteOffset;
	BYTE nRet = 0;
	UINT nTarget;
	if (nBlockPerByte == 1) {
		nRet = p[0];
	} else if (nBlockPerByte == 2) {
		for (UINT i = 0; i < 2; i++) {
			nRet <<= 4;
			nRet |= p[i * nSampleSizeByte] & 0xF;
		}
	} else if (nBlockPerByte == 3) {
		for (UINT i = 0; i < 4; i++) {
			nRet <<= 2;
			nTarget = 0;
			for (UINT j = 0; j < 3; j++) {
				nTarget <<= 1;
				nTarget |= (p[j * nSampleSizeByte] >> (3 - i)) & 0x1;
			}
			nRet |= CPackUtils::F5RevLookupTable[nTarget];
		}

	} else if (nBlockPerByte == 4) {
		for (UINT i = 0; i < 4; i++) {
			nRet <<= 2;
			nRet |= p[i * nSampleSizeByte] & 0x3;
		}
	}

	return nRet;
}

void CPCMAudioMedia::SetByteFromBlocks3264(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte)
{
	UINT nSampleSizeByte = m_nBitsPerSample / 8;
	ULONGLONG nByteOffset = nBlockOffset * nBlockPerByte * nSampleSizeByte;
	LPBYTE p = m_pSampleBuffer + nByteOffset;
	UINT nTarget;
	UINT nRes;
	if (nBlockPerByte == 1) {
		p[0] = nData;
	} else if (nBlockPerByte == 2) {
		for (UINT i = 0; i < 2; i++) {
			p[i * nSampleSizeByte] &= ~0xF;
			p[i * nSampleSizeByte] |= (nData >> ((1 - i) * 4)) & 0xF;
		}
	} else if (nBlockPerByte == 3) {
		for (UINT i = 0; i < 4; i++) {
			nTarget = 0;
			for (UINT j = 0; j < 3; j++) {
				nTarget <<= 1;
				nTarget |= (p[j * nSampleSizeByte] >> (3 - i)) & 0x1;
			}
			nRes = CPackUtils::F5LookupTable[((nData >> ((3 - i) * 2)) & 0x3)][nTarget];
			if (nRes == 4) {
				p[0 * nSampleSizeByte] ^= (0x1 << (3 - i));
			} else if (nRes == 2) {
				p[1 * nSampleSizeByte] ^= (0x1 << (3 - i));
			} else if (nRes == 1) {
				p[2 * nSampleSizeByte] ^= (0x1 << (3 - i));
			}
		}
	} else if (nBlockPerByte == 4) {
		for (UINT i = 0; i < 4; i++) {
			p[i * nSampleSizeByte] &= ~0x3;
			p[i * nSampleSizeByte] |= (nData >> ((3 - i) * 2)) & 0x3;
		}

	}
}

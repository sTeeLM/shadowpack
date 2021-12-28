#include "pch.h"
#include "PixelImageMedia.h"
#include "ShadowPack.h"
#include "ConfigManager.h"
#include "resource.h"

CPixelImageMedia::CPixelImageMedia() :
	m_nWidth(0),
	m_nHeight(0),
	m_pBlockBuffer(NULL),
	m_nEmpty(0),
	m_OptPagePixelImageMedia(NULL, IDS_OPT_PIXEL_MEDIA),
	m_bUseFileCache(FALSE)
{

}

CPixelImageMedia::~CPixelImageMedia()
{

}

void CPixelImageMedia::Free()
{
	if (m_pBlockBuffer) {
		if (m_bUseFileCache) {
			m_FileCache.Free();
		}else {
			delete[] m_pBlockBuffer;
		}
		m_bUseFileCache = FALSE;
		m_pBlockBuffer = NULL;
		m_nWidth = 0;
		m_nHeight = 0;
	}
}

BOOL CPixelImageMedia::Alloc(UINT nWidth, UINT nHeight, CPackErrors& Error)
{
	UINT nBlocks = nWidth * nHeight;
	CConfigManager::CONFIG_VALUE_T val;

	if (theApp.m_Config.GetConfig(_T("media"), _T("media_use_hd_cache"), val)) {
		m_bUseFileCache = val.n8;
	}

	if (m_pBlockBuffer) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
	if (m_bUseFileCache) {
		m_pBlockBuffer = (IMAGE_PIXEL_T *) m_FileCache.Alloc(nBlocks * sizeof(IMAGE_PIXEL_T), Error);
	}
	else {
		m_pBlockBuffer = new (std::nothrow) IMAGE_PIXEL_T[nBlocks];
		if (!m_pBlockBuffer) {
			Error.SetError(CPackErrors::PE_NOMEM);
		}
	}

	if (!m_pBlockBuffer) {
		return FALSE;
	}

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	return TRUE;
}

void CPixelImageMedia::SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue, BYTE nAlpha)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		m_pBlockBuffer[nIndex].m_nRed   = nRed;
		m_pBlockBuffer[nIndex].m_nGreen = nGreen;
		m_pBlockBuffer[nIndex].m_nBlue  = nBlue;
		m_pBlockBuffer[nIndex].m_nAlpha = nAlpha;
	}
}

void CPixelImageMedia::SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		m_pBlockBuffer[nIndex].m_nRed = nRed;
		m_pBlockBuffer[nIndex].m_nGreen = nGreen;
		m_pBlockBuffer[nIndex].m_nBlue = nBlue;;
	}
}

void CPixelImageMedia::GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue, BYTE& nAlpha)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		nRed   = m_pBlockBuffer[nIndex].m_nRed;
		nGreen = m_pBlockBuffer[nIndex].m_nGreen;
		nBlue  = m_pBlockBuffer[nIndex].m_nBlue;
		nAlpha = m_pBlockBuffer[nIndex].m_nAlpha;
	}
}

void CPixelImageMedia::GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		nRed = m_pBlockBuffer[nIndex].m_nRed;
		nGreen = m_pBlockBuffer[nIndex].m_nGreen;
		nBlue = m_pBlockBuffer[nIndex].m_nBlue;
	}
}

void CPixelImageMedia::SetScanline(UINT nY, LPBYTE pBuffer, CPixelImageMedia::PIXEL_FORMAT_T Format)
{
	ASSERT(nY < m_nHeight);
	if (nY < m_nHeight) {
		switch (Format) {
		case PIXEL_FORMAT_BGR:
			for (UINT x = 0; x < m_nWidth; x++) {
				SetPixel(x, nY, pBuffer[x * 3 + 2], pBuffer[x * 3 + 1], pBuffer[x * 3]);
			}
		break;
		case PIXEL_FORMAT_RGB:
			for (UINT x = 0; x < m_nWidth; x++) {
				SetPixel(x, nY, pBuffer[x * 3], pBuffer[x * 3 + 1], pBuffer[x * 3 + 2]);
			}
		break;
		case  PIXEL_FORMAT_RGBA:
			for (UINT x = 0; x < m_nWidth; x++) {
				SetPixel(x, nY, pBuffer[x * 4], pBuffer[x * 4 + 1], pBuffer[x * 4 + 2], pBuffer[x * 4 + 3]);
			}
		break;
		default:
			ASSERT(FALSE);
			break;
		}

	}
}

void CPixelImageMedia::GetScanline(UINT nY, LPBYTE pBuffer, CPixelImageMedia::PIXEL_FORMAT_T Format)
{
	ASSERT(nY < m_nHeight);
	if (nY < m_nHeight) {
		switch (Format) {
		case PIXEL_FORMAT_BGR:
			for (UINT x = 0; x < m_nWidth; x++) {
				GetPixel(x, nY, pBuffer[x * 3 + 2], pBuffer[x * 3 + 1], pBuffer[x * 3]);
			}
		break;
		case PIXEL_FORMAT_RGB:
			for (UINT x = 0; x < m_nWidth; x++) {
				GetPixel(x, nY, pBuffer[x * 3], pBuffer[x * 3 + 1], pBuffer[x * 3 + 2]);
			}
		break;
		case PIXEL_FORMAT_RGBA:
			for (UINT x = 0; x < m_nWidth; x++) {
				GetPixel(x, nY, pBuffer[x * 4], pBuffer[x * 4 + 1], pBuffer[x * 4 + 2], pBuffer[x * 4 + 3]);
			}
		break;
		default:
			ASSERT(FALSE);
			break;
		}

	}
}

BYTE& CPixelImageMedia::GetPixelR(UINT nX, UINT nY)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		return m_pBlockBuffer[nIndex].m_nRed;
	}
	return m_nEmpty;
}

BYTE& CPixelImageMedia::GetPixelG(UINT nX, UINT nY)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		return m_pBlockBuffer[nIndex].m_nGreen;
	}
	return m_nEmpty;
}

BYTE& CPixelImageMedia::GetPixelB(UINT nX, UINT nY)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		return m_pBlockBuffer[nIndex].m_nBlue;
	}
	return m_nEmpty;
}

BYTE& CPixelImageMedia::GetPixelA(UINT nX, UINT nY)
{
	UINT nIndex = nY * m_nWidth + nX;
	ASSERT(nIndex < GetTotalBlocks());
	if (nIndex < GetTotalBlocks()) {
		return m_pBlockBuffer[nIndex].m_nAlpha;
	}
	return m_nEmpty;
}

void CPixelImageMedia::SetScanlinePerChannel(UINT nY, LPBYTE pBuffer, CPixelImageMedia::PIXEL_FORMAT_T Format, UINT nChannel)
{
	ASSERT(nY < m_nHeight);
	if (nY < m_nHeight) {
		switch (Format) {
		case PIXEL_FORMAT_BGR:
			for (UINT x = 0; x < m_nWidth; x++) {
				(nChannel == 0 ? GetPixelB(x, nY) : (nChannel == 1 ? GetPixelG(x, nY) : GetPixelR(x, nY))) = pBuffer[x];
			}
			break;
		case PIXEL_FORMAT_RGB:
			for (UINT x = 0; x < m_nWidth; x++) {
				(nChannel == 0 ? GetPixelR(x, nY) : (nChannel == 1 ? GetPixelG(x, nY) : GetPixelB(x, nY))) = pBuffer[x];
			}
			break;
		case PIXEL_FORMAT_RGBA:
			for (UINT x = 0; x < m_nWidth; x++) {
				(nChannel == 0 ? GetPixelR(x, nY) : (nChannel == 1 ? GetPixelG(x, nY) : (nChannel == 2 ? GetPixelB(x, nY) : GetPixelA(x, nY)))) = pBuffer[x];
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

	}
}

void CPixelImageMedia::GetScanlinePerChannel(UINT nY, LPBYTE pBuffer, CPixelImageMedia::PIXEL_FORMAT_T Format, UINT nChannel)
{
	ASSERT(nY < m_nHeight);
	if (nY < m_nHeight) {
		switch (Format) {
		case PIXEL_FORMAT_BGR:
			for (UINT x = 0; x < m_nWidth; x++) {
				pBuffer[x] = (nChannel == 0 ? GetPixelB(x, nY) : (nChannel == 1 ? GetPixelG(x, nY) : GetPixelR(x, nY)));
			}
			break;
		case PIXEL_FORMAT_RGB:
			for (UINT x = 0; x < m_nWidth; x++) {
				pBuffer[x] = (nChannel == 0 ? GetPixelR(x, nY) : (nChannel == 1 ? GetPixelG(x, nY) : GetPixelB(x, nY)));
			}
			break;
		case PIXEL_FORMAT_RGBA:
			for (UINT x = 0; x < m_nWidth; x++) {
				pBuffer[x] = (nChannel == 0 ? GetPixelR(x, nY) : (nChannel == 1 ? GetPixelG(x, nY) : (nChannel == 2 ? GetPixelB(x, nY) : GetPixelA(x, nY))));
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

	}
}

UINT CPixelImageMedia::GetTotalBlocks()
{
	return m_nWidth * m_nHeight;
}

void CPixelImageMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
	m_OptPagePixelImageMedia.m_nCrypto = m_Header.dwBPBCipher;
	m_OptPagePixelImageMedia.m_nBytePerBlock = m_Header.dwBPBBlockPerByte - 1;
	m_OptPagePixelImageMedia.m_strPasswd1 = m_OptPagePixelImageMedia.m_strPasswd2
		= m_Cipher.GetPassword();
	m_OptPagePixelImageMedia.m_nTotalBlocks = GetTotalBlocks();
	m_OptPagePixelImageMedia.m_nHeaderSize = sizeof(m_Header);
	m_OptPagePixelImageMedia.m_nUsedBytes = m_Header.BPBHeader.dwDataSize;
	m_OptPagePixelImageMedia.m_strBlockUnit.LoadString(IDS_OPT_PIXEL_MEDIA_BLOCK_UNIT);
	pPropertySheet->AddPage(&m_OptPagePixelImageMedia);
}

BOOL CPixelImageMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	BOOL bDirty = FALSE;
	if (m_OptPagePixelImageMedia.m_nCrypto != m_Header.dwBPBCipher) {
		TRACE(_T("m_nCrypto change from %d to %d\n"), m_Header.dwBPBCipher, m_OptPagePixelImageMedia.m_nCrypto);
		m_Header.dwBPBCipher = m_OptPagePixelImageMedia.m_nCrypto;
		m_Cipher.SetKeyType((CPackCipher::PACK_CIPHER_TYPE_T)m_Header.dwBPBCipher, m_OptPagePixelImageMedia.m_strPasswd1);
		bDirty = TRUE;
	}

	if (m_OptPagePixelImageMedia.m_nBytePerBlock >= 0 && m_OptPagePixelImageMedia.m_nBytePerBlock <= 3 &&
			m_OptPagePixelImageMedia.m_nBytePerBlock != m_Header.dwBPBBlockPerByte - 1) {
		TRACE(_T("m_nBytePerPixel change from %d to %d\n"), m_Header.dwBPBBlockPerByte, m_OptPagePixelImageMedia.m_nBytePerBlock + 1);
		m_Header.dwBPBBlockPerByte = m_OptPagePixelImageMedia.m_nBytePerBlock + 1;
		bDirty = TRUE;
	}

	return bDirty;
}


/*
// embed 2bit data into 3 bit target
// x2,x1                     a3,a2,a1
BYTE CPixelImageMedia::CPixelBlock::F5Embed(BYTE nData, BYTE nTarget)
{
	BYTE s = 0, dig, mask;
	// cal s

	nData &= 0x3;
	nTarget &= 0x7;

	mask = nTarget;
	for (INT i = 0; i < 3; i++) {
		s ^= (mask & 0x1) * (i + 1);
		mask >>= 1;
	}

	dig = s ^ nData;

	nTarget = nTarget ^ (1 << (dig - 1));

	return nTarget;

}
// extract 2bit data from 3 bit target
BYTE CPixelImageMedia::CPixelBlock::F5Extract(BYTE nTarget) 
{
	BYTE s = 0;
	for (INT i = 0; i < 3; i++) {
		s ^= (nTarget & 0x1) * (i + 1);
		nTarget >>= 1;
	}

	return s & 0x3;
}
*/

/*

data = 0, target = 0, res = 0, res ^ target = 0, ext = 0
data = 0, target = 1, res = 0, res ^ target = 1, ext = 0
data = 0, target = 2, res = 0, res ^ target = 2, ext = 0
data = 0, target = 3, res = 7, res ^ target = 4, ext = 0
data = 0, target = 4, res = 0, res ^ target = 4, ext = 0
data = 0, target = 5, res = 7, res ^ target = 2, ext = 0
data = 0, target = 6, res = 7, res ^ target = 1, ext = 0
data = 0, target = 7, res = 7, res ^ target = 0, ext = 0

data = 1, target = 0, res = 1, res ^ target = 1, ext = 1
data = 1, target = 1, res = 1, res ^ target = 0, ext = 1
data = 1, target = 2, res = 6, res ^ target = 4, ext = 1
data = 1, target = 3, res = 1, res ^ target = 2, ext = 1
data = 1, target = 4, res = 6, res ^ target = 2, ext = 1
data = 1, target = 5, res = 1, res ^ target = 4, ext = 1
data = 1, target = 6, res = 6, res ^ target = 0, ext = 1
data = 1, target = 7, res = 6, res ^ target = 1, ext = 1

data = 2, target = 0, res = 2, res ^ target = 2, ext = 2
data = 2, target = 1, res = 5, res ^ target = 4, ext = 2
data = 2, target = 2, res = 2, res ^ target = 0, ext = 2
data = 2, target = 3, res = 2, res ^ target = 1, ext = 2
data = 2, target = 4, res = 5, res ^ target = 1, ext = 2
data = 2, target = 5, res = 5, res ^ target = 0, ext = 2
data = 2, target = 6, res = 2, res ^ target = 4, ext = 2
data = 2, target = 7, res = 5, res ^ target = 2, ext = 2

data = 3, target = 0, res = 4, res ^ target = 4, ext = 3
data = 3, target = 1, res = 3, res ^ target = 2, ext = 3
data = 3, target = 2, res = 3, res ^ target = 1, ext = 3
data = 3, target = 3, res = 3, res ^ target = 0, ext = 3
data = 3, target = 4, res = 4, res ^ target = 0, ext = 3
data = 3, target = 5, res = 4, res ^ target = 1, ext = 3
data = 3, target = 6, res = 4, res ^ target = 2, ext = 3
data = 3, target = 7, res = 3, res ^ target = 4, ext = 3

*/

BYTE CPixelImageMedia::F5LookupTable[4][8] = {
	{0,1,2,4,4,2,1,0},
	{1,0,4,2,2,4,0,1},
	{2,4,0,1,1,0,4,2},
	{4,2,1,0,0,1,2,4},
};

BYTE CPixelImageMedia::F5RevLookupTable[8] = {
  /*0,1,2,3,4,5,6,7 */
	0,1,2,3,3,2,1,0
};

BYTE CPixelImageMedia::GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte)
{
	ASSERT(nBlockPerByte <= MAX_BPB_MEDIA_BPB_SIZE && nBlockPerByte >= MIN_BPB_MEDIA_BPB_SIZE);
	IMAGE_PIXEL_T* pPixelBlock = m_pBlockBuffer; // this always is first object!
	pPixelBlock += nOffset * nBlockPerByte;
	BYTE nRet = 0;
	BYTE nTarget = 0;
	if (nBlockPerByte == 1) {          /* R: 3, G 2, B 3*/
		nRet = pPixelBlock[0].m_nRed & 0x7;
		nRet <<= 2;
		nRet |= pPixelBlock[0].m_nGreen & 0x3;
		nRet <<= 3;
		nRet |= pPixelBlock[0].m_nBlue & 0x7;
	} else if (nBlockPerByte == 2) {   /* R£º2£¬G 1£¬ B 1|R£º2£¬G 1£¬ B 1*/
		nRet = pPixelBlock[0].m_nRed & 0x3;
		nRet <<= 1;
		nRet |= pPixelBlock[0].m_nGreen & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[0].m_nBlue & 0x1;
		nRet <<= 2;

		nRet |= pPixelBlock[1].m_nRed & 0x3;
		nRet <<= 1;
		nRet |= pPixelBlock[1].m_nGreen & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[1].m_nBlue & 0x1;

	} else if (nBlockPerByte == 3) {   /* R£º1£¬G 1£¬ B 1|R£º1£¬G 1£¬ B 1|R£º1£¬G 0£¬ B 1*/
		nRet = pPixelBlock[0].m_nRed & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[0].m_nGreen & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[0].m_nBlue & 0x1;
		nRet <<= 1;

		nRet |= pPixelBlock[1].m_nRed & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[1].m_nGreen & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[1].m_nBlue & 0x1;
		nRet <<= 1;

		nRet |= pPixelBlock[2].m_nRed & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[2].m_nBlue & 0x1;
	} else if (nBlockPerByte == 4) {  
		/* F5 algo , every pixel offer 3 bits as target hide 2 bits data, max 1 bit of target changed */
		nRet = 0;
		for (INT i = 0; i < 4; i++) {
			nRet <<= 2;
			nTarget = pPixelBlock[i].m_nRed & 0x1;
			nTarget <<= 1;
			nTarget |= pPixelBlock[i].m_nGreen & 0x1;
			nTarget <<= 1;
			nTarget |= pPixelBlock[i].m_nBlue & 0x1;
			nRet |= F5RevLookupTable[nTarget];
		}
	}

	return nRet;
}

void CPixelImageMedia::SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte)
{
	ASSERT(nBlockPerByte <= MAX_BPB_MEDIA_BPB_SIZE && nBlockPerByte >= MIN_BPB_MEDIA_BPB_SIZE);
	IMAGE_PIXEL_T * pPixelBlock = m_pBlockBuffer; // this always is first object!
	pPixelBlock += nOffset * nBlockPerByte;
	BYTE nTarget = 0;
	BYTE nRet = 0;
	BYTE nRes;
	if (nBlockPerByte == 1) {          /* R: 3, G 2, B 3*/
		pPixelBlock[0].m_nRed &= ~0x7;
		pPixelBlock[0].m_nRed |= ((nData >> 5) & 0x7);

		pPixelBlock[0].m_nGreen &= ~0x3;
		pPixelBlock[0].m_nGreen |= ((nData >> 3) & 0x3);

		pPixelBlock[0].m_nBlue &= ~0x7;
		pPixelBlock[0].m_nBlue |= (nData & 0x7);

	} else if (nBlockPerByte == 2) {   /* R£º2£¬G 1£¬ B 1|R£º2£¬G 1£¬ B 1*/

		pPixelBlock[0].m_nRed &= ~0x3;
		pPixelBlock[0].m_nRed |= ((nData >> 6 ) & 0x3);

		pPixelBlock[0].m_nGreen &= ~0x1;
		pPixelBlock[0].m_nGreen |= ((nData >> 5) & 0x1);

		pPixelBlock[0].m_nBlue &= ~0x1;
		pPixelBlock[0].m_nBlue |= ((nData >> 4) & 0x1);


		pPixelBlock[1].m_nRed &= ~0x3;
		pPixelBlock[1].m_nRed |= ((nData >> 2) & 0x3);

		pPixelBlock[1].m_nGreen &= ~0x1;
		pPixelBlock[1].m_nGreen |= ((nData >> 1) & 0x1 );

		pPixelBlock[1].m_nBlue &= ~0x1;
		pPixelBlock[1].m_nBlue |= (nData & 0x1);
	} else if (nBlockPerByte == 3) {   /* R£º1£¬G 1£¬ B 1|R£º1£¬G 1£¬ B 1|R£º1£¬G 0£¬ B 1*/

		pPixelBlock[0].m_nRed &= ~0x1;
		pPixelBlock[0].m_nRed |= ((nData >> 7) & 0x1);
		 
		pPixelBlock[0].m_nGreen &= ~0x1;
		pPixelBlock[0].m_nGreen |= ((nData >> 6) & 0x1);

		pPixelBlock[0].m_nBlue &= ~0x1;
		pPixelBlock[0].m_nBlue |= ((nData >> 5) & 0x1);

		pPixelBlock[1].m_nRed &= ~0x1;
		pPixelBlock[1].m_nRed |= ((nData >> 4) & 0x1);

		pPixelBlock[1].m_nGreen &= ~0x1;
		pPixelBlock[1].m_nGreen |= ((nData >> 3) & 0x1);

		pPixelBlock[1].m_nBlue &= ~0x1;
		pPixelBlock[1].m_nBlue |= ((nData >> 2) & 0x1);

		pPixelBlock[2].m_nRed &= ~0x1;
		pPixelBlock[2].m_nRed |= ((nData >> 1) & 0x1);

		pPixelBlock[2].m_nBlue &= ~0x1;
		pPixelBlock[2].m_nBlue |= (nData & 0x1);

	} else if (nBlockPerByte == 4) {  /* F5 algo */
		/* F5 algo , every pixel offer 3 bits as target hide 2 bits data, max 1 bit of target changed */
		nRet = 0xC0;
		for (INT i = 0; i < 4; i++) {
			nTarget = pPixelBlock[i].m_nRed & 0x1;
			nTarget <<= 1;
			nTarget |= pPixelBlock[i].m_nGreen & 0x1;
			nTarget <<= 1;
			nTarget |= pPixelBlock[i].m_nBlue & 0x1;
			nRes = F5LookupTable[((nData & nRet) >> ((3 - i) * 2))][nTarget];
			if (nRes == 4) {
				pPixelBlock[i].m_nRed ^= 0x1;
			} else if (nRes == 2) {
				pPixelBlock[i].m_nGreen ^= 0x1;
			} else if (nRes == 1) { 
				pPixelBlock[i].m_nBlue ^= 0x1;
			} 
			nRet >>= 2;
		}
	}
}

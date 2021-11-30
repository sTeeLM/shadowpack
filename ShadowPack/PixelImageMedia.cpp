#include "pch.h"
#include "PixelImageMedia.h"

CPixelImageMedia::CPixelImageMedia() :
	m_nWidth(0),
	m_nHeight(0)
{

}

CPixelImageMedia::~CPixelImageMedia()
{

}

BOOL CPixelImageMedia::Alloc(UINT nBlocks, CPackErrors& Error)
{
	if (m_pBlockBuffer) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
	m_pBlockBuffer = new (std::nothrow) CPixelBlock[nBlocks];
	if (!m_pBlockBuffer) {
		Error.SetError(CPackErrors::PE_NOMEM);
		return FALSE;
	}
	m_nBlockBufferSize = nBlocks;
}

BOOL CPixelImageMedia::Alloc(UINT nWidth, UINT nHeight, CPackErrors& Error)
{
	UINT nBlocks = nWidth * nHeight;
	if (Alloc(nBlocks, Error)) {
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		return TRUE;
	}
	return FALSE;
}

void CPixelImageMedia::SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue, BYTE nAlpha)
{
	UINT nIndex = nY * m_nWidth + nX;
	CPixelBlock* pBlock = NULL;
	ASSERT(nIndex < m_nBlockBufferSize);
	if (nIndex < m_nBlockBufferSize) {
		pBlock = dynamic_cast<CPixelBlock*>(&m_pBlockBuffer[nIndex]);
		pBlock->m_nRed   = nRed;
		pBlock->m_nGreen = nGreen;
		pBlock->m_nBlue  = nBlue;
		pBlock->m_nAlpha = nAlpha;
	}
}

void CPixelImageMedia::SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue)
{
	UINT nIndex = nY * m_nWidth + nX;
	CPixelBlock* pBlock = NULL;
	ASSERT(nIndex < m_nBlockBufferSize);
	if (nIndex < m_nBlockBufferSize) {
		pBlock = dynamic_cast<CPixelBlock*>(&m_pBlockBuffer[nIndex]);
		pBlock->m_nRed = nRed;
		pBlock->m_nGreen = nGreen;
		pBlock->m_nBlue = nBlue;;
	}
}

void CPixelImageMedia::GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue, BYTE& nAlpha)
{
	UINT nIndex = nY * m_nWidth + nX;
	CPixelBlock* pBlock = NULL;
	ASSERT(nIndex < m_nBlockBufferSize);
	if (nIndex < m_nBlockBufferSize) {
		pBlock = dynamic_cast<CPixelBlock*>(&m_pBlockBuffer[nIndex]);
		nRed   = pBlock->m_nRed;
		nGreen = pBlock->m_nGreen;
		nBlue  = pBlock->m_nBlue;
		nAlpha = pBlock->m_nAlpha;
	}
}

void CPixelImageMedia::GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue)
{
	UINT nIndex = nY * m_nWidth + nX;
	CPixelBlock* pBlock = NULL;
	ASSERT(nIndex < m_nBlockBufferSize);
	if (nIndex < m_nBlockBufferSize) {
		pBlock = dynamic_cast<CPixelBlock*>(&m_pBlockBuffer[nIndex]);
		nRed = pBlock->m_nRed;
		nGreen = pBlock->m_nGreen;
		nBlue = pBlock->m_nBlue;
	}
}

void CPixelImageMedia::SetScanline(UINT nY, LPBYTE pBuffer, CPixelBlock::PIXEL_FORMAT_T Format)
{
	ASSERT(nY < m_nHeight);
	if (nY < m_nHeight) {
		switch (Format) {
		case CPixelBlock::PIXEL_FORMAT_BGR:
			for (INT x = 0; x < m_nWidth; x++) {
				SetPixel(x, nY, pBuffer[x * 3 + 2], pBuffer[x * 3 + 1], pBuffer[x * 3]);
			}
		break;
		default:
			ASSERT(FALSE);
			break;
		}

	}
}

void CPixelImageMedia::GetScanline(UINT nY, LPBYTE pBuffer, CPixelBlock::PIXEL_FORMAT_T Format)
{
	ASSERT(nY < m_nHeight);
	if (nY < m_nHeight) {
		switch (Format) {
		case CPixelBlock::PIXEL_FORMAT_BGR:
			for (INT x = 0; x < m_nWidth; x++) {
				GetPixel(x, nY, pBuffer[x * 3 + 2], pBuffer[x * 3 + 1], pBuffer[x * 3]);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}

	}
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

BYTE CPixelImageMedia::CPixelBlock::F5LookupTable[4][8] = {
	{0,0,0,7,0,7,7,7},
	{1,1,6,1,6,1,6,6},
	{2,5,2,2,5,5,2,5},
	{4,3,3,3,4,4,4,3},
};

BYTE CPixelImageMedia::CPixelBlock::F5RevLookupTable[8] = {
  /*0,1,2,3,4,5,6,7 */
	0,1,2,3,3,2,1,0
};

BYTE CPixelImageMedia::CPixelBlock::GetByteFromBlocks(CBlockBase* pBlock, UINT nBlockPerByte)
{
	ASSERT(nBlockPerByte <= 3 && nBlockPerByte >= 0);
	CPixelBlock* pPixelBlock = dynamic_cast<CPixelBlock*>(pBlock);
	BYTE nRet = 0;
	BYTE nTarget = 0;
	if (nBlockPerByte == 1) {          /* R: 3, G 2, B 2*/
		nRet = pPixelBlock[0].m_nRed & 0x7;
		nRet <<= 3;
		nRet |= pPixelBlock[0].m_nGreen & 0x3;
		nRet <<= 2;
		nRet |= pPixelBlock[0].m_nBlue & 0x3;
	} else if (nBlockPerByte == 2) {   /* R£º2£¬G 1£¬ B 1|R£º2£¬G 1£¬ B 1*/
		nRet = pPixelBlock[0].m_nRed & 0x3;
		nRet <<= 2;
		nRet |= pPixelBlock[0].m_nGreen & 0x1;
		nRet <<= 1;
		nRet |= pPixelBlock[0].m_nBlue & 0x1;
		nRet <<= 1;

		nRet |= pPixelBlock[1].m_nRed & 0x3;
		nRet <<= 2;
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
		nRet <<= 2;
		nRet |= pPixelBlock[2].m_nBlue & 0x1;
	} else if (nBlockPerByte == 4) {  
		/* F5 algo , every pixel offer 3 bits as target hide 2 bits data, max 1 bit of target changed */
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

void CPixelImageMedia::CPixelBlock::SetByteToBlocks(BYTE nData, CBlockBase* pBlock, UINT nBlockPerByte)
{
	ASSERT(nBlockPerByte <= 3 && nBlockPerByte >= 0);
	CPixelBlock* pPixelBlock = dynamic_cast<CPixelBlock*>(pBlock);
	BYTE nTarget = 0;
	BYTE nRet = 0;
	if (nBlockPerByte == 1) {          /* R: 3, G 2, B 2*/

	}
	else if (nBlockPerByte == 2) {   /* R£º2£¬G 1£¬ B 1|R£º2£¬G 1£¬ B 1*/

	}
	else if (nBlockPerByte == 3) {   /* R£º1£¬G 1£¬ B 1|R£º1£¬G 1£¬ B 1|R£º1£¬G 0£¬ B 1*/

	}
	else if (nBlockPerByte == 4) {  /* F5 algo */
		/* F5 algo , every pixel offer 3 bits as target hide 2 bits data, max 1 bit of target changed */
		
	}
}

void CPixelImageMedia::CPixelBlock::CopyFrom(const CBlockBase * pBlock)
{
	m_nAlpha = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nAlpha;
	m_nRed   = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nRed;
	m_nGreen = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nGreen;
	m_nBlue  = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nBlue;
}

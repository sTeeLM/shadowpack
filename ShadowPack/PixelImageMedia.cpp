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

BYTE CPixelImageMedia::CPixelBlock::GetByteFromBlocks(CBlockBase* pBlock, UINT nBlockPerByte)
{
	ASSERT(nBlockPerByte <= 3 && nBlockPerByte >= 0);
	if (nBlockPerByte == 1) {          /* R: 3, G 2, B 2*/

	} else if (nBlockPerByte == 2) {

	} else if (nBlockPerByte == 3) {

	}
	return 0;
}

void CPixelImageMedia::CPixelBlock::SetByteToBlocks(CBlockBase* pBlock, UINT nBlockPerByte)
{
	ASSERT(nBlockPerByte <= 3 && nBlockPerByte >= 0);
}

void CPixelImageMedia::CPixelBlock::CopyFrom(const CBlockBase * pBlock)
{
	m_nAlpha = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nAlpha;
	m_nRed   = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nRed;
	m_nGreen = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nGreen;
	m_nBlue  = (dynamic_cast<const CPixelBlock*>(pBlock))->m_nBlue;
}

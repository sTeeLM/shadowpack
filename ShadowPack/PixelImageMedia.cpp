#include "StdAfx.h"
#include "PixelImageMedia.h"

CPixelImageMedia::CPixelImageMedia(void)
{
}

CPixelImageMedia::~CPixelImageMedia(void)
{
}

CPixelImageMedia::CPixelBlock::CPixelBlock():
m_pPixel(NULL),
m_nBlockSize(0),
m_nCapicity(0)
{

}

CPixelImageMedia::CPixelBlock::~CPixelBlock()
{
	if(NULL != m_pPixel) {
		free(m_pPixel);
		m_pPixel = NULL;
	}
}

CRAWPPMedia::CMediaBlock * CPixelImageMedia::AllocBlockBuffer(CPackErrors & Error)
{
	CPixelBlock * pRet = new(std::nothrow) CPixelBlock();

	if(pRet) {
		if(pRet->ReAlloc(INIT_PIXEL_BUFFER_SIZE_IN_PIXEL, Error)) {
			return pRet;
		} else {
			delete pRet;
			pRet = NULL;
		}
	} else {
		Error.SetError(CPackErrors::PE_NOMEM);
	}

	return pRet;
}

void CPixelImageMedia::FreeBlockBuffer(CMediaBlock * pBlockBuffer)
{
	CPixelBlock * p = (CPixelBlock * )pBlockBuffer;
	delete p;
}

BOOL CPixelImageMedia::CPixelBlock::ReAlloc(ULONGLONG nSize, CPackErrors & Error)
{
	if(m_pPixel != NULL) {
		free(m_pPixel);
		m_pPixel = NULL;
	}

	m_pPixel = (CPixelBlock::image_pixel_t *)malloc((size_t)nSize * sizeof(CPixelBlock::image_pixel_t));

	m_nCapicity = nSize;
	m_nBlockSize = 0;

	if(NULL == m_pPixel) {
		Error.SetError(CPackErrors::PE_NOMEM);
	}

	return m_pPixel!= NULL;
}

BOOL CPixelImageMedia::CPixelBlock::ReadByteFromBlock(LPBYTE pBuffer, ULONGLONG nSize, 
			ULONGLONG nBlockSize, DWORD dwParam, CPackErrors & Error)
{
	BYTE r0,g0,b0; // r g b
	BYTE r1,g1,b1; // r g b
	BYTE r2,g2,b2; // r g b
	LONGLONG ratio = 1;
	LONGLONG index = 0;

	if(dwParam == PFP_1PP) {
		ratio = 1;	
	} else if(dwParam == PFP_2PP) {
		ratio = 2;
	} else if(dwParam == PFP_3PP) {
		ratio = 3;
	}

	if(nSize * ratio > nBlockSize || nBlockSize > GetCapicity()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	for(ULONGLONG i = 0 ; i < nSize * ratio; i += ratio) {
		if(dwParam == PFP_1PP) {
			r0 = Red(i) & 0x07; // 00000111
			r0 <<= 5;
			g0 = Green(i) & 0x03 ; // 00000011
			g0 <<= 3;
			b0 = Blue(i) & 0x07; // 00000111

			pBuffer[index] = r0 | g0 | b0;
			//TRACE(_T("R1[%02X][%d %d] %02X %02X %02X\n"), pBuffer[index], i, 
			//	Red(i), Green(i), Blue(i));
			index ++;
		} else if(dwParam == PFP_2PP) {
			r0 = Red(i) & 0x1;   // 00000001
			r0 <<= 7;
			g0 = Green(i) & 0x1; // 00000001
			g0 <<= 6;
			b0 = Blue(i) & 0x3;  // 00000011
			b0 <<= 4;

			r1 = Red(i + 1) & 0x3;   // 00000011
			r1 <<= 2;
			g1 = Green(i + 1) & 0x1; // 00000001
			g1 <<= 1;
			b1 = Blue(i + 1) & 0x1;  // 00000001

			pBuffer[index] = r0 | g0 | b0 | r1 | g1 | b1;
			//TRACE(_T("R2[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X \n"), 
			//	pBuffer[index], i, 
			//	color0.redQuantum(), color0.greenQuantum(), color0.blueQuantum(),
			//	x1, y1,
			//	color1.redQuantum(), color1.greenQuantum(), color1.blueQuantum());
			index ++;
		} else if (dwParam == PFP_3PP){
			r0 = Red(i) & 0x1;   // 00000001
			r0 <<= 7;
			g0 = Green(i) & 0x1; // 00000001
			g0 <<= 6;
			b0 = Blue(i) & 0x1;  // 00000001
			b0 <<= 5;

			r1 = Red(i + 1) & 0x1;   // 00000001
			r1 <<= 4;
			g1 = Green(i + 1) & 0x1; // 00000001
			g1 <<= 3;
			b1 = Blue(i + 1) & 0x1;  // 00000001
			b1 <<= 2;

			r2 = Red(i + 2) & 0x1;   // 00000001
			r2 <<= 1;
			g2 = Green(i + 2) & 0x0; // 00000000
			b2 = Blue(i + 2) & 0x1;  // 00000001

			pBuffer[index] = r0 | g0 | b0 | r1 | g1 | b1 | r2 | b2 | g2;

			//TRACE(_T("R3[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X [%d %d] %02X %02X %02X\n"), 
			//	pBuffer[index], i,
			//	color0.redQuantum(), color0.greenQuantum(), color0.blueQuantum(),
			//	x1, y1,
			//	color1.redQuantum(), color1.greenQuantum(), color1.blueQuantum(),
			//	x2, y2,
			//	color2.redQuantum(), color2.greenQuantum(), color2.blueQuantum());

			index ++;
		}
	}

	return TRUE;
}

BOOL CPixelImageMedia::CPixelBlock::WriteByteToBlock(const LPBYTE pBuffer, ULONGLONG nSize, 
				ULONGLONG nBlockSize, DWORD dwParam, CPackErrors & Error)
{
	BYTE r0,g0,b0; // r g b
	BYTE r1,g1,b1; // r g b
	BYTE r2,g2,b2; // r g b
	LONGLONG ratio = 1;
	LONGLONG index = 0;

	if(dwParam == PFP_1PP) {
		ratio = 1;	
	} else if(dwParam == PFP_2PP) {
		ratio = 2;
	} else if(dwParam == PFP_3PP) {
		ratio = 3;
	}

	if(nSize * ratio > nBlockSize|| nBlockSize > GetCapicity()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	for(ULONGLONG i = 0; i < nSize * ratio ; i += ratio) {
		if(dwParam == PFP_1PP) {
			r0 = Red(i) & 0xF8;    // 11111000
			g0 = Green(i) & 0xFC ; // 11111100
			b0 = Blue(i) & 0xF8;   // 11111000
			r0 |= (pBuffer[index] & 0xE0 ) >> 5 ; // 11100000
			g0 |= (pBuffer[index] & 0x18 ) >> 3;  // 00011000
			b0 |= (pBuffer[index] & 0x07 );       // 00000111
			Red(i) = r0;
			Green(i) = g0;
			Blue(i) = b0;

			//TRACE(_T("W1[%02X][%d %d] %02X %02X %02X\n"), pBuffer[index], i, r0, g0, b0);
			
			index ++;
		} else if(dwParam == PFP_2PP) {
			r0 = Red(i) & 0xFE;   // 11111110
			g0 = Green(i) & 0xFE; // 11111110
			b0 = Blue(i) & 0xFC;  // 11111100
			r0 |= (pBuffer[index] & 0x80 ) >> 7 ; // 10000000
			g0 |= (pBuffer[index] & 0x40 ) >> 6;  // 01000000
			b0 |= (pBuffer[index] & 0x30 ) >> 4;  // 00110000
			Red(i) = r0;
			Green(i) = g0;
			Blue(i) = b0;	

			r1 = Red(i + 1) & 0xFC;   // 11111100
			g1 = Green(i + 1) & 0xFE; // 11111110
			b1 = Blue(i + 1) & 0xFE;  // 11111110
			r1 |= (pBuffer[index] & 0x0C ) >> 2 ; // 00001100
			g1 |= (pBuffer[index] & 0x02 ) >> 1;  // 00000010
			b1 |= (pBuffer[index] & 0x01 );       // 00000001
			Red(i + 1) = r1;
			Green(i + 1) = g1;
			Blue(i + 1) = b1;

			//TRACE(_T("W2[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X\n"),  pBuffer[index], 
			//	i, r0, g0, b0,
			//	x1, y1, r1, g1, b1);

			index ++;
		} else if (dwParam == PFP_3PP){
			r0 = Red(i) & 0xFE;   // 11111110
			g0 = Green(i) & 0xFE; // 11111110
			b0 = Blue(i) & 0xFE;  // 11111110
			r0 |= (pBuffer[index] & 0x80 ) >> 7 ; // 10000000
			g0 |= (pBuffer[index] & 0x40 ) >> 6;  // 01000000
			b0 |= (pBuffer[index] & 0x20 ) >> 5;  // 00100000
			Red(i) = r0;
			Green(i) = g0;
			Blue(i) = b0;

			r1 = Red(i + 1) & 0xFE;   // 11111110
			g1 = Green(i + 1) & 0xFE; // 11111110
			b1 = Blue(i + 1) & 0xFE;  // 11111110
			r1 |= (pBuffer[index] & 0x10 ) >> 4 ; // 00010000
			g1 |= (pBuffer[index] & 0x08 ) >> 3;  // 00001000
			b1 |= (pBuffer[index] & 0x04 ) >> 2;  // 00000100
			Red(i + 1) = r1;
			Green(i + 1) = g1;
			Blue(i + 1) = b1;

			r2 = Red(i + 2) & 0xFE;   // 11111110
			g2 = Green(i + 2) & 0xFF;  // 11111111
			b2 = Blue(i + 2) & 0xFE;  // 11111110
			r2 |= (pBuffer[index] & 0x02 ) >> 1 ; // 00000010
			g2 |= (pBuffer[index] & 0x00 );       // 00000000
			b2 |= (pBuffer[index] & 0x01 );       // 00000001
			Red(i + 2) = r2;
			Green(i + 2) = g2;
			Blue(i + 2) = b2;

			//TRACE(_T("W3[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X [%d %d] %02X %02X %02X\n"),
			//	pBuffer[index],
			//	i, r0, g0, b0,
			//	x1, y1, r1, g1, b1,
			//	x2, y2, r2, g2, b2);

			index ++;
		}

	}
	return TRUE;
}
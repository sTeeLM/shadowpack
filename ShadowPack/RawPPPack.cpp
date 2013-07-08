#include "StdAfx.h"
#include "Pack.h"
#include "Resource.h"

// PF_1PP:
// r:3bits g:2bits b:3bits

// PF_2PP:
// r:1bits g 1bits b:2bits  |  r:2bits g 1bits b:1bits  

// PF_3PP:
// r:1bits g 1bits b:1bits  |  r:1bits g 1bits b:1bits  |  r:1bits g 0bits b:1bits

BOOL CPack::RawPPWriteImageInternal(CSBitmap & bmp, const LPBYTE pBuffer, size_t offset, size_t size, 
						  DWORD dwFormaParamt,  BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	size_t TotalSize = 0;
	BYTE r0,g0,b0; // r g b
	BYTE r1,g1,b1; // r g b
	BYTE r2,g2,b2; // r g b
	size_t x0, y0, x1, y1, x2, y2;
	size_t ratio = 1;
	size_t index = 0;
	INT nProgress = 0, nProgressOld = 0;

	TotalSize = bmp.Rows() * bmp.Columns();

	if(dwFormaParamt == PFP_1PP) {
		ratio = 1;	
	} else if(dwFormaParamt == PFP_2PP) {
		ratio = 2;
	} else if(dwFormaParamt == PFP_3PP) {
		ratio = 3;
	}

	if((offset + size) * ratio > TotalSize) return FALSE;

	for(size_t i = offset * ratio; i < (offset + size) * ratio ; i += ratio) {
		if(bCancel && *bCancel) {
			return FALSE;
		}
		if(NULL != fnSetProgress) {
			nProgress = i * 100 / (offset + size) * ratio;
			if(nProgressOld != nProgress) {
				fnSetProgress(nProgress);
				nProgressOld = nProgress;
			}
		}

		if(dwFormaParamt == PFP_1PP) {
			x0 = (i) % bmp.Columns();
			y0 = (i) / bmp.Columns();
			r0 = bmp.Red(x0, y0) & 0xF8;    // 11111000
			g0 = bmp.Green(x0, y0) & 0xFC ; // 11111100
			b0 = bmp.Blue(x0, y0) & 0xF8;   // 11111000
			r0 |= (pBuffer[index] & 0xE0 ) >> 5 ; // 11100000
			g0 |= (pBuffer[index] & 0x18 ) >> 3;  // 00011000
			b0 |= (pBuffer[index] & 0x07 );       // 00000111
			bmp.Red(x0, y0) = r0;
			bmp.Green(x0, y0) = g0;
			bmp.Blue(x0, y0) = b0;

			//TRACE(_T("W1[%02X][%d %d] %02X %02X %02X\n"), pBuffer[index], x0, y0, r0, g0, b0);
			
			index ++;
		} else if(dwFormaParamt == PFP_2PP) {
			x0 = (i) % bmp.Columns();
			y0 = (i) / bmp.Columns();
			r0 = bmp.Red(x0, y0) & 0xFE;   // 11111110
			g0 = bmp.Green(x0, y0) & 0xFE; // 11111110
			b0 = bmp.Blue(x0, y0) & 0xFC;  // 11111100
			r0 |= (pBuffer[index] & 0x80 ) >> 7 ; // 10000000
			g0 |= (pBuffer[index] & 0x40 ) >> 6;  // 01000000
			b0 |= (pBuffer[index] & 0x30 ) >> 4;  // 00110000
			bmp.Red(x0, y0) = r0;
			bmp.Green(x0, y0) = g0;
			bmp.Blue(x0, y0) = b0;	

			x1 = (i+1) % bmp.Columns();
			y1 = (i+1) / bmp.Columns();
			r1 = bmp.Red(x1, y1) & 0xFC;   // 11111100
			g1 = bmp.Green(x1, y1) & 0xFE; // 11111110
			b1 = bmp.Blue(x1, y1) & 0xFE;  // 11111110
			r1 |= (pBuffer[index] & 0x0C ) >> 2 ; // 00001100
			g1 |= (pBuffer[index] & 0x02 ) >> 1;  // 00000010
			b1 |= (pBuffer[index] & 0x01 );       // 00000001
			bmp.Red(x1, y1) = r1;
			bmp.Green(x1, y1) = g1;
			bmp.Blue(x1, y1) = b1;

			//TRACE(_T("W2[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X\n"),  pBuffer[index], 
			//	x0, y0, r0, g0, b0,
			//	x1, y1, r1, g1, b1);

			index ++;
		} else if (dwFormaParamt == PFP_3PP){
			x0 = (i) % bmp.Columns();
			y0 = (i) / bmp.Columns();
			r0 = bmp.Red(x0, y0) & 0xFE;   // 11111110
			g0 = bmp.Green(x0, y0) & 0xFE; // 11111110
			b0 = bmp.Blue(x0, y0) & 0xFE;  // 11111110
			r0 |= (pBuffer[index] & 0x80 ) >> 7 ; // 10000000
			g0 |= (pBuffer[index] & 0x40 ) >> 6;  // 01000000
			b0 |= (pBuffer[index] & 0x20 ) >> 5;  // 00100000
			bmp.Red(x0, y0) = r0;
			bmp.Green(x0, y0) = g0;
			bmp.Blue(x0, y0) = b0;

			x1 = (i+1) % bmp.Columns();
			y1 = (i+1) / bmp.Columns();
			r1 = bmp.Red(x1, y1) & 0xFE;   // 11111110
			g1 = bmp.Green(x1, y1) & 0xFE; // 11111110
			b1 = bmp.Blue(x1, y1) & 0xFE;  // 11111110
			r1 |= (pBuffer[index] & 0x10 ) >> 4 ; // 00010000
			g1 |= (pBuffer[index] & 0x08 ) >> 3;  // 00001000
			b1 |= (pBuffer[index] & 0x04 ) >> 2;  // 00000100
			bmp.Red(x1, y1) = r1;
			bmp.Green(x1, y1) = g1;
			bmp.Blue(x1, y1) = b1;

			x2 = (i+2) % bmp.Columns();
			y2 = (i+2) / bmp.Columns();
			r2 = bmp.Red(x2, y2) & 0xFE;   // 11111110
			g2 = bmp.Green(x2, y2) & 0xFF;  // 11111111
			b2 = bmp.Blue(x2, y2) & 0xFE;  // 11111110
			r2 |= (pBuffer[index] & 0x02 ) >> 1 ; // 00000010
			g2 |= (pBuffer[index] & 0x00 );       // 00000000
			b2 |= (pBuffer[index] & 0x01 );       // 00000001
			bmp.Red(x2, y2) = r2;
			bmp.Green(x2, y2) = g2;
			bmp.Blue(x2, y2) = b2;

			//TRACE(_T("W3[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X [%d %d] %02X %02X %02X\n"),
			//	pBuffer[index],
			//	x0, y0, r0, g0, b0,
			//	x1, y1, r1, g1, b1,
			//	x2, y2, r2, g2, b2);

			index ++;
		}
		
	}
	
	return TRUE;
}

BOOL CPack::RawPPReadImageInternal(CSBitmap & bmp, LPBYTE pBuffer, size_t offset, size_t size, 
						 DWORD dwFormaParamt,  BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	size_t TotalSize = 0;
	BYTE r0,g0,b0; // r g b
	BYTE r1,g1,b1; // r g b
	BYTE r2,g2,b2; // r g b
	size_t x0, y0, x1, y1, x2, y2;
	size_t ratio = 1;
	size_t index = 0;
	INT nProgress = 0, nProgressOld = 0;

	TotalSize = bmp.Rows() * bmp.Columns();

	if(dwFormaParamt == PFP_1PP) {
		ratio = 1;	
	} else if(dwFormaParamt == PFP_2PP) {
		ratio = 2;
	} else if(dwFormaParamt == PFP_3PP) {
		ratio = 3;
	}

	if((offset + size) * ratio > TotalSize) return FALSE;

	for(size_t i = offset * ratio; i < (offset + size) * ratio ; i += ratio) {
		
		if(bCancel && *bCancel) {
			return FALSE;
		}

		if(NULL != fnSetProgress) {
			nProgress = i * 100 / (offset + size) * ratio;
			if(nProgressOld != nProgress) {
				fnSetProgress(nProgress);
				nProgressOld = nProgress;
			}
		}

		if(dwFormaParamt == PFP_1PP) {
			x0 = (i) % bmp.Columns();
			y0 = (i) / bmp.Columns();
			r0 = bmp.Red(x0, y0) & 0x07; // 00000111
			r0 <<= 5;
			g0 = bmp.Green(x0, y0) & 0x03 ; // 00000011
			g0 <<= 3;
			b0 = bmp.Blue(x0, y0) & 0x07; // 00000111

			pBuffer[index] = r0 | g0 | b0;
			//TRACE(_T("R1[%02X][%d %d] %02X %02X %02X\n"), pBuffer[index], x0, y0, 
			//	bmp.Red(x0, y0), bmp.Green(x0, y0), bmp.Blue(x0, y0));
			index ++;
		} else if(dwFormaParamt == PFP_2PP) {
			x0 = (i) % bmp.Columns();
			y0 = (i) / bmp.Columns();
			r0 = bmp.Red(x0, y0) & 0x1;   // 00000001
			r0 <<= 7;
			g0 = bmp.Green(x0, y0) & 0x1; // 00000001
			g0 <<= 6;
			b0 = bmp.Blue(x0, y0) & 0x3;  // 00000011
			b0 <<= 4;
			
			x1 = (i+1) % bmp.Columns();
			y1 = (i+1) / bmp.Columns();
			r1 = bmp.Red(x1, y1) & 0x3;   // 00000011
			r1 <<= 2;
			g1 = bmp.Green(x1, y1) & 0x1; // 00000001
			g1 <<= 1;
			b1 = bmp.Blue(x1, y1) & 0x1;  // 00000001

			pBuffer[index] = r0 | g0 | b0 | r1 | g1 | b1;
			//TRACE(_T("R2[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X \n"), 
			//	pBuffer[index], x0, y0, 
			//	color0.redQuantum(), color0.greenQuantum(), color0.blueQuantum(),
			//	x1, y1,
			//	color1.redQuantum(), color1.greenQuantum(), color1.blueQuantum());
			index ++;
		} else if (dwFormaParamt == PFP_3PP){
			x0 = (i) % bmp.Columns();
			y0 = (i) / bmp.Columns();
			r0 = bmp.Red(x0, y0) & 0x1;   // 00000001
			r0 <<= 7;
			g0 = bmp.Green(x0, y0) & 0x1; // 00000001
			g0 <<= 6;
			b0 = bmp.Blue(x0, y0) & 0x1;  // 00000001
			b0 <<= 5;
			
			x1 = (i+1) % bmp.Columns();
			y1 = (i+1) / bmp.Columns();
			r1 = bmp.Red(x1, y1) & 0x1;   // 00000001
			r1 <<= 4;
			g1 = bmp.Green(x1, y1) & 0x1; // 00000001
			g1 <<= 3;
			b1 = bmp.Blue(x1, y1) & 0x1;  // 00000001
			b1 <<= 2;

			x2 = (i+2) % bmp.Columns();
			y2 = (i+2) / bmp.Columns();
			r2 = bmp.Red(x2, y2) & 0x1;   // 00000001
			r2 <<= 1;
			g2 = bmp.Green(x2, y2) & 0x0; // 00000000
			b2 = bmp.Blue(x2, y2) & 0x1;  // 00000001
			
			pBuffer[index] = r0 | g0 | b0 | r1 | g1 | b1 | r2 | b2 | g2;

			//TRACE(_T("R3[%02X][%d %d] %02X %02X %02X [%d %d] %02X %02X %02X [%d %d] %02X %02X %02X\n"), 
			//	pBuffer[index], x0, y0,
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

CPack::PackHeader * CPack::RawPPReadImage(LPCTSTR szSrc, LPCTSTR szExt, CSBitmap & bmp, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	Magick::Image image;
	PackHeader header, *pRet = NULL;;
	size_t size = 0;

	memset(&header, 0, sizeof(header));
	// check ext

	// load image
	try {
		// Convert a TCHAR string to a LPCSTR
		CT2CA strSrc (szSrc, CP_UTF8);
		// construct a std::string using the LPCSTR input
		std::string strStd (strSrc);
		// alloc memory
		TRACE("image file size is %s\n", strStd.c_str());
		image.read(strStd);
		bmp.FromImage(image);
		size = bmp.Rows() * bmp.Columns();
	}
	catch (const Magick::Error & err ) {
		 
		CA2CT strErr(err.what(), CP_UTF8);
		szError = strErr;
		goto error;
    }

	// test format
//	try PF_1PP, size >= sizeof(PackHeader)
	if(size < sizeof(PackHeader)) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}

	if(!RawPPReadImageInternal(bmp, (LPBYTE)&header, 0, sizeof(header), PFP_1PP)) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	if(IsValidHeader(header)) {
		if(NULL != pRet) {
			free(pRet);
			pRet = NULL;
		}
		pRet = (PackHeader *)malloc(sizeof(header) + header.dwDataSize);
		if(NULL == pRet || !RawPPReadImageInternal(bmp, (LPBYTE)pRet , 0, sizeof(header) + header.dwDataSize,
			PFP_1PP, bCancel, fnSetProgress)) {
			szError.LoadString(IDS_ERROR_INTERNAL);
			goto error;
		}
		return pRet;
	}

//	try PF_2PP, size >= sizeof(PackHeader)	* 2
	if(size < sizeof(PackHeader) * 2) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}

	if(!RawPPReadImageInternal(bmp, (LPBYTE)&header, 0, sizeof(header), PFP_2PP)) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	if(IsValidHeader(header)) {
		if(NULL != pRet) {
			free(pRet);
			pRet = NULL;
		}
		pRet = (PackHeader *)malloc(sizeof(header) + header.dwDataSize);
		if(NULL == pRet || !RawPPReadImageInternal(bmp, (LPBYTE)pRet , 0, sizeof(header) + header.dwDataSize, 
			PFP_2PP, bCancel, fnSetProgress)) {
			szError.LoadString(IDS_ERROR_INTERNAL);
			goto error;
		}
		return pRet;
	}

//	try PF_3PP, size >= sizeof(PackHeader)	* 3
	if(size < sizeof(PackHeader) * 3) {
		szError.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		goto error;
	}

	if(!RawPPReadImageInternal(bmp, (PBYTE)&header, 0, sizeof(header), PFP_3PP)) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	if(IsValidHeader(header)) {
		if(NULL != pRet) {
			free(pRet);
			pRet = NULL;
		}
		pRet = (PackHeader *)malloc(sizeof(header) + header.dwDataSize);
		if(NULL == pRet || !RawPPReadImageInternal(bmp, (PBYTE)pRet , 0, sizeof(header) + header.dwDataSize, 
			PFP_3PP, bCancel, fnSetProgress)) {
			szError.LoadString(IDS_ERROR_INTERNAL);
			goto error;
		}
		return pRet;
	}

error:
	if(NULL != pRet) {
		free(pRet);
		pRet = NULL;
	}
	return NULL;
}

BOOL CPack::RawPPWriteImage(const PackHeader * pBuffer, const CSBitmap & bmp,
		LPCTSTR szDst, LPCTSTR szExt, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress)
{
	Magick::Image image;
	LPBYTE pBufOut = NULL;

	CSBitmap new_bmp(bmp);

	if((pBufOut =(LPBYTE) malloc(sizeof(PackHeader) + pBuffer->dwCapicity)) == NULL) {
		szError.LoadString(IDS_ERROR_INTERNAL);
		goto error;
	}

	memcpy(pBufOut, pBuffer, sizeof(PackHeader) + pBuffer->dwDataSize);
	if(pBuffer->dwCapicity > pBuffer->dwDataSize) {
		PadMemory(pBufOut + sizeof(PackHeader) + pBuffer->dwDataSize, pBuffer->dwCapicity - pBuffer->dwDataSize);
	}

	if(!RawPPWriteImageInternal(new_bmp, (LPBYTE)pBufOut, 0, sizeof(PackHeader) + pBuffer->dwCapicity, 
		(PackFormat)pBuffer->dwFormat, bCancel, fnSetProgress)) {
			if(bCancel && *bCancel) {
				//szError = _T("pack data canceled!");
				szError.LoadString(IDS_ERROR_CANCELED);
			} else {
				//szError = _T("pack data error!");
				szError.LoadString(IDS_ERROR_INTERNAL);
			}
			goto error;
	}

	if(NULL != pBufOut) {
		free(pBufOut);
		pBufOut = NULL;
	}

	try {
		// Convert a TCHAR string to a LPCSTR
		CT2CA strDst (szDst, CP_UTF8);
		std::string strStdDst (strDst);
		TRACE("write to file %s\n", strStdDst.c_str());

		CT2CA strExt (szExt, CP_UTF8);
		std::string strStdFileExt (strExt);
		new_bmp.ToImage(image);
		image.magick( strStdFileExt.c_str() );
		if(strStdFileExt.compare("tiff") == 0) {
			image.compressType(Magick::LZWCompression);
		}

		image.write(strStdDst);
	}
	catch (const Magick::Error & err ) {
		 
		CA2CT strErr(err.what(), CP_UTF8);
		szError = strErr;
		goto error;
		
    }

	return TRUE;

error:
	if(NULL != pBufOut) {
		free(pBufOut);
		pBufOut = NULL;
	}
	return FALSE;
}

BOOL CPack::RawPPCanSetFormat(DWORD dwParam, const PackHeader * data, const CSBitmap & bmp)
{
	size_t size = 0;
	size = bmp.Rows() * bmp.Columns();	

	INT cap = 0;

	if(dwParam == PFP_1PP) {
		cap = size - sizeof(PackHeader);
	} else if(dwParam == PFP_2PP) {
		cap = size / 2 - sizeof(PackHeader);
	} else if(dwParam == PFP_3PP) {
		cap = size / 3 - sizeof(PackHeader);
	}

	return (cap >= data->dwDataSize);
}

BOOL CPack::RawPPSetFormat(DWORD dwParam, PackHeader * data, const CSBitmap & bmp)
{
	size_t size = 0;
	size = bmp.Rows() * bmp.Columns();

	if(dwParam != data->dwFormatParam || data->dwFormat != PF_RAWPP) {
		if(RawPPCanSetFormat(dwParam,data, bmp)) {
			data->dwFormat = PF_RAWPP;
			data->dwFormatParam = dwParam;
			if(dwParam == PFP_1PP) {
				data->dwCapicity = size - sizeof(PackHeader);
			} else if(dwParam == PFP_2PP) {
				data->dwCapicity = size/2 - sizeof(PackHeader);
			} else if(dwParam == PFP_3PP) {
				data->dwCapicity = size/3 - sizeof(PackHeader);
			} else {
				return FALSE;
			}
			return TRUE;
		} else {
			return FALSE;
		}
	} else {
		return TRUE;
	}

	return FALSE;
}

void CPack::RawPPGetSaveFilter(CString & szFilter)
{
	szFilter = _T("BMP (*.bmp)|*.bmp|")
		_T("TIFF (*.tiff)|*.tiff;|")
		_T("TGA (*.tga)|*.tga;|")
		_T("PNG (*.png)|*.png;||");
}

void CPack::RawPPSaveDefaultExt(CString & szExt)
{
	szExt = _T("PNG");
}
#include "pch.h"
#include "BMPFileMedia.h"
#include "stdlib.h"

CBMPFileMedia::CBMPFileMedia() :
	m_pfileHeader(NULL)
{
}

CBMPFileMedia::~CBMPFileMedia()
{

}

BOOL CBMPFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
	CProgressBase& Progress, CPackErrors& Errors)
{
	BITMAPFILEHEADER fileHeader;
	LPBITMAPINFOHEADER  pinfoHeader = NULL;
	BOOL bRet = FALSE;
	FILE* pFile = NULL;
	INT nPadding, nScanLineSize;
	LPBYTE pScanLine = NULL;

	if (m_pfileHeader) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}

	// open file and read meta
	if ((pFile = _tfsopen(szFilePath, _T("rb"), _SH_DENYWR)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO);
		goto err;
	}

	if (fread(&fileHeader, sizeof(fileHeader), 1, pFile) != 1) {
		Errors.SetError(CPackErrors::PE_IO);
		goto err;
	}

	if (fileHeader.bfType != 0x4d42) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
		goto err;
	}

	if ((m_pfileHeader = (LPBITMAPFILEHEADER)malloc(fileHeader.bfOffBits)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if (fseek(pFile, 0, SEEK_SET) != 0 || fread(m_pfileHeader, fileHeader.bfOffBits, 1, pFile) != 1) {
		Errors.SetError(CPackErrors::PE_IO);
		goto err;
	}

	pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));

	if (pinfoHeader->biPlanes != 1 || (pinfoHeader->biBitCount != 24)
		|| pinfoHeader->biCompression != BI_RGB) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
		goto err;
	}

	// alloc buffer
	if (!CPixelImageMedia::Alloc(pinfoHeader->biWidth, pinfoHeader->biHeight, Errors)) {
		goto err;
	}

	nPadding = (pinfoHeader->biWidth % 4);
	nPadding = nPadding != 0 ? 4 - nPadding : 0;
	nScanLineSize = (pinfoHeader->biWidth + nPadding) * 3;
	if ((pScanLine = (LPBYTE)malloc(nScanLineSize)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	Progress.SetFullScale(pinfoHeader->biHeight);

	// load scanline
	for (INT i = 0; i < pinfoHeader->biHeight; i++) {
		if (fread(pScanLine, nScanLineSize, 1, pFile) != 1) {
			Errors.SetError(CPackErrors::PE_IO);
			goto err;
		}
		CPixelImageMedia::SetScanline(i, pScanLine, CPixelImageMedia::CPixelBlock::PIXEL_FORMAT_BGR);
		Progress.Increase(1);
	}

	// test format
	if (!CPixelImageMedia::LoadMeta(PasswordGetter, Errors)) {
		goto err;
	}
	bRet = TRUE;
err:
	if (m_pfileHeader != NULL && !bRet) {
		free(m_pfileHeader);
		m_pfileHeader = NULL;
	}
	if (pFile != NULL) {
		fclose(pFile);
		pFile = NULL;
	}
	if (pScanLine != NULL) {
		free(pScanLine);
		pScanLine = NULL;
	}
	return bRet;
}

// save changes to file
BOOL CBMPFileMedia::SaveMedia(LPCTSTR szFilePath, UINT nDataSize, CProgressBase& Progress, CPackErrors& Errors)
{
	BITMAPFILEHEADER fileHeader;
	LPBITMAPINFOHEADER  pinfoHeader = NULL;
	BOOL bRet = FALSE;
	FILE* pFile = NULL;
	INT nPadding, nScanLineSize;
	LPBYTE pScanLine = NULL;

	if (!m_pfileHeader) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}

	if (!CPixelImageMedia::SetMediaUsedBytes(nDataSize,Errors)) {
		goto err;
	}

	if (!CPixelImageMedia::SaveMeta(Errors)) {
		goto err;
	}

	pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));

	// open file and write header
	if ((pFile = _tfsopen(szFilePath, _T("wb"), _SH_DENYNO)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO);
		goto err;
	}

	if (fwrite(m_pfileHeader, m_pfileHeader->bfOffBits, 1, pFile) != 1) {
		Errors.SetError(CPackErrors::PE_IO);
		goto err;
	}

	nPadding = (pinfoHeader->biWidth % 4);
	nPadding = nPadding != 0 ? 4 - nPadding : 0;
	nScanLineSize = (pinfoHeader->biWidth + nPadding) * 3;
	if ((pScanLine = (LPBYTE)malloc(nScanLineSize)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	// write scanline
	for (INT i = 0; i < pinfoHeader->biHeight; i++) {
		CPixelImageMedia::GetScanline(i, pScanLine, CPixelImageMedia::CPixelBlock::PIXEL_FORMAT_BGR);
		if (fwrite(pScanLine, nScanLineSize, 1, pFile) != 1) {
			Errors.SetError(CPackErrors::PE_IO);
			goto err;
		}
		Progress.Increase(1);
	}
	
	// done!
	m_bIsDirty = FALSE;

	bRet = TRUE;

err:
	if (pScanLine) {
		free(pScanLine);
		pScanLine = NULL;
	}
	return bRet;
}

// free all buffers
void CBMPFileMedia::CloseMedia()
{
	// free file header
	if (m_pfileHeader != NULL) {
		free(m_pfileHeader);
		m_pfileHeader = NULL;
	}
	// free buffer
	CPixelImageMedia::Free();
}


LPCTSTR CBMPFileMedia::m_szFilter = _T("BMP Files (*.bmp)|*.bmp|");
LPCTSTR CBMPFileMedia::m_szExt = _T("BMP");


BOOL CBMPFileMedia::TestExt(LPCTSTR szExt)
{
	return lstrcmpi(szExt, m_szExt) == 0;
}

LPCTSTR CBMPFileMedia::GetExtFilter()
{
	return m_szFilter;
}

CMediaBase* CBMPFileMedia::Factory()
{
	return new(std::nothrow) CBMPFileMedia();
}
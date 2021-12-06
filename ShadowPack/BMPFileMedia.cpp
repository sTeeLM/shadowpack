#include "pch.h"
#include "BMPFileMedia.h"
#include "PackUtils.h"
#include "stdlib.h"
#include "resource.h"

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
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto err;
	}

	if (fread(&fileHeader, sizeof(fileHeader), 1, pFile) != 1) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto err;
	}

	if (fileHeader.bfType != 0x4d42) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if ((m_pfileHeader = (LPBITMAPFILEHEADER)malloc(fileHeader.bfOffBits)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	if (fseek(pFile, 0, SEEK_SET) != 0 || fread(m_pfileHeader, fileHeader.bfOffBits, 1, pFile) != 1) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto err;
	}

	pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));

	if (pinfoHeader->biPlanes != 1 || (pinfoHeader->biBitCount != 24)
		|| pinfoHeader->biCompression != BI_RGB) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
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

	Progress.Reset(IDS_READ_FILE);
	Progress.SetFullScale(pinfoHeader->biHeight);

	// load scanline
	for (INT i = 0; i < pinfoHeader->biHeight; i++) {
		if (fread(pScanLine, nScanLineSize, 1, pFile) != 1) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
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
BOOL CBMPFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	LPBITMAPINFOHEADER  pinfoHeader = NULL;
	BOOL bRet = FALSE;
	FILE* pFile = NULL;
	INT nPadding, nScanLineSize;
	LPBYTE pScanLine = NULL;

	if (!m_pfileHeader) {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		goto err;
	}

	if (!CPixelImageMedia::SaveMeta(Errors)) {
		goto err;
	}

	Progress.Reset(IDS_FILL_EMPTY_SPACE);
	Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

	if (!CPixelImageMedia::FillEmptySpace(Progress, Errors)) {
		goto err;
	}

	pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));

	// open file and write header
	if ((pFile = _tfsopen(szFilePath, _T("wb"), _SH_DENYNO)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto err;
	}

	if (fwrite(m_pfileHeader, m_pfileHeader->bfOffBits, 1, pFile) != 1) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto err;
	}

	nPadding = (pinfoHeader->biWidth % 4);
	nPadding = nPadding != 0 ? 4 - nPadding : 0;
	nScanLineSize = (pinfoHeader->biWidth + nPadding) * 3;
	if ((pScanLine = (LPBYTE)malloc(nScanLineSize)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	Progress.Reset(IDS_WRITE_FILE);
	Progress.SetFullScale(pinfoHeader->biHeight);

	// write scanline
	for (INT i = 0; i < pinfoHeader->biHeight; i++) {
		CPixelImageMedia::GetScanline(i, pScanLine, CPixelImageMedia::CPixelBlock::PIXEL_FORMAT_BGR);
		if (fwrite(pScanLine, nScanLineSize, 1, pFile) != 1) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
			goto err;
		}
		Progress.Increase(1);
	}
	
	// done!
	ClearMediaDirty();

	bRet = TRUE;

err:
	if (pScanLine) {
		free(pScanLine);
		pScanLine = NULL;
	}
	if (pFile) {
		fclose(pFile);
		pFile = NULL;
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

void CBMPFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
	/*
	CString m_strbfSize;
	CString m_strbfOffBits;
	CString m_strbiWidth;
	CString m_strbiHeight;
	CString m_strbiBitCount;
	CString m_strbiCompression;
	CString m_strbiXPelsPerMeter;
	CString m_strbiYPelsPerMeter;
	*/
	LPBITMAPINFOHEADER  pinfoHeader;
	if (m_pfileHeader != NULL) {
		pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));
		m_OptPageBMPFile.m_strbfOffBits.Format(_T("%d"),m_pfileHeader->bfOffBits);
		m_OptPageBMPFile.m_strbfSize.Format(_T("%d"), m_pfileHeader->bfSize);
		m_OptPageBMPFile.m_strbiHeight.Format(_T("%d"), pinfoHeader->biHeight);
		m_OptPageBMPFile.m_strbiWidth.Format(_T("%d"), pinfoHeader->biWidth);
		m_OptPageBMPFile.m_strbiBitCount.Format(_T("%d"), pinfoHeader->biBitCount);
		m_OptPageBMPFile.m_strbiCompression.Format(_T("%d"), pinfoHeader->biCompression);
		m_OptPageBMPFile.m_strbiXPelsPerMeter.Format(_T("%d"), pinfoHeader->biXPelsPerMeter);
		m_OptPageBMPFile.m_strbiYPelsPerMeter.Format(_T("%d"), pinfoHeader->biYPelsPerMeter);
	}

	
	pPropertySheet->AddPage(&m_OptPageBMPFile);
	CPixelImageMedia::AddOptPage(pPropertySheet);
}

BOOL CBMPFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPixelImageMedia::UpdateOpts(pPropertySheet);
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
#include "pch.h"
#include "TIFFileMedia.h"
#include "resource.h"

CTIFFileMedia::CTIFFileMedia()
{

}

CTIFFileMedia::~CTIFFileMedia()
{
}

CString CTIFFileMedia::m_strLastError;
CHAR CTIFFileMedia::szBuffer[4096];
void CTIFFileMedia::ErrorHandler(const char* module, const char* fmt, va_list ap)
{
	vsnprintf(szBuffer, sizeof(szBuffer) - 1, fmt, ap);
	CA2CT str(szBuffer);
	m_strLastError = str;
}

BOOL CTIFFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	BOOL bRet = FALSE;
	tdata_t buf = NULL;
	TIFF* pTiff = NULL;

	TIFFSetErrorHandler(ErrorHandler);

	ZeroMemory(&m_TIFFInfo, sizeof(m_TIFFInfo));

#ifdef _UNICODE
	pTiff = TIFFOpenW(szFilePath, "r");
#else
	pTiff = TIFFOpen(szFilePath, "r");
#endif

	if (!pTiff) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}


	if (TIFFNumberOfDirectories(pTiff) != 1) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}
	
	if(!TIFFGetField(pTiff, TIFFTAG_IMAGELENGTH, &m_TIFFInfo.nHeight)
		|| !TIFFGetField(pTiff, TIFFTAG_IMAGEWIDTH, &m_TIFFInfo.nWidth)
		|| !TIFFGetField(pTiff, TIFFTAG_COMPRESSION, &m_TIFFInfo.nCompression)
		|| !TIFFGetField(pTiff, TIFFTAG_SAMPLESPERPIXEL, &m_TIFFInfo.nSamplesPerPixel)
		|| !TIFFGetField(pTiff, TIFFTAG_BITSPERSAMPLE, &m_TIFFInfo.nBitPerSample)
		|| !TIFFGetField(pTiff, TIFFTAG_PHOTOMETRIC, &m_TIFFInfo.nPhotoMatric)
		|| !TIFFGetField(pTiff, TIFFTAG_PLANARCONFIG, &m_TIFFInfo.nPlanarConfig)
//		|| !TIFFGetField(pTiff, TIFFTAG_XRESOLUTION, &m_TIFFInfo.fXResolution)
//		|| !TIFFGetField(pTiff, TIFFTAG_YRESOLUTION, &m_TIFFInfo.fYResolution)
//		|| !TIFFGetField(pTiff, TIFFTAG_RESOLUTIONUNIT, &m_TIFFInfo.nResolutionUnit)
		)
	{
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if (TIFFGetField(pTiff, TIFFTAG_RESOLUTIONUNIT, &m_TIFFInfo.nResolutionUnit) && m_TIFFInfo.nResolutionUnit != 0) {
		if (!TIFFGetField(pTiff, TIFFTAG_XRESOLUTION, &m_TIFFInfo.fXResolution)
			|| !TIFFGetField(pTiff, TIFFTAG_YRESOLUTION, &m_TIFFInfo.fYResolution)) {
			Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
			goto err;
		}
	}

	if (!(m_TIFFInfo.nCompression == COMPRESSION_NONE || m_TIFFInfo.nCompression == COMPRESSION_LZW
		|| m_TIFFInfo.nCompression == COMPRESSION_LZMA)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if (m_TIFFInfo.nPhotoMatric != PHOTOMETRIC_RGB) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if (m_TIFFInfo.nBitPerSample != 8) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if (!(m_TIFFInfo.nSamplesPerPixel == 3 || m_TIFFInfo.nSamplesPerPixel == 4)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	if (m_TIFFInfo.nSamplesPerPixel == 4) {
		USHORT * pExtraSamplesInfo = NULL;
		if (!TIFFGetField(pTiff, TIFFTAG_EXTRASAMPLES, &m_TIFFInfo.nExtraSamples, &pExtraSamplesInfo)) {
			Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
			goto err;
		} else {
			m_TIFFInfo.pExtraSamplesInfo = new(std::nothrow)USHORT[m_TIFFInfo.nExtraSamples];
			if (m_TIFFInfo.pExtraSamplesInfo) {
				CopyMemory(m_TIFFInfo.pExtraSamplesInfo, pExtraSamplesInfo, m_TIFFInfo.nExtraSamples * sizeof(USHORT));
			}
			else {
				Errors.SetError(CPackErrors::PE_NOMEM);
				goto err;
			}
		}
	}
	
	if ((buf = _TIFFmalloc(TIFFScanlineSize(pTiff))) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}



	// alloc buffer
	if (!CPixelImageMedia::Alloc(m_TIFFInfo.nWidth, m_TIFFInfo.nHeight, Errors)) {
		goto err;
	}

	Progress.Reset(IDS_LOAD_DATA);
	if (m_TIFFInfo.nPlanarConfig == PLANARCONFIG_CONTIG) {
		Progress.SetFullScale(m_TIFFInfo.nHeight);
		for (UINT irow = 0; irow < m_TIFFInfo.nHeight; irow++) {
			if (!TIFFReadScanline(pTiff, buf, irow)) {
				Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
				goto err;
			}
			Progress.Increase(1);
			CPixelImageMedia::SetScanline(irow, (LPBYTE)buf,
				m_TIFFInfo.nSamplesPerPixel == 4 ? CPixelImageMedia::PIXEL_FORMAT_RGBA 
				: CPixelImageMedia::PIXEL_FORMAT_RGB);
		}
	}
	else {
		Progress.SetFullScale(m_TIFFInfo.nHeight * m_TIFFInfo.nSamplesPerPixel);
		for (UINT s = 0; s < m_TIFFInfo.nSamplesPerPixel; s++) {
			for (UINT irow = 0; irow < m_TIFFInfo.nHeight; irow++) {
				if (!TIFFReadScanline(pTiff, buf, irow, s)) {
					Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
					goto err;
				}
				Progress.Increase(1);
				CPixelImageMedia::SetScanlinePerChannel(irow, (LPBYTE)buf, m_TIFFInfo.nSamplesPerPixel == 4 ? 
					CPixelImageMedia::PIXEL_FORMAT_RGBA
					: CPixelImageMedia::PIXEL_FORMAT_RGB, s);
			}
		}
	}

	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto err;
	}
	
	bRet = TRUE;

err:
	if (buf) {
		_TIFFfree(buf);
		buf = NULL;
	}
	if (pTiff) {
		TIFFClose(pTiff);
		pTiff = NULL;
	}
	if (!bRet) {
		CPixelImageMedia::Free();
	}
	if (!bRet) {
		if (m_TIFFInfo.pExtraSamplesInfo) {
			delete[] m_TIFFInfo.pExtraSamplesInfo;
		}
		ZeroMemory(&m_TIFFInfo, sizeof(m_TIFFInfo));
	}
	return bRet;
}

BOOL CTIFFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	BOOL bRet = FALSE;
	tdata_t buf = NULL;
	TIFF* pTiff = NULL;

	TIFFSetErrorHandler(ErrorHandler);

	if (!CBytePerBlockMedia::SaveMeta(Errors)) {
		goto err;
	}

#ifdef _UNICODE
	pTiff = TIFFOpenW(szFilePath, "w");
#else
	pTiff = TIFFOpen(szFilePath, "w");
#endif

	if (!pTiff) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}

	if (!TIFFSetField(pTiff, TIFFTAG_IMAGELENGTH, m_TIFFInfo.nHeight)
		|| !TIFFSetField(pTiff, TIFFTAG_IMAGEWIDTH, m_TIFFInfo.nWidth)
		|| !TIFFSetField(pTiff, TIFFTAG_COMPRESSION, m_TIFFInfo.nCompression)
		|| !TIFFSetField(pTiff, TIFFTAG_SAMPLESPERPIXEL, m_TIFFInfo.nSamplesPerPixel)
		|| !TIFFSetField(pTiff, TIFFTAG_BITSPERSAMPLE, m_TIFFInfo.nBitPerSample)
		|| !TIFFSetField(pTiff, TIFFTAG_PHOTOMETRIC, m_TIFFInfo.nPhotoMatric)
		|| !TIFFSetField(pTiff, TIFFTAG_PLANARCONFIG, m_TIFFInfo.nPlanarConfig)
//		|| !TIFFSetField(pTiff, TIFFTAG_XRESOLUTION, m_TIFFInfo.fXResolution)
//		|| !TIFFSetField(pTiff, TIFFTAG_YRESOLUTION, m_TIFFInfo.fYResolution)
//		|| !TIFFSetField(pTiff, TIFFTAG_RESOLUTIONUNIT, m_TIFFInfo.nResolutionUnit)
		) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}

	if (m_TIFFInfo.nResolutionUnit != 0) {
		if (!TIFFSetField(pTiff, TIFFTAG_RESOLUTIONUNIT, m_TIFFInfo.nResolutionUnit)
			|| !TIFFSetField(pTiff, TIFFTAG_XRESOLUTION, &m_TIFFInfo.fXResolution)
			|| !TIFFSetField(pTiff, TIFFTAG_YRESOLUTION, &m_TIFFInfo.fYResolution)) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
			goto err;
		}
	}

	if (m_TIFFInfo.nExtraSamples) {
		if (!TIFFSetField(pTiff, TIFFTAG_EXTRASAMPLES, m_TIFFInfo.nExtraSamples, m_TIFFInfo.pExtraSamplesInfo)) {
			Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		}
	}

	if ((buf = _TIFFmalloc(TIFFScanlineSize(pTiff))) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	Progress.Reset(IDS_SAVE_DATA);
	if (m_TIFFInfo.nPlanarConfig == PLANARCONFIG_CONTIG) {
		Progress.SetFullScale(m_TIFFInfo.nHeight);
		for (UINT irow = 0; irow < m_TIFFInfo.nHeight; irow++) {
			CPixelImageMedia::GetScanline(irow, (LPBYTE)buf, m_TIFFInfo.nSamplesPerPixel == 4 ? CPixelImageMedia::PIXEL_FORMAT_RGBA
				: CPixelImageMedia::PIXEL_FORMAT_RGB);
			Progress.Increase(1);
			if (!TIFFWriteScanline(pTiff, buf, irow)) {
				Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
				goto err;
			}
		}
	}
	else {
		Progress.SetFullScale(m_TIFFInfo.nHeight * m_TIFFInfo.nSamplesPerPixel);
		for (UINT s = 0; s < m_TIFFInfo.nSamplesPerPixel; s++) {
			for (UINT irow = 0; irow < m_TIFFInfo.nHeight; irow++) {
				CPixelImageMedia::GetScanlinePerChannel(irow, (LPBYTE)buf, m_TIFFInfo.nSamplesPerPixel == 4 ? CPixelImageMedia::PIXEL_FORMAT_RGBA
					: CPixelImageMedia::PIXEL_FORMAT_RGB, s);
				Progress.Increase(1);
				if (!TIFFWriteScanline(pTiff, buf, irow, s)) {
					Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
					goto err;
				}
			}
		}
	}

	if (!TIFFFlush(pTiff)) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}

	ClearMediaDirty();

	bRet = TRUE;
err:
	if (buf) {
		_TIFFfree(buf);
		buf = NULL;
	}
	if (pTiff) {
		TIFFClose(pTiff);
		pTiff = NULL;
	}
	return bRet;
}

void CTIFFileMedia::CloseMedia()
{
	CPixelImageMedia::Free();
	if (m_TIFFInfo.pExtraSamplesInfo) {
		delete[] m_TIFFInfo.pExtraSamplesInfo;
	}
	ZeroMemory(&m_TIFFInfo, sizeof(m_TIFFInfo));
}

void CTIFFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
	m_OptPageTIFFFile.m_strBitsPerSample.Format(_T("%d"), m_TIFFInfo.nBitPerSample);
	m_OptPageTIFFFile.m_strCompression.Format(_T("%d"), m_TIFFInfo.nCompression);
	m_OptPageTIFFFile.m_strImageLength.Format(_T("%d"), m_TIFFInfo.nHeight);
	m_OptPageTIFFFile.m_strImageWidth.Format(_T("%d"), m_TIFFInfo.nWidth);
	m_OptPageTIFFFile.m_strPhotoMetric.Format(_T("%d"), m_TIFFInfo.nPhotoMatric);
	m_OptPageTIFFFile.m_strPlanarConfig.Format(_T("%d"), m_TIFFInfo.nPlanarConfig);
	m_OptPageTIFFFile.m_strResolutionUnit.Format(_T("%d"), m_TIFFInfo.nResolutionUnit);
	m_OptPageTIFFFile.m_strSamplesPerPixel.Format(_T("%d"), m_TIFFInfo.nSamplesPerPixel);
	m_OptPageTIFFFile.m_strXResolution.Format(_T("%f"), m_TIFFInfo.fXResolution);
	m_OptPageTIFFFile.m_strYResolution.Format(_T("%f"), m_TIFFInfo.fYResolution);
	m_OptPageTIFFFile.m_strExtraSamples.Format(_T("%d"), m_TIFFInfo.nExtraSamples > 0 ?  m_TIFFInfo.pExtraSamplesInfo[0] : 0);

	pPropertySheet->AddPage(&m_OptPageTIFFFile);
	CPixelImageMedia::AddOptPage(pPropertySheet);
}

BOOL CTIFFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPixelImageMedia::UpdateOpts(pPropertySheet);
}

BOOL CTIFFileMedia::TestExt(LPCTSTR szExt)
{
	return (lstrcmpi(szExt, m_szExt) == 0 || lstrcmpi(szExt, _T("TIF")) == 0);
}

LPCTSTR CTIFFileMedia::GetExtFilter()
{
	return m_szFilter;
}

CMediaBase* CTIFFileMedia::Factory()
{
	return new(std::nothrow) CTIFFileMedia();
}

LPCTSTR CTIFFileMedia::m_szFilter = _T("TIFF Files (*.tif; *.tiff)|*.tif; *.tiff|");
LPCTSTR CTIFFileMedia::m_szExt = _T("TIFF");

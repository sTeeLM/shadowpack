#include "pch.h"
#include "TIFFileMedia.h"

CTIFFileMedia::CTIFFileMedia() :
	m_pTiff(NULL)
{
}

CTIFFileMedia::~CTIFFileMedia()
{
}

void CTIFFileMedia::ErrorHandler(const char* module, const char* fmt, va_list ap)
{

}

BOOL CTIFFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	BOOL bRet = FALSE;
	tdata_t buf = NULL;
	TIFFSetErrorHandler(ErrorHandler);

#ifdef _UNICODE
	m_pTiff = TIFFOpenW(szFilePath, "r");
#else
	m_pTiff = TIFFOpen(szFilePath, "r");
#endif

	if (!m_pTiff) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}


	if (TIFFNumberOfDirectories(m_pTiff) != 1) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}
	
	if(!TIFFGetField(m_pTiff, TIFFTAG_IMAGELENGTH, &m_TIFFInfo.nHeight)
		|| !TIFFGetField(m_pTiff, TIFFTAG_IMAGEWIDTH, &m_TIFFInfo.nWidth)
		|| !TIFFGetField(m_pTiff, TIFFTAG_COMPRESSION, &m_TIFFInfo.nCompression)
		|| !TIFFGetField(m_pTiff, TIFFTAG_SAMPLESPERPIXEL, &m_TIFFInfo.nSamplesPerPixel)
		|| !TIFFGetField(m_pTiff, TIFFTAG_BITSPERSAMPLE, &m_TIFFInfo.nBitPerSample)
		|| !TIFFGetField(m_pTiff, TIFFTAG_PHOTOMETRIC, &m_TIFFInfo.nPhotoMatric)
		|| !TIFFGetField(m_pTiff, TIFFTAG_PLANARCONFIG, &m_TIFFInfo.nPlanarConfig)
		|| !TIFFGetField(m_pTiff, TIFFTAG_XRESOLUTION, &m_TIFFInfo.fXResolution)
		|| !TIFFGetField(m_pTiff, TIFFTAG_YRESOLUTION, &m_TIFFInfo.fYResolution)
		|| !TIFFGetField(m_pTiff, TIFFTAG_RESOLUTIONUNIT, &m_TIFFInfo.nResolutionUnit)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
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
		if (!TIFFGetField(m_pTiff, TIFFTAG_EXTRASAMPLES, &m_TIFFInfo.nExtraSamples)) {
			Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
			goto err;
		}
	}
	if ((buf = _TIFFmalloc(TIFFScanlineSize(m_pTiff))) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	// alloc buffer
	if (!CPixelImageMedia::Alloc(m_TIFFInfo.nWidth, m_TIFFInfo.nHeight, Errors)) {
		goto err;
	}

	if (m_TIFFInfo.nPlanarConfig == PLANARCONFIG_CONTIG) {
		for (UINT irow = 0; irow < m_TIFFInfo.nHeight; irow++) {
			if (!TIFFReadScanline(m_pTiff, buf, irow)) {
				Errors.SetError(CPackErrors::PE_IO, szFilePath);
				goto err;
			}
			CPixelImageMedia::SetScanline(m_TIFFInfo.nHeight - 1 - irow,
				(LPBYTE)buf, CPixelImageMedia::CPixelBlock::PIXEL_FORMAT_RGBA);
		}
	}
	else {
		for (UINT s = 0; s < m_TIFFInfo.nSamplesPerPixel; s++) {
			for (UINT irow = 0; irow < m_TIFFInfo.nHeight; irow++) {
				if (!TIFFReadScanline(m_pTiff, buf, irow, s)) {
					Errors.SetError(CPackErrors::PE_IO, szFilePath);
					goto err;
				}
				CPixelImageMedia::SetScanlinePerChannel(m_TIFFInfo.nHeight - 1 - irow,
					(LPBYTE)buf, CPixelImageMedia::CPixelBlock::PIXEL_FORMAT_RGBA, 0);
			}
		}
	}
	
	bRet = TRUE;
	
err:
	if (!buf) {
		_TIFFfree(buf);
		buf = NULL;
	}
	if (!m_pTiff) {
		TIFFClose(m_pTiff);
		m_pTiff = NULL;
	}
	if (!bRet) {
		CPixelImageMedia::Free();
	}
	return bRet;
}

BOOL CTIFFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	return 0;
}

void CTIFFileMedia::CloseMedia()
{
	CPixelImageMedia::Free();
}

void CTIFFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CTIFFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return 0;
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

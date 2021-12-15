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

err:
	return 0;
}

BOOL CTIFFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	return 0;
}

void CTIFFileMedia::CloseMedia()
{
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

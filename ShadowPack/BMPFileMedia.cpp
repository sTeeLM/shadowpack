#include "pch.h"
#include "BMPFileMedia.h"

CBMPFileMedia::CBMPFileMedia()
{
}

CBMPFileMedia::~CBMPFileMedia()
{
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
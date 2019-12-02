#include "StdAfx.h"
#include "PackErrors.h"
#include "resource.h"

CPackErrors::CPackErrors():
	m_eErrorCode(PE_OK),
	m_eString(_T(""))
{

}

CPackErrors::~CPackErrors()
{

}

void CPackErrors:: SetError(const Magick::Error & Error)
{
	CA2CT strErr(Error.what(), CP_UTF8);
	m_eErrorCode = PE_MAGICK;
	m_eString = strErr;
}

void  CPackErrors::SetError( CPackErrors::PackErrors eErrorCode)
{
	m_eErrorCode = eErrorCode;
	switch (m_eErrorCode) {
	case PE_OK: // OK
		m_eString.LoadString(IDS_ERROR_OK);
		break;
	case PE_CANCELED:  // user canceled
		m_eString.LoadString(IDS_ERROR_CANCELED);
		break;
	case PE_NEED_PASSWORD: // need password
		m_eString.LoadString(IDS_ERROR_NEED_PASSWORD);
		break;
	case PE_IO:            // read file or write file
		m_eString.LoadString(IDS_ERROR_FILE_IO);
		break;
	case PE_TOO_LARGE_DATA: // data too large
		m_eString.LoadString(IDS_ERROR_TOO_LARGE_DATA);
		break;
	case PE_CORRUPT_DATA:   // data corrupted
		m_eString.LoadString(IDS_ERROR_CORRUPT_DATA);
		break;
	case PE_DECRYPT:        // decrypt error
		m_eString.LoadString(IDS_ERROR_DECRYPT);
		break;
	case PE_ENCRYPT:		   // encrypt error
		m_eString.LoadString(IDS_ERROR_ENCRYPT);
		break;
	case PE_OVER_CAPICITY:  // over capcity
		m_eString.LoadString(IDS_ERROR_OVER_CAPICITY);
		break;
	case PE_UNSUPPORT_PACK: // unsupport pack format
		m_eString.LoadString(IDS_ERROR_UNSUPPORT_FILE);
		break;
	case PE_INTERNAL:       // other error, memory??
		m_eString.LoadString(IDS_ERROR_INTERNAL);
		break;
	default: m_eString = _T("ERROR!");
	}
}

 CPackErrors::PackErrors  CPackErrors::GetError()
{
	return m_eErrorCode;
}

CString & CPackErrors::ToString()
{
	return m_eString;
}
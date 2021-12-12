#include "pch.h"
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



void  CPackErrors::SetError( CPackErrors::PACK_ERROR_T eErrorCode, LPCTSTR szPath /* = NULL*/, LPCTSTR szReason /* = 0*/)
{
	m_eErrorCode = eErrorCode;

	switch (m_eErrorCode) {
	case PE_CANCELED:  // user canceled
		m_eString.Format(IDS_PE_CANCELED);
		break;
	case PE_IO:             // read file or write file
		m_eString.Format(IDS_PE_IO, szPath, szReason);
		break;
	case PE_CORRUPT_DATA:   // data corrupted
		m_eString.Format(IDS_PE_CORRUPT_DATA);
		break;
	case PE_CIPHER:        // decrypt error
		m_eString.Format(IDS_PE_CIPHER);
		break;
	case PE_OVER_CAPICITY:  // over capcity
		m_eString.Format(IDS_PE_OVER_CAPICITY);
		break;
	case PE_UNSUPPORT_PACK: // unsupport pack format
		m_eString.Format(IDS_PE_UNSUPPORT_PACK, szPath);
		break;
	case PE_UNSUPPORT_MEDIA: // unsupport file
		m_eString.Format(IDS_PE_UNSUPPORT_MEDIA, szPath);
		break;
	case PE_EXISTED:        // item exist
		m_eString.Format(IDS_PE_EXISTED, szPath);
		break;
	case PE_NOMEM:          // no memory
		m_eString.Format(IDS_PE_NOMEM);
		break;
	case PE_INTERNAL:       // other error, bug??
		m_eString.Format(IDS_PE_INTERNAL);
		break;
	default: m_eString = _T("ERROR!");
	}
}

 CPackErrors::PACK_ERROR_T  CPackErrors::GetError()
{
	return m_eErrorCode;
}

 CPackErrors& CPackErrors::operator=(const CPackErrors& Errors)
 {
	 m_eErrorCode = Errors.m_eErrorCode;
	 m_eString = Errors.m_eString;
	 return *this;
 }

CString & CPackErrors::ToString()
{
	return m_eString;
}
#pragma once

#include "framework.h"

class CPackErrors
{
public:
	typedef enum {
			PE_OK = 0, // OK
			PE_CANCELED,  // user canceled
			PE_NEED_PASSWORD,  // need password
			PE_IO,             // read file or write file
			PE_CORRUPT_DATA,   // data corrupted
			PE_DECRYPT,        // decrypt error
			PE_ENCRYPT,		   // encrypt error
			PE_OVER_CAPICITY,  // over capcity
			PE_UNSUPPORT_PACK, // unsupport pack format
			PE_UNSUPPORT_MEDIA, // unsupport file
			PE_DUP_ITEM,        // dup item
			PE_EXISTED,        // item exist
			PE_NOMEM,          // no memory
			PE_INTERNAL,       // other error, bug??
			PE_COUNT
	}PACK_ERROR_T;

public:
	CPackErrors();
	~CPackErrors();
	CString & ToString();
	void SetError(PACK_ERROR_T eErrorCode, LPCTSTR szPath = NULL, LPCTSTR szReason = NULL);
	PACK_ERROR_T GetError();
private:
	PACK_ERROR_T m_eErrorCode;
	CString m_eString;
};
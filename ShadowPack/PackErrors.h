#pragma once

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
	}pack_error_t;

public:
	CPackErrors();
	~CPackErrors();
	CString & ToString();
	void SetError(pack_error_t eErrorCode, LPCTSTR szPath = NULL, LPCTSTR szReason = NULL);
	pack_error_t GetError();
private:
	pack_error_t m_eErrorCode;
	CString m_eString;
};
#pragma once

#include "Magick++.h"

class CPackErrors
{
public:
	enum PackErrors {
			PE_OK = 0, // OK
			PE_CANCELED,  // user canceled
			PE_NEED_PASSWORD, // need password
			PE_IO,            // read file or write file
			PE_TOO_LARGE_DATA, // data too large
			PE_CORRUPT_DATA,   // data corrupted
			PE_DECRYPT,        // decrypt error
			PE_ENCRYPT,		   // encrypt error
			PE_OVER_CAPICITY,  // over capcity
			PE_UNSUPPORT_PACK, // unsupport pack format
			PE_MAGICK,
			PE_INTERNAL       // other error, memory??
	};

public:
	CPackErrors();
	~CPackErrors();
	CString & ToString();
	void SetError(PackErrors eErrorCode);
	void SetError(const Magick::Error & Error);
	PackErrors GetError();
private:
	enum PackErrors m_eErrorCode;
	CString m_eString;
};
#pragma once
#include "Stream.h"
#include "PackCipher.h"

class CCipherStream :
	public CStream
{
public:
	CCipherStream(CStream * pLower);
	virtual ~CCipherStream(void);
public:
	//  µœ÷packstream
	BOOL Read(LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error);
	BOOL Write(const LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error);
	BOOL Close(CPackErrors & Error);
	BOOL Seek(LONGLONG nOffset, INT nOrg, CPackErrors & Error);
	ULONGLONG GetOffset();
	BOOL SetCipher(CPackCipher::pack_cipher_type_t nType, LPCTSTR strPassword);
private:
	CPackCipher m_Cipher;
	CStream * m_pLower;
};

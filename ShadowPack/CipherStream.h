#pragma once
#include "StreamBase.h"
#include "PackCipher.h"

class CCipherStream :
	public CStreamBase
{
public:
	CCipherStream(CStreamBase * pLower);
	virtual ~CCipherStream(void);
public:
	//  µœ÷packstream
	BOOL Read(LPVOID pBuffer, UINT nSize, CProgress & Progress, CPackErrors & Error);
	BOOL Write(const LPVOID pBuffer, UINT nSize, CProgress & Progress, CPackErrors & Error);
	BOOL Close(CPackErrors & Error);
	BOOL Seek(LONGLONG nOffset, INT nOrg, CPackErrors & Error);
	UINT GetOffset();
	BOOL SetCipher(CPackCipher::pack_cipher_type_t nType, LPCTSTR strPassword);
private:
	CPackCipher m_Cipher;
	CStreamBase * m_pLower;
};

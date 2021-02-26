#pragma once

#include "Progress.h"
#include "PackErrors.h"

class CStream
{
public:
	virtual BOOL Read(LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error) = 0;
	virtual BOOL Write(const LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error) = 0;
	virtual BOOL Close(CPackErrors & Error) = 0;
	virtual BOOL Seek(LONGLONG nOffset, INT nOrg,  CPackErrors & Error) = 0;
	virtual ULONGLONG GetOffset() = 0;
};

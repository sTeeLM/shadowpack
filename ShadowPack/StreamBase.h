#pragma once

#include "PackErrors.h"
#include "ProgressBase.h"

class CStreamBase
{
public:
	CStreamBase() {}
	virtual ~CStreamBase() {}
public:
	virtual BOOL Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors & Error) = 0;
	virtual BOOL Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors & Error) = 0;
	virtual BOOL Seek(INT nOffset, INT nOrg,  CPackErrors & Error) = 0;
	virtual UINT GetOffset() = 0;
};

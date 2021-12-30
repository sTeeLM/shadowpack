#pragma once

#include "PackErrors.h"
#include "ProgressBase.h"

class CStreamBase
{
public:
	CStreamBase() : m_nOffset(0){}
	virtual ~CStreamBase() {}
public:
	typedef enum _SEEK_TYPE_T {
		STREAM_SEEK_SET,
		STREAM_SEEK_CUR,
		STREAM_SEEK_END
	}SEEK_TYPE_T;
public:
	virtual BOOL Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors & Error) = 0;
	virtual BOOL Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors & Error) = 0;
	virtual BOOL Seek(LONGLONG nOffset, SEEK_TYPE_T Org,  CPackErrors & Error) = 0;
	virtual ULONGLONG GetOffset() { return m_nOffset; }
protected:
	ULONGLONG m_nOffset;
};

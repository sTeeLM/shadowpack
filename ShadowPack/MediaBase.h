#pragma once

#include "framework.h"
#include "PackErrors.h"
#include "PasswordGetterBase.h"
#include "ProgressBase.h"
#include "StreamBase.h"

class CMediaBase
{
public:
	CMediaBase() {}
	virtual ~CMediaBase() {}

public:
// open and close
	// load meta data of media into memory
	virtual BOOL Load(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
		CProgressBase& Progress, CPackErrors& Errors) = 0;

	// save changes to file
	virtual BOOL Save(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors) = 0;

	// free all buffers
	virtual void Close() = 0;

// show options
	virtual void ShowOptionDlg() = 0;

// read and write
	virtual INT Read(UINT nOffset, LPBYTE pBuffer, UINT nSize) = 0;
	virtual INT Write(UINT nOffset, LPBYTE pBuffer, UINT nSize) = 0;

// stream read and write
	virtual CStreamBase * GetStream() = 0;

// size
	virtual UINT GetUsedBytes() = 0;
	virtual UINT GetTotalBytes() = 0;

// misc
	virtual BOOL IsDirty() = 0;

};
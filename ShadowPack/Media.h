#pragma once

#include "Pack.h"
#include "PackErrors.h"
#include "Progress.h"
#include "PackCipher.h"

#define MEDIA_HEADER_SIG 0x11223344

class CMedia
{
public:
	typedef enum {
		MODE_READ = 0,
		MODE_WRITE
	}media_mode_t;
	class CMediaBlock
	{
	public:
		virtual BOOL ReadByteFromBlock(LPBYTE pBuffer, ULONGLONG nSize, ULONGLONG nBlockSize,
			DWORD dwParam, CPackErrors & Error) = 0;
		virtual BOOL WriteByteToBlock(const LPBYTE pBuffer, ULONGLONG nSize, ULONGLONG nBlockSize,
			DWORD dwParam, CPackErrors & Error) = 0;
		virtual ULONGLONG GetBlockSize() = 0;
		virtual ULONGLONG GetCapicity() = 0;
		virtual void SetBlockSize(ULONGLONG nBlockSize) = 0;
		virtual BOOL ReAlloc(ULONGLONG nSize, CPackErrors & Error) = 0;
	};
public:
	// read: Factory-> OpenMedia(read) -> (read stream/block) -> CloseMedia
    // write:          |- CloneMedia -> OpenMedia(write) -> (write stream /block) -> SaveMedia -> CloseMedia
	//																			|->SetToRead->CloseMedia
	virtual BOOL OpenMedia(media_mode_t mode, LPCTSTR szPath, CPasswordGetter &PasswordGetter, BOOL &bCancel, CPackErrors &Error, CProgress &Progress) = 0;
	virtual CMedia * CloneMedia(CPackErrors & Error) = 0;
	virtual BOOL SaveMedia(BOOL &bCancel, CPackErrors &Error, CProgress &Progress) = 0;
	virtual void CloseMedia() = 0;
	virtual BOOL ShowOptionDlg() = 0;
	virtual BOOL SetToRead(CPackErrors &Error) = 0;
	virtual CStream * GetStream() = 0;
	virtual ULONGLONG GetDataSize() = 0;
	virtual void SetDataSize(ULONGLONG nSize) = 0;
	virtual ULONGLONG GetCapicity() = 0;
	virtual void SetFormatChanged(BOOL bChanged) = 0;
	virtual BOOL FormatChanged() = 0;

	virtual BOOL ReadBlock(CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBLockCount, CPackErrors & Error) = 0;
	virtual BOOL WriteBlock(const CMediaBlock * pBlock, ULONGLONG nOffset,ULONGLONG nBlockCount, CPackErrors & Error) = 0;
	virtual ULONGLONG GetBlockCount() = 0;

	virtual LPCTSTR GetFilter() = 0;
	virtual LPCTSTR GetDefaultExt() = 0;
};

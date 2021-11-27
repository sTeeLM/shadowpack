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
protected:
#define MEDIA_HEADER_SIGN 0x12abcdef
	typedef struct _MEDIA_HEADER_T
	{
		DWORD dwSign;
		DWORD dwDataSize;
	}MEDIA_HEADER_T;
public:
// open and close
	// load meta data of media into memory
	virtual BOOL LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
		CProgressBase& Progress, CPackErrors& Errors) = 0;

	// save changes to file
	virtual BOOL SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors) = 0;

	// free all buffers
	virtual void CloseMedia() = 0;

// show options
	virtual void ShowMediaOptionDlg() = 0;

// stream read and write
	virtual CStreamBase * GetStream() = 0;

// size
	virtual UINT GetMediaUsedBytes() = 0;
	virtual UINT GetMediaTotalBytes() = 0;

// misc
	virtual BOOL IsMediaDirty() = 0;

};
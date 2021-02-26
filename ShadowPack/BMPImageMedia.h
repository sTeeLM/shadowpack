#pragma once

#include "PixelImageMedia.h"

class CBMPImageMedia :
	public CPixelImageMedia
{
public:
	CBMPImageMedia(void);
	virtual ~CBMPImageMedia(void);

public:
	// 实现CRAWPPMedia中的接口
	BOOL OpenFile(CMedia::media_mode_t mode, LPCTSTR szPath, BOOL & bCancel, CPackErrors & Error, CProgress & Progress);
	BOOL SaveFile(BOOL &bCancel, CPackErrors &Error, CProgress &Progress);
	void CloseFile();
	BOOL ReopenFileToRead(CPackErrors &Error);

	LPCTSTR GetFilter();
	LPCTSTR GetDefaultExt();


	// 实现CMedia中的接口
	BOOL ReadBlock(CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error);
	BOOL WriteBlock(const CMediaBlock * pBlock, ULONGLONG nOffset,ULONGLONG nBlockCount, CPackErrors & Error);
	ULONGLONG GetBlockCount();
	CMedia * CloneMedia(CPackErrors & Error);

public:
	// 实现CMediaFactory的接口
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMedia * Factory();

private:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;

	LPBITMAPFILEHEADER m_pfileHeader;
	FILE * m_pFile;
};

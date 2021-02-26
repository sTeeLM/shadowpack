#pragma once

#include "PixelImageMedia.h"

class CBMPImageMedia :
	public CPixelImageMedia
{
public:
	CBMPImageMedia(void);
	virtual ~CBMPImageMedia(void);

public:
	// ʵ��CRAWPPMedia�еĽӿ�
	BOOL OpenFile(CMedia::media_mode_t mode, LPCTSTR szPath, BOOL & bCancel, CPackErrors & Error, CProgress & Progress);
	BOOL SaveFile(BOOL &bCancel, CPackErrors &Error, CProgress &Progress);
	void CloseFile();
	BOOL ReopenFileToRead(CPackErrors &Error);

	LPCTSTR GetFilter();
	LPCTSTR GetDefaultExt();


	// ʵ��CMedia�еĽӿ�
	BOOL ReadBlock(CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error);
	BOOL WriteBlock(const CMediaBlock * pBlock, ULONGLONG nOffset,ULONGLONG nBlockCount, CPackErrors & Error);
	ULONGLONG GetBlockCount();
	CMedia * CloneMedia(CPackErrors & Error);

public:
	// ʵ��CMediaFactory�Ľӿ�
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMedia * Factory();

private:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;

	LPBITMAPFILEHEADER m_pfileHeader;
	FILE * m_pFile;
};

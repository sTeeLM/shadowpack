#pragma once
#include "PixelImageMedia.h"
#include "png.h"

class CPNGImageMedia :
	public CPixelImageMedia
{
public:
	CPNGImageMedia(void);
	virtual ~CPNGImageMedia(void);

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

	typedef struct PNGInfo {
		UINT nChannels;
		UINT nColorType;
		UINT nBitDepth;
		UINT nWidth;
		UINT nHeight;
		UINT nRowBytes;
	};

	PNGInfo m_PNGinfo;
	png_bytepp m_row_pointers;
	CString m_strErr;	

	static CProgress * m_pProgress;
	static ULONG m_nTotalRows;
	
	static void RowCallback(png_structp p, png_uint_32 u,int n);
	static void ErrCallback(png_structp png_ptr, png_const_charp message);
};

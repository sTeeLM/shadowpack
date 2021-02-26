#pragma once
#include "PixelImageMedia.h"

#include "tiff.h"

#include "tiffio.h"

#include <stdarg.h>

class CTIFFImageMedia :
	public CPixelImageMedia
{
public:
	CTIFFImageMedia(void);
	virtual ~CTIFFImageMedia(void);

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
	BOOL LoadTiffInfo(CPackErrors & Error);
	BOOL CloneTiffInfo(CTIFFImageMedia * pMediaTo);
	void FreeTiffInfo();
	BOOL AllocScanline();
	void FreeScanLine();
private:
	static void ErrorHandler(const char *module, const char *fmt, va_list ap);
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;
	
	static CString m_strLastError;

	class CTiffTagEntry {
	public:
		CTiffTagEntry() :
			m_nTag(0),
			m_pField(NULL),
			m_nValueCount(0),
			m_bAlloc(FALSE),
			m_pData(m_pRawData),
			m_nCount(0)
			{::ZeroMemory(m_pRawData, sizeof(m_pRawData));}
		virtual ~CTiffTagEntry(){}
		uint32             m_nTag;
		const TIFFField *  m_pField;
		uint32             m_nValueCount;
		BOOL               m_bAlloc;
		void *			   m_pData;
		uint32			   m_nCount;
		unin{
			
		} m_RawData;
		BYTE               m_pRawData[64]; // 大于 4 * max(TIFF datasize)
	};

	class CTiffDirEntry {
	public:
		CTiffDirEntry():
		  m_nScanlineSize(0),
          m_nSamplePerPixel(0)
		{}
		~CTiffDirEntry(){}
	public:
		static CTiffDirEntry *  LoadDirectory(TIFF * tif, CPackErrors & Error);
		BOOL SaveDirectory(TIFF * tif);
		void FreeDirectory();
	protected:
		BOOL LoadTagValue(TIFF * tif, CArray<CTiffTagEntry, CTiffTagEntry> & aTags, CPackErrors & Error);
	protected:
		CArray<CTiffTagEntry, CTiffTagEntry> m_TagInfo;
		CArray<CTiffTagEntry, CTiffTagEntry> m_ExifTagInfo;
		ULONGLONG m_nBlockCount;
		uint32 m_nScanlineSize;
		uint32 m_nSamplePerPixel;
	};

	CArray<CTiffDirEntry *, CTiffDirEntry *> m_DirInfo;
	LPBYTE * m_pScanline;
	ULONGLONG m_nBlockCount;
	TIFF * m_pTiff;
};

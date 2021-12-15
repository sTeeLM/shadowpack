#pragma once
#include "PixelImageMedia.h"
#include "png.h"
#include "OptPagePNGFile.h"

class CPNGFileMedia :
    public CPixelImageMedia
{
public:
    CPNGFileMedia();
    virtual ~CPNGFileMedia();

public:
	// 实现CMediaBase的接口
	// load meta data of media into memory
	BOOL LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
		CProgressBase& Progress, CPackErrors& Errors);

	// save changes to file
	BOOL SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);

	// free all buffers
	void CloseMedia();

	// 添加opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

public:
	// 实现CMediaFactory的接口
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMediaBase* Factory();
protected:
	static void ErrCallback(png_structp png_ptr, png_const_charp message);
	static void RowCallback(png_structp p, png_uint_32 u, int n);
protected:
	typedef struct _PNGInfo {
		UINT nChannels;
		INT nColorType;
		INT  nBitDepth;
		UINT nWidth;
		UINT nHeight;
		UINT nRowBytes;
		INT nInterlaceType;
		INT nCompressionType;
		INT nFilterType;
	}PNGInfo;
	PNGInfo m_PNGInfo;
	BOOL m_bError;
	CString m_strLastError;
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;
	CProgressBase* pProgress;
	COptPagePNGFile m_OptPagePNGFile;
};


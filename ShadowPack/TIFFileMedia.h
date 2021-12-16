#pragma once
#include "PixelImageMedia.h"
#include "libtiff\tiff.h"
#include "libtiff\tiffio.h"

class CTIFFileMedia :
    public CPixelImageMedia
{
public:
    CTIFFileMedia();
    virtual ~CTIFFileMedia();
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

protected:
	static void ErrorHandler(const char* module, const char* fmt, va_list ap);
public:
	// 实现CMediaFactory的接口
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMediaBase* Factory();

protected:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;
	TIFF* m_pTiff;
	CString m_strLastError;
	typedef struct _TIFFInfo {
		UINT nWidth;
		UINT nHeight;
		USHORT nCompression;
		USHORT nSamplesPerPixel;
		USHORT nBitPerSample;
		USHORT nPhotoMatric;
		USHORT nPlanarConfig;
		FLOAT  fXResolution;
		FLOAT  fYResolution;
		USHORT nResolutionUnit;
		USHORT nExtraSamples;
		USHORT *pExtraSamplesInfo;
	}TIFFInfo;
	TIFFInfo m_TIFFInfo;
};


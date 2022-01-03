#pragma once
#include "PixelImageMedia.h"
#include "OptPageTIFFFile.h"
#include "libtiff\tiff.h"
#include "libtiff\tiffio.h"

class CTIFFileMedia :
    public CPixelImageMedia
{
public:
    CTIFFileMedia();
    virtual ~CTIFFileMedia();
public:
	// ʵ��CMediaBase�Ľӿ�
	// load meta data of media into memory
	BOOL LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
		CProgressBase& Progress, CPackErrors& Errors);

	// save changes to file
	BOOL SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);

	// free all buffers
	void CloseMedia();

	// ���opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

protected:
	static void ErrorHandler(const char* module, const char* fmt, va_list ap);
public:
	// ʵ��CMediaFactory�Ľӿ�
	static CMediaBase* Factory();
	static LPCTSTR GetName();
	static LPCTSTR* GetExtTable();
protected:
	static LPCTSTR m_szName;
	static LPCTSTR m_szExtTable[];

	static CString m_strLastError;
	static CHAR szBuffer[];
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
	COptPageTIFFFile m_OptPageTIFFFile;
};


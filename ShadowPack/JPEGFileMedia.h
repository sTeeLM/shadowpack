#pragma once
#include "CorImageMedia.h"
#include "OptPageJPEGFile.h"
#include "MediaFactory.h"
#include "jpeglib.h"


class CJPEGFileMedia :
    public CCorImageMedia
{
public:
    CJPEGFileMedia();
    virtual ~CJPEGFileMedia();
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

	// 实现CCorImageMedia的接口
	UINT GetVSampleFactor(UINT nComponents);
	UINT GetHSampleFactor(UINT nComponents);

public:
	// 实现CMediaFactory的接口
	static CMediaBase* Factory();
	static void GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray);

protected:

	static void JStegErrorExit(j_common_ptr cinfo);
	static LPCTSTR szJPEGColorSpace[];
	static LPCTSTR szJFIFDensityUnit[];
protected:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;
	struct jpeg_decompress_struct m_Decinfo;
	jvirt_barray_ptr* m_virt_p;
	CString m_strLastError;
	BOOL m_bError;
	COptPageJPEGFile m_OptPageJPEGFile;

};


#pragma once
#include "CorImageMedia.h"
#include "jpeglib.h"

class CJPEGFileMedia :
    public CCorImageMedia
{
public:
    CJPEGFileMedia();
    virtual ~CJPEGFileMedia();
public:
	// ʵ��CMediaBase�Ľӿ�
	// load meta data of media into memory
	BOOL LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
		CProgressBase& Progress, CPackErrors& Errors);

	// save changes to file
	BOOL SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);

	// free all buffers
	void CloseMedia();

	// ����opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

	// ʵ��CCorImageMedia�Ľӿ�
	UINT GetVSampleFactor(UINT nComponents);
	UINT GetHSampleFactor(UINT nComponents);

public:
	// ʵ��CMediaFactory�Ľӿ�
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMediaBase* Factory();
protected:
	static void JStegErrorExit(j_common_ptr cinfo);
protected:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;
	struct jpeg_decompress_struct m_Decinfo;
	jvirt_barray_ptr* m_virt_p;
	CString m_strLastError;
	BOOL m_bError;
};

#pragma once
#include "PixelImageMedia.h"
#include "OptPageBMPFile.h"

class CBMPFileMedia :
    public CPixelImageMedia
{
public:
    CBMPFileMedia();
    virtual ~CBMPFileMedia();
public:
	// 实现CMediaBase的接口
	// load meta data of media into memory
	BOOL LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
		CProgressBase& Progress, CPackErrors& Errors);

	// save changes to file
	BOOL SaveMedia(LPCTSTR szFilePath, UINT nDataSize, CProgressBase& Progress, CPackErrors& Errors);

	// free all buffers
	void CloseMedia();

	// 添加opt page
	virtual void AddOptPage();

public:
	// 实现CMediaFactory的接口
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMediaBase* Factory();
protected:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;

	LPBITMAPFILEHEADER m_pfileHeader;
	COptPageBMPFile m_OptPageBMPFile;
};


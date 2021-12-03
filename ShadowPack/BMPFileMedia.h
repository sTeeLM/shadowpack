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
	// ʵ��CMediaBase�Ľӿ�
	// load meta data of media into memory
	BOOL LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter,
		CProgressBase& Progress, CPackErrors& Errors);

	// save changes to file
	BOOL SaveMedia(LPCTSTR szFilePath, UINT nDataSize, CProgressBase& Progress, CPackErrors& Errors);

	// free all buffers
	void CloseMedia();

	// ���opt page
	virtual void AddOptPage();

public:
	// ʵ��CMediaFactory�Ľӿ�
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMediaBase* Factory();
protected:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;

	LPBITMAPFILEHEADER m_pfileHeader;
	COptPageBMPFile m_OptPageBMPFile;
};


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
	BOOL SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);

	// free all buffers
	void CloseMedia();

	// 添加opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

public:
	// 实现CMediaFactory的接口
	static CMediaBase* Factory();
	static LPCTSTR GetName();
	static LPCTSTR* GetExtTable();
protected:
	static LPCTSTR m_szName;
	static LPCTSTR m_szExtTable[];

	LPBITMAPFILEHEADER m_pfileHeader;
	COptPageBMPFile m_OptPageBMPFile;
};


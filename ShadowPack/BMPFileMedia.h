#pragma once
#include "PixelImageMedia.h"
#include "OptPageBMPFile.h"
#include "MediaFactory.h"

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
	BOOL SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);

	// free all buffers
	void CloseMedia();

	// ���opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

public:
	// ʵ��CMediaFactory�Ľӿ�
	static CMediaBase* Factory();
	static void GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray);

protected:

	LPBITMAPFILEHEADER m_pfileHeader;
	COptPageBMPFile m_OptPageBMPFile;
};


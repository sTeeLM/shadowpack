#pragma once
#include "PixelImageMedia.h"
#include "OptPagePPMFile.h"

class CPPMFileMedia :
    public CPixelImageMedia
{
public:
    CPPMFileMedia();
    virtual ~CPPMFileMedia();
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
protected:
	INT m_nCols;
	INT m_nRows;
	INT m_nFormat;
	COptPagePPMFile m_OptPagePPMFile;
protected:
	static CString m_strLastError;
	static BOOL m_bError;
	static void ErrorMessage(const char* msg);
	static void ErrorQuit();
public:
	// ʵ��CMediaFactory�Ľӿ�
	static BOOL TestExt(LPCTSTR szExt);
	static LPCTSTR GetExtFilter();
	static CMediaBase* Factory();
protected:
	static LPCTSTR m_szFilter;
	static LPCTSTR m_szExt;

};

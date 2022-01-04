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
	// 实现CMediaFactory的接口
	static CMediaBase* Factory();
	static LPCTSTR GetName();
	static LPCTSTR* GetExtTable();
protected:
	static LPCTSTR m_szName;
	static LPCTSTR m_szExtTable[];

};


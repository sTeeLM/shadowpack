#pragma once
#include "FrameAudioMedia.h"
#include "audiofile.h"

class CWAVAudioMedia :
    public CFrameAudioMedia
{
public:
    CWAVAudioMedia();
    virtual ~CWAVAudioMedia();
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
	static LPCTSTR GetName();
	static LPCTSTR* GetExtTable();
protected:
	static LPCTSTR m_szName;
	static LPCTSTR m_szExtTable[];

protected:
	AFfilesetup m_FileSetup;
	static CString m_strLastError;
	static void WAVError(long nCode, const char* strMessage);
};


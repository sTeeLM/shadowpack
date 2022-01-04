#pragma once
#include "PCMAudioMedia.h"
#include "sndfile.h"

class CMiscAudioMedia :
    public CPCMAudioMedia
{
public:
    CMiscAudioMedia();
    virtual ~CMiscAudioMedia();
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
	typedef struct _MISC_AUDIO_INFO_T {
		SF_INFO snd_info;
		BOOL bCartInfoSet;
		SF_CART_INFO cart_info;
	}MISC_AUDIO_INFO_T;

	MISC_AUDIO_INFO_T m_Info;
};


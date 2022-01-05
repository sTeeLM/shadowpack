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
	typedef struct _MISC_AUDIO_META_T {
		SF_INFO snd_info;
		BOOL bCartInfoSet;
		SF_CART_INFO cart_info;
	}MISC_AUDIO_META_T;

	MISC_AUDIO_META_T m_FileMeta;
protected:
#define ONE_PASS_FRAMES 64
	INT GetBitsPerSample(INT nFormat);
	BOOL IsFloat(INT nFormat);
	BOOL GetMeta(SNDFILE* file, MISC_AUDIO_META_T& info, CPackErrors& Errors);
	BOOL SetMeta(SNDFILE* file, MISC_AUDIO_META_T& info, CPackErrors& Errors);
};


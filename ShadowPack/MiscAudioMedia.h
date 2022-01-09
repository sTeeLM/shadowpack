#pragma once
#include "PCMAudioMedia.h"
#include "OptPagePCMFileProperty.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

class CMiscAudioMedia :
    public CPCMAudioMedia
{
public:
    CMiscAudioMedia();
    virtual ~CMiscAudioMedia();
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
	static CString m_strLastLog;
	static void CBLogger(void* avcl, int level, const char* fmt, va_list vl);
	static CString GetErrorString(INT nErr);
protected:
	class CAudioMeta {
	public:

	public:
		void Free() 
		{
		}
	};

	CAudioMeta m_FileMeta;
#define PCM_LOG_BUFFER_SIZE 1024
	COptPagePCMFileProperty m_OptPagePCMFileProperty;
protected:
#define ONE_PASS_FRAMES 64
	INT CheckCodecID(INT nCodecID);
	BOOL IsFloat(INT nFormat);
	BOOL ProbeTotalFrames(AVFormatContext* pFormatCtx, LPCTSTR szFilePath,
		ULONGLONG& nTotalFrames, CPackErrors& Errors);
};


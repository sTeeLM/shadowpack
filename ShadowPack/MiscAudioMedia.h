#pragma once
#include "PCMAudioMedia.h"
#include "OptPagePCMFileProperty.h"
#include "MediaFactory.h"

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
	typedef struct _MISC_AUDIO_EXT {
		LPCTSTR szExts;
		LPCTSTR szName;
	} MISC_AUDIO_EXT;
	static CMediaBase* Factory();
	static void GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray);
	static MISC_AUDIO_EXT m_szExtTable[];
protected:
	static CString m_strLastLog;
	static void CBLogger(void* avcl, int level, const char* fmt, va_list vl);
	static CString GetErrorString(INT nErr);
	
protected:
	class CAudioMeta {
	public:
		CAudioMeta() :
			m_pFormatCtx(NULL),
			m_pCodecCtx(NULL),
			m_pCodec(NULL),
			m_TotalFrames(0)
		{

		}
		AVFormatContext* m_pFormatCtx;
		AVCodecContext* m_pCodecCtx;
		AVCodec* m_pCodec;
		ULONGLONG m_TotalFrames;
	public:
		void Free()
		{
			if (m_pFormatCtx) {
				avformat_close_input(&m_pFormatCtx);
			}
			if (m_pCodecCtx) {
				avcodec_free_context(&m_pCodecCtx);
			}
			m_pCodec = NULL;
			m_TotalFrames = 0;
		}
	};

	CAudioMeta m_FileMeta;
	COptPagePCMFileProperty m_OptPagePCMFileProperty;
protected:
#define ONE_PASS_FRAMES 4096
	static INT CheckCodecID(INT nCodecID);
	BOOL ProbeTotalFrames(AVFormatContext* pFormatCtx, AVCodecContext* pCodecCtx, LPCTSTR szFilePath,
		ULONGLONG& nTotalFrames, CPackErrors& Errors);
	INT EncodeFrame(AVFormatContext* pEncodeFormatCtx,
		AVCodecContext* pEncodeCodecCtx, AVCodecContext* pDecodeCodecCtx,
		AVFrame* pFrame, AVPacket* pPacket);
	CString FillInfoStr();
};


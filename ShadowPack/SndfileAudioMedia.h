#pragma once

#include "PCMAudioMedia.h"
#include "OptPagePCMFileProperty.h"
#include "MediaFactory.h"
#include "sndfile.h"

class CSndfileAudioMedia :
    public CPCMAudioMedia
{
public:
    CSndfileAudioMedia();
    virtual ~CSndfileAudioMedia();
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
	static void GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray);
protected:
	static CMediaFactory::MEDIA_EXT_TABLE_T m_szExtTable[];

protected:
	typedef SF_BROADCAST_INFO_VAR(16 * 1024) SF_BROADCAST_INFO_16K;
	typedef SF_CART_INFO_VAR(16 * 1024) SF_CART_INFO_16K;

	class CAudioMeta {
	public:
		SF_INFO snd_info;
		BOOL bInstSet;
		SF_INSTRUMENT inst;
		BOOL bCuesSet;
		SF_CUES cues;
		BOOL bBinfoSet;
		SF_BROADCAST_INFO_16K binfo;
		BOOL bChanMapSet;
		INT chanmap[256];
		BOOL bCartInfoSet;
		SF_CART_INFO_16K cart_info;
		CMap<INT, INT, CStringA, CStringA>meta_str;
		CString strFilePath;
	public:
		void Free() 
		{
			::ZeroMemory(&snd_info,sizeof(snd_info));
			::ZeroMemory(&inst, sizeof(inst));
			bInstSet = FALSE;
			::ZeroMemory(&cues, sizeof(cues));
			bBinfoSet = FALSE;
			::ZeroMemory(&binfo, sizeof(binfo));
			bChanMapSet = FALSE;
			::ZeroMemory(&chanmap, sizeof(chanmap));
			bCartInfoSet = FALSE;
			::ZeroMemory(&cart_info, sizeof(cart_info));
			meta_str.RemoveAll();
			strFilePath = _T("");
		}
	};

	CAudioMeta m_FileMeta;
#define PCM_LOG_BUFFER_SIZE 1024
	COptPagePCMFileProperty m_OptPagePCMFileProperty;
protected:
#define SND_FILE_ONE_PASS_FRAMES 64
	INT GetBitsPerSample(INT nFormat);
	BOOL IsFloat(INT nFormat);
	CString ReadLog(SNDFILE* file, SF_INFO &snd_info);
	CString GenerateDurationStr(SF_INFO& snd_info);
	BOOL GetMeta(SNDFILE* file, CAudioMeta& info, CPackErrors& Errors);
	BOOL SetMeta(SNDFILE* file, CAudioMeta& info, CPackErrors& Errors);
};

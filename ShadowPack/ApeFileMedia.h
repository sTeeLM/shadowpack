#pragma once
#include "PCMAudioMedia.h"
#include "MediaFactory.h"
#include "OptPagePCMFileProperty.h"
#include "All.h"
#include "MACLib.h"
class CApeFileMedia :
    public CPCMAudioMedia
{
public:
    CApeFileMedia();
    virtual ~CApeFileMedia();
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
	COptPagePCMFileProperty m_OptPageApeFileProperty;

protected:
#define APE_AUDIO_ONE_PASS_FRAMES 1024
	APE::WAVEFORMATEX m_WaveFormatEx;
	UINT m_nMacFlags;
	UINT m_nCompressLevel;
	ULONGLONG m_nTotalBlocks;
	CMap<INT, const INT&, CString, const CString&> m_ErrorString;

	void LoadErrorString();
	const CString & GetErrorString(INT nErrorCode);
	CString CompressLevel2String(UINT nCompressLevel);
	CString Flag2String(UINT nFlag);
public:
	// ʵ��CMediaFactory�Ľӿ�
	static CMediaBase* Factory();
	static void GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray);

};


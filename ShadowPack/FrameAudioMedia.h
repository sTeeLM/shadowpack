#pragma once
#include "BytePerBlockMedia.h"
#include "FileCache.h"
class CFrameAudioMedia :
    public CBytePerBlockMedia
{
public:
    CFrameAudioMedia();
    virtual ~CFrameAudioMedia();
public:
	// 实现父类接口，实现这几个接口让父类调用
	BYTE GetByteFromBlocks(ULONGLONG nOffset, UINT nBlockPerByte);
	void SetByteToBlocks(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte);
	ULONGLONG GetTotalBlocks();

	// 实现CMediaBase接口
	// 添加opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

public:
	typedef enum {
		FRAME_FORMAT_I8 = 0,
		FRAME_FORMAT_BEI16,
		FRAME_FORMAT_LEI16,
		FRAME_FORMAT_BEI32,
		FRAME_FORMAT_LEI32,
		FRAME_FORMAT_BEF32,
		FRAME_FORMAT_LEF32,
		FRAME_FORMAT_BEF64,
		FRAME_FORMAT_LEF64,
	}FRAME_FORMAT_T;
	BOOL Alloc(ULONGLONG nFrames, UINT nChannels, FRAME_FORMAT_T Fmt, CPackErrors& Error);
	void Free();
	void SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt, UINT nChannel, FRAME_FORMAT_T Fmt);
	void GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt, UINT nChannel, FRAME_FORMAT_T Fmt);
protected:
	CFileCache m_FileCache;
	BOOL m_bUseFileCache;
};


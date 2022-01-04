#pragma once
#include "BytePerBlockMedia.h"
#include "FileCache.h"
class CPCMAudioMedia :
    public CBytePerBlockMedia
{
public:
    CPCMAudioMedia();
    virtual ~CPCMAudioMedia();
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
		PCM_FORMAT_I8 = 0,
		PCM_FORMAT_BEI16,
		PCM_FORMAT_LEI16,
		PCM_FORMAT_BEI32,
		PCM_FORMAT_LEI32,
		PCM_FORMAT_BEF32,
		PCM_FORMAT_LEF32,
		PCM_FORMAT_BEF64,
		PCM_FORMAT_LEF64,
	}PCM_FORMAT_T;
	BOOL Alloc(ULONGLONG nFrames, UINT nChannels, PCM_FORMAT_T Fmt, CPackErrors& Error);
	void Free();
	void SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt, UINT nChannel, PCM_FORMAT_T Fmt);
	void GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt, UINT nChannel, PCM_FORMAT_T Fmt);
protected:
	CFileCache m_FileCache;
	BOOL m_bUseFileCache;
};


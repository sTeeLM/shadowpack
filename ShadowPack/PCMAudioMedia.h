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
	BOOL Alloc(ULONGLONG nFrames, UINT nChannels, UINT nBitsPerSample, CPackErrors& Error);
	void Free();
	void SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt);
	void GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt);
protected:
	CFileCache m_FileCache;
	BOOL m_bUseFileCache;
};


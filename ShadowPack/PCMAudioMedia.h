#pragma once
#include "BytePerBlockMedia.h"
#include "FileCache.h"
#include "OptPageBPBMedia.h"

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
	typedef enum _SAMPLE_FORMAT_T {
		SAMPLE_FMT_NONE = -1,
		SAMPLE_FMT_U8,          ///< unsigned 8 bits
		SAMPLE_FMT_S16,         ///< signed 16 bits
		SAMPLE_FMT_S32,         ///< signed 32 bits
		SAMPLE_FMT_FLT,         ///< float
		SAMPLE_FMT_DBL,         ///< double

		SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
		SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
		SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
		SAMPLE_FMT_FLTP,        ///< float, planar
		SAMPLE_FMT_DBLP,        ///< double, planar
		SAMPLE_FMT_S64,         ///< signed 64 bits
		SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

		SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
	}SAMPLE_FORMAT_T;
	BOOL Alloc(ULONGLONG nFrames, UINT nChannels, UINT nBitsPerSample, CPackErrors& Error);
	void Free();
	void SetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt);
	void GetFrame(LPVOID pBuffer, ULONGLONG nFrameOffset, UINT nFrameCnt);
	void SetSample(LPBYTE* pBuffer, UINT nLineSize, SAMPLE_FORMAT_T Format, INT &nSamples, INT nChannels);
	void GetSample(LPBYTE* pBuffer, UINT nLineSize, SAMPLE_FORMAT_T Format, INT &nSamples, INT nChannels);
protected:
	BYTE GetByteFromBlocks8(ULONGLONG nBlockOffset, UINT nBlockPerByte);
	BYTE GetByteFromBlocks162024(ULONGLONG nBlockOffset, UINT nBlockPerByte);
	BYTE GetByteFromBlocks3264(ULONGLONG nBlockOffset, UINT nBlockPerByte);

	void SetByteFromBlocks8(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte);
	void SetByteFromBlocks162024(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte);
	void SetByteFromBlocks3264(BYTE nData, ULONGLONG nBlockOffset, UINT nBlockPerByte);

protected:
	CFileCache m_FileCache;
	BOOL m_bUseFileCache;
	ULONGLONG m_nSampleCnt;
	LPBYTE m_pSampleBuffer;
	UINT m_nBitsPerSample;
	UINT m_nChannels;
	COptPageBPBMedia m_OptPCMAudioMedia;
};


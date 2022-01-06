#pragma once
#include "MediaBase.h"
#include "BytePerBlockMedia.h"
#include "OptPageBPBMedia.h"
#include "FileCache.h"

class CPixelImageMedia :
    public CBytePerBlockMedia
{
public:
	CPixelImageMedia();
	virtual ~CPixelImageMedia();

	// 定义究竟啥叫block
	typedef struct _IMAGE_PIXEL_T
	{
		BYTE m_nRed;
		BYTE m_nGreen;
		BYTE m_nBlue;
		BYTE m_nAlpha;
	}IMAGE_PIXEL_T;
	typedef enum {
		PIXEL_FORMAT_RGB = 0,
		PIXEL_FORMAT_RGBA,
		PIXEL_FORMAT_BGR
	}PIXEL_FORMAT_T;
public:
	BOOL Alloc(UINT nWidth, UINT nHeight, CPackErrors& Error);
	void Free();
	void SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue, BYTE nAlpha);
	void GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue, BYTE& nAlpha);
	void SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue);
	void GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue);
	void SetScanline(UINT nY, LPBYTE pBuffer, PIXEL_FORMAT_T Format);
	void GetScanline(UINT nY, LPBYTE pBuffer, PIXEL_FORMAT_T Format);
	void SetScanlinePerChannel(UINT nY, LPBYTE pBuffer, PIXEL_FORMAT_T Format, UINT nChannel);
	void GetScanlinePerChannel(UINT nY, LPBYTE pBuffer, PIXEL_FORMAT_T Format, UINT nChannel);
	BYTE & GetPixelR(UINT nX, UINT nY);
	BYTE & GetPixelG(UINT nX, UINT nY);
	BYTE& GetPixelB(UINT nX, UINT nY);
	BYTE& GetPixelA(UINT nX, UINT nY);

	// 实现父类接口，实现这几个接口让父类调用
	BYTE GetByteFromBlocks(ULONGLONG nOffset, UINT nBlockPerByte);
	void SetByteToBlocks(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte);
	ULONGLONG GetTotalBlocks();

	// 实现CMediaBase接口
	// 添加opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);

protected:
	UINT m_nWidth;
	UINT m_nHeight;
	IMAGE_PIXEL_T * m_pBlockBuffer;
	CFileCache m_FileCache;
	BOOL m_bUseFileCache;
	COptPageBPBMedia m_OptPagePixelImageMedia;
	BYTE m_nEmpty;
};


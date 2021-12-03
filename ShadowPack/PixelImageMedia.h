#pragma once
#include "MediaBase.h"
#include "BytePerBlockMedia.h"

class CPixelImageMedia :
    public CBytePerBlockMedia
{
public:
	CPixelImageMedia();
	virtual ~CPixelImageMedia();

	// 定义究竟啥叫block
	class CPixelBlock
	{
	public:
		typedef enum {
			PIXEL_FORMAT_RGB = 0,
			PIXEL_FORMAT_RGBA,
			PIXEL_FORMAT_BGR
		}PIXEL_FORMAT_T;
	public:
		CPixelBlock() :
			m_nRed(0),
			m_nGreen(0),
			m_nBlue(0),
			m_nAlpha(0)
		{};
		virtual ~CPixelBlock() {}
	protected:
		BYTE m_nRed;
		BYTE m_nGreen;
		BYTE m_nBlue;
		BYTE m_nAlpha;
		friend CPixelImageMedia;
	};
public:
	BOOL Alloc(UINT nWidth, UINT nHeight, CPackErrors& Error);
	void Free();
	void SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue, BYTE nAlpha);
	void GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue, BYTE& nAlpha);
	void SetPixel(UINT nX, UINT nY, BYTE nRed, BYTE nGreen, BYTE nBlue);
	void GetPixel(UINT nX, UINT nY, BYTE& nRed, BYTE& nGreen, BYTE& nBlue);
	void SetScanline(UINT nY, LPBYTE pBuffer, CPixelBlock::PIXEL_FORMAT_T Format);
	void GetScanline(UINT nY, LPBYTE pBuffer, CPixelBlock::PIXEL_FORMAT_T Format);

	// 实现父类接口，实现这几个接口让父类调用
	BYTE GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte);
	void SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte);
	UINT GetTotalBlocks();
protected:
	UINT m_nWidth;
	UINT m_nHeight;
	CPixelBlock * m_pBlockBuffer;
	static BYTE F5LookupTable[4][8];
	static BYTE F5RevLookupTable[8];

};


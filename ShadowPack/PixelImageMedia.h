#pragma once
#include "RAWPPMedia.h"


#define INIT_PIXEL_BUFFER_SIZE_IN_PIXEL (1024)

class CPixelImageMedia :
	public CRAWPPMedia
{
public:
	class CPixelBlock : public CMedia::CMediaBlock
	{
	public:
		#pragma pack(push, 1)
		typedef struct {
			BYTE Red;
			BYTE Green;
			BYTE Blue;
			BYTE Alpha;
		}image_pixel_t;
		#pragma pack(pop)
	public:
		CPixelBlock();
		virtual ~CPixelBlock();
		// 实现CMediaBlock中的接口
		BOOL ReadByteFromBlock(LPBYTE pBuffer, ULONGLONG nSize, ULONGLONG nBlockSize, 
			DWORD dwParam, CPackErrors & Error);
		BOOL WriteByteToBlock(const LPBYTE pBuffer, ULONGLONG nSize, ULONGLONG nBlockSize,
			DWORD dwParam, CPackErrors & Error);
		ULONGLONG GetBlockSize(){return m_nBlockSize;}
		void SetBlockSize(ULONGLONG nBlockSize) {m_nBlockSize = nBlockSize;}
		ULONGLONG GetCapicity() {return m_nCapicity;}
		BOOL ReAlloc(ULONGLONG nSize, CPackErrors & Error);
		image_pixel_t & GetPixelAt(ULONGLONG n){return m_pPixel[n];}
		BYTE & Red(ULONGLONG n) {return m_pPixel[n].Red;}
		BYTE & Green(ULONGLONG n) {return m_pPixel[n].Green;}
		BYTE & Blue(ULONGLONG n) {return m_pPixel[n].Blue;}
		BYTE & Alpha(ULONGLONG n) {return m_pPixel[n].Alpha;}
	protected:
		image_pixel_t * m_pPixel;
		ULONGLONG m_nBlockSize;
		ULONGLONG m_nCapicity;
	};

public:
	CPixelImageMedia(void);
	virtual ~CPixelImageMedia(void);

	// 实现CMedia中的接口
	CMediaBlock * AllocBlockBuffer(CPackErrors & Error);
	void FreeBlockBuffer(CMediaBlock * pBlockBuffer);
};

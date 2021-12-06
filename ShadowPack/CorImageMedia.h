#pragma once
#include "BytePerBlockMedia.h"
class CCorImageMedia :
    public CBytePerBlockMedia
{
public:
    CCorImageMedia();
    virtual ~CCorImageMedia();

public:
	// 定义究竟啥叫block
	class CCorBlock
	{
	public:
		CCorBlock() 
		{};
		virtual ~CCorBlock() {}
	protected:
		friend CCorImageMedia;
	};

public:
	BOOL Alloc(UINT nWidth, UINT nHeight, CPackErrors& Error);
	void Free();

public:
	// 实现父类接口，实现这几个接口让父类调用
	BYTE GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte);
	void SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte);
	UINT GetTotalBlocks();

	// 实现CMediaBase接口
	// 添加opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);
};


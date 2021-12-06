#pragma once
#include "BytePerBlockMedia.h"
class CCorImageMedia :
    public CBytePerBlockMedia
{
public:
    CCorImageMedia();
    virtual ~CCorImageMedia();

public:
	// ���徿��ɶ��block
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
	// ʵ�ָ���ӿڣ�ʵ���⼸���ӿ��ø������
	BYTE GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte);
	void SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte);
	UINT GetTotalBlocks();

	// ʵ��CMediaBase�ӿ�
	// ���opt page
	virtual void AddOptPage(CMFCPropertySheet* pPropertySheet);
	virtual BOOL UpdateOpts(CMFCPropertySheet* pPropertySheet);
};


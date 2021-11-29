#pragma once
#include "MediaBase.h"

// 几个“Block”包含一个byte的Media抽象实现
class CBytePerBlockMedia :
    public CMediaBase, public CStreamBase
{
public:
    CBytePerBlockMedia();
    virtual ~CBytePerBlockMedia();

protected:

#define BPB_MEDIA_HEADER_SIGN 0x54188FE1
	typedef struct _BPB_MEDIA_HEADER_T
	{
		MEDIA_HEADER_T MediaHeader;
		DWORD dwBPBMediaSign;
		DWORD dwBlockPerByte;
	}BPB_MEDIA_HEADER_T;

	// 子类需要定义Block长啥样
	class CBlockBase
	{
	public:
		CBlockBase() {};
		virtual ~CBlockBase() {};
	public:
		virtual BYTE GetByteFromBlocks(CBlockBase* pBlock, UINT nBlockPerByte) = 0;
		virtual void SetByteToBlocks(CBlockBase* pBlock, UINT nBlockPerByte) = 0;
		virtual void CopyFrom(const CBlockBase * pBlock) = 0;
	};
	
public:	

	// 给子类调用的工具函数
	virtual BOOL Alloc(UINT nBlocks, CPackErrors& Error) = 0;
	virtual void Free();
	void SetBlocks(UINT nBlockOffset, CBlockBase* pBlock, UINT nBlockCnt);
	void GetBlocks(UINT nBlockOffset, CBlockBase* pBlock, UINT nBlockCnt);

	// 给子类调用的工具函数
	BOOL LoadMeta(CPasswordGetterBase& PasswordGetter, CPackErrors& Errors);
	BOOL SaveMeta(CPackErrors& Errors);

	// 实现了CStreamBase的接口
	BOOL Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Seek(INT nOffset, INT nOrg, CPackErrors& Error);
	UINT GetOffset();

// 实现CMediaBase接口
	// stream read and write
	CStreamBase* GetStream();
	// size
	UINT GetMediaUsedBytes();
	UINT GetMediaTotalBytes();

	// misc
	BOOL IsMediaDirty();

protected:
	BPB_MEDIA_HEADER_T m_Header;
	CBlockBase* m_pBlockBuffer;
	UINT m_nBlockBufferSize;
};


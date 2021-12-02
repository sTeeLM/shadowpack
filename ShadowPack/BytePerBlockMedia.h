#pragma once
#include "MediaBase.h"
#include "PackCipher.h"

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
		MEDIA_HEADER_T BPBHeader;
		DWORD dwBPBMediaSign;
		DWORD dwBPBBlockPerByte;
	}BPB_MEDIA_HEADER_T;

#define MAX_BPB_MEDIA_BPB_SIZE 4
#define MIN_BPB_MEDIA_BPB_SIZE 1

	// 子类需要定义Block长啥样
	class CBlockBase
	{
	public:
		CBlockBase() {};
		virtual ~CBlockBase() {};
	public:
		virtual BYTE GetByteFromBlocks(CBlockBase* pBlock, UINT nBlockPerByte) = 0;
		virtual void SetByteToBlocks(BYTE nData, CBlockBase* pBlock, UINT nBlockPerByte) = 0;
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
#define BPB_STREAM_BATCH_SIZE 4096
	BOOL Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Seek(INT nOffset, CStreamBase::SEEK_TYPE_T Org, CPackErrors& Error);

// 实现CMediaBase接口
	// stream read and write
	CStreamBase* GetStream();
	// size
	UINT GetMediaUsedBytes();
	UINT GetMediaTotalBytes();
	BOOL SetMediaUsedBytes(UINT nSize, CPackErrors& Error);

	// misc
	BOOL IsMediaDirty() { return m_bIsDirty; }

protected:
	BOOL TestHeaderValid(const BPB_MEDIA_HEADER_T* pHeader);
	BOOL RawReadData(LPVOID pBuffer, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors);
	BOOL RawWriteData(LPVOID pBuffer, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors);

protected:
	BPB_MEDIA_HEADER_T m_Header;
	CBlockBase* m_pBlockBuffer;
	UINT m_nBlockBufferSize;
	CPackCipher m_Cipher;
	BOOL m_bIsDirty;
};


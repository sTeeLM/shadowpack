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
		DWORD dwBPBCipher;
	}BPB_MEDIA_HEADER_T;

#define MAX_BPB_MEDIA_BPB_SIZE 4
#define MIN_BPB_MEDIA_BPB_SIZE 1

	
public:	

	// 子类需要实现的接口函数，我会调用
	virtual BYTE GetByteFromBlocks(ULONGLONG nOffset, UINT nBlockPerByte) = 0;
	virtual void SetByteToBlocks(BYTE nData, ULONGLONG nOffset, UINT nBlockPerByte) = 0;
	virtual ULONGLONG GetTotalBlocks() = 0;

	// 给子类调用的接口，子类会调
	BOOL LoadMeta(CPasswordGetterBase& PasswordGetter, CPackErrors& Errors);
	BOOL SaveMeta(CPackErrors& Errors);
	BOOL FillEmptySpace(CProgressBase& Progress, CPackErrors& Errors);

	// 实现了CStreamBase的接口

	BOOL Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Seek(LONGLONG nOffset, CStreamBase::SEEK_TYPE_T Org, CPackErrors& Error);

	// 实现CMediaBase接口
	// stream read and write
	CStreamBase* GetStream();
	// size
	ULONGLONG GetMediaUsedBytes();
	ULONGLONG GetMediaTotalBytes();
	BOOL SetMediaUsedBytes(ULONGLONG nSize, CPackErrors& Error);

	// misc
	BOOL IsMediaDirty() { return m_bIsDirty; }
	void SetMediaDirty() { m_bIsDirty = TRUE; }
	void ClearMediaDirty() { m_bIsDirty = FALSE; }

	BOOL TestHeaderValid(const MEDIA_HEADER_T* pHeader);

protected:
	BOOL RawReadData(LPVOID pBuffer, ULONGLONG nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors);
	BOOL RawWriteData(LPVOID pBuffer, ULONGLONG nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors);

protected:
#define BPB_STREAM_BATCH_SIZE 4096
static BYTE ReadWriteBuffer[BPB_STREAM_BATCH_SIZE];
#ifdef DEBUG
	void DumpHeader(CString strWhen, BPB_MEDIA_HEADER_T& Header);
#endif

protected:
	BPB_MEDIA_HEADER_T m_Header;
	CPackCipher m_Cipher;
	BOOL m_bIsDirty;
};


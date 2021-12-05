#pragma once
#include "MediaBase.h"
#include "PackCipher.h"

// ������Block������һ��byte��Media����ʵ��
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

	// ������Ҫʵ�ֵĽӿں������һ����
	virtual BYTE GetByteFromBlocks(UINT nOffset, UINT nBlockPerByte) = 0;
	virtual void SetByteToBlocks(BYTE nData, UINT nOffset, UINT nBlockPerByte) = 0;
	virtual UINT GetTotalBlocks() = 0;

	// ��������õĽӿڣ�������
	BOOL LoadMeta(CPasswordGetterBase& PasswordGetter, CPackErrors& Errors);
	BOOL SaveMeta(CPackErrors& Errors);

	// ʵ����CStreamBase�Ľӿ�
#define BPB_STREAM_BATCH_SIZE 4096
	BOOL Read(LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Write(const LPVOID pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Error);
	BOOL Seek(INT nOffset, CStreamBase::SEEK_TYPE_T Org, CPackErrors& Error);

// ʵ��CMediaBase�ӿ�
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
	BOOL RawReadData(LPVOID pBuffer, UINT nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors);
	BOOL RawWriteData(LPVOID pBuffer, UINT nOffset, UINT nSize, UINT nBPBBlockPerByte, CPackErrors& Errors);

protected:
	BPB_MEDIA_HEADER_T m_Header;
	CPackCipher m_Cipher;
	BOOL m_bIsDirty;
};

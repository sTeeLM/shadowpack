#pragma once

#include "Media.h"
#include "Stream.h"

class CRAWPPMedia :
	public CMedia, CStream
{
// 类型定义
public:
	#pragma pack(push, 1)
	typedef struct {
		DWORD dwSignature; 
		DWORD dwCipherType;
		union {
			struct {
				WORD wFormat;
				WORD wParam;
			};
			DWORD dwFormat;
		}uFormat;
		ULONGLONG uDataSize;
		ULONGLONG uCapicity;
	}rawpp_media_header_t;
	#pragma pack(pop)

	typedef enum{
		PFP_INVALID = 0,
		PFP_1PP,
		PFP_2PP,
		PFP_3PP,
		PFP_COUNT
	} rawpp_media_format_param_t;

	typedef enum {
		PF_NONE = 0,
		PF_RAWPP,
		PF_COUNT
	}rawpp_media_format_t;

// 构造函数
public:
	CRAWPPMedia(void);
	virtual ~CRAWPPMedia(void);

public:
	// 定义实际操作物理媒体的类应该实现的接口
	// 打开文件
	virtual BOOL OpenFile(media_mode_t mode, LPCTSTR szPath, BOOL &bCancel, CPackErrors &Error, CProgress &Progress) = 0;
	virtual BOOL SaveFile(BOOL &bCancel, CPackErrors &Error, CProgress &Progress) = 0;
	virtual BOOL ReopenFileToRead(CPackErrors &Error) = 0;
	virtual void CloseFile() = 0;
	virtual CMediaBlock * AllocBlockBuffer(CPackErrors & Error) = 0;
	virtual void FreeBlockBuffer(CMediaBlock * pBlockBuffer) = 0;

	// 实现Stream的接口
	BOOL Read(LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error);
	BOOL Write(const LPVOID pBuffer, ULONGLONG nSize, CProgress & Progress, CPackErrors & Error);
	BOOL Close(CPackErrors & Error);
	BOOL Seek(LONGLONG nOffset, INT nOrg,  CPackErrors & Error);
	ULONGLONG GetOffset(void) {return m_nOffset;}

	// 实现Media的部分接口
	BOOL OpenMedia(media_mode_t mode, LPCTSTR szPath, CPasswordGetter &PasswordGetter, BOOL &bCancel, CPackErrors &Error, CProgress &Progress);
	BOOL SaveMedia(BOOL &bCancel, CPackErrors &Error, CProgress &Progress);
	void CloseMedia();
	BOOL ShowOptionDlg();
	BOOL SetToRead(CPackErrors &Error);
	CStream * GetStream() {return m_pStream;}
	ULONGLONG GetDataSize() {return m_Header.uDataSize;}
	void SetDataSize(ULONGLONG nSize){ m_Header.uDataSize = nSize;}
	void SetFormatChanged(BOOL bChanged = TRUE){m_bFormatChanged = bChanged;}
	BOOL FormatChanged(){return m_bFormatChanged;}

/*  如下 接口在派生类中实现
	virtual BOOL ReadBlock(CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBLockCount, CPackErrors & Error) = 0;
	virtual BOOL WriteBlock(const CMediaBlock * pBlock, ULONGLONG nOffset,ULONGLONG nBlockCount, CPackErrors & Error) = 0;
	virtual ULONGLONG GetBlockCount() = 0;
	virtual LPCTSTR GetFilter() = 0;
	virtual LPCTSTR GetDefaultExt() = 0;
	virtual CMedia * CloneMedia() = 0;
*/

protected:
	rawpp_media_header_t & GetMediaHeader(){return m_Header;}
	ULONGLONG GetFreeSize() {return m_Header.uCapicity - m_Header.uDataSize;}
	ULONGLONG GetCapicity() {return m_Header.uCapicity;}
	WORD GetFormat(){return m_Header.uFormat.wFormat;}
	WORD GetFormatParam(){return  m_Header.uFormat.wParam;}
	BOOL IsEncryptMedia() {return m_Header.dwCipherType != CPackCipher::CIPHER_NONE;}
	CPackCipher::pack_cipher_type_t GetCipherType() {return (CPackCipher::pack_cipher_type_t)m_Header.dwCipherType;}
	void SetMediaFilePath(LPCTSTR szPath) {m_strMediaFilePath = szPath;}
	LPCTSTR GetMediaFilePath(){return m_strMediaFilePath;}
	media_mode_t GetMode(){return m_Mode;}
	ULONGLONG GetMediaHeaderSize() {return (ULONGLONG) sizeof(m_Header);}
	BOOL IsValidMediaHeader();
	ULONGLONG GetFormatParamCap(rawpp_media_format_param_t t);
	BOOL CanSetNewFormatParam(rawpp_media_format_param_t t);
	void SetMediaMode(media_mode_t mode) {m_Mode = mode;}
	void SetPassword(LPCTSTR szPass) {m_strPassword = szPass;}
	LPCTSTR GetPassword(){return m_strPassword;}
	BOOL RandomFillFreeSpace(BOOL & bCancel, CPackErrors &Error, CProgress &Progress);
	void CloneCore(CRAWPPMedia * pFrom);
	void SetOrgMedia(CMedia * pFrom) {m_pOrgMedia = pFrom;}
	CMedia * GetOrgMedia() {return m_pOrgMedia;}
	void DettachOrgMedia(){m_pOrgMedia = NULL;}
protected:
	rawpp_media_header_t m_Header;
	CStream * m_pStream;
	CString m_strMediaFilePath;
	media_mode_t m_Mode;
	BOOL m_bFormatChanged;
	CMediaBlock * m_pBlockBuffer;
	ULONGLONG m_nOffset;
	CString m_strPassword;
	CMedia * m_pOrgMedia;
};

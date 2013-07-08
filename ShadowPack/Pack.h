#pragma once

// Pack.h : ͷ�ļ�
//
#include "PackItem.h"
#include "SBitmap.h"
#include "jpeglib.h"

typedef CString & (*CB_GET_PASSWORD)();
typedef void (*CB_SET_PROGRESS)(INT nProgress);

#define PFP_1PP 1
#define PFP_2PP 2
#define PFP_3PP 3

#define PFP_1PJ 1
#define PFP_2PJ 2
#define PFP_4PJ 3

#define PACKAGE_HEADER_SIG 0x11223344

class CCorBuffer;

class CPack
{
public:
	enum PackFormat {
		PF_NONE = 0,
		PF_RAWPP = 1, // 1 bytes per pixel
		PF_JSTEG  = 2
	};

	enum EncryptMethod {
		EM_NONE = 0,
		EM_AES = 1, // AES
		EM_BLOWFISH = 2, // RC5
		EM_CAST = 3, // CAST
	};
private:
		typedef struct {
		DWORD dwSignature; 
		DWORD dwEncryptType;
		DWORD dwFormat;
		DWORD dwFormatParam;
		DWORD dwCount;
		DWORD dwDataSize;
		DWORD dwCapicity;
	}PackHeader;

	typedef PackHeader *(*HANDLER_READ_IMAGE)(LPCTSTR szSrc, LPCTSTR szExt, CSBitmap & bmp, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);
	typedef BOOL (*HANDLER_WRITE_IMAGE)(const PackHeader * data, const CSBitmap & bmp, LPCTSTR szDst, LPCTSTR szExt, 
		CString & szError, BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);
	typedef BOOL (*HANDLER_CAN_SET_FORMAT)(DWORD dwParam, const PackHeader * data, const CSBitmap & bmp);
	typedef BOOL (*HANDLER_SET_FORMAT)(DWORD dwParam, PackHeader * data, const CSBitmap & bmp);
	typedef void (*HANDLE_GET_SAVE_FILTER)(CString & szFilter);
	typedef void (*HANDLE_GET_SAVE_DEFAULT_EXT)(CString & szExt);


	typedef struct {
		TCHAR * szName;
		PackFormat eFormat;
		TCHAR ** szExts;
		HANDLER_READ_IMAGE fnReadImage;
		HANDLER_WRITE_IMAGE fnWriteImage;
		HANDLER_CAN_SET_FORMAT fnCanSetFormat;
		HANDLER_SET_FORMAT fnSetFormat;
		HANDLE_GET_SAVE_FILTER fnGetSaveFilter;
		HANDLE_GET_SAVE_DEFAULT_EXT fnGetSaveDefaultExt;
	}PackHandler;

	
	static CPack * LoadFromImageByHandle(LPCTSTR szSrc, LPCTSTR szExt,PackHandler * handle, CString & szError, BOOL * bCancel,
							 CB_GET_PASSWORD fnGetPass, CB_SET_PROGRESS fnSetProgress);

public:
	CPack(void);
	virtual ~CPack(void);
	static CPack * LoadFromImage(LPCTSTR szSrc, LPCTSTR szFileExt, CString & szError,  BOOL * bCancel, 
		CB_GET_PASSWORD fnGetPass, CB_SET_PROGRESS fnSetProgress);
	BOOL SaveToImage(LPCTSTR szSrc, LPCTSTR szFileExt, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);
	DWORD GetPackItemCount() const;
	CPackItem * GetPackItem(UINT nIndex);
	BOOL AddPackItem(CPackItem * pItem, UINT & nIndex, CString & Error);
	BOOL RemovePackItem(UINT nIndex, UINT nCount = 1);
	BOOL IsDirty();
	BOOL IsEmpty();
	BOOL Clear();
	const EncryptMethod GetEncryptMethod() const {return (EncryptMethod)m_Header.dwEncryptType;}
	void  SetEncryptMethod(EncryptMethod eMethod){m_Header.dwEncryptType = (DWORD)eMethod;}
	const CString & GetPassword() const {return m_szPassword;}
	void  SetPassword(const CString & szPassword);
	BOOL  CanSetFormat(PackFormat eFormat, DWORD dwParam) const;
	const PackFormat GetPackFormat() const {return (PackFormat)m_Header.dwFormat;}
	const DWORD GetPackFormatParam() const {return m_Header.dwFormatParam;}
	BOOL  SetPackFormat(PackFormat eFormat, DWORD dwParam);
	DWORD GetTotalSize() {return m_Header.dwDataSize + sizeof(m_Header); }
	DWORD GetDataSize()  {return m_Header.dwDataSize; }
	DWORD GetCapicity() {return m_Header.dwCapicity; }
	const CString & GetFilter();
	const CString & GetDefaultExt();
private:
	void SetCapicity();
	static void GenerateKey(BYTE * key, LPCTSTR szPassword);
	static BOOL DecryptData(PBYTE pBuffer, size_t size, EncryptMethod eEmethod, LPCTSTR szPassword);
	static BOOL EncryptData(PBYTE pBuffer, size_t size, EncryptMethod eEmethod, LPCTSTR szPassword);
	static BOOL ReadRawImage(Magick::Image &image, PBYTE pBuffer, size_t offset, 
		size_t size, PackFormat eFormat,  BOOL * bCancel = NULL, CB_SET_PROGRESS fnSetProgress = NULL);
	static BOOL WriteRawImage(Magick::Image &image, const PBYTE pBuffer, size_t offset, 
		size_t size, PackFormat eFormat,  BOOL * bCancel = NULL, CB_SET_PROGRESS fnSetProgress = NULL);
	static BOOL IsValidHeader(const PackHeader & header);
	static PackHeader * LoadBufferFromImage(Magick::Image &image,  BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);

	static PackHandler * GetHandlerByFormat(PackFormat eFormat);

	static std::list<CPack::PackHandler *> GetHandlersByExt(LPCTSTR szExt);

	static void PadMemory(LPBYTE buffer, size_t size);
private:

	PackHeader m_Header;
	CArray<CPackItem *, CPackItem *> m_PackItemList;
	CString m_szPassword;
	BOOL m_bIsDirty;
	CSBitmap m_Bmp;

	CString m_szFilter;
	CString m_szExt;

	static PackHandler m_Handler[3];

	// for  PF_RAWPP
	static PackHeader * RawPPReadImage(LPCTSTR szSrc, LPCTSTR szExt, CSBitmap & bmp, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);
	static BOOL RawPPWriteImage(const PackHeader * data, const CSBitmap & bmp, LPCTSTR szSrc, LPCTSTR szExt, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);
	static BOOL RawPPCanSetFormat(DWORD dwParam, const PackHeader * data, const CSBitmap & bmp);
	static BOOL RawPPSetFormat( DWORD dwParam, PackHeader * data, const CSBitmap & bmp);
	static void RawPPGetSaveFilter(CString & szFilter);
	static void RawPPSaveDefaultExt(CString & szExt);

	static BOOL RawPPWriteImageInternal(CSBitmap & bmp, const LPBYTE pBuffer, size_t offset, size_t size, 
						  DWORD dwFormaParamt,  BOOL * bCancel = NULL, CB_SET_PROGRESS fnSetProgress = NULL);
	static BOOL RawPPReadImageInternal(CSBitmap & bmp, LPBYTE pBuffer, size_t offset, size_t size, 
						 DWORD dwFormaParamt,  BOOL * bCancel = NULL, CB_SET_PROGRESS fnSetProgress = NULL);
	// for  PF_JSTEG
	static PackHeader * JStegReadImage(LPCTSTR szSrc, LPCTSTR szExt, CSBitmap & bmp, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);
	static BOOL JStegWriteImage(const PackHeader * data, const CSBitmap & bmp, LPCTSTR szSrc, LPCTSTR szExt,CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);
	static BOOL JStegCanSetFormat( DWORD dwParam, const PackHeader * data, const CSBitmap & bmp);
	static BOOL JStegSetFormat(DWORD dwParam, PackHeader * data, const CSBitmap & bmp);
	static void JStegGetSaveFilter(CString & szFilter);
	static void JStegSaveDefaultExt(CString & szExt);
	static BOOL RawPJReadDataInternal(CCorBuffer & sbuf, LPBYTE pRet , size_t size,
			DWORD dwFormatParam, BOOL * bCancel = NULL, CB_SET_PROGRESS fnSetProgress = NULL);
	typedef struct {
		CCorBuffer * buffer;
		BOOL bError;
		DWORD dwFormatParam;
	}JStegParam;
	static void JStegErrorExit(j_common_ptr cinfo);
	static void JStegReadData(j_common_ptr cinfo, JCOEF data);
	static JCOEF JStegWriteData(j_common_ptr cinfo, JCOEF data);
	static INT JStegGetCap(INT x, INT y);

	// for none
	static PackHeader * DefaultReadImage(LPCTSTR szSrc, LPCTSTR szExt, CSBitmap & bmp, CString & szError, 
		BOOL * bCancel, CB_SET_PROGRESS fnSetProgress);


};

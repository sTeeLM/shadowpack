#pragma once

#include <atlpath.h>

#include "PackErrors.h"
#include "Progress.h"
#include "Stream.h"

#define PACKAGE_ITEM_SIG 0xCCDDEEFF
#define ROOT_NAME _T("ROOT")

class CPackItem;
class CMedia;

typedef BOOL (*CB_WALK)(CPackItem * pItem, BOOL bIsEOD, LPVOID pParam);
typedef BOOL (*CB_REMOVE)(CPackItem * pItem, LPVOID pParam);

class CPackItem
{
public:
	typedef enum {
		TYPE_NONE,
		TYPE_DIR,
		TYPE_FILE,
		TYPE_EOD
	}pack_item_type_t;

	typedef enum {
		REF_NONE,
		REF_DISK,
		REF_MEDIA
	}pack_item_ref_t;

	typedef struct {
		DWORD dwSignature;
		DWORD dwType;      // pack_item_type_t
		DWORD dwNameSize; // not include \0
		DWORD dwDataSize;
	}pack_item_header_t;

public:
	CPackItem(CPackItem * pParent);
	virtual ~CPackItem(void);

public:
	static BOOL IsValidItemHeader(const pack_item_header_t & header);
	BOOL RemoveChild(CPackItem* pItem, CB_REMOVE cb = NULL, LPVOID pParam = NULL);
	BOOL RemoveAllChildren(CB_REMOVE cb = NULL, LPVOID pParam = NULL);
	CString GetName() {return m_strName;}
	void SetName(LPCTSTR szName);
	CString GetExt();
	CPackItem * GetChild(INT nIndex) {return m_Children[nIndex];}
	void AddChild(CPackItem * pItem);
	CPackItem * GetParent() {return m_pParent;}
	INT GetChildrenCount() {return m_Children.GetCount(); }
	LONGLONG GetNameSize() {return m_ItemHeader.dwNameSize;}
	LONGLONG GetHeaderSize() {return (LONGLONG) sizeof(m_ItemHeader);}
	LONGLONG GetDataSize() {return m_ItemHeader.dwDataSize;}
	LONGLONG GetEODSize() {
		if(IsDir())
			return m_ItemHeader.dwNameSize + sizeof(m_ItemHeader);
		else
			return 0;
	}
	LONGLONG GetTotalSize() {
		if(IsDir())
			return 2 * (m_ItemHeader.dwNameSize + sizeof(m_ItemHeader));
		else
			return m_ItemHeader.dwDataSize + m_ItemHeader.dwNameSize + sizeof(m_ItemHeader);
	}
	void SetDataSize(LONGLONG size) {m_ItemHeader.dwDataSize = (DWORD)size;}
	void SetType(pack_item_type_t type) {m_ItemHeader.dwType = (DWORD)type;}
	pack_item_type_t GetType(){ return (pack_item_type_t) m_ItemHeader.dwType;}
	BOOL IsRoot(){return lstrcmp(GetName(),ROOT_NAME) == 0;}
	BOOL IsFile(){return GetType() == TYPE_FILE;}
	BOOL IsDir(){return GetType() == TYPE_DIR;}
	void SetRef(pack_item_ref_t ref) {m_eRef = ref;}
	pack_item_ref_t GetRef(){ return m_eRef;}
	BOOL IsRefMedia() {return GetRef() == REF_MEDIA;}
	BOOL IsRefDisk() {return GetRef() == REF_DISK;}
	void SetDiskLocation(LPCTSTR szPath) {m_DiskLocation = szPath;}
	LPCTSTR GetDiskLocation() {return m_DiskLocation;}
	void SetOffset(ULONGLONG nOffset) {m_nOffset = nOffset;}
	ULONGLONG GetOffset() {return m_nOffset;}
	BOOL HasChild(LPCTSTR szName, pack_item_type_t type);

	BOOL WalkItemTree(CB_WALK cb, LPVOID pParam);

	static LONGLONG EvalSize(pack_item_type_t type, LPCTSTR szPath, LONGLONG nDataSize);

	static CPackItem * CreatePackItemFromDir(CPackItem * pParent, LPCTSTR szPath, LPCTSTR szName, CPackErrors & Error);
	static CPackItem * CreatePackItemFromFile(CPackItem * pParent, LPCTSTR szPath,LPCTSTR szName, LONGLONG nDataSize, CPackErrors & Error);

	static CPackItem * CreatePackItemFromMediaFile(CPackItem * pParent, pack_item_header_t & Header, 
		CStream * pInput, BOOL & bCancel, CProgress &Progress, CPackErrors & Error);

	static CPackItem * CreatePackItemFromMediaDir(CPackItem * pParent, pack_item_header_t & Header, 
		CStream * pInput, BOOL & bCancel, CProgress &Progress, CPackErrors & Error);

	BOOL ExportToDiskPath(CStream * pOutput, LPCTSTR szDstPath, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress,BOOL bChangeRefToDisk = FALSE, BOOL bAllowOverwrite = FALSE);

	BOOL CopyFile(LPCTSTR szSrc, LPCTSTR szDst, BOOL & bCancel, BOOL bAllowOverwrite, CPackErrors & Error, CProgress & Progress);

	BOOL CopyFile(CStream * pInput, LPCTSTR szDstPath, BOOL & bCancel, BOOL bAllowOverwrite, CPackErrors & Error,
		CProgress & Progress);

	BOOL CopyFile(LPCTSTR szSrcPath, CStream * pOutput, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress);

	BOOL CopyFile(CStream * pInput, CStream * pOutput, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress);

	BOOL SaveToMediaEOD(CStream * pInput, CStream * pOutput,  CProgress & Progress, CPackErrors & Error);

	BOOL SaveToMedia(CStream * pInput, CStream * pOutput, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress);

protected:
	pack_item_header_t m_ItemHeader;
	CArray<CPackItem *, CPackItem *> m_Children;
	CPackItem * m_pParent;
	CString m_strName;  // item name
	pack_item_ref_t  m_eRef;

	// if REF_DISK
	CPath m_DiskLocation;  // disk location

	// if REF_MEDIA
	LONGLONG m_nOffset;
};

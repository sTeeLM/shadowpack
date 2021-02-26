#pragma once


#include "PackItem.h"
#include "PackUtils.h"
#include "Progress.h"
#include "PasswordGetter.h"

class CMedia;

class CPack :
	public CPackItem
{
public:
	CPack(void);
	virtual ~CPack(void);

	BOOL IsDirty();
	void SetDirty(BOOL bDirty = TRUE);
	BOOL IsEmpty();

	void AttachInputMedia(CMedia * pMedia) {m_pInputMedia = pMedia;}
	void AttachOutputMedia(CMedia * pMedia) {m_pOutputMedia = pMedia;}

	CMedia * GetInputMedia() {return m_pInputMedia;}
	CMedia * GetOutputMedia() {return m_pOutputMedia;}

	ULONGLONG GetTotalDataSize() {return m_nTotalDataSize;}
	void SetTotalDataSize(LONGLONG n);
	void DecTotalDataSize(LONGLONG n);
	void IncTotalDataSize(LONGLONG n);


	void SetCurrentDir(CPackItem * pItem);
	CPackItem * GetCurrentDir();

	class CAddItemCBParam
	{
	public:
		CAddItemCBParam(){}
		~CAddItemCBParam(){}
		CProgress * pProgress;
		LARGE_INTEGER nTotalSize;
		CPackItem * pItem;
		CPackErrors * pError;
		BOOL * pbCancel;

	};

	class CAddItemListDirCBParam {
	public:
		CAddItemListDirCBParam() {}
		~CAddItemListDirCBParam() {}
		CArray<CString, CString> aPathToAdd;
		CArray<DWORD, DWORD> aType;
		CPackItem * pRoot;
		CPackErrors * pError;
		CProgress * pProgress;
	};

	class CDeleteItemCBParam {
	public:
		CDeleteItemCBParam(){}
		~CDeleteItemCBParam(){}
		CPackErrors * pError;
		CProgress * pProgress;
		BOOL * pbCancel;
		CPack * pThis;
		BOOL bIsRollBack;
	};

	class CExportItemCBParam {
	public:
		CExportItemCBParam() {}
		~CExportItemCBParam() {}
		CPackErrors * pError;
		CProgress * pProgress;
		BOOL * pbCancel;
		CPack * pThis;
		BOOL bIsGetSize;
		BOOL bOnMediaOnly;
		BOOL bChangeRefToDisk;
		LARGE_INTEGER nTotalSize;
		INT nTotalItemCnt;
		CString strExportRoot;
	};

	class CSaveItemCBParam{
	public:
		CSaveItemCBParam() {}
		~CSaveItemCBParam() {}
		CPackErrors * pError;
		CProgress * pProgress;
		BOOL * pbCancel;
		CPack * pThis;
	};

	static BOOL AddItemDirGetSizeCB(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam);
	static BOOL AddItemDirCreateItemCB(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam);
	static BOOL AddItemDirListItemCB(LPCTSTR szPath, LPWIN32_FIND_DATA pffd, LPVOID pParam);
	static BOOL DeleteItemCB(CPackItem * pItem, LPVOID pParam);
	static BOOL ExportItemCB(CPackItem * pItem,BOOL bIsEOD, LPVOID pParam);
	static BOOL SaveItemCB(CPackItem * pItem, BOOL bIsEOD, LPVOID pParam);
	
	BOOL LoadPack(LPCTSTR szPathName, LPCTSTR szExt, CPasswordGetter & PasswordGetter, 
		BOOL & bCancel, CPackErrors & Error, CProgress & Progress);


	// ������ظ���ֱ�Ӱ���DIR
	BOOL AddItemDirInternal(LPCTSTR szPathToAdd, CPackItem * pRoot, CPackErrors & Error, 
					   BOOL & bCancel, CProgress & Progress);

	// ����ظ�
	BOOL AddItemFile(LPCTSTR szPathToAdd, CPackItem * pRoot, CPackErrors & Error);

	// ����ظ��������Ƿ��Ǹ��������ǰ�������ö��
	BOOL AddItemDir(LPCTSTR szPathToAdd, CPackItem * pRoot, CPackErrors & Error, 
					   BOOL & bCancel, CProgress & Progress);

	BOOL DeleteItem(CPackItem * pItem, CPackErrors & Error, 
					   BOOL & bCancel, CProgress & Progress);

	BOOL Clear(CPackErrors & Error, BOOL & bCancel, CProgress & Progress);

	// ���Ŀ���Ƿ����
	BOOL ExportItemToDiskPath(CArray<CPackItem *, CPackItem *> & aExport, LPCTSTR szDstDir, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress, BOOL bMediaOnly = FALSE, BOOL bChangeRefToDisk = FALSE);	
    // ���Ŀ���Ƿ����
	BOOL ExportItemDirToDiskPath(CPackItem * pItem, CExportItemCBParam & Param);
    // ���Ŀ���Ƿ����
	BOOL ExportItemFileToDiskPath(CPackItem * pItem, LPCTSTR szDstPath, LPCTSTR szDstName, BOOL & bCancel, CPackErrors & Error,
		CProgress & Progress, BOOL bMediaOnly = FALSE, BOOL bChangeRefToDisk = FALSE, BOOL bAllowOverwrite = FALSE);
	BOOL ExportItemFileToDiskPath(CPackItem * pItem, CExportItemCBParam & Param);

	BOOL SavePack(LPCTSTR szPathName, CPasswordGetter & PasswordGetter, BOOL & bCancel,  CPackErrors & Error, CProgress & Progress);

private:
	BOOL m_isDirty;
	LONGLONG m_nTotalDataSize;
	CPackItem * m_pCurrentDir;
	CMedia  * m_pInputMedia;
	CMedia  * m_pOutputMedia;
};

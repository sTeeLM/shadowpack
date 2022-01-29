#pragma once

#include "PackManagerBase.h"
#include "MediaBase.h"
#include "StreamBase.h"
// CPackManager

class CPackManager : public CListCtrl, public CPackManagerBase
{
	DECLARE_DYNAMIC(CPackManager)

public:
	CPackManager();
	virtual ~CPackManager();
public:
	void Initialize(CWnd* pParent, UINT nID);
	
protected:
	DECLARE_MESSAGE_MAP()

protected:
	class CPackItemDropTarget :  public COleDropTarget
	{
	public:
		CPackItemDropTarget(CPackManager* pOwner) { m_pOwner = pOwner; m_pParent = NULL; m_bCanDrop = FALSE; }
		virtual ~CPackItemDropTarget() {};
	public:
		DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
			DWORD dwKeyState, CPoint point);

		BOOL TestDropOK(COleDataObject* pDataObject);

		DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
			DWORD dwKeyState, CPoint point);

		BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
			DROPEFFECT dropEffect, CPoint point);

		void OnDragLeave(CWnd* pWnd);

		void SetParent(CWnd* pParent) { m_pParent = pParent; }
	protected:
		CPackManager* m_pOwner;
		CWnd* m_pParent;
		BOOL m_bCanDrop;
	};

	class CPackItem {
		CPackItem() : 
			m_nSize(0), 
			m_pData(NULL),
			m_hFile(INVALID_HANDLE_VALUE)
		{}
	public:
		virtual ~CPackItem() { }
	private:
#define PACK_ITEM_SIGN 0x12345678
#define READ_WRITE_BUFFER_SIZE (4096)
		typedef struct _PACK_ITEM_T {
			DWORD dwSign;
			UINT nDataSize;
			time_t nTime;
			UINT nNameSize;
		}PACK_ITEM_T;
		CString m_strFileName;
		CTime m_Time;
		UINT m_nSize;
		LPBYTE m_pData;
		HANDLE m_hFile;
	public:
		static CPackItem* CreateItemFromStream(CStreamBase* pStream, LPCTSTR szCacheDir, BOOL bUseFileCache, CProgressBase& Progress, CPackErrors& Errors);
		static CPackItem* CreateItemFromFile(LPCTSTR szFilePath, LPCTSTR szCacheDir, BOOL bUseFileCache, CProgressBase& Progress, CPackErrors& Errors);
		static void Free(CPackItem* pItem);
		BOOL WriteItemToStream(CStreamBase* pStream, CProgressBase& Progress, CPackErrors& Errors);
		BOOL WriteItemToFile(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);
		static BOOL Stream2Handle(CStreamBase* pStream, HANDLE hFile, LPCTSTR szHandleFileName, UINT nSize, CProgressBase& Progress, CPackErrors& Errors);
		static BOOL Handle2Stream(HANDLE hFile, LPCTSTR szHandleFileName, CStreamBase* pStream, UINT nSize, CProgressBase& Progress, CPackErrors& Errors);
		static BOOL CFile2Handle(CFile* file, HANDLE hFile, LPCTSTR szHandleFileName, UINT nSize, CProgressBase& Progress, CPackErrors& Errors);
		static BOOL Handle2CFile(HANDLE hFile, LPCTSTR szHandleFileName, CFile* file, UINT nSize, CProgressBase& Progress, CPackErrors& Errors);
		static BOOL CFile2Memory(CFile* file, LPBYTE pBuffer, UINT nSize, CProgressBase& Progress, CPackErrors& Errors);
		static BOOL Memory2CFile(LPBYTE pBuffer, CFile* file, UINT nSize, CProgressBase& Progress, CPackErrors& Errors);
		CString GetName();
		ULONGLONG GetDataSize();
		ULONGLONG GetTotalSize();
		CString FormatDateItme();
	private:
		static BYTE ReadWriteBuffer[READ_WRITE_BUFFER_SIZE];
	};

public:
	virtual BOOL MediaAttached();
	virtual void AttachMedia(CMediaBase* pMedia);
	virtual BOOL LoadMedia(LPCTSTR szFilePath, CProgressBase& Progress,
		CPasswordGetterBase& Password, CPackErrors& Errors);
	virtual BOOL SaveMedia(CProgressBase& Progress, CPackErrors& Errors);
	virtual void DettachMedia();


	virtual UINT GetSelectedItemCnt();
	virtual CString GetFirstSelectedItemName();
	virtual BOOL ExportFirstSelectedItemToFile(LPCTSTR szItemPath,
		CProgressBase& Progress, CPackErrors& Errors);
	virtual BOOL ExportSelectedItemsToDir(LPCTSTR szItemPath,
		CProgressBase& Progress, CPackErrors& Errors);
	virtual BOOL AddItemFromFile(LPCTSTR szMediaPath,
		CProgressBase& Progress, CPackErrors& Errors);
	virtual BOOL AddItemFromFileMulti(const CArray<CString>& aryFileNames,
		CProgressBase& Progress, CPackErrors& Errors);
	virtual void DeleteSelectedItems();
	virtual void ClearAllItems();


	virtual CMediaBase* GetMedia();

	virtual BOOL IsDirty();

	virtual ULONGLONG GetTotalSize();

private:
	BOOL InsertPackItem(CPackItem * pItem, CPackErrors& Errors);
	BOOL ItemExist(LPCTSTR szItemName);
private:
	CMediaBase* m_pMedia;
	CString m_strMediaPath;
	ULONGLONG m_nTotalSize;
	BOOL m_bDirty;
	BOOL m_bUseDiskCache;
	CString m_strCachePath;
	CWnd* m_pParent;
	CPackItemDropTarget m_DragDropTarget;
};



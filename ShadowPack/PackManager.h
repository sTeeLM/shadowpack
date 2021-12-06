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
	class CPackItem {
		CPackItem() : m_nSize(0), m_pData(NULL){}
	public:
		virtual ~CPackItem() { if(m_pData) delete[] m_pData; }
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
	public:
		static CPackItem* CreateItemFromStream(CStreamBase* pStream, CProgressBase& Progress, CPackErrors& Errors);
		static CPackItem* CreateItemFromFile(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);
		static void Free(CPackItem* pItem);
		BOOL WriteItemToStream(CStreamBase* pStream, CProgressBase& Progress, CPackErrors& Errors);
		BOOL WriteItemToFile(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors);
		CString GetName();
		UINT GetDataSize();
		UINT GetTotalSize();
		CString FormatDateItme();
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
	virtual void DeleteSelectedItems();
	virtual void ClearAllItems();


	virtual CMediaBase* GetMedia();

	virtual BOOL IsDirty();

	virtual UINT GetTotalSize();

private:
	BOOL InsertPackItem(CPackItem * pItem, CPackErrors& Errors);
private:
	CMediaBase* m_pMedia;
	CString m_strMediaPath;
	UINT m_nTotalSize;
	BOOL m_bDirty;
};



#pragma once

#include "FileManagerBase.h"
#include "MediaBase.h"
// CFileManager

class CFileManager : public CListCtrl, public CFileManagerBase
{
	DECLARE_DYNAMIC(CFileManager)

public:
	CFileManager();
	virtual ~CFileManager();
public:
	void Initialize(CWnd* pParent, UINT nID);
protected:
	DECLARE_MESSAGE_MAP()

protected:
	class CPackItem {
	public:
		CPackItem() : m_nSize(0){}
		virtual ~CPackItem() {}
	public:
		CString m_strFileName;
		COleDateTime m_Date;
		UINT m_nSize;
	};

public:
	virtual BOOL MediaAttached();
	virtual void AttachMedia(CMediaBase* pMedia);
	virtual BOOL LoadMedia(CProgressBase& Progress,
		CPasswordGetterBase& Password, CPackErrors& Errors);
	virtual BOOL SaveMedia(CProgressBase& Progress,
		CPasswordGetterBase& Password, CPackErrors& Errors);
	virtual void DettachMedia();


	virtual UINT GetSelectedItemCnt();
	virtual CString GetFirstSelectedItemName();
	virtual BOOL ExportFirstSelectedItemToFile(LPCTSTR szMediaPath,
		CProgressBase& Progress, CPackErrors& Errors);
	virtual BOOL ExportSelectedItemsToDir(LPCTSTR szMediaPath,
		CProgressBase& Progress, CPackErrors& Errors);
	virtual BOOL AddItemFromFile(LPCTSTR szMediaPath,
		CProgressBase& Progress, CPackErrors& Errors);
	virtual void DeleteSelectedItems();
	virtual void ClearAllItems();


	virtual CMediaBase* GetMedia();

	virtual BOOL IsDirty();

private:
	CMediaBase* m_pMedia;
};



#pragma once

#include "framework.h"
#include "ProgressBase.h"
#include "PasswordGetterBase.h"
#include "PackErrors.h"
#include "MediaBase.h"

class CFileManagerBase
{
public:
	CFileManagerBase() {}
	virtual ~CFileManagerBase() {}
public:

	virtual BOOL MediaAttached() = 0;
	virtual void AttachMedia(CMediaBase * pMedia) = 0;
	virtual BOOL LoadMedia(CProgressBase& Progress,
		CPasswordGetterBase& Password, CPackErrors& Errors) = 0;
	virtual BOOL SaveMedia(CProgressBase& Progress,
		CPasswordGetterBase& Password, CPackErrors& Errors) = 0;
	virtual void DettachMedia() = 0;


	virtual UINT GetSelectedItemCnt() = 0;
	virtual CString GetFirstSelectedItemName() = 0;
	virtual BOOL ExportFirstSelectedItemToFile(LPCTSTR szMediaPath, 
		CProgressBase& Progress, CPackErrors& Errors) = 0;
	virtual BOOL ExportSelectedItemsToDir(LPCTSTR szMediaPath,
		CProgressBase& Progress, CPackErrors& Errors) = 0;
	virtual BOOL AddItemFromFile(LPCTSTR szMediaPath,
		CProgressBase& Progress, CPackErrors& Errors) = 0;
	virtual void DeleteSelectedItems() = 0;
	virtual void ClearAllItems() = 0;

	virtual CMediaBase* GetMedia() = 0;

	virtual BOOL IsDirty() = 0;
};
#pragma once

#include "PackItem.h"

// CPackItemList

class CPackItemList : public CListCtrl
{
	DECLARE_DYNAMIC(CPackItemList)

public:
	CPackItemList();
	virtual ~CPackItemList();
	void Initialize(CWnd * pParent);
	void LoadPackItem(CPackItem * pItem);
	BOOL DeleteAllItems();
protected:
	DECLARE_MESSAGE_MAP()

	LONG GetIconIndexByExt(const CString & ext);

	CImageList m_imageList;
	CMap<CString,LPCTSTR,LONG,LONG>m_IcoMap;
};



// PackItemList.cpp : 实现文件
//

#include "stdafx.h"
#include "ShadowPack.h"
#include "PackItemList.h"
#include "PackUtils.h"

// CPackItemList

IMPLEMENT_DYNAMIC(CPackItemList, CListCtrl)

CPackItemList::CPackItemList()
{

}

CPackItemList::~CPackItemList()
{
}

void CPackItemList::Initialize(CWnd * pParent)
{
	CRect rect;
	CString str;
	INT nColInterval;
	DWORD dwStyle;
 

	SubclassDlgItem(IDC_LIST_DATA, pParent);
	GetClientRect(&rect);
	nColInterval = rect.Width()/8;

	str.LoadString(IDS_COLUMN_NAME);
	InsertColumn(0, (LPCTSTR)str, LVCFMT_LEFT, nColInterval);
	str.LoadString(IDS_COLUMN_TYPE);
	InsertColumn(1, (LPCTSTR)str, LVCFMT_LEFT, nColInterval*2);
	str.LoadString(IDS_COLUMN_SIZE);
	InsertColumn(2, (LPCTSTR)str, LVCFMT_LEFT, nColInterval*2);
	str.LoadString(IDS_COLUMN_REF);
	InsertColumn(3, (LPCTSTR)str, LVCFMT_LEFT, rect.Width()-4*nColInterval);

	dwStyle = GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	SetExtendedStyle(dwStyle);

	// create image list
	m_imageList.Create(16,16,ILC_COLOR32,0,100);

	// 0 is up arrow
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_UP));

	// 1 is dir
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_DIR));

	// 2 is default
	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_DEFAULT));

	SetImageList(&m_imageList,LVSIL_SMALL);
}

LONG CPackItemList::GetIconIndexByExt(const CString & ext)
{
	HRESULT hr;
	SHFILEINFO stInfo = {0};
	LONG lRet = 0;

	if(!m_IcoMap.Lookup(ext, lRet)) {
		hr = SHGetFileInfo(ext,FILE_ATTRIBUTE_NORMAL,&stInfo,sizeof(stInfo),SHGFI_ICON|SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON);
		if(SUCCEEDED(hr)) {
			lRet = m_imageList.Add(stInfo.hIcon);
			m_IcoMap.SetAt(ext, lRet);
		} else {
			lRet = 2; // default ico
		}
		TRACE(_T("image list NOT hit: GetIconIndexByExt %s return %d\n"), ext, lRet);
	} else {
		TRACE(_T("image list hit: GetIconIndexByExt %s return %d\n"), ext, lRet);
	}

	return lRet;
}

BOOL CPackItemList::DeleteAllItems()
{	
	return CListCtrl::DeleteAllItems();
}

void CPackItemList::LoadPackItem(CPackItem * pItem)
{
	INT nIndex = 0;
	LVITEM lvi;
	CString strItem;
	CPackItem * pChild;
	ULONGLONG size = 0;

	CListCtrl::DeleteAllItems();

	if(!pItem->IsRoot()) {
		this->InsertItem(0, _T(".."), 0);
		SetItemData(0, (DWORD_PTR)pItem->GetParent());
		nIndex ++;
	}

	for(INT i = 0 ; i < pItem->GetChildrenCount(); i ++) {
		pChild = pItem->GetChild(i);
	// Insert the first item
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		strItem.Format(_T("%s"), pChild->GetName());
		lvi.iItem = nIndex;
		lvi.iSubItem = 0;
		lvi.pszText = (LPTSTR)(LPCTSTR)strItem;
		if(pChild->IsFile()) {
			lvi.iImage = GetIconIndexByExt(pChild->GetExt());
		} else {
			lvi.iImage = 1;
		}
		InsertItem(&lvi);
	// Set subitem 1
		strItem.LoadString(pChild->IsFile() ? IDS_ITEM_TYPE_FILE : IDS_ITEM_TYPE_DIR);
		lvi.iSubItem = 1;
		lvi.pszText = (LPTSTR)(LPCTSTR)strItem;;
		SetItem(&lvi);			
	// Set subitem 2
		size = pChild->GetTotalSize();
		CPackUtils::TranslateSize(size, strItem);
		lvi.iSubItem = 2;
		lvi.pszText = (LPTSTR)(LPCTSTR)strItem;;
		SetItem(&lvi);	
	// Set subitem 3
		if(pChild->IsRefDisk()) {
			strItem.Format(IDS_ITEM_REF_DISK);
		} else if(pChild->IsRefMedia()) {
			strItem.Format(IDS_ITEM_REF_MEDIA);
		} else {
			strItem.Format(IDS_ITEM_REF_OTHER);
		}
		lvi.iSubItem = 3;
		lvi.pszText = (LPTSTR)(LPCTSTR)strItem;;
		SetItem(&lvi);

		SetItemData(nIndex, (DWORD_PTR) pChild);
		nIndex ++;
	}

}

BEGIN_MESSAGE_MAP(CPackItemList, CListCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CPackItemList 消息处理程序

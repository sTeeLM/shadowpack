// FileManager.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "FileManager.h"

// CFileManager

IMPLEMENT_DYNAMIC(CFileManager, CListCtrl)

CFileManager::CFileManager():
	m_pMedia(NULL)
{

}

CFileManager::~CFileManager()
{
}

void CFileManager::Initialize(CWnd* pParent, UINT nID)
{
	SubclassDlgItem(nID, pParent);
}

BOOL CFileManager::MediaAttached()
{
	return m_pMedia != NULL;
}

 void CFileManager::AttachMedia(CMediaBase* pMedia)
{
	m_pMedia = pMedia;
}

 BOOL CFileManager::LoadMedia(CProgressBase& Progress, CPasswordGetterBase& Password, CPackErrors& Errors)
 {
	 return 0;
 }

 BOOL CFileManager::SaveMedia(CProgressBase& Progress, CPasswordGetterBase& Password, CPackErrors& Errors)
 {
	 return 0;
 }

 void CFileManager::DettachMedia()
{
	 delete m_pMedia;
	 m_pMedia = NULL;
}

UINT CFileManager::GetSelectedItemCnt()
{
	return CListCtrl::GetSelectedCount();
}

CString CFileManager::GetFirstSelectedItemName()
{
	
}

BOOL CFileManager::ExportFirstSelectedItemToFile(LPCTSTR szMediaPath, CProgressBase& Progress, CPackErrors& Errors)
{
	return 0;
}

BOOL CFileManager::ExportSelectedItemsToDir(LPCTSTR szMediaPath, CProgressBase& Progress, CPackErrors& Errors)
{
	return 0;
}

BOOL CFileManager::AddItemFromFile(LPCTSTR szMediaPath, CProgressBase& Progress, CPackErrors& Errors)
{
	return 0;
}

void CFileManager::DeleteSelectedItems()
{
}

void CFileManager::ClearAllItems()
{
}

CMediaBase* CFileManager::GetMedia()
{
	return m_pMedia;
}

BOOL CFileManager::IsDirty()
{
	return 0;
}


BEGIN_MESSAGE_MAP(CFileManager, CListCtrl)
END_MESSAGE_MAP()



// CFileManager 消息处理程序



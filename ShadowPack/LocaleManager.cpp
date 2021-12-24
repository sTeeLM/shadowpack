// LocalManager.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "LocaleManager.h"


// CLocaleManager
CLocaleManager::LOCAL_TABLE_T CLocaleManager::m_LocalTable[] = {
    { LOCALE_CHINESE_SC, IDS_LOCAL_CHINESE_SC, LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED, NULL, NULL },
    { LOCALE_ENGLISH_US, IDS_LOCAL_ENGLISH_US, LANG_ENGLISH, SUBLANG_ENGLISH_US, NULL, _T("lang\\langEnglishUS.dll") },
};
CLocaleManager::CLocaleManager()
{

}

CLocaleManager::~CLocaleManager()
{

}

BOOL CLocaleManager::Initialize()
{
    m_LocalTable[LOCALE_CHINESE_SC].hResourceHandle = AfxGetResourceHandle();
    if (m_LocalTable[LOCALE_CHINESE_SC].hResourceHandle == NULL) {
        return FALSE;
    }
    for (UINT i = 1; i < _countof(m_LocalTable); i++) {
        m_LocalTable[i].hResourceHandle = ::LoadLibrary(m_LocalTable[i].szResourceDll);
        if (m_LocalTable[i].hResourceHandle == NULL) {
            TRACE(_T("lang pack %s load failed!\n"), m_LocalTable[i].szResourceDll);
        }
    }
    return TRUE;
}

BOOL CLocaleManager::SetLocale(LOCAL_ID_T Locale)
{
    if (Locale < LOCALE_CNT && m_LocalTable[Locale].Locale == Locale && m_LocalTable[Locale].hResourceHandle) {
        AfxSetResourceHandle(m_LocalTable[Locale].hResourceHandle);
        if (SetThreadLocale(MAKELCID(MAKELANGID(
            m_LocalTable[Locale].dwLangureID, m_LocalTable[Locale].dwSubLangureID
        ), SORT_DEFAULT))) {
            if (SetThreadUILanguage(MAKELCID(MAKELANGID(
                m_LocalTable[Locale].dwLangureID, m_LocalTable[Locale].dwSubLangureID
            ), SORT_DEFAULT)) == MAKELCID(MAKELANGID(
                m_LocalTable[Locale].dwLangureID, m_LocalTable[Locale].dwSubLangureID
            ), SORT_DEFAULT)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}


// CLocaleManager 成员函数

#pragma once

#include "resource.h"

// CLocaleManager 命令目标

class CLocaleManager : public CObject
{
public:
	CLocaleManager();
	virtual ~CLocaleManager();
public:
	typedef enum _LOCAL_ID_T {
		LOCALE_CHINESE_SC = 0, /* default */
		LOCALE_ENGLISH_US,
		LOCALE_CNT
	}LOCAL_ID_T;
	BOOL Initialize();
	BOOL SetLocale(LOCAL_ID_T Locale);
	static UINT GetLocaleCount() { return (UINT)LOCALE_CNT; }
	static CString GetLocalName(UINT nIndex)
	{
		CString strRet;
		
		if (nIndex < LOCALE_CNT) {
			strRet.LoadString(m_LocalTable[nIndex].nLocaleDesc);
		}
		else {
			strRet.LoadString(IDS_LOCALE_UNKNOWN);
		}
		return strRet;
	}
protected:
	typedef struct _LOCAL_TABLE_T
	{
		LOCAL_ID_T Locale;
		UINT nLocaleDesc;
		DWORD dwLangureID;
		DWORD dwSubLangureID;
		HMODULE hResourceHandle;
		LPCTSTR szResourceDll;
	}LOCAL_TABLE_T;
	static LOCAL_TABLE_T m_LocalTable[];
};



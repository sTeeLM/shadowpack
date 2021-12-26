// ShadowPackConfig.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "ConfigManager.h"

//static BYTE blob[] = {1,2,3,4,5,6,7,8,9,0};
// CConfigManager
const CConfigManager::CONFIG_ENTRY_T CConfigManager::m_ConfigEntry[] = {
/*  供测试用
	{_T("main"), _T("int8"), CONFIG_VALUE_TYPE_INT8,   { .n8 = -100}},
	{_T("main"), _T("int16"), CONFIG_VALUE_TYPE_INT16, { .n16 = -200}},
	{_T("main"), _T("int32"), CONFIG_VALUE_TYPE_INT32, { .n32 = 300}},
	{_T("main"), _T("int64"), CONFIG_VALUE_TYPE_INT64, { .n64 = 400}},
	{_T("main"), _T("uint8"), CONFIG_VALUE_TYPE_UINT8, { .u8 = 100}},
	{_T("main"), _T("uint16"), CONFIG_VALUE_TYPE_UINT16, {.u16 = 200}},
	{_T("main"), _T("uint32"), CONFIG_VALUE_TYPE_UINT32, {.u32 = 300}},
	{_T("main"), _T("uint64"), CONFIG_VALUE_TYPE_UINT64, {.u64 = 400}},
	{_T("main"), _T("string"), CONFIG_VALUE_TYPE_STRING, {.str = (LPTSTR)_T("this is a test 啊啊")}},
	{_T("main"), _T("blog"), CONFIG_VALUE_TYPE_BLOB, {.blob = {.nLength = sizeof(blob), .pBody = blob}}},
	{_T("main"), _T("double"), CONFIG_VALUE_TYPE_DOUBLE, {.d = 300.123}},
	{_T("main"), _T("float"), CONFIG_VALUE_TYPE_FLOAT, {.f = 400.456f}},
	*/
	{_T("main"), _T("locale"), CONFIG_VALUE_TYPE_INT8,   {.n8 = 0}},
};

CConfigManager::CConfigManager() :
	m_Type(CONFIG_LOCATION_FILE),
	m_strLocation(_T("")),
	m_hBase(NULL)
{

}

CConfigManager::~CConfigManager()
{

}

void  CConfigManager::DumpHex(LPCTSTR szTitle, LPBYTE pBuffer, UINT nSize)
{
	BYTE Buffer[8];
	LPBYTE p = pBuffer;
	TRACE(_T("%s:\n"), szTitle);
	for (UINT i = 0; i < nSize / 8; i++) {
		::CopyMemory(Buffer, pBuffer + i * 8, 8);
		p = pBuffer + (i + 1) * 8;
		TRACE(_T("%02hx %02hx %02hx %02hx %02hx %02hx %02hx %02hx\n"), 
			Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6], Buffer[7]);
	}
	if(nSize % 8) {
		::ZeroMemory(Buffer, sizeof(Buffer));
		::CopyMemory(Buffer, p, nSize % 8);
		TRACE(_T("%02hx %02hx %02hx %02hx %02hx %02hx %02hx %02hx\n"),
			Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6], Buffer[7]);
	}
}

const CConfigManager::CONFIG_ENTRY_T* CConfigManager::FindConfigEntry(LPCTSTR szSession, LPCTSTR szKey)
{
	for (UINT i = 0; i < _countof(m_ConfigEntry); i++) {
		if (!lstrcmpi(szSession, m_ConfigEntry[i].szSession) && !lstrcmpi(szKey, m_ConfigEntry[i].szKey)) {
			return &m_ConfigEntry[i];
		}
	}
	return NULL;
}


BOOL CConfigManager::GetConfig(LPCTSTR szSession, LPCTSTR szKey, CONFIG_VALUE_T& val)
{
	const CONFIG_ENTRY_T* pEntry = FindConfigEntry(szSession, szKey);
	BOOL bRet = FALSE;
	DWORD dwValue = 0;
	QWORD qwValue = 0;
	if (pEntry) {
		switch (pEntry->nType) {
		case CONFIG_VALUE_TYPE_INT8:
		case CONFIG_VALUE_TYPE_INT16:
		case CONFIG_VALUE_TYPE_INT32:
		case CONFIG_VALUE_TYPE_UINT8:
		case CONFIG_VALUE_TYPE_UINT16:
		case CONFIG_VALUE_TYPE_UINT32:
		case CONFIG_VALUE_TYPE_FLOAT:
			if ((m_Type == CONFIG_LOCATION_REG ? 
				GetConfigRegDWORD(szSession, szKey, dwValue) 
				: GetConfigFileDWORD(szSession, szKey, dwValue))) {
				switch (pEntry->nType) {
				case CONFIG_VALUE_TYPE_INT8:
					val.n8 = (INT8)dwValue; break;
				case CONFIG_VALUE_TYPE_INT16:
					val.n16 = (INT16)dwValue; break;
				case CONFIG_VALUE_TYPE_INT32:
					val.n32 = (INT32)dwValue; break;
				case CONFIG_VALUE_TYPE_UINT8:
					val.u8 = (UINT8)dwValue; break;
				case CONFIG_VALUE_TYPE_UINT16:
					val.u16 = (UINT16)dwValue; break;
				case CONFIG_VALUE_TYPE_UINT32:
					val.u32 = (UINT32)dwValue; break;
				case CONFIG_VALUE_TYPE_FLOAT: {
					FLOAT* p = (FLOAT*)(&dwValue);
					val.f = *p;
					}break;
				}
				bRet = TRUE;
			}
			break;
		case CONFIG_VALUE_TYPE_INT64:
		case CONFIG_VALUE_TYPE_UINT64:
		case CONFIG_VALUE_TYPE_DOUBLE:
			if ((m_Type == CONFIG_LOCATION_REG ? GetConfigRegQWORD(szSession, szKey, qwValue):
				GetConfigFileQWORD(szSession, szKey, qwValue))) {
				switch (pEntry->nType) {
				case CONFIG_VALUE_TYPE_INT64:
					val.n64 = (INT64)qwValue; break;
				case CONFIG_VALUE_TYPE_UINT64:
					val.u64 = (UINT64)qwValue; break;
				case CONFIG_VALUE_TYPE_DOUBLE: {
					DOUBLE* p = (DOUBLE*)(&qwValue);
					val.d = *p;
					}break;
				}
				bRet = TRUE;
			}
			break;
		case CONFIG_VALUE_TYPE_STRING:
			bRet = (m_Type == CONFIG_LOCATION_REG ? GetConfigRegString(szSession, szKey, val.str) :
				GetConfigFileString(szSession, szKey, val.str));
			break;
		case CONFIG_VALUE_TYPE_BLOB:
			bRet = (m_Type == CONFIG_LOCATION_REG ? GetConfigRegBlob(szSession, szKey, val.blob.pBody, val.blob.nLength) :
				GetConfigFileBlob(szSession, szKey, val.blob.pBody, val.blob.nLength));
			break;
		}
	}

	return bRet;
}

BOOL CConfigManager::SetConfig(LPCTSTR szSession, LPCTSTR szKey, CONFIG_VALUE_T val)
{
	const CONFIG_ENTRY_T* pEntry = FindConfigEntry(szSession, szKey);
	BOOL bRet = FALSE;
	DWORD dwValue = 0;
	QWORD qwValue = 0;
	if (pEntry) {
		switch (pEntry->nType) {
		case CONFIG_VALUE_TYPE_INT8:
		case CONFIG_VALUE_TYPE_INT16:
		case CONFIG_VALUE_TYPE_INT32:
		case CONFIG_VALUE_TYPE_UINT8:
		case CONFIG_VALUE_TYPE_UINT16:
		case CONFIG_VALUE_TYPE_UINT32:
		case CONFIG_VALUE_TYPE_FLOAT:
			switch (pEntry->nType) {
			case CONFIG_VALUE_TYPE_INT8:
				dwValue = val.n8; break;
			case CONFIG_VALUE_TYPE_INT16:
				dwValue = val.n16; break;
			case CONFIG_VALUE_TYPE_INT32:
				dwValue = val.n32; break;
			case CONFIG_VALUE_TYPE_UINT8:
				dwValue = val.u8; break;
			case CONFIG_VALUE_TYPE_UINT16:
				dwValue = val.u16; break;
			case CONFIG_VALUE_TYPE_UINT32:
				dwValue = val.u32; break;
			case CONFIG_VALUE_TYPE_FLOAT:
				{
				FLOAT* p = (FLOAT*)&dwValue;
				*p = val.f;
				}
			}
			bRet = (m_Type == CONFIG_LOCATION_REG ?
				SetConfigRegDWORD(szSession, szKey, dwValue) : SetConfigFileDWORD(szSession, szKey, dwValue));
			break;
		case CONFIG_VALUE_TYPE_INT64:
		case CONFIG_VALUE_TYPE_UINT64:
		case CONFIG_VALUE_TYPE_DOUBLE:
			switch (pEntry->nType) {
			case CONFIG_VALUE_TYPE_INT64:
				qwValue = val.n64; break;
			case CONFIG_VALUE_TYPE_UINT64:
				qwValue = val.u64; break;
			case CONFIG_VALUE_TYPE_DOUBLE:
				{
				DOUBLE* p = (DOUBLE*)&dwValue;
				*p = val.d;
				}
			}
			bRet = (m_Type == CONFIG_LOCATION_REG ? SetConfigRegQWORD(szSession, szKey, qwValue) :
				SetConfigFileQWORD(szSession, szKey, qwValue));
			break;
		case CONFIG_VALUE_TYPE_STRING:
			bRet = (m_Type == CONFIG_LOCATION_REG ? SetConfigRegString(szSession, szKey, val.str) :
				SetConfigFileString(szSession, szKey, val.str));
			break;
		case CONFIG_VALUE_TYPE_BLOB:
			bRet = (m_Type == CONFIG_LOCATION_REG ? SetConfigRegBlob(szSession, szKey, val.blob.pBody, val.blob.nLength) :
				SetConfigFileBlob(szSession, szKey, val.blob.pBody, val.blob.nLength));
			break;
		}
	}

	return bRet;
}

void CConfigManager::DumpConfig()
{
	BOOL bRet = FALSE;
	CONFIG_VALUE_T val = { 0 };
	for (UINT i = 0; i < _countof(m_ConfigEntry); i++) {
		::ZeroMemory(&val, sizeof(val));
		bRet = GetConfig(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, val);
		switch (m_ConfigEntry[i].nType) {
		case CONFIG_VALUE_TYPE_INT8:
			TRACE(_T("INT8 [%s] %s = %d default %d result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.n8, m_ConfigEntry[i].Default.n8, bRet);
			break;
		case CONFIG_VALUE_TYPE_INT16:
			TRACE(_T("INT16 [%s] %s = %hd default %hd result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.n16, m_ConfigEntry[i].Default.n16, bRet);
			break;
		case CONFIG_VALUE_TYPE_INT32:
			TRACE(_T("INT32 [%s] %s = %I32d default %I32d result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.n32, m_ConfigEntry[i].Default.n32, bRet);
			break;
		case CONFIG_VALUE_TYPE_INT64:
			TRACE(_T("INT64 [%s] %s = %I64d default %I64d result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.n64, m_ConfigEntry[i].Default.n64, bRet);
			break;
		case CONFIG_VALUE_TYPE_UINT8:
			TRACE(_T("UINT8 [%s] %s = %u default %u result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.u8, m_ConfigEntry[i].Default.u8, bRet);
			break;
		case CONFIG_VALUE_TYPE_UINT16:
			TRACE(_T("UINT16 [%s] %s = %hu default %hu result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.u16, m_ConfigEntry[i].Default.u16, bRet);
			break;
		case CONFIG_VALUE_TYPE_UINT32:
			TRACE(_T("UINT32 [%s] %s = %I32u default %I32u result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.u32, m_ConfigEntry[i].Default.u32, bRet);
			break;
		case CONFIG_VALUE_TYPE_UINT64:
			TRACE(_T("UINT64 [%s] %s = %I64u default %I64u result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.u64, m_ConfigEntry[i].Default.u64, bRet);
			break;
		case CONFIG_VALUE_TYPE_FLOAT:
			TRACE(_T("FLOAT [%s] %s = %f default %f result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.f, m_ConfigEntry[i].Default.f, bRet);
			break;
		case CONFIG_VALUE_TYPE_DOUBLE:
			TRACE(_T("DOUBLE [%s] %s = %Lf default %Lf result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.d, m_ConfigEntry[i].Default.d, bRet);
			break;
		case CONFIG_VALUE_TYPE_STRING:
			TRACE(_T("STRING [%s] %s = <%s> default <%s> result : % d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.str, m_ConfigEntry[i].Default.str, bRet);
			if (val.str) {
				free((void *)val.str);
				val.str = NULL;
			}
			break;
		case CONFIG_VALUE_TYPE_BLOB:
			TRACE(_T("BLOB [%s] %s length %d (should be %d) result: %d\n"),
				m_ConfigEntry[i].szSession,
				m_ConfigEntry[i].szKey,
				val.blob.nLength, m_ConfigEntry[i].Default.blob.nLength, bRet);
			DumpHex(_T("Default"), m_ConfigEntry[i].Default.blob.pBody, m_ConfigEntry[i].Default.blob.nLength);
			DumpHex(_T("Real"), val.blob.pBody, val.blob.nLength);
			if (val.blob.pBody) {
				free(val.blob.pBody);
				val.blob.pBody = NULL;
			}
			break;
		}
	}
}

void CConfigManager::SetConfigFile(LPCTSTR szFilePath)
{
	m_strLocation = szFilePath;
	m_Type = CONFIG_LOCATION_FILE;
}

void CConfigManager::SetConfigReg(HKEY hBase, LPCTSTR szRegkey)
{
	m_strLocation = szRegkey;
	m_hBase = hBase;
	m_Type = CONFIG_LOCATION_REG;
}

BOOL CConfigManager::CreateDefaultReg()
{
	HKEY hKey = NULL;
	HKEY hSubKey = NULL;
	DWORD dwValue = 0;
	QWORD qwValue = 0;
	LSTATUS nRet = 0;
	BOOL bRet = FALSE;

	if (ConfigExist()) {
		goto err;
	}

	if (ERROR_SUCCESS != (nRet = ::RegCreateKey(m_hBase, (LPCTSTR)m_strLocation, &hKey))) {
		TRACE(_T("create location %s failed %d!\n"), m_strLocation, nRet);
		goto err;
	}

	for (UINT i = 0; i < _countof(m_ConfigEntry); i++) {
		if ((nRet = ::RegCreateKey(hKey, m_ConfigEntry[i].szSession, &hSubKey)) != ERROR_SUCCESS) {
			TRACE(_T("create key %s failed %d!\n"), m_ConfigEntry[i].szSession, nRet);
			goto err;
		}
		::RegCloseKey(hSubKey);
		hSubKey = NULL;
		switch (m_ConfigEntry[i].nType) {
		case CONFIG_VALUE_TYPE_INT8:
		case CONFIG_VALUE_TYPE_INT16:
		case CONFIG_VALUE_TYPE_INT32:
		case CONFIG_VALUE_TYPE_UINT8:
		case CONFIG_VALUE_TYPE_UINT16:
		case CONFIG_VALUE_TYPE_UINT32:
		case CONFIG_VALUE_TYPE_FLOAT:
			switch (m_ConfigEntry[i].nType) {
			case CONFIG_VALUE_TYPE_INT8:
				dwValue = m_ConfigEntry[i].Default.n8; break;
			case CONFIG_VALUE_TYPE_INT16:
				dwValue = m_ConfigEntry[i].Default.n16; break;
			case CONFIG_VALUE_TYPE_INT32:
				dwValue = m_ConfigEntry[i].Default.n32; break;
			case CONFIG_VALUE_TYPE_UINT8:
				dwValue = m_ConfigEntry[i].Default.u8; break;
			case CONFIG_VALUE_TYPE_UINT16:
				dwValue = m_ConfigEntry[i].Default.u16; break;
			case CONFIG_VALUE_TYPE_UINT32:
				dwValue = m_ConfigEntry[i].Default.u32; break;
			case CONFIG_VALUE_TYPE_FLOAT:
				{
					FLOAT* p = (FLOAT*)&dwValue;
					*p = m_ConfigEntry[i].Default.f;
				}break;
			}
			if (!(bRet = SetConfigRegDWORD(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, dwValue))) {
				goto err;
			}
			break;
		case CONFIG_VALUE_TYPE_INT64:
		case CONFIG_VALUE_TYPE_UINT64:
		case CONFIG_VALUE_TYPE_DOUBLE:
			switch (m_ConfigEntry[i].nType) {
			case CONFIG_VALUE_TYPE_INT64:
				qwValue = m_ConfigEntry[i].Default.n64; break;
			case CONFIG_VALUE_TYPE_UINT64:
				qwValue = m_ConfigEntry[i].Default.u64; break;
			case CONFIG_VALUE_TYPE_DOUBLE:
				{
					DOUBLE* p = (DOUBLE*)&qwValue;
					*p = m_ConfigEntry[i].Default.d;
				}break;
			}
			if (!(bRet = SetConfigRegQWORD(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, qwValue))) {
				goto err;
			}
			break;
		case CONFIG_VALUE_TYPE_STRING:
			if (!(bRet = SetConfigRegString(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, m_ConfigEntry[i].Default.str))) {
				goto err;
			}
			break;
		case CONFIG_VALUE_TYPE_BLOB:
			if (!(bRet = SetConfigRegBlob(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, m_ConfigEntry[i].Default.blob.pBody,
				m_ConfigEntry[i].Default.blob.nLength))) {
				goto err;
			}
			break;
		}
		
	}
	bRet = TRUE;
err:
	if (hKey) {
		::RegCloseKey(hKey);
		hKey = NULL;
	}
	if (hSubKey) {
		::RegCloseKey(hSubKey);
		hSubKey = NULL;
	}

	return bRet;
}

BOOL CConfigManager::CreateDefaultFile()
{
	DWORD dwValue = 0;
	QWORD qwValue = 0;
	BOOL bRet = FALSE;

	if (ConfigExist()) {
		goto err;
	}

	for (UINT i = 0; i < _countof(m_ConfigEntry); i++) {
		switch (m_ConfigEntry[i].nType) {
		case CONFIG_VALUE_TYPE_INT8:
		case CONFIG_VALUE_TYPE_INT16:
		case CONFIG_VALUE_TYPE_INT32:
		case CONFIG_VALUE_TYPE_UINT8:
		case CONFIG_VALUE_TYPE_UINT16:
		case CONFIG_VALUE_TYPE_UINT32:
		case CONFIG_VALUE_TYPE_FLOAT:
			switch (m_ConfigEntry[i].nType) {
			case CONFIG_VALUE_TYPE_INT8:
				dwValue = m_ConfigEntry[i].Default.n8; break;
			case CONFIG_VALUE_TYPE_INT16:
				dwValue = m_ConfigEntry[i].Default.n16; break;
			case CONFIG_VALUE_TYPE_INT32:
				dwValue = m_ConfigEntry[i].Default.n32; break;
			case CONFIG_VALUE_TYPE_UINT8:
				dwValue = m_ConfigEntry[i].Default.u8; break;
			case CONFIG_VALUE_TYPE_UINT16:
				dwValue = m_ConfigEntry[i].Default.u16; break;
			case CONFIG_VALUE_TYPE_UINT32:
				dwValue = m_ConfigEntry[i].Default.u32; break;
			case CONFIG_VALUE_TYPE_FLOAT:
				{
					FLOAT* p = (FLOAT*)&dwValue;
					*p = m_ConfigEntry[i].Default.f;
				}break;
			}
			if (!(bRet = SetConfigFileDWORD(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, dwValue))) {
				goto err;
			}
			break;
		case CONFIG_VALUE_TYPE_STRING:
			if (!(bRet = SetConfigFileString(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, m_ConfigEntry[i].Default.str))) {
				goto err;
			}
			break;
		case CONFIG_VALUE_TYPE_INT64:
		case CONFIG_VALUE_TYPE_UINT64:
		case CONFIG_VALUE_TYPE_DOUBLE:
			switch (m_ConfigEntry[i].nType) {
			case CONFIG_VALUE_TYPE_INT64:
				qwValue = m_ConfigEntry[i].Default.n64; break;
			case CONFIG_VALUE_TYPE_UINT64:
				qwValue = m_ConfigEntry[i].Default.u64; break;
			case CONFIG_VALUE_TYPE_DOUBLE:
				{
					DOUBLE* p = (DOUBLE*)&qwValue;
					*p = m_ConfigEntry[i].Default.d;
				}break;
			}
			if (!(bRet = SetConfigFileQWORD(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, qwValue))) {
				goto err;
			}
			break;
		case CONFIG_VALUE_TYPE_BLOB:
			if (!(bRet = SetConfigFileBlob(m_ConfigEntry[i].szSession, m_ConfigEntry[i].szKey, m_ConfigEntry[i].Default.blob.pBody,
				m_ConfigEntry[i].Default.blob.nLength))) {
				goto err;
			}
			break;
		}
	}
	bRet = TRUE;
err:
	return bRet;
}

BOOL CConfigManager::CreateDefault()
{
	if (m_Type == CONFIG_LOCATION_REG) {
		return CreateDefaultReg();
	}
	else if (m_Type == CONFIG_LOCATION_FILE) {
		return CreateDefaultFile();
	}
	return FALSE;
}


// CConfigManager 成员函数
BOOL CConfigManager::GetConfigRegDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD& dwValue)
{
	CString strSubkey = m_strLocation + _T("\\");
	DWORD dwType, dwSize;
	LSTATUS nRet;
	strSubkey += szSession;
	dwSize = sizeof(dwValue);
	if (ERROR_SUCCESS != (nRet = ::RegGetValue(m_hBase, (LPCTSTR)strSubkey, szKey, RRF_RT_REG_DWORD, &dwType, &dwValue, &dwSize))) {
		TRACE(_T("RegGetValue failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}
	return TRUE;
}

BOOL CConfigManager::GetConfigRegQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD& qwValue)
{
	CString strSubkey = m_strLocation + _T("\\");
	DWORD dwType, dwSize;
	LSTATUS nRet;
	strSubkey += szSession;
	dwSize = sizeof(qwValue);
	if (ERROR_SUCCESS != (nRet = ::RegGetValue(m_hBase, (LPCTSTR)strSubkey, szKey, RRF_RT_REG_QWORD, &dwType, &qwValue, &dwSize))) {
		TRACE(_T("RegGetValue failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}
	return TRUE;
}

BOOL CConfigManager::ConfigExist()
{
	LSTATUS nRet;
	HKEY hKey = NULL;
	CFile file;
	CFileException ex;
	if (m_Type == CONFIG_LOCATION_REG) {
		// exist?
		if (ERROR_SUCCESS == (nRet = ::RegOpenKey(m_hBase, (LPCTSTR)m_strLocation, &hKey))) {
			::RegCloseKey(hKey);
			hKey = NULL;
			return TRUE;
		}
	} else if (m_Type == CONFIG_LOCATION_FILE) {
		if (file.Open(m_strLocation, CFile::modeRead | CFile::shareDenyWrite, &ex)) {
			file.Close();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CConfigManager::GetConfigRegString(LPCTSTR szSession, LPCTSTR szKey, LPTSTR& pString)
{
	CString strSubkey = m_strLocation + _T("\\");
	DWORD dwType, dwSize = 0;
	LSTATUS nRet;
	TCHAR* pBuffer = NULL;
	strSubkey += szSession;

	pString = NULL;

	// get buffer size
	if (ERROR_SUCCESS != (nRet = ::RegGetValue(m_hBase, (LPCTSTR)strSubkey, szKey, RRF_RT_REG_SZ, &dwType, NULL, &dwSize))) {
		TRACE(_T("RegGetValue failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}
	// alloc buffer
	if ((pBuffer = (TCHAR*)malloc(dwSize + 10)) == NULL) {
		return FALSE;
	}
	::ZeroMemory(pBuffer, dwSize + 10);
	// get data
	if (ERROR_SUCCESS != (nRet = ::RegGetValue(m_hBase, (LPCTSTR)strSubkey, szKey, RRF_RT_REG_SZ, &dwType, pBuffer, &dwSize))) {
		TRACE(_T("RegGetValue failed %s %s %d\n"), szSession, szKey, nRet);
		free(pBuffer);
		pBuffer = NULL;
		return FALSE;
	}
	// set value
	pString = pBuffer;
	return TRUE;
}

BOOL CConfigManager::GetConfigRegBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE& pBuffer, UINT& nSize)
{
	CString strSubkey = m_strLocation + _T("\\");
	DWORD dwType, dwSize;
	LSTATUS nRet;
	strSubkey += szSession;
	dwSize = 0;
	// get buffer size
	if (ERROR_SUCCESS != (nRet = ::RegGetValue(m_hBase, (LPCTSTR)strSubkey, szKey, RRF_RT_REG_BINARY, &dwType, NULL, &dwSize))) {
		TRACE(_T("RegGetValue failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}
	// alloc buffer
	if ((pBuffer = (LPBYTE)malloc(dwSize)) == NULL) {
		return FALSE;
	}
	::ZeroMemory(pBuffer, dwSize);

	if (ERROR_SUCCESS != (nRet = ::RegGetValue(m_hBase, (LPCTSTR)strSubkey, szKey, RRF_RT_REG_BINARY, &dwType, pBuffer, &dwSize))) {
		TRACE(_T("RegGetValue failed %s %s %d\n"), szSession, szKey, nRet);
		free(pBuffer);
		pBuffer = NULL;
		return FALSE;
	}
	nSize = dwSize;
	return TRUE;
}

BOOL CConfigManager::SetConfigRegDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD dwValue)
{
	CString strSubkey = m_strLocation + _T("\\");
	LSTATUS nRet;
	HKEY hSubKey = NULL;

	strSubkey += szSession;
	if (ERROR_SUCCESS != (nRet = ::RegOpenKey(m_hBase, (LPCTSTR)strSubkey, &hSubKey))) {
		TRACE(_T("RegOpenKey failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}

	if (ERROR_SUCCESS != (nRet = ::RegSetValueEx(hSubKey, szKey, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue)))) {
		TRACE(_T("RegSetValueEx failed %s %s %d\n"), szSession, szKey, nRet);
		::RegCloseKey(hSubKey);
		return FALSE;
	}
	::RegCloseKey(hSubKey);
	return TRUE;
}

BOOL CConfigManager::SetConfigRegQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD qwValue)
{
	CString strSubkey = m_strLocation + _T("\\");
	LSTATUS nRet;
	HKEY hSubKey = NULL;

	strSubkey += szSession;
	if (ERROR_SUCCESS != (nRet = ::RegOpenKey(m_hBase, (LPCTSTR)strSubkey, &hSubKey))) {
		TRACE(_T("RegOpenKey failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}

	if (ERROR_SUCCESS != (nRet = ::RegSetValueEx(hSubKey, szKey, 0, REG_QWORD, (LPBYTE)&qwValue, sizeof(qwValue)))) {
		TRACE(_T("RegSetValueEx failed %s %s %d\n"), szSession, szKey, nRet);
		::RegCloseKey(hSubKey);
		return FALSE;
	}
	::RegCloseKey(hSubKey);
	return TRUE;
}

BOOL CConfigManager::SetConfigRegString(LPCTSTR szSession, LPCTSTR szKey, LPCTSTR szValue)
{
	CString strSubkey = m_strLocation + _T("\\");
	LSTATUS nRet;
	HKEY hSubKey = NULL;

	strSubkey += szSession;
	if (ERROR_SUCCESS != (nRet = ::RegOpenKey(m_hBase, (LPCTSTR)strSubkey, &hSubKey))) {
		TRACE(_T("RegOpenKey failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}

	if (ERROR_SUCCESS != (nRet = ::RegSetValueEx(hSubKey, szKey, 0, REG_SZ, (LPBYTE)szValue, 
		(lstrlen(szValue) + 1) * sizeof(TCHAR)))) {
		TRACE(_T("RegSetValueEx failed %s %s %d\n"), szSession, szKey, nRet);
		::RegCloseKey(hSubKey);
		return FALSE;
	}
	::RegCloseKey(hSubKey);
	return TRUE;
}

BOOL CConfigManager::SetConfigRegBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE pBuffer, UINT nSize)
{
	CString strSubkey = m_strLocation + _T("\\");
	LSTATUS nRet;
	HKEY hSubKey = NULL;

	strSubkey += szSession;
	if (ERROR_SUCCESS != (nRet = ::RegOpenKey(m_hBase, (LPCTSTR)strSubkey, &hSubKey))) {
		TRACE(_T("RegOpenKey failed %s %s %d\n"), szSession, szKey, nRet);
		return FALSE;
	}

	if (ERROR_SUCCESS != (nRet = ::RegSetValueEx(hSubKey, szKey, 0, REG_BINARY, pBuffer, nSize))) {
		TRACE(_T("RegSetValueEx failed %s %s %d\n"), szSession, szKey, nRet);
		::RegCloseKey(hSubKey);
		return FALSE;
	}
	::RegCloseKey(hSubKey);
	return TRUE;
}


BOOL CConfigManager::GetConfigFileDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD& dwValue)
{
	return ::GetPrivateProfileStruct(szSession, szKey, &dwValue, sizeof(dwValue), m_strLocation);
}

BOOL CConfigManager::GetConfigFileQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD& qwValue)
{
	return ::GetPrivateProfileStruct(szSession, szKey, &qwValue, sizeof(qwValue), m_strLocation);
}

BOOL CConfigManager::GetConfigFileString(LPCTSTR szSession, LPCTSTR szKey, LPTSTR& pString)
{
	CString strLength = szKey;
	strLength += _T(".size");
	UINT nSize = 0;

	if (::GetPrivateProfileStruct(szSession, strLength, &nSize, sizeof(nSize), m_strLocation)) {
		if (!(pString = (LPTSTR)malloc((nSize + 1) * sizeof(TCHAR)))) {
			return FALSE;
		}
		::ZeroMemory(pString, (nSize + 1) * sizeof(TCHAR));
		if (::GetPrivateProfileString(szSession, szKey, NULL, pString, (nSize + 1) * sizeof(TCHAR), m_strLocation)) {
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CConfigManager::GetConfigFileBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE& pBuffer, UINT& nSize)
{
	CString strLength = szKey;
	strLength += _T(".length");

	if (::GetPrivateProfileStruct(szSession, strLength, &nSize, sizeof(nSize), m_strLocation)) {
		if (!(pBuffer = (LPBYTE)malloc(nSize))) {
			return FALSE;
		}
		::ZeroMemory(pBuffer, nSize);
		if (::GetPrivateProfileStruct(szSession, szKey, pBuffer, nSize, m_strLocation)) {
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CConfigManager::SetConfigFileDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD dwValue)
{
	return ::WritePrivateProfileStruct(szSession, szKey, &dwValue, sizeof(dwValue), m_strLocation);
}

BOOL CConfigManager::SetConfigFileQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD qwValue)
{
	return ::WritePrivateProfileStruct(szSession, szKey, &qwValue, sizeof(qwValue), m_strLocation);
}

BOOL CConfigManager::SetConfigFileString(LPCTSTR szSession, LPCTSTR szKey, LPCTSTR szValue)
{
	CString strLength = szKey;
	strLength += _T(".size");

	DWORD dwLength = lstrlen(szValue);
	if (::WritePrivateProfileStruct(szSession, (LPCTSTR)strLength, &dwLength, sizeof(dwLength), m_strLocation)) {
		return ::WritePrivateProfileString(szSession, szKey, szValue, m_strLocation);
	}
	return FALSE;
}

BOOL CConfigManager::SetConfigFileBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE pBuffer, UINT nSize)
{
	CString strLength = szKey;
	strLength += _T(".length");

	if (::WritePrivateProfileStruct(szSession, (LPCTSTR)strLength, &nSize, sizeof(nSize), m_strLocation)) {
		return ::WritePrivateProfileStruct(szSession, szKey, pBuffer, nSize, m_strLocation);
	}
	return FALSE;
}

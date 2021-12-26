#pragma once

// CConfigManager 命令目标

class CConfigManager : public CObject
{
public:
	CConfigManager();
	virtual ~CConfigManager();

public:
	void SetConfigFile(LPCTSTR szFilePath);
	void SetConfigReg(HKEY hBase, LPCTSTR szRegkey);
;
	BOOL CreateDefault();

	typedef enum _CONFIG_VALUE_TYPE_T
	{
		CONFIG_VALUE_TYPE_INT8,
		CONFIG_VALUE_TYPE_INT16,
		CONFIG_VALUE_TYPE_INT32,
		CONFIG_VALUE_TYPE_INT64,
		CONFIG_VALUE_TYPE_UINT8,
		CONFIG_VALUE_TYPE_UINT16,
		CONFIG_VALUE_TYPE_UINT32,
		CONFIG_VALUE_TYPE_UINT64,
		CONFIG_VALUE_TYPE_FLOAT,
		CONFIG_VALUE_TYPE_DOUBLE,
		CONFIG_VALUE_TYPE_STRING,
		CONFIG_VALUE_TYPE_BLOB,

	}CONFIG_VALUE_TYPE_T;

	typedef struct _CONFIG_VALUE_BLOB_T
	{
		UINT nLength;
		LPBYTE pBody;
	}CONFIG_VALUE_BLOB_T;

	typedef union _CONFIG_VALUE_T{
		UINT8  u8;
		UINT16 u16;
		UINT32 u32;
		UINT64 u64;
		INT8  n8;
		INT16 n16;
		INT32 n32;
		INT64 n64;
		FLOAT f;
		DOUBLE d;
		LPTSTR str;
		CONFIG_VALUE_BLOB_T blob;
	}CONFIG_VALUE_T;

	typedef struct _CONFIG_ENTRY_T
	{
		LPCTSTR szSession;
		LPCTSTR szKey;
		CONFIG_VALUE_TYPE_T nType;
		CONFIG_VALUE_T Default;
	}CONFIG_ENTRY_T;

	typedef enum _CONFIG_LOCATION_T
	{
		CONFIG_LOCATION_FILE,
		CONFIG_LOCATION_REG
	}CONFIG_LOCATION_T;

	BOOL GetConfig(LPCTSTR szSession, LPCTSTR szKey, CONFIG_VALUE_T& val);
	BOOL SetConfig(LPCTSTR szSession, LPCTSTR szKey, CONFIG_VALUE_T val);
	BOOL ConfigExist();

	void DumpConfig();
protected:
	BOOL CreateDefaultReg();
	BOOL CreateDefaultFile();
	BOOL GetConfigRegDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD& dwValue);
	BOOL GetConfigRegQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD& qwValue);
	BOOL GetConfigRegString(LPCTSTR szSession, LPCTSTR szKey, LPTSTR & pString);
	BOOL GetConfigRegBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE& pBuffer, UINT& nSize);
	BOOL SetConfigRegDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD dwValue);
	BOOL SetConfigRegQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD qwValue);
	BOOL SetConfigRegString(LPCTSTR szSession, LPCTSTR szKey, LPCTSTR szValue);
	BOOL SetConfigRegBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE pBuffer, UINT nSize);

	BOOL GetConfigFileDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD& dwValue);
	BOOL GetConfigFileQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD& qwValue);
	BOOL GetConfigFileString(LPCTSTR szSession, LPCTSTR szKey, LPTSTR& pString);
	BOOL GetConfigFileBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE& pBuffer, UINT& nSize);
	BOOL SetConfigFileDWORD(LPCTSTR szSession, LPCTSTR szKey, DWORD dwValue);
	BOOL SetConfigFileQWORD(LPCTSTR szSession, LPCTSTR szKey, QWORD qwValue);
	BOOL SetConfigFileString(LPCTSTR szSession, LPCTSTR szKey, LPCTSTR szValue);
	BOOL SetConfigFileBlob(LPCTSTR szSession, LPCTSTR szKey, LPBYTE pBuffer, UINT nSize);

	void DumpHex(LPCTSTR szTitle, LPBYTE pBuffer, UINT nSize);
	const CONFIG_ENTRY_T* FindConfigEntry(LPCTSTR szSession, LPCTSTR szKey);
protected:
	static const CONFIG_ENTRY_T m_ConfigEntry[];
	CONFIG_LOCATION_T m_Type;
	CString m_strLocation;
	HKEY m_hBase;
};



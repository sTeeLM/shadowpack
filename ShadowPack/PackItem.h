#pragma once

#include "PackErrors.h"

class CPackItem
{
public:
	typedef struct {
		DWORD dwSignature;
		DWORD dwNameSize; // not include \0
		DWORD dwDataSize;
	}PackItemHeader;
	// name string(ASCII format!)
	// data block
public:
	CPackItem(void);
	virtual ~CPackItem(void);
	DWORD GetTotalSize() const;
	const CString & GetName() const;
	static CPackItem * CreatePackItemFromFile(const CString & szItemName, const CString & szFilePath,CPackErrors & Error);
	static CPackItem * CreatePackItemFromMemory(const PackItemHeader * pData, size_t size, CPackErrors & Error);
	BOOL ExportDataToFile(const CString & szItemName, const CString & szFilePath, CPackErrors & Error);
	BOOL ExportDataToMemory(PBYTE pBuffer, size_t size, CPackErrors & Error);
private:
	static BOOL IsValidHeader(const PackItemHeader & header);
private:
	PackItemHeader m_Header;
	CString m_szName;
	PBYTE   m_pData;
};

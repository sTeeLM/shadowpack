#pragma once


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
	static CPackItem * CreatePackItemFromFile(const CString & szItemName, const CString & szFilePath, CString & szError);
	static CPackItem * CreatePackItemFromMemory(const PackItemHeader * pData, size_t size, CString & szError);
	BOOL ExportDataToFile(const CString & szItemName, const CString & szFilePath, CString & szError);
	BOOL ExportDataToMemory(PBYTE pBuffer, size_t size, CString & szError);
private:
	static BOOL IsValidHeader(const PackItemHeader & header);
private:
	PackItemHeader m_Header;
	CString m_szName;
	PBYTE   m_pData;
};

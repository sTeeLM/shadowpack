#pragma once

class CSBitmap
{
public:
	#pragma pack(push, 1)
	typedef struct {
		BYTE Red;
		BYTE Green;
		BYTE Blue;
		BYTE Alpha;
	}Pixel;
	#pragma pack(pop)
public:
	CSBitmap(void);
	CSBitmap(const CSBitmap & src);
	virtual ~CSBitmap(void);
	BYTE & Red(INT X, INT Y);
	BYTE & Green(INT X, INT Y);
	BYTE & Blue(INT X, INT Y);
	BYTE & Alpha(INT X, INT Y);
	INT Columns() const;
	INT Rows() const;
	Pixel * GetScanLine(INT Y);

	BOOL SetScanLineRGB(INT Y, INT size, const LPBYTE buffer);
	BOOL GetScanLineRGB(INT Y, INT size, LPBYTE buffer);

	BOOL GetScanLineRGBA(INT Y, INT size, LPBYTE buffer);
	BOOL SetScanLineRGBA(INT Y, INT size, const LPBYTE buffer);

	// 
	BOOL GetScanLineBGR(INT Y, INT size, LPBYTE buffer);
	BOOL SetScanLineBGR(INT Y, INT size, const LPBYTE buffer);
	BOOL ReAllocBuffer(INT X, INT Y);
	BOOL SetBufferData(INT offset, INT size, void * buffer);
	LPBYTE GetBufferPointer(INT offset) const;
private:
	INT    m_Column;
	INT	   m_Row;

	Pixel * m_pBuffer;

	BYTE m_Dumy;
};

#pragma once

#include "Magick++.h"

class CSBitmap
{
public:
	CSBitmap(void);
	CSBitmap(const CSBitmap & src);
	virtual ~CSBitmap(void);
	BYTE & Red(INT X, INT Y);
	BYTE & Green(INT X, INT Y);
	BYTE & Blue(INT X, INT Y);
	INT Columns() const;
	INT Rows() const;
	BOOL FromImage(Magick::Image & image);
	BOOL ToImage(Magick::Image & image);
	BOOL ReAllocBuffer(INT X, INT Y);
	BOOL SetBufferData(INT offset, INT size, void * buffer);
	void * GetBufferPointer(INT offset) const;
private:
	INT    m_Column;
	INT	   m_Row;

	#pragma pack(push, 1)
	typedef struct {
		BYTE Red;
		BYTE Green;
		BYTE Blue;
	}Pixel;
	#pragma pack(pop)

	Pixel * m_pBuffer;

	BYTE m_Dumy;
};

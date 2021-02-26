#include "StdAfx.h"
#include "SBitmap.h"

CSBitmap::CSBitmap(void) : 
	m_pBuffer(NULL)
	,m_Column(0)
	,m_Row(0)
	,m_Dumy(0)
{
}

CSBitmap::CSBitmap(const CSBitmap & src)
{
	if(src.m_Column > 0 && src.m_Row > 0 && m_pBuffer != NULL) {
		m_Column = src.m_Column;
		m_Row = src.m_Row;
		m_pBuffer = (Pixel*)malloc(m_Column * m_Row * sizeof(Pixel));
		if(NULL != m_pBuffer) {
			memcpy(m_pBuffer, src.m_pBuffer, m_Row * m_Column * sizeof(Pixel));
		} else {
			m_Column = m_Row = 0;
		}
	}
}

CSBitmap::~CSBitmap(void)
{
	if(NULL != m_pBuffer) {
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	m_Column = m_Row = 0;

}

BYTE & CSBitmap::Red(INT X, INT Y)
{
	if(m_Column > 0 && m_Row > 0 && NULL != m_pBuffer && X < m_Column && Y < m_Row) {
		return m_pBuffer[Y * m_Column + X].Red;
	}

	return m_Dumy;
}

BYTE & CSBitmap::Green(INT X, INT Y)
{
	if(m_Column > 0 && m_Row > 0 && NULL != m_pBuffer && X < m_Column && Y < m_Row) {
		return m_pBuffer[Y * m_Column + X].Green;
	}

	return m_Dumy;
}

BYTE & CSBitmap::Blue(INT X, INT Y)
{
	if(m_Column > 0 && m_Row > 0 && NULL != m_pBuffer && X < m_Column && Y < m_Row) {
		return m_pBuffer[Y * m_Column + X].Blue;
	}
	return m_Dumy;
}


INT CSBitmap::Columns() const
{
	return m_Column;
}

INT CSBitmap::Rows() const
{
	return m_Row;
}

BOOL CSBitmap::ReAllocBuffer(INT X, INT Y)
{
	if(NULL != m_pBuffer) {
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	m_Column = m_Row = 0;

	m_pBuffer = (Pixel *)malloc(sizeof(Pixel) * X * Y);

	if(NULL != m_pBuffer) {
		memset(m_pBuffer, 0, sizeof(Pixel) * X * Y);
		m_Column = X;
		m_Row = Y;
		return TRUE;
	}

	return FALSE;
}

BOOL CSBitmap::SetBufferData(INT offset, INT size, void * buffer)
{
	LPBYTE p = (LPBYTE) m_pBuffer;
	
	if(offset < 0 || size < 0 || offset + size > m_Column * m_Row * (INT) sizeof(Pixel))
		return FALSE;

	memcpy(p + offset, buffer, size);

	return TRUE;
}

LPBYTE CSBitmap::GetBufferPointer(INT offset) const
{
	LPBYTE p = (LPBYTE) m_pBuffer;

	if(offset < 0 || offset + 1 > m_Column * m_Row * (INT) sizeof(Pixel))
		return NULL;
	
	return p + offset;
}
BOOL CSBitmap::GetScanLineRGB(INT Y, INT size, LPBYTE buffer)
{
	Pixel * p = GetScanLine(Y);
	
	if(m_Column * 3 < size) {
		return FALSE;
	}

	if(p) {
		for(INT i =  0 ; i < m_Column; i ++) {
			buffer[i*3]  = p[i].Red;
			buffer[i*3 + 1] = p[i].Green;
			buffer[i*3 + 2] = p[i].Blue;
		}

		return TRUE;
	}
	
	return FALSE;
}

BOOL CSBitmap::SetScanLineRGB(INT Y, INT size, const LPBYTE buffer)
{
	Pixel * p = GetScanLine(Y);
	
	if(m_Column * 3 < size) {
		return FALSE;
	}

	if(p) {
		for(INT i =  0 ; i < m_Column; i ++) {
			p[i].Alpha = 0xFF;
			p[i].Red = buffer[i*3];
			p[i].Green = buffer[i*3 + 1];
			p[i].Blue = buffer[i*3 + 2];
		}

		return TRUE;
	}
	
	return FALSE;
}

BOOL CSBitmap::SetScanLineRGBA(INT Y, INT size, const LPBYTE buffer)
{
	Pixel * p = GetScanLine(Y);

	if(m_Column * 4 < size) {
		return FALSE;
	}

	memcpy(p, buffer, m_Column * sizeof(Pixel));

	return TRUE;
}

BOOL CSBitmap::GetScanLineRGBA(INT Y, INT size, LPBYTE buffer)
{
	Pixel * p = GetScanLine(Y);

	if(m_Column * 4 < size) {
		return FALSE;
	}

	memcpy(buffer, p, m_Column * sizeof(Pixel));

	return TRUE;
}

BOOL CSBitmap::SetScanLineBGR(INT Y, INT size, const LPBYTE buffer)
{
	
	Pixel * p = GetScanLine(Y);
	
	if(m_Column * 3 < size) {
		return FALSE;
	}

	if(p) {
		for(INT i =  0 ; i < m_Column; i ++) {
			p[i].Alpha = 0xFF;
			p[i].Blue = buffer[i*3];
			p[i].Green = buffer[i*3 + 1];
			p[i].Red = buffer[i*3 + 2];
		}

		return TRUE;
	}
	
	return FALSE;
}

BOOL CSBitmap::GetScanLineBGR(INT Y, INT size, LPBYTE buffer)
{
	Pixel * p = GetScanLine(Y);
	
	if(m_Column * 3 < size) {
		return FALSE;
	}

	if(p) {
		for(INT i =  0 ; i < m_Column; i ++) {
			buffer[i*3] = p[i].Blue; 
			buffer[i*3 + 1] = p[i].Green;
			buffer[i*3 + 2] = p[i].Red;
		}

		return TRUE;
	}

	return FALSE;
}

CSBitmap::Pixel * CSBitmap::GetScanLine(INT Y)
{
	if(m_pBuffer) {
		return m_pBuffer + Y * m_Column;
	}
	return NULL;
}
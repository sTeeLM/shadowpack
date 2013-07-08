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
		m_pBuffer = (Pixel*)malloc(m_Column * m_Row * 3);
		if(NULL != m_pBuffer) {
			memcpy(m_pBuffer, src.m_pBuffer, m_Row * m_Column * 3);
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
BOOL CSBitmap::FromImage(Magick::Image & image)
{
	if(image.columns() > 0 && image.rows() > 0) {
		m_Column = image.columns();
		m_Row    = image.rows();
		if(NULL != m_pBuffer) {
			free(m_pBuffer);
			m_pBuffer = NULL;
		}

		m_pBuffer = (Pixel *)malloc(3 * m_Column * m_Row);
		if(NULL == m_pBuffer) {
			m_Column = m_Row = 0;
			return FALSE;
		}

		try {
			image.write(0,0,m_Column, m_Row, "RGB", MagickCore::CharPixel, m_pBuffer);
		}
		catch (const Magick::Error & err ) {
			free(m_pBuffer);
			m_Column = m_Row = 0;
			return FALSE; 	
		}

		return TRUE;

	}

	return FALSE;
}

BOOL CSBitmap::ToImage(Magick::Image & image)
{
	if(m_Column > 0 && m_Row > 0 && NULL != m_pBuffer) {
		try {
			image.read(m_Column,m_Row, "RGB", MagickCore::CharPixel, m_pBuffer);
		}
		catch (const Magick::Error & err ) {
			return FALSE; 	
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CSBitmap::ReAllocBuffer(INT X, INT Y)
{
	if(NULL != m_pBuffer) {
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	m_Column = m_Row = 0;

	m_pBuffer = (Pixel *)malloc(3 * X * Y);

	if(NULL != m_pBuffer) {
		memset(m_pBuffer, 0, 3 * X * Y);
		m_Column = X;
		m_Row = Y;
		return TRUE;
	}

	return FALSE;
}

BOOL CSBitmap::SetBufferData(INT offset, INT size, void * buffer)
{
	LPBYTE p = (LPBYTE) m_pBuffer;
	
	if(offset < 0 || size < 0 || offset + size > m_Column * m_Row * 3)
		return FALSE;

	memcpy(p + offset, buffer, size);

	return TRUE;
}

void * CSBitmap::GetBufferPointer(INT offset) const
{
	LPBYTE p = (LPBYTE) m_pBuffer;

	if(offset < 0 || offset + 1 > m_Column * m_Row * 3)
		return NULL;
	
	return p + offset;
}

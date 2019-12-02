#include "StdAfx.h"
#include "CorBuffer.h"
#include "PackErrors.h"

CCorBuffer::CCorBuffer(void)
	:m_nSize(0)
	,m_nIndex(0)
	,m_nCapcity(0)
	,m_pBuffer(NULL)
{
}

CCorBuffer::~CCorBuffer(void)
{
	if(NULL != m_pBuffer) {
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	m_nSize = m_nIndex = m_nCapcity = 0;
}


BOOL CCorBuffer::WriteCor(BYTE Cor)
{
	LPBYTE pNewBuffer = NULL;

	if(m_nSize == m_nCapcity && m_nIndex == m_nSize) { /* need realloc memory */
		pNewBuffer = (LPBYTE)malloc(m_nCapcity + 4096);
		if(pNewBuffer == NULL) {
			return FALSE;
		}
		if(NULL != m_pBuffer && m_nSize > 0) {
			memcpy(pNewBuffer, m_pBuffer, m_nSize);
			free(m_pBuffer);
		}
		m_pBuffer = pNewBuffer;
		m_nCapcity += 4096;
	}

	m_pBuffer[m_nIndex] = Cor;

	if(m_nIndex >= m_nSize) {
		m_nSize ++;
	}

	m_nIndex ++;

	return TRUE;
}

BOOL CCorBuffer::SetData(const LPBYTE pData, INT Bytes, CorFormat eFormat, CPackErrors & Error,
						 BOOL * bCancel/* = NULL */, CB_SET_PROGRESS fnSetProgress /* = NULL */)
{
	INT size = 0;
	INT i;
	INT nProgress = 0, nNewProgress;

	if(eFormat == CF_1BC) {
		size = Bytes * 8;
	} else if(eFormat == CF_2BC) {
		size = Bytes * 8 / 2;
	} else if(eFormat == CF_4BC) {
		size = (Bytes * 8) / 4;
	}

	if(NULL != m_pBuffer) {
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	m_pBuffer = (LPBYTE)malloc(size);
	if(NULL == m_pBuffer) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
	m_nIndex = 0;
	m_nSize = size;
	m_nCapcity = size;

	memset(m_pBuffer, 0, size);

	for(i = 0 ; i < Bytes; i ++) {
		if(bCancel && *bCancel) {
			Error.SetError(CPackErrors::PE_CANCELED);
			return FALSE;
		}
		if(fnSetProgress) {
			nNewProgress = i * 50 / Bytes;
			if(nNewProgress != nProgress) {
				fnSetProgress(nNewProgress);
				nProgress = nNewProgress;
			}
		}
		if(eFormat == CF_1BC) {
			m_pBuffer[i*8] |= pData[i] & 0x01;
			m_pBuffer[i*8 + 1] |= (pData[i] & 0x02) >> 1;
			m_pBuffer[i*8 + 2] |= (pData[i] & 0x04) >> 2;
			m_pBuffer[i*8 + 3] |= (pData[i] & 0x08) >> 3;
			m_pBuffer[i*8 + 4] |= (pData[i] & 0x10) >> 4;
			m_pBuffer[i*8 + 5] |= (pData[i] & 0x20) >> 5;
			m_pBuffer[i*8 + 6] |= (pData[i] & 0x40) >> 6;
			m_pBuffer[i*8 + 7] |= (pData[i] & 0x80) >> 7;
		} else if(eFormat == CF_2BC) {
			m_pBuffer[i*4] |= pData[i] & 0x03;
			m_pBuffer[i*4 + 1] |= (pData[i] & 0x0C) >> 2;
			m_pBuffer[i*4 + 2] |= (pData[i] & 0x30) >> 4;
			m_pBuffer[i*4 + 3] |= (pData[i] & 0xC0) >> 6;
		} else if(eFormat == CF_4BC) {
			m_pBuffer[i*2] |= pData[i] & 0x0F;
			m_pBuffer[i*2 + 1] |= (pData[i] & 0xF0) >> 4;
		}		
	}

	return TRUE;
}

BOOL CCorBuffer::GetData(LPBYTE pData, INT Bytes, CorFormat eFormat, CPackErrors & Error,
						 BOOL * bCancel/* = NULL */, CB_SET_PROGRESS fnSetProgress /* = NULL */)
{
	INT size, i;
	INT nProgress = 0, nNewProgress;

	register BYTE buffer;
	if(eFormat == CF_1BC) {
		size = Bytes * 8;
	} else if(eFormat == CF_2BC) {
		size = Bytes * 8 / 2;
	} else if(eFormat == CF_4BC) {
		size = (Bytes * 8) / 4;
	}

	if(size > m_nSize || NULL == m_pBuffer)
		return FALSE;

	for(i = 0 ; i < Bytes; i ++) {
		if(bCancel && *bCancel) {
			Error.SetError(CPackErrors::PE_CANCELED);
			return FALSE;
		}
		if(fnSetProgress) {
			nNewProgress = i * 50 / Bytes;
			if(nNewProgress != nProgress) {
				fnSetProgress(nNewProgress);
				nProgress = nNewProgress;
			}
		}
		buffer = 0;
		if(eFormat == CF_1BC) {
			buffer |= m_pBuffer[i*8 + 7] & 0x01; buffer <<= 1;
			buffer |= m_pBuffer[i*8 + 6] & 0x01; buffer <<= 1;
			buffer |= m_pBuffer[i*8 + 5] & 0x01; buffer <<= 1;
			buffer |= m_pBuffer[i*8 + 4] & 0x01; buffer <<= 1;
			buffer |= m_pBuffer[i*8 + 3] & 0x01; buffer <<= 1;
			buffer |= m_pBuffer[i*8 + 2] & 0x01; buffer <<= 1;
			buffer |= m_pBuffer[i*8 + 1] & 0x01; buffer <<= 1;
			buffer |= m_pBuffer[i*8] & 0x01;
			pData[i] = buffer;
		} else if(eFormat == CF_2BC) {
			buffer |= m_pBuffer[i*4 + 3] & 0x03; buffer <<= 2;
			buffer |= m_pBuffer[i*4 + 2] & 0x03; buffer <<= 2;
			buffer |= m_pBuffer[i*4 + 1] & 0x03; buffer <<= 2;
			buffer |= m_pBuffer[i*4] & 0x03;
			pData[i] = buffer;
		} else if(eFormat == CF_4BC) {
			buffer |= m_pBuffer[i*2 + 1] & 0x0F; buffer <<= 4;
			buffer |= m_pBuffer[i*2] & 0x0F;
			pData[i] = buffer;
		}
	}
	return TRUE;
}

BOOL CCorBuffer::ReadCor(LPBYTE pCors)
{
	if(m_nSize > 0 && m_nIndex < m_nSize && NULL != pCors && NULL != m_pBuffer) {
		*pCors = m_pBuffer[m_nIndex ++ ];
		return TRUE;
	}

	return FALSE;
}

INT CCorBuffer::Size()
{
	return m_nSize;
}

INT CCorBuffer::Index()
{
	return m_nIndex;
}

void CCorBuffer::Rewind()
{
	m_nIndex = 0;
}

BOOL CCorBuffer::Eof()
{
	return m_nSize == m_nIndex;
}

#pragma once

#include "Pack.h"

class CCorBuffer
{
public:
	enum CorFormat {
		CF_1BC = 1,
		CF_2BC = 2, // 1 bits per cor
		CF_4BC = 3
	};
public:
	CCorBuffer(void);
	virtual ~CCorBuffer(void);
	BOOL WriteCor(BYTE Cor);
	BOOL ReadCor(LPBYTE pCor);
	INT Size();
	INT Index();
	void Rewind();
	BOOL Eof();
	BOOL SetData(const LPBYTE pData, INT Bytes, CorFormat eFormat, BOOL * bCancel = NULL, CB_SET_PROGRESS fnSetProgress = NULL);
	BOOL GetData(LPBYTE pData, INT Bytes, CorFormat eFormat, BOOL * bCancel = NULL, CB_SET_PROGRESS fnSetProgress = NULL);
private:
	INT m_nSize;
	INT m_nIndex;
	INT m_nCapcity;
	LPBYTE m_pBuffer;
};

#include "StdAfx.h"
#include "BMPImageMedia.h"
#include "Media.h"
#include <share.h>



CBMPImageMedia::CBMPImageMedia(void):
m_pfileHeader(NULL),
m_pFile(NULL)
{
}

CBMPImageMedia::~CBMPImageMedia(void)
{
}

LPCTSTR CBMPImageMedia::m_szFilter = _T("BMP Files (*.bmp)|*.bmp|");
LPCTSTR CBMPImageMedia::m_szExt = _T("BMP");

BOOL CBMPImageMedia::ReadBlock(CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error)
{
	LPBITMAPINFOHEADER  pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));
	CPixelBlock * pPixel = (CPixelBlock *)pBlock;
	LONGLONG nPadding, nRow, nColumn, nIndex, nLeft;
	BYTE Buffer[3] = {0}; // BGR

	TRACE(_T("CBMPImageMedia::ReadBlock: %lld, %lld\n"), nOffset, nBlockCount);

	if(!m_pFile)
		return FALSE;

	if(nOffset + nBlockCount > GetBlockCount()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(pPixel->GetCapicity() < nBlockCount) {
		if(!pPixel->ReAlloc(nBlockCount, Error)) {
			return FALSE;
		}
	}

	nPadding = (pinfoHeader->biWidth % 4);
	nPadding = nPadding != 0 ? 4 - nPadding : 0;

	nRow = nOffset / pinfoHeader->biWidth;
	nColumn = nOffset % pinfoHeader->biWidth;

	LONG off = (LONG) (m_pfileHeader->bfOffBits + (nRow * (pinfoHeader->biWidth + nPadding) + nColumn) * 3);

	if(fseek(m_pFile, off ,SEEK_SET)) {
		Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), CPackUtils::GetLastStdError(errno));
		return FALSE;
	}

	off = ftell(m_pFile);

	nLeft = nBlockCount;
	nIndex = 0;
	while(nLeft) {
		if(fread(Buffer, sizeof(Buffer), 1, m_pFile) != 1) {
			Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), CPackUtils::GetLastStdError(errno));
			return FALSE;
		}

		pPixel->GetPixelAt(nIndex).Red = Buffer[2];
		pPixel->GetPixelAt(nIndex).Green = Buffer[1];
		pPixel->GetPixelAt(nIndex).Blue = Buffer[0];
		pPixel->GetPixelAt(nIndex).Alpha = 0xFF;
		nColumn ++;
		if(nColumn == pinfoHeader->biWidth) {
			if(nPadding) {
				if(fseek(m_pFile, (long)(nPadding * 3), SEEK_CUR)) {
					Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), CPackUtils::GetLastStdError(errno));
					return FALSE;
				}
			}
			nColumn = 0;
			nRow ++;
		}
		nLeft --;
		nIndex ++;
	}

	pBlock->SetBlockSize(nBlockCount);

	return TRUE;
}

BOOL CBMPImageMedia::WriteBlock(const CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error)
{
	LPBITMAPINFOHEADER  pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));
	CPixelBlock * pPixel = (CPixelBlock *)pBlock;
	LONGLONG nPadding, nRow, nColumn, nIndex, nLeft;
	BYTE Buffer[3] = {0}; // BGR
	BYTE PaddingBuffer[3 * 4] = {0xFF};

	if(!m_pFile) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(nOffset + nBlockCount > GetBlockCount()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}
	nPadding = (pinfoHeader->biWidth % 4);
	nPadding = nPadding != 0 ? 4 - nPadding : 0;

	nRow = nOffset / pinfoHeader->biWidth;
	nColumn = nOffset % pinfoHeader->biWidth;

	if(fseek(m_pFile, 
		(long)(m_pfileHeader->bfOffBits + (nRow * (pinfoHeader->biWidth + nPadding) + nColumn) * 3) ,SEEK_SET)) {
			Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), CPackUtils::GetLastStdError(errno));
		return FALSE;
	}

	nLeft = nBlockCount;
	nIndex = 0;
	while(nLeft) {

		Buffer[2] = pPixel->GetPixelAt(nIndex).Red;
		Buffer[1] = pPixel->GetPixelAt(nIndex).Green;
		Buffer[0] = pPixel->GetPixelAt(nIndex).Blue;

		if(fwrite(Buffer, sizeof(Buffer), 1, m_pFile) != 1) {
			Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), CPackUtils::GetLastStdError(errno));
			return FALSE;
		}

		nColumn ++;
		if(nColumn == pinfoHeader->biWidth) {
			if(nPadding) {
				if(fwrite(PaddingBuffer, (long)(nPadding * 3), 1, m_pFile) != 1) {
					Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), CPackUtils::GetLastStdError(errno));
					return FALSE;
				}
			}
			nColumn = 0;
			nRow ++;
		}
		nLeft --;
		nIndex ++;
	}

	return TRUE;
}

BOOL CBMPImageMedia::ReopenFileToRead(CPackErrors &Error)
{

	if(m_pFile) {
		fclose(m_pFile);
		m_pFile = NULL;
	}

	if((m_pFile = _tfsopen(GetMediaFilePath(),_T("rb"), _SH_DENYWR)) == NULL) {
		Error.SetError(CPackErrors::PE_IO, GetMediaFilePath() ,CPackUtils::GetLastStdError(errno));
		return FALSE;
	}

	return TRUE;
}

ULONGLONG CBMPImageMedia::GetBlockCount()
{
	LPBITMAPINFOHEADER  pinfoHeader = NULL;
	if(m_pfileHeader != NULL) {
		pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));
		return pinfoHeader->biWidth * 
			(pinfoHeader->biHeight > 0 ? pinfoHeader->biHeight : (0 - pinfoHeader->biHeight));
	}
	return 0;
}

BOOL CBMPImageMedia::OpenFile(CMedia::media_mode_t mode, LPCTSTR szPath, BOOL & bCancel, CPackErrors & Error, CProgress & Progress)
{
	m_pFile = NULL;
	BITMAPFILEHEADER fileHeader;
	LPBITMAPINFOHEADER  pinfoHeader = NULL;
	BOOL bRet = FALSE;

	if((m_pFile = _tfsopen(szPath, mode == CMedia::MODE_READ ? _T("rb") : _T("wb"), _SH_DENYWR)) == NULL) {
		Error.SetError(CPackErrors::PE_IO, szPath,CPackUtils::GetLastStdError(errno));
		goto err;
	}

	if(mode == CMedia::MODE_READ) {

		if(fread(&fileHeader, sizeof(fileHeader), 1, m_pFile) != 1) {
			Error.SetError(CPackErrors::PE_IO, szPath, CPackUtils::GetLastStdError(errno));
			goto err;
		}

		if(fileHeader.bfType != 0x4d42) {
			Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
			goto err;
		}

		if((m_pfileHeader = (LPBITMAPFILEHEADER)malloc(fileHeader.bfOffBits)) == NULL) {
			Error.SetError(CPackErrors::PE_NOMEM);
			goto err;
		}
		

		if(fseek(m_pFile, 0, SEEK_SET) != 0|| fread(m_pfileHeader, fileHeader.bfOffBits, 1, m_pFile) != 1) {
			Error.SetError(CPackErrors::PE_IO, szPath, CPackUtils::GetLastStdError(errno));
			goto err;
		}

		pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));

		if(pinfoHeader->biPlanes != 1 || (pinfoHeader->biBitCount != 24) 
			|| pinfoHeader->biCompression != BI_RGB ) {
			Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
			goto err;
		}
	} else {
		pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));
		if(fwrite(m_pfileHeader, m_pfileHeader->bfOffBits, 1, m_pFile) != 1) {
			Error.SetError(CPackErrors::PE_IO, szPath, CPackUtils::GetLastStdError(errno));
			goto err;
		}

	}

	bRet = TRUE;

	return bRet;
err:
	if(m_pfileHeader != NULL) {
		free(m_pfileHeader);
		m_pfileHeader = NULL;
	}
	if(m_pFile != NULL) {
		fclose(m_pFile);
		m_pFile = NULL;
	}
	return bRet;
}

BOOL CBMPImageMedia::SaveFile(BOOL &bCancel, CPackErrors &Error, CProgress &Progress)
{
	return TRUE;
}

void CBMPImageMedia::CloseFile()
{
	if(m_pfileHeader != NULL) {
		free(m_pfileHeader);
		m_pfileHeader = NULL;
	}
	if(m_pFile != NULL) {
		fflush(m_pFile);
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

CMedia * CBMPImageMedia::CloneMedia(CPackErrors & Error)
{
	CBMPImageMedia * pMedia = new(std::nothrow) CBMPImageMedia();
	if(pMedia) {
		pMedia->CloneCore(this);
		if(m_pfileHeader != NULL) {
			LPBITMAPINFOHEADER pinfoHeader = (LPBITMAPINFOHEADER)(((LPBYTE)m_pfileHeader) + sizeof(BITMAPFILEHEADER));
			pMedia->m_pfileHeader = (LPBITMAPFILEHEADER)malloc(m_pfileHeader->bfOffBits);
			if(pMedia->m_pfileHeader) {
				memcpy(pMedia->m_pfileHeader, m_pfileHeader, m_pfileHeader->bfOffBits);
			} else {
				delete pMedia;
				pMedia = NULL;
			}
		}

	}
	if(!pMedia) {
		Error.SetError(CPackErrors::PE_NOMEM);
	}
	return pMedia;
}

LPCTSTR CBMPImageMedia::GetFilter()
{
	return m_szFilter; 
}

LPCTSTR CBMPImageMedia::GetDefaultExt()
{
	return m_szExt;
}

BOOL CBMPImageMedia::TestExt(LPCTSTR szExt)
{
	return lstrcmpi(szExt, m_szExt) == 0;
}

LPCTSTR CBMPImageMedia::GetExtFilter()
{
	return m_szFilter;
}

CMedia * CBMPImageMedia::Factory()
{
	return new(std::nothrow) CBMPImageMedia();
}
#include "StdAfx.h"
#include "PNGImageMedia.h"
#include "PackUtils.h"


// 全部读入内存。。。。，没有仔细研究

CPNGImageMedia::CPNGImageMedia(void):
m_row_pointers(NULL)
{
}

CPNGImageMedia::~CPNGImageMedia(void)
{
}

LPCTSTR CPNGImageMedia::m_szFilter = _T("PNG Files (*.png)|*.png|");
LPCTSTR CPNGImageMedia::m_szExt = _T("PNG");

BOOL CPNGImageMedia::ReadBlock(CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error)
{
	CPixelBlock * pPixel = (CPixelBlock *)pBlock;
	LONGLONG nRow, nColumn, nIndex, nLeft;

	if(nOffset + nBlockCount > GetBlockCount()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	if(pPixel->GetCapicity() < nBlockCount) {
		if(!pPixel->ReAlloc(nBlockCount, Error)) {
			return FALSE;
		}
	}

	nRow = nOffset / m_PNGinfo.nWidth;
	nColumn = nOffset % m_PNGinfo.nWidth;	

	nRow = m_PNGinfo.nHeight - nRow - 1;
	nLeft = nBlockCount;
	nIndex = 0;

	while(nLeft) {
		pPixel->GetPixelAt(nIndex).Red = m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels];
		pPixel->GetPixelAt(nIndex).Green = m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels + 1];
		pPixel->GetPixelAt(nIndex).Blue = m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels + 2];
		if(m_PNGinfo.nChannels == 4) {
			pPixel->GetPixelAt(nIndex).Alpha = m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels + 3];
		} else {
			pPixel->GetPixelAt(nIndex).Alpha = 0xFF;
		}
		
		nColumn ++;
		if(nColumn == m_PNGinfo.nWidth) {
			nColumn = 0;
			nRow --;
		}
		nLeft --;
		nIndex ++;
		if(nRow < 0 && nLeft > 0) {
			Error.SetError(CPackErrors::PE_INTERNAL);
			return FALSE;
		}
	}

	pBlock->SetBlockSize(nBlockCount);

	return TRUE;
}

BOOL CPNGImageMedia::WriteBlock(const CMediaBlock * pBlock, ULONGLONG nOffset, ULONGLONG nBlockCount, CPackErrors & Error)
{
	CPixelBlock * pPixel = (CPixelBlock *)pBlock;
	LONGLONG nRow, nColumn, nIndex, nLeft;

	if(nOffset + nBlockCount > GetBlockCount()) {
		Error.SetError(CPackErrors::PE_INTERNAL);
		return FALSE;
	}

	nRow = nOffset / m_PNGinfo.nWidth;
	nColumn = nOffset % m_PNGinfo.nWidth;	

	nRow = m_PNGinfo.nHeight - nRow - 1;
	nLeft = nBlockCount;
	nIndex = 0;

	while(nLeft) {
		m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels] = pPixel->GetPixelAt(nIndex).Red;
		m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels + 1] = pPixel->GetPixelAt(nIndex).Green;
		m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels + 2] = pPixel->GetPixelAt(nIndex).Blue;
		if(m_PNGinfo.nChannels == 4) {
			m_row_pointers[nRow][nColumn * m_PNGinfo.nChannels + 3] = pPixel->GetPixelAt(nIndex).Alpha;
		}
		nColumn ++;
		if(nColumn == m_PNGinfo.nWidth) {
			nColumn = 0;
			nRow --;
		}
		nLeft --;
		nIndex ++;
		if(nRow < 0 && nLeft > 0) {
			Error.SetError(CPackErrors::PE_INTERNAL);
			return FALSE;
		}
	}

	return TRUE;
}

ULONGLONG CPNGImageMedia::GetBlockCount()
{
	return m_PNGinfo.nWidth * m_PNGinfo.nHeight;
}

ULONG CPNGImageMedia::m_nTotalRows = 0;
CProgress * CPNGImageMedia::m_pProgress = NULL;
void CPNGImageMedia::RowCallback(png_structp p, png_uint_32 u,int n)
{
	if(m_pProgress && m_nTotalRows != 0) {
		m_pProgress->UpdateProgress((double)u * 100 / m_nTotalRows);
	}
}

void CPNGImageMedia::ErrCallback(png_structp png_ptr, png_const_charp message)
{
	CPNGImageMedia * pThis = (CPNGImageMedia *)png_get_error_ptr(png_ptr);
	CA2CT szMessage(message);
	pThis->m_strErr = szMessage;
}


BOOL CPNGImageMedia::SaveFile(BOOL &bCancel, CPackErrors &Error, CProgress &Progress)
{

	// 写文件！
	FILE * fp = NULL;	
	png_structp png_ptr = NULL; //png文件句柄
	png_infop	info_ptr = NULL;//png图像信息句柄
	BOOL bRet = FALSE;

	if(_tfopen_s(&fp, GetMediaFilePath(), _T("wb")) != 0 || fp == NULL) {
		Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), CPackUtils::GetLastStdError(errno));
		goto error;
	}

	// 初始化libpng的数据结构 :png_ptr, info_ptr
	if((png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL) {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	if((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
		Error.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	png_set_error_fn(png_ptr, this, ErrCallback, NULL);

	// 设置错误的返回点
	if(setjmp(png_jmpbuf(png_ptr))) {
		Error.SetError(CPackErrors::PE_IO, GetMediaFilePath(), m_strErr);
		if(fp) {
			fclose(fp);
			fp = NULL;
		}
		png_destroy_write_struct(&png_ptr, &info_ptr);
		m_pProgress = NULL;
		return FALSE;
	}

	png_set_write_status_fn(png_ptr, RowCallback);

	// 把png结构体和文件流io进行绑定 
	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, 
		info_ptr,
		m_PNGinfo.nWidth, 
		m_PNGinfo.nHeight, 
		m_PNGinfo.nBitDepth, 
		m_PNGinfo.nColorType, 
		PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_BASE, 
		PNG_FILTER_TYPE_BASE); 
	
	m_pProgress = &Progress;
	m_nTotalRows = m_PNGinfo.nHeight;
	Progress.PushRange(0.0, 100.0);

	png_write_info(png_ptr, info_ptr);

	png_set_rows(png_ptr, info_ptr, m_row_pointers);

	png_write_image(png_ptr, (png_bytepp)m_row_pointers);

	png_write_end(png_ptr, NULL);

	Progress.PopRange();

	bRet = TRUE;

	if(fp) {
		fclose(fp);
		fp = NULL;
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);
	m_pProgress = NULL;

	return bRet;

error:
	if(fp) {
		fclose(fp);
		fp = NULL;
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);
	m_pProgress = NULL;
	return bRet;
}

#define PNG_BYTES_TO_CHECK	8

BOOL CPNGImageMedia::ReopenFileToRead(CPackErrors &Error)
{
	return TRUE;
}

BOOL CPNGImageMedia::OpenFile(CMedia::media_mode_t mode, LPCTSTR szPath, BOOL &bCancel, CPackErrors &Error, CProgress &Progress)
{
	FILE * fp = NULL;	
	png_structp png_ptr = NULL; //png文件句柄
	png_infop	info_ptr = NULL;//png图像信息句柄
	BYTE checkheader[PNG_BYTES_TO_CHECK]; //查询是否png头
	BOOL bRet = FALSE;

	if(mode == MODE_READ) { // 读取 m_PNGinfo 和 m_row_pointers
		if(_tfopen_s(&fp, szPath, _T("rb")) != 0 || fp == NULL) {
			Error.SetError(CPackErrors::PE_IO, szPath, CPackUtils::GetLastStdError(errno));
			goto error;
		}

		if (fread(checkheader, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) { //读取png文件长度错误直接退出
			Error.SetError(CPackErrors::PE_IO, szPath, CPackUtils::GetLastStdError(errno));
			goto error;
		}

		if(png_sig_cmp(checkheader, 0, PNG_BYTES_TO_CHECK)) { //0正确, 非0错误
			Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
			goto error;
		}

		// 初始化libpng的数据结构 :png_ptr, info_ptr
		if((png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL) {
			Error.SetError(CPackErrors::PE_NOMEM);
			goto error;
		}

		if((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
			Error.SetError(CPackErrors::PE_NOMEM);
			goto error;
		}

		png_set_error_fn(png_ptr, this, ErrCallback, NULL);

		// 设置错误的返回点
		if(setjmp(png_jmpbuf(png_ptr))) {
			Error.SetError(CPackErrors::PE_IO, szPath, m_strErr);
			if(fp) {
				fclose(fp);
				fp = NULL;
			}
			if (m_row_pointers) {
				for (UINT row = 0; row < m_PNGinfo.nHeight; row++) {
					if(m_row_pointers[row]) {
						free(m_row_pointers[row]);
						m_row_pointers[row] = NULL;
					}
				}
				free(m_row_pointers);
				m_row_pointers = NULL;
			}
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			m_pProgress = NULL;
			return FALSE;
		}

		png_set_read_status_fn(png_ptr, RowCallback);

		rewind(fp);

		// 把png结构体和文件流io进行绑定 
		png_init_io(png_ptr, fp);

		png_read_info(png_ptr,info_ptr);

		m_PNGinfo.nChannels 	= png_get_channels(png_ptr, info_ptr); //通道数量
		m_PNGinfo.nColorType 	= png_get_color_type(png_ptr, info_ptr);//颜色类型
		m_PNGinfo.nBitDepth     = png_get_bit_depth(png_ptr, info_ptr);//位深度	
		m_PNGinfo.nWidth 	    = png_get_image_width(png_ptr, info_ptr);//宽
		m_PNGinfo.nHeight       = png_get_image_height(png_ptr, info_ptr);//高
		m_PNGinfo.nRowBytes     = png_get_rowbytes(png_ptr, info_ptr);

		if(!(m_PNGinfo.nChannels == 4 && m_PNGinfo.nColorType == PNG_COLOR_TYPE_RGB_ALPHA 
			|| m_PNGinfo.nChannels == 3 && m_PNGinfo.nColorType == PNG_COLOR_TYPE_RGB) ) {
			Error.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
			goto error;
		}

		//读取png文件信息
		m_pProgress = &Progress;
		m_nTotalRows = m_PNGinfo.nHeight;
		Progress.PushRange(0, 100.0);

	   if (m_PNGinfo.nHeight > PNG_UINT_32_MAX/png_sizeof(png_bytep))
		  png_error(png_ptr, "Image is too high to process with png_read_png()");

		(void)png_set_interlace_handling(png_ptr);

		png_read_update_info(png_ptr, info_ptr);

		m_row_pointers = (png_bytepp)malloc(m_PNGinfo.nHeight * (sizeof (png_bytep)));
		if(!m_row_pointers) {
			Error.SetError(CPackErrors::PE_NOMEM);
			goto error;
		}

		for (UINT i = 0; i < m_PNGinfo.nHeight; i++) {
			m_row_pointers[i] = NULL;
		}

		for (UINT i = 0; i < m_PNGinfo.nHeight; i++) {
			m_row_pointers[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
			if(!m_row_pointers[i]) {
				Error.SetError(CPackErrors::PE_NOMEM);
				goto error;
			}
		}

		png_set_rows(png_ptr, info_ptr, m_row_pointers);

		png_read_image(png_ptr, m_row_pointers);

		/* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
		png_read_end(png_ptr, info_ptr);

		Progress.PopRange();
		m_pProgress = NULL;

		if(fp != NULL) {
			fclose(fp);
			fp = NULL;
		}
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		m_pProgress = NULL;

		bRet = TRUE;

	} else {
		bRet = TRUE; // 什么都不干
	}

	return bRet;

error:
	if(fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
	if (m_row_pointers) {
		for (UINT row = 0; row < m_PNGinfo.nHeight; row++) {
			if(m_row_pointers[row]) {
				free(m_row_pointers[row]);
				m_row_pointers[row] = NULL;
			}
		}
		free(m_row_pointers);
		m_row_pointers = NULL;
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	m_pProgress = NULL;
	return bRet;
}

void CPNGImageMedia::CloseFile()
{
	if (m_row_pointers) {
		for (UINT row = 0; row < m_PNGinfo.nHeight; row++) {
			if(m_row_pointers[row]) {
				free(m_row_pointers[row]);
				m_row_pointers[row] = NULL;
			}
		}
		free(m_row_pointers);
		m_row_pointers = NULL;
	}
}


CMedia * CPNGImageMedia::CloneMedia(CPackErrors & Error)
{
	CPNGImageMedia * pMedia = new(std::nothrow) CPNGImageMedia();
	if(pMedia) {
		pMedia->CloneCore(this);
		memcpy(&pMedia->m_PNGinfo, &m_PNGinfo, sizeof(m_PNGinfo));
		if(m_row_pointers != NULL) {
			pMedia->m_row_pointers = (png_bytepp)malloc(pMedia->m_PNGinfo.nHeight * (sizeof (png_bytep)));
			if(pMedia->m_row_pointers) {
				for (UINT i = 0; i < pMedia->m_PNGinfo.nHeight; i++) {
					pMedia->m_row_pointers[i] = NULL;
				}

				for (UINT i = 0; i < pMedia->m_PNGinfo.nHeight; i++) {
					pMedia->m_row_pointers[i] = (png_bytep)malloc(pMedia->m_PNGinfo.nRowBytes);
					if(!pMedia->m_row_pointers[i]) {
						goto error;
					}
				}
			}
		}
	}
	if(!pMedia) {
		Error.SetError(CPackErrors::PE_NOMEM);
	}
	return pMedia;

error:
	if(pMedia && pMedia->m_row_pointers) {
		for (UINT row = 0; row < pMedia->m_PNGinfo.nHeight; row++) {
			if(pMedia->m_row_pointers[row]) {
				free(pMedia->m_row_pointers[row]);
				pMedia->m_row_pointers[row] = NULL;
			}
		}
		free(pMedia->m_row_pointers);
		pMedia->m_row_pointers = NULL;
	}
	delete pMedia;
	Error.SetError(CPackErrors::PE_NOMEM);
	return NULL;
}

LPCTSTR CPNGImageMedia::GetFilter()
{
	return m_szFilter; 
}

LPCTSTR CPNGImageMedia::GetDefaultExt()
{
	return m_szExt;
}

BOOL CPNGImageMedia::TestExt(LPCTSTR szExt)
{
	return lstrcmpi(szExt, m_szExt) == 0;
}

LPCTSTR CPNGImageMedia::GetExtFilter()
{
	return m_szFilter;
}

CMedia * CPNGImageMedia::Factory()
{
	return new(std::nothrow) CPNGImageMedia();
}
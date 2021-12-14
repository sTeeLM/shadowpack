#include "pch.h"
#include "PNGFileMedia.h"
#include "PackUtils.h"

CPNGFileMedia::CPNGFileMedia()
{
}

CPNGFileMedia::~CPNGFileMedia()
{
}

void CPNGFileMedia::ErrCallback(png_structp png_ptr, png_const_charp message)
{
	/*
	CPNGImageMedia* pThis = (CPNGImageMedia*)png_get_error_ptr(png_ptr);
	CA2CT szMessage(message);
	pThis->m_strErr = szMessage;
	*/
}

void CPNGFileMedia::RowCallback(png_structp p, png_uint_32 u, int n)
{
	/*
	if (m_pProgress && m_nTotalRows != 0) {
		m_pProgress->UpdateProgress((double)u * 100 / m_nTotalRows);
	}
	*/
}


#define PNG_BYTES_TO_CHECK	8
BOOL CPNGFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	FILE* fp = NULL;
	png_structp png_ptr = NULL; //png文件句柄
	png_infop	info_ptr = NULL;//png图像信息句柄
	BYTE checkheader[PNG_BYTES_TO_CHECK]; //查询是否png头
	png_bytepp row_pointers = NULL;
	BOOL bRet;

	if (_tfopen_s(&fp, szFilePath, _T("rb")) != 0 || fp == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto error;
	}

	if (fread(checkheader, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) { //读取png文件长度错误直接退出
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto error;
	}

	if (png_sig_cmp(checkheader, 0, PNG_BYTES_TO_CHECK)) { //0正确, 非0错误
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
		goto error;
	}

	// 初始化libpng的数据结构 :png_ptr, info_ptr
	if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	png_set_error_fn(png_ptr, this, ErrCallback, NULL);
	png_set_read_status_fn(png_ptr, RowCallback);
	rewind(fp);

	// 把png结构体和文件流io进行绑定 
	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);

	m_PNGInfo.nChannels = png_get_channels(png_ptr, info_ptr); //通道数量
	m_PNGInfo.nColorType = png_get_color_type(png_ptr, info_ptr);//颜色类型
	m_PNGInfo.nBitDepth = png_get_bit_depth(png_ptr, info_ptr);//位深度	
	m_PNGInfo.nWidth = png_get_image_width(png_ptr, info_ptr);//宽
	m_PNGInfo.nHeight = png_get_image_height(png_ptr, info_ptr);//高
	m_PNGInfo.nRowBytes = png_get_rowbytes(png_ptr, info_ptr);

	if (!(m_PNGInfo.nChannels == 4 && m_PNGInfo.nColorType == PNG_COLOR_TYPE_RGB_ALPHA
		|| m_PNGInfo.nChannels == 3 && m_PNGInfo.nColorType == PNG_COLOR_TYPE_RGB)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
		goto error;
	}

	if (m_PNGInfo.nHeight > PNG_UINT_32_MAX / png_sizeof(png_bytep)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA);
		goto error;
	}

	// alloc buffer
	if (!CPixelImageMedia::Alloc(m_PNGInfo.nWidth, m_PNGInfo.nHeight, Errors)) {
		goto error;
	}

	png_set_interlace_handling(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	row_pointers = (png_bytepp)malloc(m_PNGInfo.nHeight * (sizeof(png_bytep)));
	if (!row_pointers) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
		row_pointers[i] = NULL;
	}

	for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
		row_pointers[i] = (png_bytep)malloc(m_PNGInfo.nRowBytes);
		if (!row_pointers[i]) {
			Errors.SetError(CPackErrors::PE_NOMEM);
			goto error;
		}
	}

	png_set_rows(png_ptr, info_ptr, row_pointers);
	
	png_read_image(png_ptr, row_pointers);

	/* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, info_ptr);

	// fill buffer
	for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
		CPixelImageMedia::SetScanline(i, row_pointers[i], m_PNGInfo.nChannels == 4 ? 
			CPixelImageMedia::CPixelBlock::PIXEL_FORMAT_RGBA : CPixelImageMedia::CPixelBlock::PIXEL_FORMAT_RGB);
	}

	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto error;
	}
	PNG_COMPRESSION_TYPE_BASE;
	bRet = TRUE;
error:
	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
	if (row_pointers) {
		for (UINT row = 0; row < m_PNGInfo.nHeight; row++) {
			if (row_pointers[row]) {
				free(row_pointers[row]);
				row_pointers[row] = NULL;
			}
		}
		free(row_pointers);
		row_pointers = NULL;
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	if (!bRet)
		CPixelImageMedia::Free();
    return bRet;
}

BOOL CPNGFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
    return 0;
}

void CPNGFileMedia::CloseMedia()
{
	// free buffer
	CPixelImageMedia::Free();
}

void CPNGFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CPNGFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
    return 0;
}

BOOL CPNGFileMedia::TestExt(LPCTSTR szExt)
{
    return lstrcmpi(szExt, m_szExt) == 0;
}

LPCTSTR CPNGFileMedia::GetExtFilter()
{
    return m_szFilter;
}

CMediaBase* CPNGFileMedia::Factory()
{
    return new(std::nothrow) CPNGFileMedia();
}

LPCTSTR CPNGFileMedia::m_szFilter = _T("PNG Files (*.png)|*.png|");
LPCTSTR CPNGFileMedia::m_szExt = _T("PNG");
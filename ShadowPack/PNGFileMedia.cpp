#include "pch.h"
#include "PNGFileMedia.h"
#include "PackUtils.h"
#include "resource.h"

CPNGFileMedia::CPNGFileMedia() :
	m_bError(FALSE),
	m_strLastError(_T("")),
	pProgress(NULL)
{

}

CPNGFileMedia::~CPNGFileMedia()
{
}

void CPNGFileMedia::ErrCallback(png_structp png_ptr, png_const_charp message)
{
	CPNGFileMedia* pThis = (CPNGFileMedia*)png_get_error_ptr(png_ptr);
	if (!pThis->m_bError) {
		CA2CT szMessage(message);
		TRACE(_T("%s\n"), (LPCTSTR)szMessage);
		pThis->m_bError = TRUE;
		pThis->m_strLastError = (LPCTSTR)szMessage;
		TRACE(_T("%s\n"), pThis->m_strLastError);
		AfxThrowUserException();
	}
	
}

void CPNGFileMedia::RowCallback(png_structp png_ptr, png_uint_32 u, int n)
{
	/*
	if (m_pProgress && m_nTotalRows != 0) {
		m_pProgress->UpdateProgress((double)u * 100 / m_nTotalRows);
	}
	*/
	CPNGFileMedia* pThis = (CPNGFileMedia*)png_get_error_ptr(png_ptr);
	TRACE(_T("%d %d\n"), u, n);
	pThis->pProgress->SetScale((u + 1) * (n + 1));
}


#define PNG_BYTES_TO_CHECK	8
BOOL CPNGFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	FILE* fp = NULL;
	png_structp png_ptr = NULL; //png文件句柄
	png_infop	info_ptr = NULL;//png图像信息句柄
	BYTE checkheader[PNG_BYTES_TO_CHECK]; //查询是否png头
	png_bytepp row_pointers = NULL;
	png_bytep  row_pointer_cache = NULL;
	BOOL bRet = FALSE;

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

	png_get_IHDR(
		png_ptr,
		info_ptr,
		&m_PNGInfo.nWidth,
		&m_PNGInfo.nHeight,
		&m_PNGInfo.nBitDepth,
		&m_PNGInfo.nColorType,
		&m_PNGInfo.nInterlaceType,
		&m_PNGInfo.nCompressionType,
		&m_PNGInfo.nFilterType
	);

	m_PNGInfo.nChannels = png_get_channels(png_ptr, info_ptr); //通道数量
	m_PNGInfo.nRowBytes = png_get_rowbytes(png_ptr, info_ptr);

	if (!(m_PNGInfo.nChannels == 4 && m_PNGInfo.nColorType == PNG_COLOR_TYPE_RGB_ALPHA
		|| m_PNGInfo.nChannels == 3 && m_PNGInfo.nColorType == PNG_COLOR_TYPE_RGB)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto error;
	}

	if (m_PNGInfo.nHeight > PNG_UINT_32_MAX / png_sizeof(png_bytep)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto error;
	}

	// alloc buffer
	if (!CPixelImageMedia::Alloc(m_PNGInfo.nWidth, m_PNGInfo.nHeight, Errors)) {
		goto error;
	}

	m_bError = FALSE;
	try {
		png_set_interlace_handling(png_ptr);

		png_read_update_info(png_ptr, info_ptr);
	} catch (CException* e) {
		e->Delete();
	}

	if (m_bError) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto error;
	}

	row_pointers = (png_bytepp)malloc(m_PNGInfo.nHeight * (sizeof(png_bytep)));
	if (!row_pointers) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
		row_pointers[i] = NULL;
	}

	if (!m_bUseFileCache) {
		for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
			row_pointers[i] = (png_bytep)malloc(m_PNGInfo.nRowBytes);
			if (!row_pointers[i]) {
				Errors.SetError(CPackErrors::PE_NOMEM);
				goto error;
			}
		}
	} else {
		row_pointer_cache = (png_bytep)m_FileCache.Alloc(m_PNGInfo.nRowBytes * m_PNGInfo.nHeight, Errors);
		if (!row_pointer_cache) {
			goto error;
		}
		for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
			row_pointers[i] = row_pointer_cache + i * m_PNGInfo.nRowBytes;
		}
	}

	m_bError = FALSE;
	pProgress = &Progress;
	Progress.Reset(IDS_READ_FILE);
	Progress.SetFullScale(m_PNGInfo.nHeight * (m_PNGInfo.nInterlaceType ? 7 : 1));
	try {
		png_set_rows(png_ptr, info_ptr, row_pointers);
		png_read_image(png_ptr, row_pointers);

		/* Read rest of file, and get additional chunks in info_ptr - REQUIRED */
		png_read_end(png_ptr, info_ptr);
	} catch (CException* e) {
		e->Delete();
	}

	if (m_bError) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto error;
	}

	Progress.Reset(IDS_LOAD_DATA);
	Progress.SetFullScale(m_PNGInfo.nHeight);
	// fill buffer
	for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
		CPixelImageMedia::SetScanline(i, row_pointers[i], m_PNGInfo.nChannels == 4 ? 
			CPixelImageMedia::PIXEL_FORMAT_RGBA : CPixelImageMedia::PIXEL_FORMAT_RGB);
		Progress.Increase(1);
		if (Progress.IsCanceled(Errors)) {
			goto error;
		}
	}

	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto error;
	}

	bRet = TRUE;
error:
	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
	if (!m_bUseFileCache) {
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
	}
	else {
		if (row_pointers) {
			free(row_pointers);
			row_pointers = NULL;
		}
		if (row_pointer_cache) {
			m_FileCache.Free(row_pointer_cache);
			row_pointer_cache = NULL;
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	if (!bRet)
		CPixelImageMedia::Free();
    return bRet;
}

BOOL CPNGFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	FILE* fp = NULL;
	png_structp png_ptr = NULL; //png文件句柄
	png_infop	info_ptr = NULL;//png图像信息句柄
	BOOL bRet = FALSE;
	png_bytepp row_pointers = NULL;
	png_bytep row_pointer_cache = NULL;

	row_pointers = (png_bytepp)malloc(m_PNGInfo.nHeight * (sizeof(png_bytep)));
	if (!row_pointers) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	if (!CBytePerBlockMedia::SaveMeta(Errors)) {
		goto error;
	}

	Progress.Reset(IDS_FILL_EMPTY_SPACE);
	Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

	if (!CBytePerBlockMedia::FillEmptySpace(Progress, Errors)) {
		goto error;
	}

	for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
		row_pointers[i] = NULL;
	}

	if (!m_bUseFileCache) {
		for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
			row_pointers[i] = (png_bytep)malloc(m_PNGInfo.nRowBytes);
			if (!row_pointers[i]) {
				Errors.SetError(CPackErrors::PE_NOMEM);
				goto error;
			}
		}
	}
	else {
		row_pointer_cache = (png_bytep)m_FileCache.Alloc(m_PNGInfo.nRowBytes * m_PNGInfo.nHeight, Errors);
		if (!row_pointer_cache) {
			goto error;
		}
		for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
			row_pointers[i] = row_pointer_cache + i * m_PNGInfo.nRowBytes;
		}
	}

	if (_tfopen_s(&fp, szFilePath, _T("wb")) != 0 || fp == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto error;
	}

	// 初始化libpng的数据结构 :png_ptr, info_ptr
	if ((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto error;
	}

	png_set_error_fn(png_ptr, this, ErrCallback, NULL);
	png_set_write_status_fn(png_ptr, RowCallback);

	// 把png结构体和文件流io进行绑定 
	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr,
		info_ptr,
		m_PNGInfo.nWidth,
		m_PNGInfo.nHeight,
		m_PNGInfo.nBitDepth,
		m_PNGInfo.nColorType,
		m_PNGInfo.nInterlaceType,
		m_PNGInfo.nCompressionType,
		m_PNGInfo.nFilterType);

	Progress.Reset(IDS_SAVE_DATA);
	Progress.SetFullScale(m_PNGInfo.nHeight);
	// fill buffer
	for (UINT i = 0; i < m_PNGInfo.nHeight; i++) {
		CPixelImageMedia::GetScanline(i, row_pointers[i], m_PNGInfo.nChannels == 4 ?
			CPixelImageMedia::PIXEL_FORMAT_RGBA : CPixelImageMedia::PIXEL_FORMAT_RGB);
		Progress.Increase(1);
		if (Progress.IsCanceled(Errors)) {
			goto error;
		}
	}

	m_bError = FALSE;
	pProgress = &Progress;
	Progress.SetFullScale(m_PNGInfo.nHeight * (m_PNGInfo.nInterlaceType ? 7 : 1));
	try {
		png_write_info(png_ptr, info_ptr);

		png_set_rows(png_ptr, info_ptr, row_pointers);

		png_write_image(png_ptr, (png_bytepp)row_pointers);

		png_write_end(png_ptr, NULL);
	}
	catch (CException* e) {
		e->Delete();
	}

	if (m_bError) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto error;
	}

	ClearMediaDirty();

	bRet = TRUE;
error:
	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
	if (!m_bUseFileCache) {
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
	}
	else {
		if (row_pointers) {
			free(row_pointers);
			row_pointers = NULL;
		}
		if (row_pointer_cache) {
			m_FileCache.Free(row_pointer_cache);
			row_pointer_cache = NULL;
		}
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return bRet;
}

void CPNGFileMedia::CloseMedia()
{
	// free buffer
	CPixelImageMedia::Free();
	ZeroMemory(&m_PNGInfo, sizeof(m_PNGInfo));
}

void CPNGFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
	m_OptPagePNGFile.m_strBitDepth.Format(_T("%d"), m_PNGInfo.nBitDepth);
	m_OptPagePNGFile.m_strColorType.Format(_T("%d"), m_PNGInfo.nColorType);
	m_OptPagePNGFile.m_strCompressionType.Format(_T("%d"), m_PNGInfo.nCompressionType);
	m_OptPagePNGFile.m_strFilterType.Format(_T("%d"), m_PNGInfo.nFilterType);
	m_OptPagePNGFile.m_strHeigth.Format(_T("%d"), m_PNGInfo.nHeight);
	m_OptPagePNGFile.m_strInterlaceType.Format(_T("%d"), m_PNGInfo.nInterlaceType);
	m_OptPagePNGFile.m_strWidth.Format(_T("%d"), m_PNGInfo.nWidth);
	m_OptPagePNGFile.m_strChannels.Format(_T("%d"), m_PNGInfo.nChannels);

	pPropertySheet->AddPage(&m_OptPagePNGFile);
	CPixelImageMedia::AddOptPage(pPropertySheet);
}

BOOL CPNGFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPixelImageMedia::UpdateOpts(pPropertySheet);
}

void CPNGFileMedia::GetMediaInfo(CArray<CMediaFactory::CMediaInfo>& InfoArray)
{
	CMediaFactory::CMediaInfo Info;
	Info.Exts.Add(_T("png"));
	Info.fnFactory = Factory;
	Info.nCatagory = IDS_MEDIA_IMAGE_FILE;
	Info.strName = _T("Portable Network Graphic");
	InfoArray.Add(Info);
}

CMediaBase* CPNGFileMedia::Factory()
{
    return new(std::nothrow) CPNGFileMedia();
}
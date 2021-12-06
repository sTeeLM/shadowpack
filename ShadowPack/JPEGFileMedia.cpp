#include "pch.h"
#include "JPEGFileMedia.h"
#include "PackUtils.h"


CJPEGFileMedia::CJPEGFileMedia()
{

}

CJPEGFileMedia::~CJPEGFileMedia()
{

}

INT nReadCnt;
INT nWriteCnt;

void CJPEGFileMedia::JStegErrorExit(j_common_ptr cinfo)
{

}

void CJPEGFileMedia::JStegReadData(j_common_ptr cinfo, JCOEF data)
{
//    TRACE(_T("JCOEF %04hx\n"), data);
    nReadCnt++;
}

JCOEF CJPEGFileMedia::JStegWriteData(j_common_ptr cinfo, JCOEF data)
{
    nWriteCnt++;
    return data;
}

BOOL CJPEGFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    struct jpeg_steg_mgr jsteg;
    FILE* pFile = NULL;
    INT row_stride;
    JSAMPARRAY row_buffer = NULL;

    ZeroMemory(&cinfo, sizeof(cinfo));

    nReadCnt = nWriteCnt = 0;

    // open file
    if ((pFile = _tfsopen(szFilePath, _T("rb"), _SH_DENYWR)) == NULL) {
        Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
        goto err;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = JStegErrorExit;

    jpeg_create_decompress(&cinfo);

    cinfo.steg = &jsteg;
    jsteg.steg_read = JStegReadData;
    jsteg.steg_write = JStegWriteData;

    jpeg_stdio_src(&cinfo, pFile);

    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);


    TRACE(_T("output_width = %d, output_height = %d\n"), 
        cinfo.output_width,
        cinfo.output_height
        );

    TRACE(_T("estmate blocks: %d\n"), cinfo.MCUs_per_row * cinfo.MCU_rows_in_scan * cinfo.blocks_in_MCU * 64);

    if (cinfo.output_components != 3) {
        Errors.SetError(CPackErrors::PE_UNSUPPORT_PACK);
        goto err;
    }

    row_stride = cinfo.output_width * cinfo.output_components;

    row_buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, row_buffer, 1);
        //if(!bmp.SetBufferData((cinfo.output_scanline-1)*row_stride, row_stride, buffer[0]))
    }
    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    fclose(pFile);

    TRACE(_T("read cnt %d, write cnt %d\n"), nReadCnt, nWriteCnt);

    return TRUE;
err:
    if (pFile) {
        fclose(pFile);
    }
    return FALSE;
}

BOOL CJPEGFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
    return 0;
}

void CJPEGFileMedia::CloseMedia()
{
}

void CJPEGFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
}

BOOL CJPEGFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
    return 0;
}

LPCTSTR CJPEGFileMedia::m_szFilter = _T("JPEG Files (*.jpg; *.jpeg)|*.jpg; *.jpeg|");
LPCTSTR CJPEGFileMedia::m_szExt = _T("JPEG");

BOOL CJPEGFileMedia::TestExt(LPCTSTR szExt)
{
    return (lstrcmpi(szExt, m_szExt) == 0 || lstrcmpi(szExt, _T("JPG")) == 0);
}

LPCTSTR CJPEGFileMedia::GetExtFilter()
{
    return m_szFilter;
}

CMediaBase* CJPEGFileMedia::Factory()
{
    return new(std::nothrow) CJPEGFileMedia();
}

#include "pch.h"
#include "JPEGFileMedia.h"
#include "PackUtils.h"


CJPEGFileMedia::CJPEGFileMedia()
{

}

CJPEGFileMedia::~CJPEGFileMedia()
{

}

void CJPEGFileMedia::JStegErrorExit(j_common_ptr cinfo)
{
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message) (cinfo, buffer);

//    TRACE(_T("%s\n"), LPCTSTR(CA2CT(buffer)));
    throw "FUCK";
}


BOOL CJPEGFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
    struct jpeg_error_mgr jerr;
    FILE* pFile = NULL;
    INT row_stride;
    JSAMPARRAY row_buffer = NULL;

    ZeroMemory(&m_Decinfo, sizeof(m_Decinfo));

    // open file
    if ((pFile = _tfsopen(szFilePath, _T("rb"), _SH_DENYWR)) == NULL) {
        Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
        goto err;
    }

    m_Decinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = JStegErrorExit;

    jpeg_create_decompress(&m_Decinfo);

    try {
        jpeg_stdio_src(&m_Decinfo, pFile);

        jpeg_read_header(&m_Decinfo, TRUE);

    }
    catch (char * perr) {
        TRACE(_T("%s\n"), LPCTSTR(CA2CT(perr)));;
    }

    jvirt_barray_ptr* p = jpeg_read_coefficients(&m_Decinfo);




    jpeg_finish_decompress(&m_Decinfo);

    jpeg_destroy_decompress(&m_Decinfo);

    fclose(pFile);

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

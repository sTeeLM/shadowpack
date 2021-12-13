#include "pch.h"
#include "JPEGFileMedia.h"
#include "PackUtils.h"
#include "resource.h"


CJPEGFileMedia::CJPEGFileMedia() :
    m_bError(FALSE),
    m_virt_p(NULL)
{

}

CJPEGFileMedia::~CJPEGFileMedia()
{

}


void CJPEGFileMedia::JStegErrorExit(j_common_ptr cinfo)
{
    CJPEGFileMedia* pThis = (CJPEGFileMedia*)cinfo->client_data;
    char buffer[JMSG_LENGTH_MAX];
    if (!pThis->m_bError) {
        (*cinfo->err->format_message) (cinfo, buffer);
        CA2CT tbuffer(buffer);
        pThis->m_bError = TRUE;
        pThis->m_strLastError = (LPCTSTR)tbuffer;
        TRACE(_T("%s\n"), pThis->m_strLastError);
        AfxThrowUserException();
    }
}


BOOL CJPEGFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, 
    CProgressBase& Progress, CPackErrors& Errors)
{
    struct jpeg_error_mgr jerr;
    FILE* pFile = NULL;

    ZeroMemory(&m_Decinfo, sizeof(m_Decinfo));

    // open file
    if ((pFile = _tfsopen(szFilePath, _T("rb"), _SH_DENYWR)) == NULL) {
        Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
        goto err;
    }

    m_Decinfo.err = jpeg_std_error(&jerr);
    m_Decinfo.client_data = this;
    jerr.error_exit = JStegErrorExit;

    m_bError = FALSE;
    try {
        jpeg_create_decompress(&m_Decinfo);
        jpeg_stdio_src(&m_Decinfo, pFile);
        jpeg_read_header(&m_Decinfo, TRUE);
    } catch (CException* e) {
        e->Delete();
    }

    if (m_bError) {
        Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
        goto err;
    }

    if (m_Decinfo.num_components != 3) {
        Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
        goto err;
    }

    if (!(m_virt_p = jpeg_read_coefficients(&m_Decinfo))) {
        Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
        goto err;
    }

    if (!CCorImageMedia::Alloc(m_Decinfo.image_width, m_Decinfo.image_height,
        m_Decinfo.num_components, Errors)) {
        goto err;
    }

    Progress.Reset(IDS_READ_FILE);
    Progress.SetFullScale(CCorImageMedia::GetTotalCoeffs());
    for (unsigned short icomp = 0; icomp < m_Decinfo.num_components; icomp++) {
        unsigned int currow = 0;
        while (currow < CCorImageMedia::GetHeightInBlocks(icomp)) {
            JBLOCKARRAY p_array = (*(m_Decinfo.mem->access_virt_barray))
                ((j_common_ptr)&m_Decinfo, m_virt_p[icomp], currow, 1, FALSE);
            for (unsigned int iblock = 0; iblock < CCorImageMedia::GetWidthInBlocks(icomp); iblock++) {
                for (unsigned int icoeff = 0; icoeff < CCorImageMedia::GetCoeffPerBlock(); icoeff++) {
                    CCorImageMedia::AddCoeff(icomp, iblock, currow, icoeff, p_array[0][iblock][icoeff]);
                    Progress.Increase(1);
                }
            }
            currow++;
            if (Progress.IsCanceled()) {
                Errors.SetError(CPackErrors::PE_CANCELED);
                goto err;
            }
        }
    }

    if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
        goto err;
    }

    fclose(pFile);
    pFile = NULL;
    return TRUE;
err:
    if (pFile) {
        fclose(pFile);
    }
    jpeg_destroy_decompress(&m_Decinfo);
    m_virt_p = NULL;
    CCorImageMedia::Free();
    return FALSE;
}

BOOL CJPEGFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
    struct jpeg_compress_struct Cominfo;
    struct jpeg_error_mgr jerr;
    FILE* pFile = NULL;

    ZeroMemory(&Cominfo, sizeof(Cominfo));

    Cominfo.err = jpeg_std_error(&jerr);
    Cominfo.client_data = this;
    jerr.error_exit = JStegErrorExit;

    if (!CBytePerBlockMedia::SaveMeta(Errors)) {
        goto err;
    }

    Progress.Reset(IDS_FILL_EMPTY_SPACE);
    Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

    if (!CBytePerBlockMedia::FillEmptySpace(Progress, Errors)) {
        goto err;
    }

    // open file and write data
    if ((pFile = _tfsopen(szFilePath, _T("wb"), _SH_DENYNO)) == NULL) {
        Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
        goto err;
    }

    m_bError = FALSE;
    try {
        jpeg_create_compress(&Cominfo);
        jpeg_copy_critical_parameters(&m_Decinfo, &Cominfo);
        jpeg_stdio_dest(&Cominfo, pFile);
        // write file header
        jpeg_write_coefficients(&Cominfo, m_virt_p);
    } catch (CException* e) {
        e->Delete();
    }

    if (m_bError) {
        Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
        goto err;
    }

    Progress.Reset(IDS_WRITE_FILE);
    Progress.SetFullScale(CCorImageMedia::GetTotalCoeffs());

    for (unsigned short icomp = 0; icomp < Cominfo.num_components; icomp++) {
        unsigned int currow = 0;
        while (currow < GetHeightInBlocks(icomp)) {
            JBLOCKARRAY p_array = (*(Cominfo.mem->access_virt_barray))
                ((j_common_ptr)&Cominfo, m_virt_p[icomp], currow, 1, TRUE);
            for (unsigned int iblock = 0; iblock < GetWidthInBlocks(icomp); iblock++) {
                for (unsigned int icoeff = 0; icoeff < GetCoeffPerBlock(); icoeff++) {
                    p_array[0][iblock][icoeff] = CCorImageMedia::GetCoeff(icomp, iblock, currow, icoeff);
                    Progress.Increase(1);
                }
            }
            currow++;
            if (Progress.IsCanceled()) {
                Errors.SetError(CPackErrors::PE_CANCELED);
                goto err;
            }
        }
    }
    jpeg_finish_compress(&Cominfo);
    jpeg_destroy_compress(&Cominfo);
    fclose(pFile);
    pFile = NULL;

    // done!
    ClearMediaDirty();

    return TRUE;
err:
    if (pFile) {
        fclose(pFile);
    }
    jpeg_finish_compress(&Cominfo);
    jpeg_destroy_compress(&Cominfo);
    return FALSE;
}

void CJPEGFileMedia::CloseMedia()
{
//    jpeg_finish_decompress(&m_Decinfo);
    jpeg_destroy_decompress(&m_Decinfo);
    ZeroMemory(&m_Decinfo, sizeof(m_Decinfo));
    m_virt_p = NULL;
    CCorImageMedia::Free();
}

// JCS_UNKNOWN,		/* error/unspecified */
// JCS_GRAYSCALE,		/* monochrome */
// JCS_RGB,		/* red/green/blue */
// JCS_YCbCr,		/* Y/Cb/Cr (also known as YUV) */
// JCS_CMYK,		/* C/M/Y/K */
// JCS_YCCK		/* Y/Cb/Cr/K */
LPCTSTR CJPEGFileMedia::szJPEGColorSpace[] =
{
    _T("JCS_UNKNOWN"),
    _T("JCS_GRAYSCALE"),
    _T("JCS_RGB"),
    _T("JCS_YCbCr"),
    _T("JCS_CMYK"),
    _T("JCS_YCCK"),
};

/* into the JFIF APP0 marker.  density_unit can be 0 for unknown, */
/* 1 for dots/inch, or 2 for dots/cm.  Note that the pixel aspect */
LPCTSTR CJPEGFileMedia::szJFIFDensityUnit[] =
{
    _T("unknown"),
    _T("dots/inch"),
    _T("dots/cm"),
};
void CJPEGFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
/*
    CString m_strImageWidth;
    CString m_strImageHeight;
    CString m_strJFIFMajor;
    CString m_strJFIFMinor;
    CString m_strDensityUnit;
    CString m_strNumComp;
    CString m_strColorSpace;
    CString m_strXDensity;
    CString m_strYDensity;
*/
    m_OptPageJPEGFile.m_strImageWidth.Format(_T("%d"), m_Decinfo.image_width);
    m_OptPageJPEGFile.m_strImageHeight.Format(_T("%d"), m_Decinfo.image_height);
    m_OptPageJPEGFile.m_strJFIFMajor.Format(_T("%d"), m_Decinfo.JFIF_major_version);
    m_OptPageJPEGFile.m_strJFIFMinor.Format(_T("%d"), m_Decinfo.JFIF_minor_version);
    m_OptPageJPEGFile.m_strDensityUnit.Format(_T("%s"), m_Decinfo.density_unit < 3 ? 
        szJFIFDensityUnit[m_Decinfo.density_unit] : _T("UNKNOWN"));
    m_OptPageJPEGFile.m_strNumComp.Format(_T("%d"), m_Decinfo.num_components);
    m_OptPageJPEGFile.m_strColorSpace.Format(_T("%s"), m_Decinfo.jpeg_color_space < 6 ? 
        szJPEGColorSpace[m_Decinfo.jpeg_color_space] : _T("UNKNOWN"));
    m_OptPageJPEGFile.m_strXDensity.Format(_T("%d"), m_Decinfo.X_density);
    m_OptPageJPEGFile.m_strYDensity.Format(_T("%d"), m_Decinfo.Y_density);

    pPropertySheet->AddPage(&m_OptPageJPEGFile);
    CCorImageMedia::AddOptPage(pPropertySheet);
}

BOOL CJPEGFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
    return CCorImageMedia::UpdateOpts(pPropertySheet);
}

UINT CJPEGFileMedia::GetVSampleFactor(UINT nComponents)
{
    if(m_Decinfo.comp_info)
        return m_Decinfo.comp_info[nComponents].v_samp_factor;
    return 0;
}

UINT CJPEGFileMedia::GetHSampleFactor(UINT nComponents)
{
    if(m_Decinfo.comp_info)
        return m_Decinfo.comp_info[nComponents].h_samp_factor;
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

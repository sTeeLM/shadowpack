#include "pch.h"
#include "PPMFileMedia.h"
#include "PackUtils.h"
#include "resource.h"
#include "netpbm\ppm.h"


CPPMFileMedia::CPPMFileMedia() :
	m_nCols(0),
	m_nRows(0),
	m_nFormat(0)
{

}

CPPMFileMedia::~CPPMFileMedia()
{
}

CString CPPMFileMedia::m_strLastError = _T("");
BOOL CPPMFileMedia::m_bError = FALSE;
void CPPMFileMedia::ErrorMessage(const char* msg)
{
	m_strLastError = ((LPCTSTR)CA2CT(msg));
}

void CPPMFileMedia::ErrorQuit()
{
	if (!m_bError) {
		m_bError = TRUE;
		AfxThrowUserException();
	}
}

BOOL CPPMFileMedia::LoadMedia(LPCTSTR szFilePath, CPasswordGetterBase& PasswordGetter, CProgressBase& Progress, CPackErrors& Errors)
{
	BOOL bRet = FALSE;
	FILE* pFile = NULL;
	pixval maxval;
	pixel* row_buffer = NULL;

	// open file and read meta
	if ((pFile = _tfsopen(szFilePath, _T("rb"), _SH_DENYWR)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto err;
	}

	pm_setusererrormsgfn(ErrorMessage);
	pm_setusererrorquitfn(ErrorQuit);

	m_bError = FALSE;
	try {
		ppm_readppminit(pFile, &m_nCols, &m_nRows, &maxval, &m_nFormat);
	}
	catch (CException* e) {
		e->Delete();
	}

	if (m_bError) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}

	if (maxval > PPM_MAXMAXVAL || (m_nFormat != PPM_FORMAT && m_nFormat != RPPM_FORMAT)) {
		Errors.SetError(CPackErrors::PE_UNSUPPORT_MEDIA, szFilePath);
		goto err;
	}

	// alloc buffer
	if (!CPixelImageMedia::Alloc(m_nCols, m_nRows, Errors)) {
		goto err;
	}

	m_bError = FALSE;
	try {
		row_buffer = ppm_allocrow(m_nCols);
	}
	catch (CException* e) {
		e->Delete();
	}
	if (m_bError) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	Progress.Reset(IDS_READ_FILE);
	Progress.SetFullScale(m_nRows);
	m_bError = FALSE;
	try {
		for (UINT i = 0; i < m_nRows; i++) {
			ppm_readppmrow(pFile, row_buffer, m_nCols, maxval, m_nFormat);
			for (UINT j = 0; j < m_nCols; j++) {
				CPixelImageMedia::SetPixel(j, i, row_buffer[j].r, row_buffer[j].g, row_buffer[j].b);
			}
			Progress.Increase(1);
			if (Progress.IsCanceled(Errors)) {
				goto err;
			}
		}
	}
	catch (CException* e) {
		e->Delete();
	}

	if (m_bError) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}

	// test format
	if (!CBytePerBlockMedia::LoadMeta(PasswordGetter, Errors)) {
		goto err;
	}
	bRet = TRUE;
err:
	if (pFile != NULL) {
		fclose(pFile);
		pFile = NULL;
	}
	if (row_buffer != NULL) {
		ppm_freerow(row_buffer);
		row_buffer = NULL;
	}
	if (!bRet)
		CloseMedia();
	return bRet;
}

BOOL CPPMFileMedia::SaveMedia(LPCTSTR szFilePath, CProgressBase& Progress, CPackErrors& Errors)
{
	BOOL bRet = FALSE;
	FILE* pFile = NULL;
	pixval maxval = PPM_MAXMAXVAL;
	pixel* row_buffer = NULL;

	if (!CBytePerBlockMedia::SaveMeta(Errors)) {
		goto err;
	}

	Progress.Reset(IDS_FILL_EMPTY_SPACE);
	Progress.SetFullScale(GetMediaTotalBytes() - GetMediaUsedBytes());

	if (!CBytePerBlockMedia::FillEmptySpace(Progress, Errors)) {
		goto err;
	}

	// open file and write header
	if ((pFile = _tfsopen(szFilePath, _T("wb"), _SH_DENYNO)) == NULL) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, CPackUtils::GetLastStdError(errno));
		goto err;
	}

	pm_setusererrormsgfn(ErrorMessage);
	pm_setusererrorquitfn(ErrorQuit);

	m_bError = FALSE;
	try {
		ppm_writeppminit(pFile, m_nCols, m_nRows, maxval, m_nFormat == PPM_FORMAT);
	}
	catch (CException* e) {
		e->Delete();
	}
	if (m_bError) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}

	m_bError = FALSE;
	try {
		row_buffer = ppm_allocrow(m_nCols);
	}
	catch (CException* e) {
		e->Delete();
	}
	if (m_bError) {
		Errors.SetError(CPackErrors::PE_NOMEM);
		goto err;
	}

	Progress.Reset(IDS_WRITE_FILE);
	Progress.SetFullScale(m_nRows);
	m_bError = FALSE;
	pm_plain_output = 0;
	try {
		for (UINT i = 0; i < m_nRows; i++) {
			for (UINT j = 0; j < m_nCols; j++) {
				BYTE R, G, B;
				CPixelImageMedia::GetPixel(j, i, R, G, B);
				row_buffer[j].r = R;
				row_buffer[j].g = G;
				row_buffer[j].b = B;
			}
			ppm_writeppmrow(pFile, row_buffer, m_nCols, maxval, m_nFormat == PPM_FORMAT);
			Progress.Increase(1);
			if (Progress.IsCanceled(Errors)) {
				goto err;
			}
		}
	}
	catch (CException* e) {
		e->Delete();
	}

	if (m_bError) {
		Errors.SetError(CPackErrors::PE_IO, szFilePath, m_strLastError);
		goto err;
	}
	// done!
	ClearMediaDirty();

	bRet = TRUE;
err:
	if (pFile != NULL) {
		fclose(pFile);
		pFile = NULL;
	}
	if (row_buffer != NULL) {
		ppm_freerow(row_buffer);
		row_buffer = NULL;
	}
	return bRet;
}

void CPPMFileMedia::CloseMedia()
{
	CPixelImageMedia::Free();
	m_nCols = m_nRows = m_nFormat = 0;
}

void CPPMFileMedia::AddOptPage(CMFCPropertySheet* pPropertySheet)
{
	m_OptPagePPMFile.m_strPPMImageWidth.Format(_T("%d"), m_nCols);
	m_OptPagePPMFile.m_strPPMImageHeigth.Format(_T("%d"), m_nRows);
	m_OptPagePPMFile.m_strPPMImageFormat.Format(_T("%s"), m_nFormat == PPM_FORMAT ? _T("plain") : _T("raw"));
	pPropertySheet->AddPage(&m_OptPagePPMFile);
	CPixelImageMedia::AddOptPage(pPropertySheet);
}

BOOL CPPMFileMedia::UpdateOpts(CMFCPropertySheet* pPropertySheet)
{
	return CPixelImageMedia::UpdateOpts(pPropertySheet);
}


LPCTSTR CPPMFileMedia::GetName()
{
	return m_szName;
}

LPCTSTR* CPPMFileMedia::GetExtTable()
{
	return m_szExtTable;
}

CMediaBase* CPPMFileMedia::Factory()
{
	return new(std::nothrow) CPPMFileMedia();
}

LPCTSTR CPPMFileMedia::m_szExtTable[] = {
	_T("ppm"),
	NULL
};
LPCTSTR CPPMFileMedia::m_szName = _T("Portable PixMap");
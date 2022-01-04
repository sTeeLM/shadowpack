#include "pch.h"
#include "resource.h"
#include "MediaBase.h"

BOOL CMediaBase::ShowMediaOptionDlg(CPackErrors& Errors)
{
	CMediaOptionDlg* pDlg = new CMediaOptionDlg();
	BOOL bRet;
	CString strText;
	if (pDlg) {
		AddOptPage(pDlg);
		strText.LoadString(IDS_OPTIONS);
		pDlg->SetTitle(strText);
		if (pDlg->DoModal() == IDOK) {
			if (UpdateOpts(pDlg)) {
				SetMediaDirty();
			}
		}
		delete pDlg;
		pDlg = NULL;
		bRet = TRUE;
	} else {
		Errors.SetError(CPackErrors::PE_INTERNAL);
		bRet = FALSE;
	}
	return bRet;
}

BOOL CMediaBase::TestHeaderValid(const MEDIA_HEADER_T* pHeader)
{
	if (pHeader) {
		if (pHeader->dwSign == MEDIA_HEADER_SIGN) {
			if (pHeader->dwHeaderSize >= sizeof(MEDIA_HEADER_T)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

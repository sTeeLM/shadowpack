#include "pch.h"
#include "MediaBase.h"

BOOL CMediaBase::ShowMediaOptionDlg(CPackErrors& Errors)
{
	CMediaOptionDlg* pDlg = new CMediaOptionDlg();
	BOOL bRet;
	if (pDlg) {
		AddOptPage(pDlg);
		pDlg->SetTitle(_T("ÉèÖÃ"));
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

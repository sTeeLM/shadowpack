#include "pch.h"
#include "MediaBase.h"

void CMediaBase::ShowMediaOptionDlg()
{
	m_pOptDlg = new CMediaOptionDlg();
	if (m_pOptDlg) {
		AddOptPage(m_pOptDlg);
		m_pOptDlg->SetTitle(_T("ÉèÖÃ"));
		if(m_pOptDlg->DoModal() == IDOK) {
			UpdateOpts(m_pOptDlg);
		}
		delete m_pOptDlg;
		m_pOptDlg = NULL;
	}
}

#include "pch.h"
#include "MediaBase.h"

void CMediaBase::ShowMediaOptionDlg()
{
	AddOptPage();
	m_OptDlg.SetTitle(_T("����"));
	m_OptDlg.DoModal();
}

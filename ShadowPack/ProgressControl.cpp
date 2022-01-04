// ProgressControl.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "ProgressControl.h"


// CProgressControl

IMPLEMENT_DYNAMIC(CProgressControl, CProgressCtrl)

CProgressControl::CProgressControl() :
	m_nFullScale(0),
	m_nCurrent(0),
	m_bCancel(FALSE)
{

}

CProgressControl::~CProgressControl()
{
}

void CProgressControl::Reset(UINT nIDS /* = 0*/)
{
	m_nCurrent = m_nFullScale = 0;
	m_bCancel = FALSE;
	CString strText;
	CProgressCtrl::SetPos(0);
	if (nIDS) {
		strText.LoadString(nIDS);
		m_ctlFooterInfo.SetWindowText((LPCTSTR)strText);
	} else {
		m_ctlFooterInfo.SetWindowText(_T(""));
	}
}

void CProgressControl::Show(BOOL bShow)
{
	ShowWindow(bShow);
	m_ctlFooterInfo.ShowWindow(bShow);
}

void CProgressControl::Initialize(CWnd* pParent, UINT nID, UINT nInfoID)
{
	SubclassDlgItem(nID, pParent);
	m_ctlFooterInfo.SubclassDlgItem(nInfoID, pParent);
}

void CProgressControl::SetFullScale(ULONGLONG nVal)
{
	TRACE("SetFullScale %ld\n", nVal);
	m_nFullScale = nVal;
}

void CProgressControl::Increase(ULONGLONG nVal)
{
	INT nProgressOld, nProgressNew;

	if ((nProgressOld = (INT)(m_nCurrent * 100 / m_nFullScale)) > 100) {
		TRACE(_T("PROGRESS  %d > 100\n"), nProgressOld);
		nProgressOld = 100;
	}
	m_nCurrent += nVal;
	if ((nProgressNew = (INT)(m_nCurrent * 100 / m_nFullScale)) > 100) {
		TRACE(_T("PROGRESS  %d > 100\n"), nProgressNew);
		nProgressNew = 100;
	}

	if (nProgressNew != nProgressOld) {
		CProgressCtrl::SetPos(nProgressNew);
		TRACE("Progress %d\n", nProgressNew);
	}
}

void CProgressControl::SetScale(ULONGLONG nVal)
{
	INT nProgressOld, nProgressNew;

	if ((nProgressOld = (INT)(m_nCurrent * 100 / m_nFullScale)) > 100) {
		TRACE(_T("PROGRESS  %d > 100\n"), nProgressOld);
		nProgressOld = 100;
	}
	m_nCurrent = nVal;
	if ((nProgressNew = (INT)(m_nCurrent * 100 / m_nFullScale)) > 100) {
		TRACE(_T("PROGRESS  %d > 100\n"), nProgressNew);
		nProgressNew = 100;
	}

	if (nProgressNew != nProgressOld) {
		CProgressCtrl::SetPos(nProgressNew);
		TRACE("Progress %d\n", nProgressNew);
	}
}
void CProgressControl::Cancel(void)
{
	m_bCancel = TRUE;
}

BOOL CProgressControl::IsCanceled(CPackErrors& Errors)
{
	Errors.SetError(CPackErrors::PE_CANCELED);
	return m_bCancel;
}

BEGIN_MESSAGE_MAP(CProgressControl, CProgressCtrl)
END_MESSAGE_MAP()



// CProgressControl 消息处理程序



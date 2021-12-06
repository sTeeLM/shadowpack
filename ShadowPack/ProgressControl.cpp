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

	nProgressOld = (INT)(m_nCurrent * 100 / m_nFullScale);
	m_nCurrent += nVal;
	nProgressNew = (INT)(m_nCurrent * 100 / m_nFullScale);

	if (nProgressNew != nProgressOld) {
		CProgressCtrl::SetPos(nProgressNew);
	}

	TRACE("Increase %d, progress %ld\n", nVal, nProgressNew);
}

void CProgressControl::Cancel(void)
{
	m_bCancel = TRUE;
}

BOOL CProgressControl::IsCanceled(void)
{
	return m_bCancel;
}

BEGIN_MESSAGE_MAP(CProgressControl, CProgressCtrl)
END_MESSAGE_MAP()



// CProgressControl 消息处理程序



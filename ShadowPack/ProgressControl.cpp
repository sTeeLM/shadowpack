// ProgressControl.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "ProgressControl.h"
#include "PackUtils.h"


// CProgressControl

IMPLEMENT_DYNAMIC(CProgressControl, CProgressCtrl)

CProgressControl::CProgressControl() :
	m_nFullScale(0),
	m_nCurrent(0),
	m_bCancel(FALSE),
	m_bShowETA(FALSE),
	m_bFirstSet(FALSE),
	m_prgsColor(RGB(0,0,255)),
	m_freeColor(RGB(255,255,255)),
	m_prgsTextColor(RGB(255,255,255)),
	m_freeTextColor(RGB(0, 0, 255))
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
	CProgressCtrl::SetRange(0, 100);
	CProgressCtrl::RedrawWindow();
	if (nIDS) {
		strText.LoadString(nIDS);
		m_ctlFooterInfo.SetWindowText((LPCTSTR)strText);
	} else {
		m_ctlFooterInfo.SetWindowText(_T(""));
	}
	m_StartTime = CTime::GetCurrentTime();
	m_CurrentTime = CTime::GetCurrentTime();
	m_bShowETA = FALSE;
	m_bFirstSet = FALSE;
}

void CProgressControl::Show(BOOL bShow)
{
	ShowWindow(bShow);
	m_ctlFooterInfo.ShowWindow(bShow);
	CProgressCtrl::RedrawWindow();
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
		if (!m_bFirstSet) {
			m_bFirstSet = TRUE;
			m_StartTime = CTime::GetCurrentTime();
		}
		m_CurrentTime = CTime::GetCurrentTime();
		CProgressCtrl::SetPos(nProgressNew);
		CProgressCtrl::RedrawWindow();
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
		if (!m_bFirstSet) {
			m_bFirstSet = TRUE;
			m_StartTime = CTime::GetCurrentTime();
		}
		m_CurrentTime = CTime::GetCurrentTime();
		CProgressCtrl::SetPos(nProgressNew);
		CProgressCtrl::RedrawWindow();
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
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CProgressControl 消息处理程序

void CProgressControl::OnPaint()
{
	//首先判断设置是否有效
	INT nPos, nUpper, nLower;
	CProgressCtrl::GetRange(nLower, nUpper);

	nPos = CProgressCtrl::GetPos();

	if (nLower >= nUpper) {
		return;
	}

	if (nPos > nUpper)nPos = nPos;
	if (nPos < nLower)nPos = nLower;

	CPaintDC dc(this); // device context for painting
	// 不为绘图消息调用 CProgressCtrl::OnPaint()


	//获取有效的进度条的位置和大小
	CRect LeftRect, RightRect, ClientRect;
	GetClientRect(ClientRect);
	LeftRect = RightRect = ClientRect;

	//计算显示进度的比例
	double Fraction = (double)(nPos - nLower) / ((double)(nUpper - nLower));

	//绘制整个进度条中的有效进度
	LeftRect.right = LeftRect.left + (int)((LeftRect.right - LeftRect.left) * Fraction);
	dc.FillSolidRect(LeftRect, m_prgsColor);

	//绘制剩余进度
	RightRect.left = LeftRect.right;
	dc.FillSolidRect(RightRect, m_freeColor);

	CTimeSpan  DiffTime = m_CurrentTime - m_StartTime;
	CString str;
	if ((nPos - nLower) != 0) {
		LONGLONG nTotalSec = DiffTime.GetTotalSeconds() * (double)(nUpper - nLower)  / (double)(nPos - nLower);
		LONGLONG nLeftSec = nTotalSec - DiffTime.GetTotalSeconds();
		TRACE(_T("Left %I64d sec\n"), nLeftSec);
		if (nTotalSec > 60 && !m_bShowETA) {
			m_bShowETA = TRUE;
		}
		if (m_bShowETA) {
			str.Format(_T("Percent: %d%%  ETA[D:H:M:S]: %s"), (int)(Fraction * 100.0),
				(LPCTSTR)CPackUtils::FormatSecond((double)nLeftSec));
		}
		else {
			str.Format(_T(" %d%%"), (int)(Fraction * 100.0));
		}
	}

	if (str.GetLength()) {
		//设置文字背景颜色为透明
		dc.SetBkMode(TRANSPARENT);

		//为了能够在进度和剩余进度中显示不同颜色的字体，需要分别设置两边的字体颜色并绘图

		CRgn rgn;
		rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, LeftRect.bottom);
		dc.SelectClipRgn(&rgn);
		dc.SetTextColor(m_prgsTextColor);
		dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		rgn.DeleteObject();
		rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, RightRect.bottom);
		dc.SelectClipRgn(&rgn);
		dc.SetTextColor(m_freeTextColor);
		dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

}

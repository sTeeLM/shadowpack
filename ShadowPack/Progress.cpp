#include "StdAfx.h"
#include "Progress.h"
#include "Resource.h"

CProgress::CProgress(void):
m_R(1.0), m_S(0.0), m_P(0),
m_nFullScale(0),
m_nCurrentPos(0),
m_nCurrentScale(0)
{
}

CProgress::~CProgress(void)
{
}

void CProgress::Initialize(CWnd * pParent)
{
	m_ctlProgress.SubclassDlgItem(IDC_PROGRESS, pParent);
	m_ctlProgress.SetPos(0);
	m_ctlInfo.SubclassDlgItem(IDC_INFO1, pParent);
	m_ctlInfo.SetWindowText(_T(""));
}

void CProgress::Reset()
{
	m_S = 0.0;
	m_R = 1.0;
	m_P = 0;
	while(!m_RangeStack.empty())
		m_RangeStack.pop();
	m_ctlProgress.SetRange(0, 100);
	m_ctlProgress.SetPos(0);
	m_nFullScale = 0;
	m_nCurrentPos = 0;
}

// push range
void CProgress::PushRange(DOUBLE m, DOUBLE n)
{
	DOUBLE newR, newS;
	ProgressRange range;

	if(n <= m || m > 100.0 || n < 0) {
		TRACE(_T("CProgress::PushRange: wrong range [%f,%f] !\n"), m, n);
		return;
	}

	newR = m_R * (n - m) / 100.0;
	newS = m_S + m_R * m;


	range.R = m_R;
	range.S = m_S;
	m_RangeStack.push(range);

	TRACE(_T("set new range [%f, %f], R(%f) -> (%f), new S(%f) -> (%f)\n"), m, n, m_R, newR, m_S, newS);

	m_R = newR;
	m_S = newS;
}

// update progress, 0 <= p <=100
void CProgress::UpdateProgress(DOUBLE p)
{
	INT nPercent;

	if(p < 0.0) p = 0;
	if(p > 100.0) p = 100.0;

	nPercent = (INT)(p * m_R + m_S);

	if(nPercent != m_P) {
		TRACE(_T("CProgress::UpdateProgress(%f)->%d\n"), p, nPercent);
		m_P = nPercent;
		m_ctlProgress.SetPos(nPercent);
	}
}

void CProgress::PopRange()
{
	ProgressRange range;
	if(!m_RangeStack.empty()) {
		range = m_RangeStack.top();
		TRACE(_T("restory R(%f) -> (%f), S(%f) -> (%f)\n"),range.R, m_R, range.S, m_S);
		m_R = range.R;
		m_S = range.S;
		m_RangeStack.pop();
	} else {
		TRACE(_T("CProgress::PopRange: empty stack!!!\n"));
	}
}

void CProgress::IncScale(ULONGLONG n)
{
	if(m_nFullScale == 0) {
		TRACE(_T("WARN: m_nFullScale = 0!\n"));
		return;
	}
	m_nCurrentScale += n;
	n = m_nCurrentScale * 100 / m_nFullScale;
	if(n > 100) n = 100;
	if(m_nCurrentPos != n) {
		m_ctlProgress.SetPos((INT)n);
		TRACE(_T("CProgress::IncScale %d\n"), n);
		m_nCurrentPos = (INT)n;
	}
}

void CProgress::SetInfo(LPCTSTR szInfo)
{
	m_ctlInfo.SetWindowText(szInfo);
}

void CProgress::ShowProgressBar(BOOL bShow /*= TRUE*/)
{
	m_ctlProgress.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

void CProgress::ShowInfoBar(BOOL bShow /*= TRUE*/)
{
	m_ctlInfo.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}
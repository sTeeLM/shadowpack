#pragma once

#include <stack>

#include "InfoBar.h"

class CProgress : public CObject
{
public:
	CProgress(void);
	virtual ~CProgress(void);
public:
	// for know scale of his self (f1 call f2 call f3...)
	// reset
	void Reset();
	void Initialize(CWnd * pParent);
	// push range
	void PushRange(DOUBLE m, DOUBLE n);
	// update progress, 0 <= p <=100
	void UpdateProgress(DOUBLE p); 
	void PopRange();

	// for not know scale of his self (dir walk...)
	void SetFullScale(ULONGLONG n){m_nFullScale = n; m_nCurrentPos = 0;m_nCurrentScale = 0;}
	void IncScale(ULONGLONG n); 

	void SetInfo(LPCTSTR szInfo);
	void ShowProgressBar(BOOL bShow = TRUE);
	void ShowInfoBar(BOOL bShow = TRUE);
private:

	CProgressCtrl m_ctlProgress;
	CInfoBar  m_ctlInfo;

	typedef struct {
		DOUBLE R;
		DOUBLE S;
	}ProgressRange;

	DOUBLE m_R;
	DOUBLE m_S;
	INT m_P;
	std::stack<ProgressRange> m_RangeStack;

	ULONGLONG m_nFullScale;
	ULONGLONG m_nCurrentScale;
	INT m_nCurrentPos;
};

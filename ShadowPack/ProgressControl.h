#pragma once

#include "ProgressBase.h"

// CProgressControl

class CProgressControl : public CProgressCtrl, public CProgressBase
{
	DECLARE_DYNAMIC(CProgressControl)

public:
	CProgressControl();
	virtual ~CProgressControl();

public:
	void Initialize(CWnd* pParent, UINT nID);

public:
	void SetFullScale(UINT nVal);
	void Increase(UINT nVal);
	void Cancel(void);
	BOOL IsCanceled(void);
	void Reset();

protected:
	DECLARE_MESSAGE_MAP()
};



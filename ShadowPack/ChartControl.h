#pragma once


// CPackChart

class CChartControl : public CStatic
{
	DECLARE_DYNAMIC(CChartControl)

public:
	CChartControl();
	virtual ~CChartControl();
	void SetFreePercent(INT nPercent, BOOL bEnable = TRUE);
	void Initialize(CWnd * pParent, UINT nID);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint( );

	INT m_nPercent;
	BOOL m_bEnable;
};



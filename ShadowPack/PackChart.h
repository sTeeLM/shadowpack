#pragma once


// CPackChart

class CPackChart : public CStatic
{
	DECLARE_DYNAMIC(CPackChart)

public:
	CPackChart();
	virtual ~CPackChart();
	void SetFreePercent(INT nPercent, BOOL bEnable = TRUE);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint( );

	INT m_nPercent;
	BOOL m_bEnable;
};



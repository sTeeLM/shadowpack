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
	void Initialize(CWnd* pParent, UINT nID, UINT nInfoID);

public:
	void SetFullScale(ULONGLONG nVal);
	void Increase(ULONGLONG nVal);
	void Cancel(void);
	BOOL IsCanceled(CPackErrors& Errors);
	void Reset(UINT nIDS = 0);
	void SetScale(ULONGLONG nVal);
	void Show(BOOL bShow = TRUE);

	afx_msg void OnPaint();
protected:
	DECLARE_MESSAGE_MAP()

	CStatic m_ctlFooterInfo;
	ULONGLONG m_nFullScale;
	ULONGLONG m_nCurrent;
	BOOL m_bCancel;
	BOOL m_bShowETA;
	BOOL m_bFirstSet;

	COLORREF m_prgsColor; //���������Ȳ�����ɫ
	COLORREF m_freeColor; //������������ಿ����ɫ
	COLORREF m_prgsTextColor; //���Ȳ���������ɫ
	COLORREF m_freeTextColor; //�հײ���������ɫ

	CTime m_StartTime;
	CTime m_CurrentTime;
};



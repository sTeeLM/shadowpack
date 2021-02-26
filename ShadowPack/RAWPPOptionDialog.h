#pragma once

#include "Resource.h"
#include "OptionDialog.h"

// CRAWPPOptionDialog 对话框

class CRAWPPOptionDialog : public COptionDialog
{
	DECLARE_DYNAMIC(CRAWPPOptionDialog)

public:
	CRAWPPOptionDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRAWPPOptionDialog();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_OPTION_RAWPP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nFormat;
	BOOL m_bEnable1PP;
	BOOL m_bEnable2PP;
	BOOL m_bEnable3PP;
};

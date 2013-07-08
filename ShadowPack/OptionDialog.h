#pragma once
#include "afxwin.h"


// COptionDialog 对话框

class COptionDialog : public CDialog
{
	DECLARE_DYNAMIC(COptionDialog)

public:
	COptionDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	int m_nFormat;
	BOOL m_bEncrypt;
	CString m_szPassword;
	BOOL m_bEnable1PP;
	BOOL m_bEnable2PP;
	BOOL m_bEnable3PP;
	BOOL m_bEnable1PJ;
	BOOL m_bEnable2PJ;
	BOOL m_bEnable4PJ;
protected:
	CString m_szPassword1;
public:
	afx_msg void OnBnClickedChkEncrypt();
	CButton m_ctlEncrypt;
	INT m_nEncryptMethod;
};

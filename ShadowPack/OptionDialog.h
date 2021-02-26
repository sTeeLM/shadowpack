#pragma once
#include "afxwin.h"


// COptionDialog 对话框

class COptionDialog : public CDialog
{
	DECLARE_DYNAMIC(COptionDialog)

public:
	COptionDialog(UINT IDD, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	BOOL m_bEncrypt;
	CString m_szPassword;
	CString m_szPassword1;
public:
	afx_msg void OnBnClickedChkEncrypt();
	CButton m_ctlEncrypt;
	INT m_nEncryptMethod;
};

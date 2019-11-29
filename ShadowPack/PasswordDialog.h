#pragma once
#include "afxwin.h"


// CPasswordDialog 对话框

class CPasswordDialog : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDialog)

public:
	CPasswordDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPasswordDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPassword;
	afx_msg void OnBnClickedOk();
};

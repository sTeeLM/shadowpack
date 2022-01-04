#pragma once

#include "PasswordGetterBase.h"
// CPasswordDlg 对话框

class CPasswordDlg : public CDialogEx, public CPasswordGetterBase
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPasswordDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PASSWORD };
#endif

public:
	CString GetPassword();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPassword;
};

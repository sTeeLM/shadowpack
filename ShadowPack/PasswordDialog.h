#pragma once
#include "afxwin.h"


// CPasswordDialog �Ի���

class CPasswordDialog : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDialog)

public:
	CPasswordDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPasswordDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPassword;
	afx_msg void OnBnClickedOk();
};

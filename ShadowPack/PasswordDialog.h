#pragma once
#include "afxwin.h"

#include "PasswordGetter.h"

// CPasswordDialog �Ի���

class CPasswordDialog : public CDialog,CPasswordGetter
{
	DECLARE_DYNAMIC(CPasswordDialog)

public:
	CPasswordDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPasswordDialog();

	CString GetPassword();

// �Ի�������
	enum { IDD = IDD_DIALOG_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPassword;
	afx_msg void OnBnClickedOk();
};

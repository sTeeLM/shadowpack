// PasswordDlg.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "PasswordDlg.h"
#include "afxdialogex.h"


// CPasswordDlg 对话框

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialogEx)

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PASSWORD, pParent)
	, m_strPassword(_T(""))
{

}

CPasswordDlg::~CPasswordDlg()
{
}

CString CPasswordDlg::GetPassword()
{
	if (DoModal() == IDOK) {
		return m_strPassword;
	} else {
		return _T("");
	}
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	if (pDX->m_bSaveAndValidate && m_strPassword.GetLength() == 0) {
		AfxMessageBox(IDS_PASSWORD_CAN_NOT_NULL);
		pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialogEx)
END_MESSAGE_MAP()


// CPasswordDlg 消息处理程序

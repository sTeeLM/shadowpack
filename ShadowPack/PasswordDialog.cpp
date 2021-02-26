// PasswordDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ShadowPack.h"
#include "PasswordDialog.h"


// CPasswordDialog 对话框

IMPLEMENT_DYNAMIC(CPasswordDialog, CDialog)

CPasswordDialog::CPasswordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDialog::IDD, pParent)
	, m_strPassword(_T(""))
{

}

CPasswordDialog::~CPasswordDialog()
{
}

void CPasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
}


BEGIN_MESSAGE_MAP(CPasswordDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CPasswordDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CPasswordDialog 消息处理程序

void CPasswordDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

CString CPasswordDialog::GetPassword()
{
	m_strPassword = _T("");
	if(DoModal() == IDOK)
		return m_strPassword;
	return _T("");
}
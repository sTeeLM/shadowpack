// PasswordDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ShadowPack.h"
#include "PasswordDialog.h"


// CPasswordDialog �Ի���

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


// CPasswordDialog ��Ϣ�������

void CPasswordDialog::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

CString CPasswordDialog::GetPassword()
{
	m_strPassword = _T("");
	if(DoModal() == IDOK)
		return m_strPassword;
	return _T("");
}
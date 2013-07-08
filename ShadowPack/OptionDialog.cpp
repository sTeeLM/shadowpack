// OptionDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ShadowPack.h"
#include "OptionDialog.h"


// COptionDialog 对话框

IMPLEMENT_DYNAMIC(COptionDialog, CDialog)

COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
	, m_nFormat(0)
	, m_bEncrypt(FALSE)
	, m_szPassword(_T(""))
	, m_szPassword1(_T(""))
	, m_nEncryptMethod(0)
	, m_bEnable1PP(FALSE)
	, m_bEnable2PP(FALSE)
	, m_bEnable3PP(FALSE)
	, m_bEnable1PJ(FALSE)
	, m_bEnable2PJ(FALSE)
	, m_bEnable4PJ(FALSE)
{

}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RTO_1PP, m_nFormat);
	DDX_Check(pDX, IDC_CHK_ENCRYPT, m_bEncrypt);
	DDX_Text(pDX, IDC_EDIT_PASSWORD1, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_szPassword1);
	DDX_Control(pDX, IDC_CHK_ENCRYPT, m_ctlEncrypt);
	DDX_CBIndex(pDX, IDC_COMBO_ENCRYPT_METHOD, m_nEncryptMethod);
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
	ON_BN_CLICKED(IDOK, &COptionDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COptionDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHK_ENCRYPT, &COptionDialog::OnBnClickedChkEncrypt)
END_MESSAGE_MAP()

BOOL COptionDialog::OnInitDialog()
{
	TRACE(_T("COptionDialog init \n"));
	m_szPassword1 = m_szPassword;
	GetDlgItem(IDC_COMBO_ENCRYPT_METHOD)->EnableWindow(m_bEncrypt);
	GetDlgItem(IDC_EDIT_PASSWORD1)->EnableWindow(m_bEncrypt);
	GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(m_bEncrypt);
	GetDlgItem(IDC_RTO_1PP)->EnableWindow(m_bEnable1PP);
	GetDlgItem(IDC_RTO_2PP)->EnableWindow(m_bEnable2PP);
	GetDlgItem(IDC_RTO_3PP)->EnableWindow(m_bEnable3PP);
	GetDlgItem(IDC_RTO_1PJ)->EnableWindow(m_bEnable1PJ);
	GetDlgItem(IDC_RTO_2PJ)->EnableWindow(m_bEnable2PJ);
	GetDlgItem(IDC_RTO_3PJ)->EnableWindow(m_bEnable4PJ);
	CDialog::OnInitDialog();
	return TRUE;
}

// COptionDialog 消息处理程序

void COptionDialog::OnBnClickedOk()
{
	UpdateData();
	if(m_bEncrypt && (m_szPassword1.IsEmpty() || m_szPassword.IsEmpty())) {
		AfxMessageBox(IDS_ERROR_EMPTY_PASSWORD);
	} else if(m_bEncrypt && m_szPassword1.Compare(m_szPassword) != 0) {
		AfxMessageBox(IDS_ERROR_MISMATCH_PASSWORD);
	} else {
		OnOK();
	}
}

void COptionDialog::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void COptionDialog::OnBnClickedChkEncrypt()
{
	GetDlgItem(IDC_COMBO_ENCRYPT_METHOD)->EnableWindow(m_ctlEncrypt.GetCheck());
	GetDlgItem(IDC_EDIT_PASSWORD1)->EnableWindow(m_ctlEncrypt.GetCheck());
	GetDlgItem(IDC_EDIT_PASSWORD2)->EnableWindow(m_ctlEncrypt.GetCheck());
}

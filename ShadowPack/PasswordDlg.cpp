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
{

}

CPasswordDlg::~CPasswordDlg()
{
}

CString CPasswordDlg::GetPassword()
{
	return CString();
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialogEx)
END_MESSAGE_MAP()


// CPasswordDlg 消息处理程序

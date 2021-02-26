// RAWPPOptionDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ShadowPack.h"
#include "RAWPPOptionDialog.h"


// CRAWPPOptionDialog 对话框

IMPLEMENT_DYNAMIC(CRAWPPOptionDialog, COptionDialog)

CRAWPPOptionDialog::CRAWPPOptionDialog(CWnd* pParent /*=NULL*/)
	: COptionDialog(CRAWPPOptionDialog::IDD, pParent)
	, m_nFormat(0)
	, m_bEnable1PP(FALSE)
	, m_bEnable2PP(FALSE)
	, m_bEnable3PP(FALSE)
{

}

CRAWPPOptionDialog::~CRAWPPOptionDialog()
{

}

BOOL CRAWPPOptionDialog::OnInitDialog()
{
	TRACE(_T("CRAWPPOptionDialog init \n"));

	GetDlgItem(IDC_RTO_1PP)->EnableWindow(m_bEnable1PP);
	GetDlgItem(IDC_RTO_2PP)->EnableWindow(m_bEnable2PP);
	GetDlgItem(IDC_RTO_3PP)->EnableWindow(m_bEnable3PP);

	COptionDialog::OnInitDialog();
	return TRUE;
}

void CRAWPPOptionDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RTO_1PP, m_nFormat);
}


BEGIN_MESSAGE_MAP(CRAWPPOptionDialog, COptionDialog)
END_MESSAGE_MAP()


// CRAWPPOptionDialog 消息处理程序

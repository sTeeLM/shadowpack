// PixelImageMediaOptionDlg.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "PixelImageMediaOptionDlg.h"
#include "afxdialogex.h"


// CPixelImageMediaOptionDlg 对话框

IMPLEMENT_DYNAMIC(CPixelImageMediaOptionDlg, CMFCPropertyPage)

CPixelImageMediaOptionDlg::CPixelImageMediaOptionDlg(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_PIXELMEDIA)
{

}

CPixelImageMediaOptionDlg::~CPixelImageMediaOptionDlg()
{
}

void CPixelImageMediaOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPixelImageMediaOptionDlg, CMFCPropertyPage)
END_MESSAGE_MAP()


// CPixelImageMediaOptionDlg 消息处理程序

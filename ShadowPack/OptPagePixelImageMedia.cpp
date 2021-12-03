// PixelImageMediaOptionDlg.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPagePixelImageMedia.h"
#include "afxdialogex.h"


// COptPagePixelImageMedia 对话框

IMPLEMENT_DYNAMIC(COptPagePixelImageMedia, CMFCPropertyPage)

COptPagePixelImageMedia::COptPagePixelImageMedia(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_PIXELMEDIA)
{

}

COptPagePixelImageMedia::~COptPagePixelImageMedia()
{
}

void COptPagePixelImageMedia::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptPagePixelImageMedia, CMFCPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_CRYPTO, &COptPagePixelImageMedia::OnCbnSelchangeComboCrypto)
END_MESSAGE_MAP()


// COptPagePixelImageMedia 消息处理程序


void COptPagePixelImageMedia::OnCbnSelchangeComboCrypto()
{
	// TODO: 在此添加控件通知处理程序代码
}

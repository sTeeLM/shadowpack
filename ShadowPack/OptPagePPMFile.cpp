// OptPagePPMFile.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPagePPMFile.h"
#include "afxdialogex.h"


// COptPagePPMFile 对话框

IMPLEMENT_DYNAMIC(COptPagePPMFile, CMFCPropertyPage)

COptPagePPMFile::COptPagePPMFile(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_PPM_FILE)
	, m_strPPMImageWidth(_T(""))
	, m_strPPMImageHeigth(_T(""))
	, m_strPPMImageFormat(_T(""))
{

}

COptPagePPMFile::~COptPagePPMFile()
{
}

void COptPagePPMFile::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PPM_IMAGE_WIDTH, m_strPPMImageWidth);
	DDX_Text(pDX, IDC_PPM_IMAGE_HEIGHT, m_strPPMImageHeigth);
	DDX_Text(pDX, IDC_PPM_IMAGE_FORMAT, m_strPPMImageFormat);
}


BEGIN_MESSAGE_MAP(COptPagePPMFile, CMFCPropertyPage)
END_MESSAGE_MAP()


// COptPagePPMFile 消息处理程序

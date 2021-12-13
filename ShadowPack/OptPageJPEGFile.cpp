// OptPageJPEGFile.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPageJPEGFile.h"
#include "afxdialogex.h"


// COptPageJPEGFile 对话框

IMPLEMENT_DYNAMIC(COptPageJPEGFile, CMFCPropertyPage)

COptPageJPEGFile::COptPageJPEGFile(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_JPEG_FILE)
{

}

COptPageJPEGFile::~COptPageJPEGFile()
{
}

void COptPageJPEGFile::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_JPEG_IMAGE_WIDTH, m_strImageWidth);
	DDX_Text(pDX, IDC_JPEG_IMAGE_HEIGHT, m_strImageHeight);
	DDX_Text(pDX, IDC_JPEG_JFIF_MAJOR, m_strJFIFMajor);
	DDX_Text(pDX, IDC_JPEG_JFIF_MINOR, m_strJFIFMinor);
	DDX_Text(pDX, IDC_JPEG_DENSITY_UNIT, m_strDensityUnit);
	DDX_Text(pDX, IDC_JPEG_NUM_COMP, m_strNumComp);
	DDX_Text(pDX, IDC_JPEG_COLOR_SPACE, m_strColorSpace);
	DDX_Text(pDX, IDC_JPEG_X_DENSITY, m_strXDensity);
	DDX_Text(pDX, IDC_JPEG_Y_DENSITY, m_strYDensity);
}


BEGIN_MESSAGE_MAP(COptPageJPEGFile, CMFCPropertyPage)
END_MESSAGE_MAP()


// COptPageJPEGFile 消息处理程序

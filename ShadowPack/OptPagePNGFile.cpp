// OptPagePNGFile.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPagePNGFile.h"
#include "afxdialogex.h"


// COptPagePNGFile 对话框

IMPLEMENT_DYNAMIC(COptPagePNGFile, CMFCPropertyPage)

COptPagePNGFile::COptPagePNGFile(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_PNG_FILE)
	, m_strWidth(_T(""))
	, m_strHeigth(_T(""))
	, m_strBitDepth(_T(""))
	, m_strColorType(_T(""))
	, m_strInterlaceType(_T(""))
	, m_strCompressionType(_T(""))
	, m_strFilterType(_T(""))
	, m_strChannels(_T(""))
{

}

COptPagePNGFile::~COptPagePNGFile()
{
}

void COptPagePNGFile::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PNG_WIDTH, m_strWidth);
	DDX_Text(pDX, IDC_PNG_HEIGHT, m_strHeigth);
	DDX_Text(pDX, IDC_PNG_BIT_DEPTH, m_strBitDepth);
	DDX_Text(pDX, IDC_PNG_COLOR_TYPE, m_strColorType);
	DDX_Text(pDX, IDC_PNG_INTERLACE_TYPE, m_strInterlaceType);
	DDX_Text(pDX, IDC_PNG_COMPRESSION_TYPE, m_strCompressionType);
	DDX_Text(pDX, IDC_PNG_FILTER_TYPE, m_strFilterType);
	DDX_Text(pDX, IDC_PNG_CHANNELS, m_strChannels);
}


BEGIN_MESSAGE_MAP(COptPagePNGFile, CMFCPropertyPage)
END_MESSAGE_MAP()


// COptPagePNGFile 消息处理程序

// OptPageBMPFile.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPageBMPFile.h"
#include "afxdialogex.h"


// COptPageBMPFile 对话框

IMPLEMENT_DYNAMIC(COptPageBMPFile, CMFCPropertyPage)

COptPageBMPFile::COptPageBMPFile(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_BMP_FILE)
{

}

COptPageBMPFile::~COptPageBMPFile()
{
}

void COptPageBMPFile::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BFSIZE, m_strbfSize);
	DDX_Text(pDX, IDC_BFOFFBITS, m_strbfOffBits);
	DDX_Text(pDX, IDC_BIWIDTH, m_strbiWidth);
	DDX_Text(pDX, IDC_BIHEIGTH, m_strbiHeight);
	DDX_Text(pDX, IDC_BIBITCOUNT, m_strbiBitCount);
	DDX_Text(pDX, IDC_BICOMPRESSION, m_strbiCompression);
	DDX_Text(pDX, IDC_BIXPELSPERMETER, m_strbiXPelsPerMeter);
	DDX_Text(pDX, IDC_BIYPELSPERMETER, m_strbiYPelsPerMeter);
}


BEGIN_MESSAGE_MAP(COptPageBMPFile, CMFCPropertyPage)

END_MESSAGE_MAP()

BOOL COptPageBMPFile::OnInitDialog()
{
	return CMFCPropertyPage::OnInitDialog();
}
// COptPageBMPFile 消息处理程序

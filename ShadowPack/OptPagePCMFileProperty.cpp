// COptPagePCMFileProperty.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPagePCMFileProperty.h"
#include "afxdialogex.h"


// COptPagePCMFileProperty 对话框

IMPLEMENT_DYNAMIC(COptPagePCMFileProperty, CMFCPropertyPage)

COptPagePCMFileProperty::COptPagePCMFileProperty(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_PCM_FILE)
	, m_strPCMProperty(_T(""))
{

}

COptPagePCMFileProperty::~COptPagePCMFileProperty()
{
}

void COptPagePCMFileProperty::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PCM_AUDIO_PROPERTY, m_strPCMProperty);
}


BEGIN_MESSAGE_MAP(COptPagePCMFileProperty, CMFCPropertyPage)
END_MESSAGE_MAP()


// COptPagePCMFileProperty 消息处理程序

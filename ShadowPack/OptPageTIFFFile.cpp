// OptPageTIFFFile.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPageTIFFFile.h"
#include "afxdialogex.h"


// COptPageTIFFFile 对话框

IMPLEMENT_DYNAMIC(COptPageTIFFFile, CMFCPropertyPage)

COptPageTIFFFile::COptPageTIFFFile(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_TIFF_FILE)
	, m_strImageLength(_T(""))
	, m_strImageWidth(_T(""))
	, m_strCompression(_T(""))
	, m_strSamplesPerPixel(_T(""))
	, m_strBitsPerSample(_T(""))
	, m_strPhotoMetric(_T(""))
	, m_strPlanarConfig(_T(""))
	, m_strXResolution(_T(""))
	, m_strYResolution(_T(""))
	, m_strResolutionUnit(_T(""))
	, m_strExtraSamples(_T(""))
{

}

COptPageTIFFFile::~COptPageTIFFFile()
{
}

void COptPageTIFFFile::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_IMAGELENGTH, m_strImageLength);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_IMAGEWIDTH, m_strImageWidth);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_COMPRESSION, m_strCompression);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_SAMPLESPERPIXEL, m_strSamplesPerPixel);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_BITSPERSAMPLE, m_strBitsPerSample);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_PHOTOMETRIC, m_strPhotoMetric);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_PLANARCONFIG, m_strPlanarConfig);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_XRESOLUTION, m_strXResolution);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_YRESOLUTION, m_strYResolution);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_RESOLUTIONUNIT, m_strResolutionUnit);
	DDX_Text(pDX, IDC_TIFF_TIFFTAG_EXTRASAMPLES, m_strExtraSamples);
}


BEGIN_MESSAGE_MAP(COptPageTIFFFile, CMFCPropertyPage)
END_MESSAGE_MAP()


// COptPageTIFFFile 消息处理程序

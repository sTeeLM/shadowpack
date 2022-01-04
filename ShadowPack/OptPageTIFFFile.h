#pragma once


// COptPageTIFFFile 对话框

class COptPageTIFFFile : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPageTIFFFile)

public:
	COptPageTIFFFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPageTIFFFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_TIFF_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strImageLength;
	CString m_strImageWidth;
	CString m_strCompression;
	CString m_strSamplesPerPixel;
	CString m_strBitsPerSample;
	CString m_strPhotoMetric;
	CString m_strPlanarConfig;
	CString m_strXResolution;
	CString m_strYResolution;
	CString m_strResolutionUnit;
	CString m_strExtraSamples;
};

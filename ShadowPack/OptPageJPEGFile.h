#pragma once


// COptPageJPEGFile 对话框

class COptPageJPEGFile : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPageJPEGFile)

public:
	COptPageJPEGFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPageJPEGFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_JPG_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strImageWidth;
	CString m_strImageHeight;
	CString m_strJFIFMajor;
	CString m_strJFIFMinor;
	CString m_strDensityUnit;
	CString m_strNumComp;
	CString m_strColorSpace;
	CString m_strXDensity;
	CString m_strYDensity;
};

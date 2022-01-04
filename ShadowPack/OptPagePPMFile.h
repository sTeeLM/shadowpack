#pragma once


// COptPagePPMFile 对话框

class COptPagePPMFile : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPagePPMFile)

public:
	COptPagePPMFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPagePPMFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_PPM_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPPMImageWidth;
	CString m_strPPMImageHeigth;
	CString m_strPPMImageFormat;
};

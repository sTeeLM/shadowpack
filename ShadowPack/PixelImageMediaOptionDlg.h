#pragma once


// CPixelImageMediaOptionDlg 对话框

class CPixelImageMediaOptionDlg : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPixelImageMediaOptionDlg)

public:
	CPixelImageMediaOptionDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPixelImageMediaOptionDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_PIXELMEDIA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

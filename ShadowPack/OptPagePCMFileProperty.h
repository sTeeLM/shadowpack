#pragma once


// COptPagePCMFileProperty 对话框

class COptPagePCMFileProperty : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPagePCMFileProperty)

public:
	COptPagePCMFileProperty(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPagePCMFileProperty();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_PCM_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPCMProperty;
};

#pragma once


// COptPageBMPFile 对话框

class COptPageBMPFile : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPageBMPFile)

public:
	COptPageBMPFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPageBMPFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_BMP_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strbfSize;
	CString m_strbfOffBits;
	CString m_strbiWidth;
	CString m_strbiHeight;
	CString m_strbiBitCount;
	CString m_strbiCompression;
	CString m_strbiXPelsPerMeter;
	CString m_strbiYPelsPerMeter;
};

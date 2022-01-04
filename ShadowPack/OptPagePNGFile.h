#pragma once


// COptPagePNGFile 对话框

class COptPagePNGFile : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPagePNGFile)

public:
	COptPagePNGFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPagePNGFile();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_PNG_FILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strWidth;
	CString m_strHeigth;
	CString m_strBitDepth;
	CString m_strColorType;
	CString m_strInterlaceType;
	CString m_strCompressionType;
	CString m_strFilterType;
	CString m_strChannels;
};

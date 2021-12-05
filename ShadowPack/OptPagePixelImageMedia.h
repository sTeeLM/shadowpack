#pragma once


// CPixelImageMediaOptionDlg 对话框

class COptPagePixelImageMedia : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPagePixelImageMedia)

public:
	COptPagePixelImageMedia(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPagePixelImageMedia();
	BOOL OnInitDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_PIXELMEDIA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboCrypto();
	CString m_strPasswd1;
	CString m_strPasswd2;
	CComboBox m_ctlCrypto;
	INT m_nCrypto;
	INT m_nBytePerPixel;
	UINT m_nTotalBlocks;
	UINT m_nUsedBytes;
protected:
	void SetBPPInfo();
public:
	afx_msg void OnBnClickedRadio1bp4p();
	afx_msg void OnBnClickedRadio1bp3p();
	afx_msg void OnBnClickedRadio1bp2p();
	afx_msg void OnBnClickedRadio1bp1p();
};

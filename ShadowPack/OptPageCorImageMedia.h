#pragma once


// COptPageCorImageMedia 对话框

class COptPageCorImageMedia : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPageCorImageMedia)

public:
	COptPageCorImageMedia(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COptPageCorImageMedia();
	BOOL OnInitDialog();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_CORMEDIA };
#endif

public:
	CString m_strPasswd1;
	CString m_strPasswd2;
	CComboBox m_ctlCrypto;
	INT m_nCrypto;
	INT m_nBytePer2Cor;
	UINT m_nTotalBlocks;
	UINT m_nUsedBytes;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	void SetBPCInfo();
public:
	afx_msg void OnCbnSelchangeComboCrypto();
	afx_msg void OnBnClickedRadio1bp8c();
	afx_msg void OnBnClickedRadio1bp6c();
	afx_msg void OnBnClickedRadio1bp4c();
	afx_msg void OnBnClickedRadio1bp2c();
};

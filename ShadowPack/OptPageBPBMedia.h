#pragma once


// COptPageBPBMedia 对话框

class COptPageBPBMedia : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(COptPageBPBMedia)

public:
	COptPageBPBMedia(CWnd* pParent = nullptr, UINT nIDCaption = 0);   // 标准构造函数
	virtual ~COptPageBPBMedia();
	BOOL OnInitDialog();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_BYTE_PER_BLOCK_MEDIA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPasswd1;
	CString m_strPasswd2;
	CComboBox m_ctlCrypto;
	INT m_nCrypto;
	INT m_nBytePerBlock;
	ULONGLONG m_nTotalBlocks;
	ULONGLONG m_nUsedBytes;
	UINT m_nHeaderSize;
	CString m_strBlockUnit;
protected:
	void SetBPBInfo();
	afx_msg void OnCbnSelchangeComboCrypto();
	afx_msg void OnBnClickedRadio1bp4b();
	afx_msg void OnBnClickedRadio1bp3b();
	afx_msg void OnBnClickedRadio1bp2b();
	afx_msg void OnBnClickedRadio1bp1b();
};

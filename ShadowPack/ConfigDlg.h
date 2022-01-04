#pragma once


// CConfigDlg 对话框

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CConfigDlg();

	BOOL OnInitDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIGBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnSelMediaHdCache();
	afx_msg void OnBnClickedBtnSelPackHdCache();
	afx_msg void OnBnClickedCheckMediaHdCache();
	afx_msg void OnBnClickedCheckPackHdCache();
	afx_msg void OnBnClickedCheckMediaHdCacheUseTmp();
	afx_msg void OnBnClickedCheckPackHdCacheUseTmp();

public:
	CComboBox m_ctlComboLang;
	BOOL m_bMediaHDCacheEnable;
	BOOL m_bPackHDCacheEnable;
	BOOL m_bMediaHDCacheUseTmp;
	BOOL m_bPackHDCacheUseTmp;
	UINT m_nCurrentLocale;
	CString m_strMediaCacheTmpDir;
	CString m_strMediaCacheCustomDir;
	CString m_strPackCacheTmpDir;
	CString m_strPackCacheCustomDir;
protected:
	void UpdateUI();
};

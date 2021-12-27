
// ShadowPackDlg.h: 头文件
//

#pragma once

#include "ChartControl.h"
#include "ProgressControl.h"
#include "PackManager.h"
#include "PasswordDlg.h"

// CShadowPackDlg 对话框
class CShadowPackDlg : public CDialog
{
// 构造
public:
	CShadowPackDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHADOWPACK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnMediaOpen();
	afx_msg void OnBnClickedBtnMediaClose();
	afx_msg void OnBnClickedBtnMediaSave();
	afx_msg void OnBnClickedBtnMediaOption();
	afx_msg void OnBnClickedBtnItemExport();
	afx_msg void OnBnClickedBtnItemAdd();
	afx_msg void OnBnClickedBtnItemClearAll();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnItemDelete();
	afx_msg void OnLvnItemChangedListData(NMHDR* pNMHDR, LRESULT* pResult);
public:
	CChartControl m_ctlCapicityChart;
	CStatic m_ctlCapicityInfo;
	CProgressControl m_ctlProgress;
	CPackManager m_ctlFileManager;
	CPasswordDlg m_dlgPassword;

public:
	CString m_szMediaPathName;
	CString m_szItemPathName;
	BOOL m_bCloseOnSave;
	BOOL m_bInProgress;
	BOOL m_bQuitOnClose;

protected:
	typedef void (CShadowPackDlg::* FN_PACK_THREAD) ();
	class CPackThreadParam
	{
	public:
		CPackThreadParam(CShadowPackDlg* pThis, FN_PACK_THREAD pFn):
			m_pThis(pThis),
			m_pFn(pFn)
		{}
		virtual ~CPackThreadParam() {}
	public:
		CShadowPackDlg* m_pThis;
		FN_PACK_THREAD  m_pFn;
	};
	void StartThread(FN_PACK_THREAD fn);
	static UINT __cdecl fnThread(LPVOID p);
protected:
	void UpdateUI();
	void ThreadOpenMedia();
	void ThreadSaveMedia();
	void ThreadExportItem();
	void ThreadAddItem();
	BOOL ShowConfigDlg();

};

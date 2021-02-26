
// ShadowPackDlg.h : 头文件
//

#pragma once

#include "PackChart.h"
#include "Progress.h"
#include "PackItemList.h"
#include "PackItem.h"
#include "Pack.h"
#include "Media.h"
#include "PackErrors.h"
#include "PasswordDialog.h"

// CShadowPackDlg 对话框
class CShadowPackDlg : public CDialog
{
// 构造
public:
	CShadowPackDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SHADOWPACK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
public:
	static UINT __cdecl fnThread( LPVOID pParam );
	static CString fnGetPassword();
	
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

typedef void (CShadowPackDlg::*FN_PACK_THREAD) (LPVOID pParam);

	class CPackThreadParam {
	public:
		CPackThreadParam() {};
		virtual ~CPackThreadParam() {};
	public:
		CShadowPackDlg * m_pThis;
		FN_PACK_THREAD m_pfn;
		LPVOID m_pParam;
	};

protected:
	CPasswordDialog m_dlgPassword;
	CPackChart m_ctlImageQuota;
	CProgress m_Progress;
	CStatic   m_ctlInfo2;
	CPackItemList m_ctlPackItemList;
	CPack m_PackRoot;
	BOOL m_bInProgress;
	BOOL m_bShowProgressBar;
	BOOL m_bQuit;
	BOOL m_bCancel;

	// cache...
	CString m_szPathName;
	CString m_szFileExt;

#ifdef _DEBUG
	CMemoryState m_msOld;
#endif


protected:
	void Quit();
	void UpdateUI();
	void CloseMedia();
	BOOL ShowLocationDirDlg(CString & strDir);
	void StartThread(FN_PACK_THREAD fn, LPVOID pParam = NULL, BOOL bShowProgressBar = TRUE);
	void ThreadSaveMedia(LPVOID pParam);
	void ThreadOpenMedia(LPVOID pParam);
	void ThreadCloseMedia(LPVOID pParam);
	void ThreadClearItem(LPVOID pParam);
	void ThreadExportItem(LPVOID pParam);
	void ThreadDeleteItem(LPVOID pParam);
	void ThreadAddItemDir(LPVOID pParam);
	void ThreadAddItemFile(LPVOID pParam);
	

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedItemDelete();
	afx_msg void OnBnClickedMediaOpen();
	afx_msg void OnBnClickedMediaClose();
	afx_msg void OnBnClickedMediaOption();
	afx_msg void OnBnClickedMediaSave();
	afx_msg void OnBnClickedItemExport();
	afx_msg void OnBnClickedItemAddDir();
	afx_msg void OnBnClickedItemAddFile();
	afx_msg void OnBnClickedItemClear();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnLvnItemChangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult);
};

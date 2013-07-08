// ShadowPackDlg.h : 头文件
//

#pragma once

#include "Pack.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "PackChart.h"

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
	static CString & fnGetPassword();
	static void fnSetProgress(INT nPercent);
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose( );
	DECLARE_MESSAGE_MAP()

	CPack * m_pPack;
	CString m_szPathName;
	CString m_szFileExt;
	BOOL AddListItem(UINT nID, LPCTSTR szName, UINT nSize);
	BOOL RemoveListItem(UINT nID, UINT nCount);
	BOOL ClearListItem();
	void TranslateSize(UINT nSize, CString & strOut);
	static UINT LoadImage(LPVOID pParam);
	static UINT SaveImage(LPVOID pParam);
	void EndSave(BOOL bOK, CString & szError);
	void EndLoad(BOOL bOK, CString & szError);
	void BeginSave();
	void BeginLoad();
	void BeginLoadSave();
	void UpdateUI();
	void CloseImage();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnLoadImage();
	afx_msg void OnBnClickedBtnSaveImage();
	afx_msg void OnBnClickedBtnOption();
	afx_msg void OnBnClickedBtnAddData();
	afx_msg void OnBnClickedBtnRemoveData();
	afx_msg void OnBnClickedBtnClearData();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnLbnSelchangeLstData();
	afx_msg void OnLvnItemchangedLstData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnExportData();
	afx_msg void OnBnClickedBtnCancel();

	CPackChart m_ctlImageQuota;
	CListCtrl m_ctlPackItemList;
	static CString m_szPackPassword;
	static BOOL m_bCancel;
	BOOL m_bInProgress;
	BOOL m_bQuit;
	BOOL m_bCloseImage;
	CProgressCtrl m_ctlProgress;
	CStatic m_ctlInfo1;
	CStatic m_ctlInfo2;
};

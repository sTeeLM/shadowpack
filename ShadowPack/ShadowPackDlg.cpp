﻿
// ShadowPackDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ShadowPack.h"
#include "ShadowPackDlg.h"
#include "afxdialogex.h"
#include "MediaFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CShadowPackDlg 对话框



CShadowPackDlg::CShadowPackDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SHADOWPACK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShadowPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO_CAPICITY, m_ctlCapicityInfo);
	DDX_Control(pDX, IDC_INFO_FOOTER, m_ctlFooterInfo);
}

BEGIN_MESSAGE_MAP(CShadowPackDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_MEDIA_OPEN, &CShadowPackDlg::OnBnClickedBtnMediaOpen)
	ON_BN_CLICKED(IDC_BTN_MEDIA_CLOSE, &CShadowPackDlg::OnBnClickedBtnMediaClose)
	ON_BN_CLICKED(IDC_BTN_MEDIA_SAVE, &CShadowPackDlg::OnBnClickedBtnMediaSave)
	ON_BN_CLICKED(IDC_BTN_MEDIA_OPTION, &CShadowPackDlg::OnBnClickedBtnMediaOption)
	ON_BN_CLICKED(IDC_BTN_ITEM_EXPORT, &CShadowPackDlg::OnBnClickedBtnItemExport)
	ON_BN_CLICKED(IDC_BTN_ITEM_ADD, &CShadowPackDlg::OnBnClickedBtnItemAdd)
	ON_BN_CLICKED(IDC_BTN_ITEM_CLEAR_ALL, &CShadowPackDlg::OnBnClickedBtnItemClearAll)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CShadowPackDlg::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_ITEM_DELETE, &CShadowPackDlg::OnBnClickedBtnItemDelete)
END_MESSAGE_MAP()


// CShadowPackDlg 消息处理程序

BOOL CShadowPackDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	m_ctlCapicityChart.Initialize(this, IDC_IMAGE_CAPICITY);
	m_ctlProgress.Initialize(this, IDC_PROGRESS);
	m_ctlFileManager.Initialize(this, IDC_LIST_DATA);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShadowPackDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CShadowPackDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CShadowPackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


UINT __cdecl CShadowPackDlg::fnThread(LPVOID p)
{
	CPackThreadParam* pParam = (CPackThreadParam*)p;
	(pParam->m_pThis->*(pParam->m_pFn))();
	delete pParam;
	return 0;
}

void CShadowPackDlg::StartThread(FN_PACK_THREAD fn)
{
	CPackThreadParam* pParam = new CPackThreadParam(this, fn);
	AfxBeginThread(fnThread, pParam);
}

BOOL CShadowPackDlg::ShowLocationDirDlg(CString& strDir)
{
	// get dir location
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = GetSafeHwnd();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH * 2];
	szFolder[0] = 0;
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
			bRet = TRUE;
		IMalloc* pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
	strDir = szFolder;
	return bRet;
}

void CShadowPackDlg::ThreadOpenMedia()
{
	CPackErrors Errors;
	if (!m_ctlFileManager.LoadMedia(m_ctlProgress, m_dlgPassword, Errors)) {
		AfxMessageBox(Errors.ToString());
		m_ctlFileManager.DettachMedia();
	}
}


void CShadowPackDlg::OnBnClickedBtnMediaOpen()
{
	CString szFilter = CMediaFactory::CreateExtTable();
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		szFilter, NULL, 0, TRUE);
	if (!m_ctlFileManager.MediaAttached()) {
		if (dlg.DoModal() == IDOK) {
			m_szMediaPathName = dlg.GetPathName();
			m_pMedia = CMediaFactory::CreateMediaFromExt(dlg.GetFileExt());
			m_ctlProgress.Reset();
			if(m_pMedia) {
				m_ctlFileManager.AttachMedia(m_pMedia);
				StartThread(&CShadowPackDlg::ThreadOpenMedia);
			} 
			else
			{
				CPackErrors Errors;
				AfxMessageBox(Errors.ToString());
			}
		}
	}
}


void CShadowPackDlg::OnBnClickedBtnMediaClose()
{
	if (m_ctlFileManager.MediaAttached()) {
		if (m_ctlFileManager.IsDirty()) {
			if (AfxMessageBox(_T("Save?"), MB_YESNO) == IDYES) {
				m_bCloseOnSave = TRUE;
				OnBnClickedBtnMediaSave();
			} else {
				m_ctlFileManager.DettachMedia();
			}
		} else {
			m_ctlFileManager.DettachMedia();
		}
	}
}

void CShadowPackDlg::ThreadSaveMedia()
{
	CPackErrors Errors;
	if (!m_ctlFileManager.Save(m_ctlProgress, m_dlgPassword, Errors)) {
		AfxMessageBox(Errors.ToString());
	} else {
		if (m_bCloseOnSave) {
			m_ctlFileManager.DettachMedia();
			m_bCloseOnSave = FALSE;
		}
	}
}

void CShadowPackDlg::OnBnClickedBtnMediaSave()
{
	if (m_ctlFileManager.MediaAttached() && m_ctlFileManager.IsDirty()) {
		m_ctlProgress.Reset();
		StartThread(&CShadowPackDlg::ThreadSaveMedia);
	}
}


void CShadowPackDlg::OnBnClickedBtnMediaOption()
{
	if(m_ctlFileManager.MediaAttached())
		m_ctlFileManager.GetMedia()->ShowOptionDlg();
}

void CShadowPackDlg::ThreadExportItem()
{
	CPackErrors Errors;
	if (m_ctlFileManager.GetSelectedItemCnt() == 1) {
		if (!m_ctlFileManager.ExportFirstSelectedItemToFile(m_szItemPathName, m_ctlProgress, Errors)) {
			AfxMessageBox(Errors.ToString());
		}
	} else {
		if (!m_ctlFileManager.ExportSelectedItemsToDir(m_szItemPathName, m_ctlProgress, Errors)) {
			AfxMessageBox(Errors.ToString());
		}
	}

}

void CShadowPackDlg::OnBnClickedBtnItemExport()
{
	if (m_ctlFileManager.MediaAttached() && m_ctlFileManager.GetSelectedItemCnt()) {
		m_ctlProgress.Reset();
		if (m_ctlFileManager.GetSelectedItemCnt() == 1) {
			CFileDialog dlg(FALSE, NULL, m_ctlFileManager.GetFirstSelectedItemName(),
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
			if (dlg.DoModal() == IDOK) {
				m_szItemPathName = dlg.GetPathName();
				StartThread(&CShadowPackDlg::ThreadExportItem);
			}
		} else {
			if (ShowLocationDirDlg(m_szItemPathName)) {
				StartThread(&CShadowPackDlg::ThreadExportItem);
			}
		}
		
	}
}

void CShadowPackDlg::ThreadAddItem()
{
	CPackErrors Errors;
	if (!m_ctlFileManager.AddItemFromFile(m_szItemPathName, m_ctlProgress, Errors)) {
		AfxMessageBox(Errors.ToString());
	}
}

void CShadowPackDlg::OnBnClickedBtnItemAdd()
{
	static TCHAR BASED_CODE szFilter[] =
		_T("All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_EXPLORER, szFilter, NULL, 0, TRUE);
	if (dlg.DoModal() == IDOK) {
		m_szItemPathName = dlg.GetPathName();
		StartThread(&CShadowPackDlg::ThreadAddItem);
	}
}

void CShadowPackDlg::OnBnClickedBtnItemDelete()
{
	m_ctlFileManager.DeleteSelectedItems();
}

void CShadowPackDlg::OnBnClickedBtnItemClearAll()
{
	m_ctlFileManager.ClearAllItems();
}


void CShadowPackDlg::OnBnClickedBtnCancel()
{
	m_ctlProgress.Cancel();
}




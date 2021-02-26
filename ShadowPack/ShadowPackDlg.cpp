
// ShadowPackDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShadowPack.h"
#include "ShadowPackDlg.h"
#include "PackErrors.h"
#include "PasswordDialog.h"
#include "PackUtils.h"
#include "PackItem.h"
#include "MediaFactory.h"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CShadowPackDlg 对话框




CShadowPackDlg::CShadowPackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShadowPackDlg::IDD, pParent),
	m_bInProgress(FALSE),
	m_bShowProgressBar(FALSE),
	m_bQuit(FALSE),
	m_bCancel(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShadowPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO2, m_ctlInfo2);
}

BEGIN_MESSAGE_MAP(CShadowPackDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CShadowPackDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CShadowPackDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_ITEM_DELETE, &CShadowPackDlg::OnBnClickedItemDelete)
	ON_BN_CLICKED(IDC_BTN_MEDIA_OPEN, &CShadowPackDlg::OnBnClickedMediaOpen)
	ON_BN_CLICKED(IDC_BTN_MEDIA_CLOSE, &CShadowPackDlg::OnBnClickedMediaClose)
	ON_BN_CLICKED(IDC_BTN_MEDIA_OPTION, &CShadowPackDlg::OnBnClickedMediaOption)
	ON_BN_CLICKED(IDC_BTN_MEDIA_SAVE, &CShadowPackDlg::OnBnClickedMediaSave)
	ON_BN_CLICKED(IDC_BTN_ITEM_EXPORT, &CShadowPackDlg::OnBnClickedItemExport)
	ON_BN_CLICKED(IDC_BTN_ITEM_ADD_DIR, &CShadowPackDlg::OnBnClickedItemAddDir)
	ON_BN_CLICKED(IDC_BTN_ITEM_ADD_FILE, &CShadowPackDlg::OnBnClickedItemAddFile)
	ON_BN_CLICKED(IDC_BTN_ITEM_CLEAR_ALL, &CShadowPackDlg::OnBnClickedItemClear)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CShadowPackDlg::OnBnClickedBtnCancel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, &CShadowPackDlg::OnLvnItemChangedListData)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DATA, &CShadowPackDlg::OnNMDblclkListData)
END_MESSAGE_MAP()


// CShadowPackDlg 消息处理程序

BOOL CShadowPackDlg::OnInitDialog()
{
#ifdef _DEBUG
	m_msOld.Checkpoint();
#endif

	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_ctlImageQuota.Initialize(this);

	m_Progress.Initialize(this);

	m_ctlPackItemList.Initialize(this);

	UpdateUI();

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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
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

void CShadowPackDlg::UpdateUI()
{
	BOOL bMediaBtnOther = FALSE;
	BOOL bMediaBtnOpen  = FALSE;
	BOOL bShowProgress = FALSE;
	BOOL bItemBtnClear = FALSE;
	BOOL bItemBtnDelete = FALSE;
	BOOL bItemBtnOther  = FALSE;
	BOOL bItemBtnExport  = FALSE;
	BOOL bMediaBtnSave = FALSE;

	CString szInfo, szCap, szUsed;

	bShowProgress = m_bInProgress;

	bMediaBtnOther = (m_PackRoot.GetInputMedia() != NULL && ! bShowProgress);
	bMediaBtnOpen  = (m_PackRoot.GetInputMedia() == NULL && ! bShowProgress);
	bMediaBtnSave  = (m_PackRoot.GetInputMedia() != NULL && (m_PackRoot.IsDirty() 
		||m_PackRoot.GetOutputMedia() && m_PackRoot.GetOutputMedia()->FormatChanged())
		&& !bShowProgress);
	bItemBtnClear = (!m_PackRoot.IsEmpty() && !bShowProgress);
	bItemBtnOther = (m_PackRoot.GetInputMedia() != NULL && ! bShowProgress);
	bItemBtnExport = !m_bInProgress && m_ctlPackItemList.GetSelectedCount() != 0;
	bItemBtnDelete = bItemBtnExport;

	GetDlgItem(IDC_BTN_MEDIA_OPEN)->EnableWindow(bMediaBtnOpen);
	GetDlgItem(IDC_BTN_MEDIA_CLOSE)->EnableWindow(bMediaBtnOther);
	GetDlgItem(IDC_BTN_MEDIA_SAVE)->EnableWindow(bMediaBtnSave);
	GetDlgItem(IDC_BTN_MEDIA_OPTION)->EnableWindow(bMediaBtnOther);

	GetDlgItem(IDC_BTN_ITEM_DELETE)->EnableWindow(bItemBtnDelete);
	GetDlgItem(IDC_BTN_ITEM_CLEAR_ALL)->EnableWindow(bItemBtnClear);
	GetDlgItem(IDC_BTN_ITEM_ADD_FILE)->EnableWindow(bItemBtnOther);
	GetDlgItem(IDC_BTN_ITEM_EXPORT)->EnableWindow(bItemBtnExport);
	GetDlgItem(IDC_BTN_ITEM_ADD_DIR)->EnableWindow(bItemBtnOther);

	GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(bShowProgress && !m_bCancel);

	m_ctlPackItemList.EnableWindow(!bShowProgress);

	m_Progress.ShowProgressBar(bShowProgress && m_bShowProgressBar);
	m_Progress.ShowInfoBar(bShowProgress && !m_bShowProgressBar);
	m_Progress.SetInfo(_T(""));

	CMenu *pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu != NULL) {
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND| m_bInProgress? MF_DISABLED : MF_ENABLED);
	}

	CPackUtils::TranslateSize(m_PackRoot.GetOutputMedia() != NULL ? m_PackRoot.GetOutputMedia()->GetCapicity() : 0, szCap);
	CPackUtils::TranslateSize(m_PackRoot.GetTotalDataSize(), szUsed);
	szInfo.Format(IDS_CAPICITY_USED, szCap, szUsed);
	m_ctlInfo2.SetWindowText(szInfo);

	if(!bShowProgress) {
		if(m_PackRoot.GetOutputMedia() && m_PackRoot.GetOutputMedia()->GetCapicity() != 0)
			m_ctlImageQuota.SetFreePercent((INT)((m_PackRoot.GetOutputMedia()->GetCapicity() - m_PackRoot.GetTotalDataSize()) * 100 / m_PackRoot.GetOutputMedia()->GetCapicity()));
		else
			m_ctlImageQuota.SetFreePercent(100, FALSE);
	}
}


void CShadowPackDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CShadowPackDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

CString CShadowPackDlg::fnGetPassword()
{
	CString strPassword(_T(""));

	CPasswordDialog dlg;
	if(dlg.DoModal() == IDOK) {
		strPassword = dlg.m_strPassword;
	} else {
		strPassword = (_T(""));
	}

	return strPassword;
}

void CShadowPackDlg::Quit()
{
	CDialog::OnCancel();
}

void CShadowPackDlg::OnClose()
{
	if(!m_PackRoot.IsEmpty()) {
		m_bQuit = TRUE;
		OnBnClickedMediaClose();
	} else {
		Quit();
	}
}

UINT __cdecl CShadowPackDlg::fnThread( LPVOID p )
{
	CPackThreadParam * pParam = (CPackThreadParam *)p;
	CShadowPackDlg * pThis = pParam->m_pThis;
	FN_PACK_THREAD pfn = pParam->m_pfn;
	(pThis->*pfn)(pParam->m_pParam);
	pThis->m_bInProgress = FALSE;
	pThis->m_bShowProgressBar = FALSE;
	pThis->m_bCancel = FALSE;
	pThis->UpdateUI();
	delete pParam;
	if(pThis->m_bQuit) {
		pThis->Quit();
	}
	return 0;
}

void CShadowPackDlg::StartThread(FN_PACK_THREAD fn, LPVOID param, BOOL bShowProgressBar)
{
	CPackThreadParam * pParam = new(std::nothrow) CPackThreadParam();
	if(pParam) {
		pParam->m_pfn = fn;
		pParam->m_pThis = this;
		pParam->m_pParam = param;
		m_bInProgress = TRUE;
		m_bCancel = FALSE;
		m_bShowProgressBar = bShowProgressBar;
		UpdateUI();
		AfxBeginThread(fnThread, pParam);
	}
}


void CShadowPackDlg::ThreadOpenMedia(LPVOID pParam)
{
	CPackErrors Error;
	CString strError;

	if(!m_PackRoot.LoadPack(m_szPathName, m_szFileExt, (CPasswordGetter &)m_dlgPassword, m_bCancel, Error, m_Progress)) {
		goto error;
	}
	
	m_ctlPackItemList.LoadPackItem(&m_PackRoot);
	return;
error:
	m_ctlPackItemList.LoadPackItem(&m_PackRoot);
	strError.Format(IDS_ERROR_OPEN_MEDIA, Error.ToString());
	AfxMessageBox(strError);
	return;
}


void CShadowPackDlg::OnBnClickedMediaOpen()
{
	// create ext table from factory
	CString szFilter = CMediaFactory::CreateExtTable();
	CPackErrors Error;
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL, 0, TRUE);
	
	if(dlg.DoModal() == IDOK) {

		m_szPathName = dlg.GetPathName();
		m_szFileExt  = dlg.GetFileExt();
		
		StartThread(&CShadowPackDlg::ThreadOpenMedia, NULL, FALSE);
	}
}

void CShadowPackDlg::ThreadCloseMedia(LPVOID pParam)
{
	CPackErrors Error;
	if(!m_PackRoot.Clear(Error, m_bCancel, m_Progress)) {
		AfxMessageBox(Error.ToString());
		goto error;
	}

	m_PackRoot.SetDirty(FALSE);
	if(m_PackRoot.GetInputMedia()) {
		m_PackRoot.GetInputMedia()->CloseMedia();
		m_PackRoot.AttachInputMedia(NULL);
	}
	if(m_PackRoot.GetOutputMedia()) {
		m_PackRoot.GetOutputMedia()->CloseMedia();
		m_PackRoot.AttachOutputMedia(NULL);
	}

	m_ctlPackItemList.LoadPackItem(&m_PackRoot);

	return;

error:
	m_ctlPackItemList.LoadPackItem(&m_PackRoot);
	m_bQuit = FALSE;
	return;
}

void CShadowPackDlg::CloseMedia()
{
	StartThread(&CShadowPackDlg::ThreadCloseMedia, NULL, FALSE);
}

void CShadowPackDlg::OnBnClickedMediaClose()
{
	if(NULL != m_PackRoot.GetInputMedia() || m_PackRoot.GetOutputMedia()) {
		if(m_PackRoot.IsDirty() || m_PackRoot.GetOutputMedia()->FormatChanged()) {
			if(AfxMessageBox(IDS_SAVE_DIRTY,MB_YESNO) == IDYES) {
				OnBnClickedMediaSave();
			} else {
				CloseMedia();
			}
		} else {
			CloseMedia();
		}	
	}
}

void CShadowPackDlg::OnBnClickedMediaOption()
{
	if( m_PackRoot.GetOutputMedia()->ShowOptionDlg()) {
		UpdateUI();
	}
}

void CShadowPackDlg::ThreadSaveMedia(LPVOID pParam)
{
	BOOL bRet;
	CPackErrors Error;
	CString strError;
	m_bCancel = FALSE;

	bRet = m_PackRoot.SavePack(m_szPathName, (CPasswordGetter &) m_dlgPassword ,m_bCancel, Error, m_Progress);

	if(!bRet) {
		strError.Format(IDS_ERROR_SAVE_MEDIA, Error.ToString());
		AfxMessageBox(strError);
		m_bQuit = FALSE;
		return;
	}

	m_PackRoot.SetDirty(FALSE);

	m_ctlPackItemList.LoadPackItem(m_PackRoot.GetCurrentDir());

	if(m_bQuit) {
		ThreadCloseMedia(pParam);
	}
}

void CShadowPackDlg::OnBnClickedMediaSave()
{
	CFileDialog dlg(FALSE, m_PackRoot.GetOutputMedia()->GetDefaultExt(), _T("Output"), 
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);

	if(dlg.DoModal() == IDOK) {
		m_szPathName = dlg.GetPathName();
		m_szFileExt  = dlg.GetFileExt();
		StartThread(&CShadowPackDlg::ThreadSaveMedia, NULL, FALSE);
	}
}

BOOL CShadowPackDlg::ShowLocationDirDlg(CString & strDir)
{
		// get dir location
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = GetSafeHwnd();
		bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;  
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		BOOL bRet = FALSE;  
		TCHAR szFolder[MAX_PATH*2];  
		szFolder[0] = 0; 
		if (pidl)  
		{  
			if (SHGetPathFromIDList(pidl, szFolder))    
				bRet = TRUE;  
			 IMalloc *pMalloc = NULL;  
			 if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)  
			{   
				pMalloc->Free(pidl);   
				pMalloc->Release();  
			 }  
		}  
		strDir = szFolder;
		return bRet;
}

void CShadowPackDlg::ThreadExportItem(LPVOID pParam)
{
	POSITION pos = m_ctlPackItemList.GetFirstSelectedItemPosition();
	CArray<CPackItem *, CPackItem *> aExport;
	CPackErrors Error;
	BOOL bRet = FALSE;
	CString strError;

	if (pos != NULL) {
		while(pos) {
			INT nItem = m_ctlPackItemList.GetNextSelectedItem(pos);
			CPackItem * p = (CPackItem *) m_ctlPackItemList.GetItemData(nItem);
			aExport.Add(p);
			TRACE(_T("add %s to export\n"), p->GetName());
		}

		if(aExport.GetSize() == 0) {
			return;
		}

		CString strDstPath;
		if(aExport.GetSize() == 1 && aExport[0]->IsFile()) { // 检查是不是单一文件？
			CFileDialog dlg(FALSE, NULL, aExport[0]->GetName(), 
				OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
			if(dlg.DoModal() == IDOK) {
				strDstPath = dlg.GetPathName();
				if(!m_PackRoot.ExportItemFileToDiskPath(aExport[0], dlg.GetFolderPath(), dlg.GetFileName(),m_bCancel, Error, m_Progress,
					FALSE, FALSE, TRUE)) {
					strError.Format(IDS_ERROR_EXPORT_ITEM, Error.ToString());
					AfxMessageBox(strError);
				}
				
			}
		} else {
			if(ShowLocationDirDlg(strDstPath)) {
				if(!m_PackRoot.ExportItemToDiskPath(aExport, strDstPath, m_bCancel, Error, m_Progress)) {
					strError.Format(IDS_ERROR_EXPORT_ITEM, Error.ToString());
					AfxMessageBox(strError);
				}
			}
		}
	}


}

void CShadowPackDlg::OnBnClickedItemExport()
{
	StartThread(&CShadowPackDlg::ThreadExportItem, NULL, FALSE);
}

void CShadowPackDlg::ThreadDeleteItem(LPVOID pParam)
{
	POSITION pos;
	CPackErrors Error;

	pos = m_ctlPackItemList.GetFirstSelectedItemPosition();

	while(pos) {
		INT nItem = m_ctlPackItemList.GetNextSelectedItem(pos);
		CPackItem * pItem = (CPackItem *) m_ctlPackItemList.GetItemData(nItem);
		if(!m_PackRoot.DeleteItem(pItem, Error, m_bCancel, m_Progress)) {
			AfxMessageBox(Error.ToString());
			break;
		}
		
	}

	m_ctlPackItemList.LoadPackItem(m_PackRoot.GetCurrentDir());
}

void CShadowPackDlg::OnBnClickedItemDelete()
{
	StartThread(&CShadowPackDlg::ThreadDeleteItem, NULL, FALSE);
}


void CShadowPackDlg::ThreadAddItemDir(LPVOID pParam)
{
	CPackErrors Error;
	if(!m_PackRoot.AddItemDir(m_szPathName, m_PackRoot.GetCurrentDir(), Error, m_bCancel, m_Progress)) {
		AfxMessageBox(Error.ToString());		
	}

	m_ctlPackItemList.LoadPackItem(m_PackRoot.GetCurrentDir());
}

void CShadowPackDlg::OnBnClickedItemAddDir()
{
	if(ShowLocationDirDlg(m_szPathName)) {
		StartThread(&CShadowPackDlg::ThreadAddItemDir, NULL, FALSE);
	}	
}

void CShadowPackDlg::ThreadAddItemFile(LPVOID pParam)
{
	POSITION pos;
	CFileDialog * pdlg = (CFileDialog *)pParam;
	CPackErrors Error;
	pos = pdlg->GetStartPosition();
	while(pos) {
		CString strPath = pdlg->GetNextPathName(pos);
		if(!m_PackRoot.AddItemFile(strPath, m_PackRoot.GetCurrentDir(), Error)) {
			AfxMessageBox(Error.ToString());
			break;
		}
		CString info;
		info.Format(IDS_ADD_FILE, strPath);
		m_Progress.SetInfo(info);
	}
	delete [] pdlg->GetOFN().lpstrFile;
	delete pdlg;
	m_ctlPackItemList.LoadPackItem(m_PackRoot.GetCurrentDir());
}

void CShadowPackDlg::OnBnClickedItemAddFile()
{
	static TCHAR BASED_CODE szFilter[] = 
		_T("All Files (*.*)|*.*||");
	INT nMaxFiles = 1024;
	TCHAR * pBuffer = new(std::nothrow) TCHAR[(nMaxFiles * (MAX_PATH + 1)) + 1];
	if(!pBuffer) {
		return;
	}
	ZeroMemory(pBuffer, (nMaxFiles * (MAX_PATH + 1)) + 1);
	CFileDialog *pdlg = new CFileDialog(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT|OFN_EXPLORER  , szFilter, NULL, 0, TRUE);
	if(pdlg) {
		pdlg->GetOFN().lpstrFile = pBuffer;
		pdlg->GetOFN().nMaxFile = nMaxFiles;

		if(pdlg->DoModal() == IDOK) {
			StartThread(&CShadowPackDlg::ThreadAddItemFile, pdlg, FALSE);
		} else {
			delete [] pBuffer;
			pBuffer = NULL;
			delete pdlg;
			pdlg = NULL;
		}
	}
}

void CShadowPackDlg::ThreadClearItem(LPVOID pParam)
{
	CPackErrors Error;

	if(!m_PackRoot.Clear(Error, m_bCancel, m_Progress)) {
		AfxMessageBox(Error.ToString());
	}
	
	m_ctlPackItemList.LoadPackItem(m_PackRoot.GetCurrentDir());
}

void CShadowPackDlg::OnBnClickedItemClear()
{
	StartThread(&CShadowPackDlg::ThreadClearItem, NULL, FALSE);
}

void CShadowPackDlg::OnBnClickedBtnCancel()
{
	GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(FALSE);
	m_bCancel = TRUE;
}

void CShadowPackDlg::OnLvnItemChangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CString szInfo;
	BOOL bEnable = TRUE;	
	POSITION pos = m_ctlPackItemList.GetFirstSelectedItemPosition();

	//TRACE(_T("OnLvnItemChangedListData %d %u %u\n"), pNMLV->iItem, pNMLV->uOldState, pNMLV->uNewState);
	
	if (pos == NULL)
	{
	   TRACE(_T("No items were selected!\n"));
	   bEnable = FALSE;
	}
	else
	{
	   while (pos)
	   {
		  INT nItem = m_ctlPackItemList.GetNextSelectedItem(pos);
		  CPackItem * pItem = (CPackItem *)m_ctlPackItemList.GetItemData(nItem);
		  if(pItem->IsRoot()) {
			bEnable = FALSE;
		  }
	   }
	}

	GetDlgItem(IDC_BTN_ITEM_DELETE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_ITEM_EXPORT)->EnableWindow(bEnable);

	*pResult = 0;
}


void CShadowPackDlg::OnNMDblclkListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	//TRACE(_T("OnNMDblclkListData: item %d db clicked!\n"), pNMItemActivate->iItem);
	if(pNMItemActivate->iItem >= 0) {
		CPackItem * pItem = (CPackItem *) m_ctlPackItemList.GetItemData(pNMItemActivate->iItem);
		if(pItem != NULL) {
			if(pItem->IsDir() || pItem->IsRoot()) {
				m_ctlPackItemList.LoadPackItem(pItem);
				m_PackRoot.SetCurrentDir(pItem);
			} else if(pItem->IsFile()) {
				CString strFile;
				strFile.Format(_T("should open file %s"), pItem->GetDiskLocation());
				AfxMessageBox(strFile);
			}
		}
	}
	*pResult = 0;
}

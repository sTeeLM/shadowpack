// ShadowPackDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShadowPack.h"
#include "ShadowPackDlg.h"
#include "OptionDialog.h"
#include "PasswordDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	: CDialog(CShadowPackDlg::IDD, pParent)
	,m_szPathName(_T(""))
	,m_szFileExt(_T(""))
	,m_pPack(NULL)
	,m_bInProgress(FALSE)
	,m_bQuit(FALSE)
	,m_bCloseImage(FALSE)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShadowPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_DATA, m_ctlPackItemList);
	DDX_Control(pDX, IDC_PROGRESS_IMAGE, m_ctlProgress);
	DDX_Control(pDX, IDC_STATIC_INFO1, m_ctlInfo1);
	DDX_Control(pDX, IDC_STATIC_INFO2, m_ctlInfo2);
}

BEGIN_MESSAGE_MAP(CShadowPackDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CShadowPackDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CShadowPackDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_LOAD_IMAGE, &CShadowPackDlg::OnBnClickedBtnLoadImage)
	ON_BN_CLICKED(IDC_BTN_SAVE_IMAGE, &CShadowPackDlg::OnBnClickedBtnSaveImage)
	ON_BN_CLICKED(IDC_BTN_OPTION, &CShadowPackDlg::OnBnClickedBtnOption)
	ON_BN_CLICKED(IDC_BTN_ADD_DATA, &CShadowPackDlg::OnBnClickedBtnAddData)
	ON_BN_CLICKED(IDC_BTN_REMOVE_DATA, &CShadowPackDlg::OnBnClickedBtnRemoveData)
	ON_BN_CLICKED(IDC_BTN_CLEAR_DATA, &CShadowPackDlg::OnBnClickedBtnClearData)
	ON_BN_CLICKED(IDC_BTN_CLOSE_IMAGE, &CShadowPackDlg::OnBnClickedBtnClose)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_DATA, &CShadowPackDlg::OnLvnItemchangedLstData)
	ON_BN_CLICKED(IDC_BTN_EXPORT_DATA, &CShadowPackDlg::OnBnClickedBtnExportData)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CShadowPackDlg::OnBnClickedBtnCancel)
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
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	m_pPack = NULL;

	CRect rect;
	m_ctlPackItemList.GetClientRect(&rect);
	INT nColInterval = rect.Width()/5;

	CString str;

	str.LoadString(IDS_COLUMN_ID);
	m_ctlPackItemList.InsertColumn(0, (LPCTSTR)str, LVCFMT_LEFT, nColInterval);
	str.LoadString(IDS_COLUMN_NAME);
	m_ctlPackItemList.InsertColumn(1, (LPCTSTR)str, LVCFMT_LEFT, nColInterval*3);
	str.LoadString(IDS_COLUMN_SIZE);
	m_ctlPackItemList.InsertColumn(2, (LPCTSTR)str, LVCFMT_LEFT, rect.Width()-4*nColInterval);

	DWORD dwStyle = m_ctlPackItemList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_ctlPackItemList.SetExtendedStyle(dwStyle);

	m_ctlImageQuota.SubclassDlgItem(IDC_IMAGE_QUOTA, this);

	LONG lEx = ::GetWindowLong(m_ctlImageQuota.GetSafeHwnd(), GWL_STYLE);

	lEx |= SS_OWNERDRAW;

	lEx = ::SetWindowLong(m_ctlImageQuota.GetSafeHwnd(), GWL_STYLE, lEx);

	m_ctlProgress.SetRange(0, 100);

	UpdateUI();
 
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShadowPackDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	TRACE(_T("CShadowPackDlg::OnSysCommand %u %d\n"), nID, lParam);
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		if(nID & SC_CLOSE && m_bInProgress) {
			// nothing
		} else {
			CDialog::OnSysCommand(nID, lParam);
		}
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
CString CShadowPackDlg::m_szPackPassword = (_T(""));
CString & CShadowPackDlg::fnGetPassword()
{
	CPasswordDialog dlg;
	if(dlg.DoModal() == IDOK) {
		m_szPackPassword = dlg.m_strPassword;
	} else {
		m_szPackPassword = (_T(""));
	}

	return m_szPackPassword;
}

void CShadowPackDlg::fnSetProgress(INT nPercent)
{
	TRACE(_T("PROGRESS %d\n"), nPercent );
	::SendMessage(::GetDlgItem(AfxGetMainWnd()->GetSafeHwnd(), IDC_PROGRESS_IMAGE), PBM_SETPOS, nPercent, 0);
}

void CShadowPackDlg::TranslateSize(UINT nSize, CString & strOut)
{
	double dSize = (double)nSize;
	if(dSize > 1024) {
		dSize /= 1024;
		if(dSize > 1024) {
			dSize /= 1024;
			strOut.Format(_T("%.1f M"), dSize);
		} else {
			strOut.Format(_T("%.1f K"), dSize);
		}
	} else {
		strOut.Format(_T("%.1f"), dSize);
	}
}

BOOL CShadowPackDlg::AddListItem(UINT nID, LPCTSTR szName, UINT nSize)
{
	LVITEM lvi;
	CString strItem;

// Insert the first item
	lvi.mask = LVIF_TEXT;
	strItem.Format(_T("%u"), nID);
	lvi.iItem = nID;
	lvi.iSubItem = 0;
	lvi.pszText = (LPTSTR)(LPCTSTR)strItem;
	m_ctlPackItemList.InsertItem(&lvi);
// Set subitem 1
	strItem.Format(_T("%s"), szName);
	lvi.iSubItem = 1;
	lvi.pszText = (LPTSTR)(LPCTSTR)strItem;
	m_ctlPackItemList.SetItem(&lvi);
// Set subitem 2
	TranslateSize(nSize, strItem);
	lvi.iSubItem = 2;
	lvi.pszText = (LPTSTR)(LPCTSTR)strItem;;
	m_ctlPackItemList.SetItem(&lvi);
	
	return TRUE;
}

BOOL CShadowPackDlg::RemoveListItem(UINT nID, UINT nCount)
{
	for(UINT i = 0  ; i < nCount ; i++) {
		m_ctlPackItemList.DeleteItem(nID);
	}
	return TRUE;
}

BOOL CShadowPackDlg::ClearListItem()
{
	m_ctlPackItemList.DeleteAllItems();

	return TRUE;
}

void CShadowPackDlg::EndSave(BOOL bOK, CString & szError)
{
	CString strError;
	if(!bOK) {
		strError.Format(IDS_ERROR_SAVE_IMAGE, szError);
		AfxMessageBox(strError);
	}
	UpdateUI();
	if(m_bCloseImage) {
		CloseImage();
	}
	if(m_bQuit) {
		//AfxGetMainWnd()->PostMessage(WM_CLOSE);
		CDialog::OnCancel();
	}
}

void CShadowPackDlg::EndLoad(BOOL bOK, CString & szError)
{
	INT nIndex;
	CString strError;
	if(!bOK) {
		strError.Format(IDS_ERROR_LOAD_IMAGE, szError);
		AfxMessageBox(strError);
	} else {
		for(nIndex = 0 ; nIndex < m_pPack->GetPackItemCount(); nIndex ++) {
			CPackItem * pItem = m_pPack->GetPackItem(nIndex);
			AddListItem(nIndex, pItem->GetName(), pItem->GetTotalSize());
		}
		m_ctlImageQuota.SetFreePercent((m_pPack->GetCapicity() - m_pPack->GetDataSize()) * 100 / m_pPack->GetCapicity());
	}
	UpdateUI();
}

void CShadowPackDlg::BeginLoadSave()
{
	GetDlgItem(IDC_BTN_LOAD_IMAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SAVE_IMAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLOSE_IMAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_OPTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_ADD_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_REMOVE_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLEAR_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_EXPORT_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LST_DATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CANCEL)->ShowWindow(SW_SHOW);
	m_ctlProgress.EnableWindow(TRUE);
	m_ctlProgress.ShowWindow(SW_SHOW);
	m_ctlInfo1.ShowWindow(SW_HIDE);
	m_ctlInfo2.ShowWindow(SW_HIDE);

	m_bInProgress = TRUE;

	CMenu *pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu != NULL) {
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_DISABLED);
	}

}

void CShadowPackDlg::BeginLoad()
{
	BeginLoadSave();
}

void CShadowPackDlg::BeginSave()
{
	BeginLoadSave();
}

UINT __cdecl CShadowPackDlg::LoadImage( LPVOID pParam )
{
	CShadowPackDlg *pThis = (CShadowPackDlg *)pParam;

	CString szError;

	m_bCancel = FALSE;

	pThis->m_pPack = CPack::LoadFromImage(pThis->m_szPathName, pThis->m_szFileExt, szError, &m_bCancel, fnGetPassword, fnSetProgress);
	
	pThis->EndLoad(pThis->m_pPack  != NULL,  szError);

	return 0;
}

UINT __cdecl CShadowPackDlg::SaveImage( LPVOID pParam )
{
	CShadowPackDlg *pThis = (CShadowPackDlg *)pParam;

	CString szError;

	m_bCancel = FALSE;

	pThis->EndSave(pThis->m_pPack->SaveToImage(pThis->m_szPathName, pThis->m_szFileExt, szError, &m_bCancel, fnSetProgress),  szError);

	return 0;
}

void CShadowPackDlg::OnBnClickedBtnLoadImage()
{
	static TCHAR BASED_CODE szFilter[] = 
		_T("All Image File(*.*)")
		_T("|*.*||");

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL, 0, TRUE);
	
	if(dlg.DoModal() == IDOK) {
		if(NULL != m_pPack) {
			delete m_pPack;
			m_pPack = NULL;
		}
		
		m_szPathName = dlg.GetPathName();

		m_szFileExt  = dlg.GetFileExt();

		BeginLoad();

		AfxBeginThread(LoadImage, this);
	}
}

void CShadowPackDlg::OnBnClickedBtnSaveImage()
{
	if(NULL != m_pPack) {
		CString szFilter = m_pPack->GetFilter();
		CString szDefExt = m_pPack->GetDefaultExt();
		CFileDialog dlg(FALSE, (LPCTSTR)szDefExt, _T("Output"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (LPCTSTR)szFilter, NULL, 0, TRUE);
		if(dlg.DoModal() == IDOK) {
			m_szPathName = dlg.GetPathName();
			m_szFileExt  = dlg.GetFileExt();
			BeginSave();
			AfxBeginThread(SaveImage, this);
		}
	}
}

void CShadowPackDlg::OnBnClickedBtnOption()
{
	COptionDialog dlg;

	if(NULL != m_pPack) {
		dlg.m_bEncrypt = m_pPack->GetEncryptMethod() != CPack::EM_NONE;
		dlg.m_szPassword = m_pPack->GetPassword();
		if(dlg.m_bEncrypt) {
			dlg.m_nEncryptMethod =(INT)( m_pPack->GetEncryptMethod() ) - 1;
		}
		dlg.m_bEnable1PP = m_pPack->CanSetFormat(CPack::PF_RAWPP, 1);
		dlg.m_bEnable2PP = m_pPack->CanSetFormat(CPack::PF_RAWPP, 2);
		dlg.m_bEnable3PP = m_pPack->CanSetFormat(CPack::PF_RAWPP, 3);
		dlg.m_bEnable1PJ = m_pPack->CanSetFormat(CPack::PF_JSTEG, 1);
		dlg.m_bEnable2PJ = m_pPack->CanSetFormat(CPack::PF_JSTEG, 2);
		dlg.m_bEnable4PJ = m_pPack->CanSetFormat(CPack::PF_JSTEG, 3);

		if(m_pPack->GetPackFormat() == CPack::PF_RAWPP) {
			if(m_pPack->GetPackFormatParam() == 1)
				dlg.m_nFormat = 0;
			if(m_pPack->GetPackFormatParam() == 2)
				dlg.m_nFormat = 1;
			if(m_pPack->GetPackFormatParam() == 3)
				dlg.m_nFormat = 2;
		} else if(m_pPack->GetPackFormat() == CPack::PF_JSTEG) {
			if(m_pPack->GetPackFormatParam() == 1)
				dlg.m_nFormat = 3;
			if(m_pPack->GetPackFormatParam() == 2)
				dlg.m_nFormat = 4;
			if(m_pPack->GetPackFormatParam() == 3)
				dlg.m_nFormat = 5;
		}

		if(dlg.DoModal() == IDOK) {
			m_pPack->SetPassword(dlg.m_szPassword);
			if(dlg.m_bEncrypt) {
				m_pPack->SetEncryptMethod((CPack::EncryptMethod)(dlg.m_nEncryptMethod + 1));
			} else {
				m_pPack->SetEncryptMethod(CPack::EM_NONE);
			}

			if(dlg.m_nFormat >= 0 && dlg.m_nFormat <= 2) {
				m_pPack->SetPackFormat(CPack::PF_RAWPP, dlg.m_nFormat + 1);

			} else if(dlg.m_nFormat >= 3 && dlg.m_nFormat <= 5) {
				m_pPack->SetPackFormat(CPack::PF_JSTEG, dlg.m_nFormat - 2);
			}

			m_ctlImageQuota.SetFreePercent((m_pPack->GetCapicity() - m_pPack->GetDataSize()) * 100 / m_pPack->GetCapicity());
			UpdateUI();
		}
	}
}

void CShadowPackDlg::OnBnClickedBtnAddData()
{
	UINT nIndex;
	static TCHAR BASED_CODE szFilter[] = 
		_T("All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL, 0, TRUE);
	CString strError;
	if(NULL != m_pPack) {
		if(dlg.DoModal() == IDOK) {
			CString szError;
			CPackItem * pPackItem = CPackItem::CreatePackItemFromFile(dlg.GetFileName(), dlg.GetPathName(), szError);
			if(NULL == pPackItem) {
				strError.Format(IDS_ERROR_ADD_DATA, szError);
				AfxMessageBox(strError);
			} else if(!m_pPack->AddPackItem(pPackItem, nIndex, szError)) {
				strError.Format(IDS_ERROR_ADD_DATA, szError);
				AfxMessageBox(strError);
			} else {
				AddListItem(nIndex, pPackItem->GetName(), pPackItem->GetTotalSize());
				m_ctlImageQuota.SetFreePercent((m_pPack->GetCapicity() - m_pPack->GetDataSize()) * 100 / m_pPack->GetCapicity());
			}
			UpdateUI();
		}
	}

}

void CShadowPackDlg::OnBnClickedBtnRemoveData()
{

	INT nCount = m_ctlPackItemList.GetSelectedCount();
	if(nCount > 0 ) {
		INT nItem = m_ctlPackItemList.GetNextItem(-1, LVNI_SELECTED);
		if(nItem >= 0) {
			if(NULL != m_pPack && m_pPack->GetPackItemCount() != 0) {
				CPackItem * pPackItem = m_pPack->GetPackItem(nItem);
				if(NULL != pPackItem) {
					if(m_pPack->RemovePackItem(nItem, nCount)) {
						RemoveListItem(nItem, nCount);
						m_ctlImageQuota.SetFreePercent((m_pPack->GetCapicity()
							- m_pPack->GetDataSize()) * 100 / m_pPack->GetCapicity());
					}
				}
				UpdateUI();
			}
		}
	}

}


void CShadowPackDlg::OnClose()
{
	if(!m_bInProgress) {
		if(m_pPack != NULL) {
			if(m_pPack!=NULL) {
				m_bQuit = TRUE;
				OnBnClickedBtnClose();
			}
		} else {
			CDialog::OnCancel();
		}
	} else {
		AfxMessageBox(IDS_ERROR_IN_PROGRESS);
	}
}

void CShadowPackDlg::OnBnClickedBtnClearData()
{
	if(NULL != m_pPack && m_pPack->GetPackItemCount() != 0) {
		if(m_pPack->Clear()) {
			UpdateUI();
			m_ctlImageQuota.SetFreePercent(100);
			ClearListItem();
		}
	}
}

void CShadowPackDlg::CloseImage()
{
	if(NULL != m_pPack) {
		delete m_pPack;
		m_pPack = NULL;
	}
	ClearListItem();
	m_ctlImageQuota.SetFreePercent(100, FALSE);
	UpdateUI();
	m_bCloseImage = FALSE;
}

void CShadowPackDlg::OnBnClickedBtnClose()
{
	if(m_pPack != NULL) {
		m_bCloseImage = TRUE;
		if(m_pPack->IsDirty()) {
			if(AfxMessageBox(IDS_SAVE_DIRTY,MB_YESNO) == IDYES) {
				OnBnClickedBtnSaveImage();
			} else {
				CloseImage();
				if(m_bQuit) {
					CDialog::OnCancel();
				}
			}
		} else {
			CloseImage();
			if(m_bQuit) {
				CDialog::OnCancel();
			}
		}
	}
}


void CShadowPackDlg::UpdateUI()
{
	if(m_pPack == NULL) {
		GetDlgItem(IDC_BTN_LOAD_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_SAVE_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CLOSE_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_OPTION)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_ADD_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_REMOVE_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CLEAR_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_EXPORT_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_LST_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CANCEL)->ShowWindow(SW_HIDE);
		m_ctlProgress.SetPos(0);
		m_ctlProgress.EnableWindow(FALSE);
		m_ctlProgress.ShowWindow(SW_HIDE);
		m_ctlInfo1.ShowWindow(SW_SHOW);
		m_ctlInfo2.ShowWindow(SW_SHOW);
		m_ctlInfo1.SetWindowText(_T(""));
		m_ctlInfo2.SetWindowText(_T(""));
	} else {
		CString szInfo, szCap, szUsed;
		GetDlgItem(IDC_BTN_LOAD_IMAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_SAVE_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CLOSE_IMAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_OPTION)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_ADD_DATA)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BTN_REMOVE_DATA)->EnableWindow(FALSE);
		//GetDlgItem(IDC_BTN_EXPORT_DATA)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CLEAR_DATA)->EnableWindow(!m_pPack->IsEmpty());
		GetDlgItem(IDC_LST_DATA)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CANCEL)->ShowWindow(SW_HIDE);
		m_ctlProgress.SetPos(0);
		m_ctlProgress.EnableWindow(FALSE);
		m_ctlProgress.ShowWindow(SW_HIDE);
		m_ctlInfo1.ShowWindow(SW_SHOW);
		m_ctlInfo2.ShowWindow(SW_SHOW);
		m_ctlInfo1.SetWindowText(_T(""));
		TranslateSize(m_pPack->GetCapicity(), szCap);
		TranslateSize(m_pPack->GetDataSize(), szUsed);
		szInfo.Format(IDS_CAPICITY_USED, szCap, szUsed);
		m_ctlInfo2.SetWindowText(szInfo);
	}
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu != NULL) {
		pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
	}
	m_bInProgress = FALSE;
}


void CShadowPackDlg::OnLvnItemchangedLstData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CString szInfo;
	TRACE(_T("OnLvnItemchangedLstData %d %u %u\n"), pNMLV->iItem, pNMLV->uOldState, pNMLV->uNewState);
	
	GetDlgItem(IDC_BTN_REMOVE_DATA)->EnableWindow(m_ctlPackItemList.GetSelectedCount() != 0);
	GetDlgItem(IDC_BTN_EXPORT_DATA)->EnableWindow(m_ctlPackItemList.GetSelectedCount() == 1);

	if(m_ctlPackItemList.GetSelectedCount() > 0) {
		szInfo.Format(IDS_SELECTED, m_ctlPackItemList.GetSelectedCount());
		m_ctlInfo1.SetWindowText(szInfo);
	} else {
		m_ctlInfo1.SetWindowText(_T(""));
	}

	*pResult = 0;
}

void CShadowPackDlg::OnBnClickedBtnExportData()
{
	CString strError;
	CString szError;
	INT nCount = m_ctlPackItemList.GetSelectedCount();
	if( nCount == 1 ) {
		INT nItem = m_ctlPackItemList.GetNextItem(-1, LVNI_SELECTED);
		if(nItem >= 0) {
			if(NULL != m_pPack && m_pPack->GetPackItemCount() != 0) {
				CPackItem * pItem = m_pPack->GetPackItem(nItem);
				if(NULL != pItem) {
					CFileDialog dlg(FALSE, NULL,pItem->GetName() , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0, TRUE);
					if(dlg.DoModal() == IDOK) {
						if(!pItem->ExportDataToFile(pItem->GetName(), dlg.GetPathName(),szError)) {
							strError.Format(IDS_ERROR_EXPORT_DATA, szError);
							AfxMessageBox(strError);
						}
					}
				}
			}
		}
	}
}

BOOL CShadowPackDlg::m_bCancel = FALSE;

void CShadowPackDlg::OnBnClickedBtnCancel()
{
	m_bCancel = TRUE;
}

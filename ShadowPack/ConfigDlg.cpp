// ConfigDlg.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"
#include "LocaleManager.h"
#include "PackUtils.h"


// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONFIGBOX, pParent)
	, m_bMediaHDCacheEnable(FALSE)
	, m_bPackHDCacheEnable(FALSE)
	, m_bMediaHDCacheUseTmp(FALSE)
	, m_bPackHDCacheUseTmp(FALSE)
	, m_nCurrentLocale(0)
	, m_strMediaHDCachePath(_T(""))
	, m_strPackHDCachePath(_T(""))
{

}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LANG, m_ctlComboLang);
	DDX_Check(pDX, IDC_CHECK_MEDIA_HD_CACHE, m_bMediaHDCacheEnable);
	DDX_Check(pDX, IDC_CHECK_PACK_HD_CACHE, m_bPackHDCacheEnable);
	DDX_Check(pDX, IDC_CHECK_MEDIA_HD_CACHE_USE_TMP, m_bMediaHDCacheUseTmp);
	DDX_Check(pDX, IDC_CHECK_PACK_HD_CACHE_USE_TMP, m_bPackHDCacheUseTmp);
	DDX_Text(pDX, IDC_EDIT_MEDIA_HD_CACHE_DIR, m_strMediaHDCachePath);
	DDX_Text(pDX, IDC_EDIT_PACK_HD_CACHE_DIR, m_strPackHDCachePath);

	if (pDX->m_bSaveAndValidate) {
		m_nCurrentLocale = m_ctlComboLang.GetCurSel();
	}

	if (pDX->m_bSaveAndValidate) {
		if (m_bMediaHDCacheEnable && !m_strMediaHDCachePath.GetLength()
			|| m_bPackHDCacheEnable && !m_strPackHDCachePath.GetLength()) {
			AfxMessageBox(IDS_HD_PATH_CAN_NOT_NULL);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CConfigDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SEL_MEDIA_HD_CACHE, &CConfigDlg::OnBnClickedBtnSelMediaHdCache)
	ON_BN_CLICKED(IDC_BTN_SEL_PACK_HD_CACHE, &CConfigDlg::OnBnClickedBtnSelPackHdCache)
	ON_BN_CLICKED(IDC_CHECK_MEDIA_HD_CACHE, &CConfigDlg::OnBnClickedCheckMediaHdCache)
	ON_BN_CLICKED(IDC_CHECK_PACK_HD_CACHE, &CConfigDlg::OnBnClickedCheckPackHdCache)
	ON_BN_CLICKED(IDC_CHECK_MEDIA_HD_CACHE_USE_TMP, &CConfigDlg::OnBnClickedCheckMediaHdCacheUseTmp)
	ON_BN_CLICKED(IDC_CHECK_PACK_HD_CACHE_USE_TMP, &CConfigDlg::OnBnClickedCheckPackHdCacheUseTmp)
END_MESSAGE_MAP()

BOOL CConfigDlg::OnInitDialog()
{
	if (CDialogEx::OnInitDialog()) {
		for (INT i = 0; i < CLocaleManager::GetLocaleCount(); i++) {
			m_ctlComboLang.AddString(CLocaleManager::GetLocalName(i));
		}
		m_ctlComboLang.SetCurSel(m_nCurrentLocale);
		UpdateUI();
		return TRUE;
	}
	return FALSE;
}
// CConfigDlg 消息处理程序


void CConfigDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CConfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CConfigDlg::OnBnClickedBtnSelMediaHdCache()
{
	CString strRet;
	if (!CPackUtils::ShowLocationDirDlg(this, strRet)) {
		strRet = CPackUtils::GetTempPath();
	}

	if (strRet.Right(1).Compare(_T("\\"))) {
		strRet += _T("\\");
	}

	GetDlgItem(IDC_EDIT_MEDIA_HD_CACHE_DIR)->SetWindowText(strRet);
}


void CConfigDlg::OnBnClickedBtnSelPackHdCache()
{
	CString strRet;
	if (!CPackUtils::ShowLocationDirDlg(this, strRet)) {
		strRet = CPackUtils::GetTempPath();
	}

	if (strRet.Right(1).Compare(_T("\\"))) {
		strRet += _T("\\");
	}

	GetDlgItem(IDC_EDIT_PACK_HD_CACHE_DIR)->SetWindowText(strRet);
}

void CConfigDlg::UpdateUI()
{
	CString strTemp = CPackUtils::GetTempPath();
	BOOL bMediaCacheHDEnable = IsDlgButtonChecked(IDC_CHECK_MEDIA_HD_CACHE);
	BOOL bCustomMediaHDCacheDirEnable = bMediaCacheHDEnable
		&& !IsDlgButtonChecked(IDC_CHECK_MEDIA_HD_CACHE_USE_TMP);
	BOOL bPackCacheHDEnable = IsDlgButtonChecked(IDC_CHECK_PACK_HD_CACHE);
	BOOL bCustomPackHDCacheDirEnable = bPackCacheHDEnable
		&& !IsDlgButtonChecked(IDC_CHECK_PACK_HD_CACHE_USE_TMP);

	GetDlgItem(IDC_CHECK_MEDIA_HD_CACHE_USE_TMP)->EnableWindow(bMediaCacheHDEnable);
	GetDlgItem(IDC_BTN_SEL_MEDIA_HD_CACHE)->EnableWindow(bCustomMediaHDCacheDirEnable);
	GetDlgItem(IDC_CHECK_PACK_HD_CACHE_USE_TMP)->EnableWindow(bPackCacheHDEnable);
	GetDlgItem(IDC_BTN_SEL_PACK_HD_CACHE)->EnableWindow(bCustomPackHDCacheDirEnable);
	if (bMediaCacheHDEnable && !bCustomMediaHDCacheDirEnable) {
		GetDlgItem(IDC_EDIT_MEDIA_HD_CACHE_DIR)->SetWindowText(strTemp);
	}
	else {
		GetDlgItem(IDC_EDIT_MEDIA_HD_CACHE_DIR)->SetWindowText(_T(""));
	}
	if (bPackCacheHDEnable && !bCustomPackHDCacheDirEnable) {
		GetDlgItem(IDC_EDIT_PACK_HD_CACHE_DIR)->SetWindowText(strTemp);
	}
	else {
		GetDlgItem(IDC_EDIT_PACK_HD_CACHE_DIR)->SetWindowText(_T(""));
	}
}

void CConfigDlg::OnBnClickedCheckMediaHdCache()
{
	UpdateUI();
}


void CConfigDlg::OnBnClickedCheckPackHdCache()
{
	UpdateUI();
}


void CConfigDlg::OnBnClickedCheckMediaHdCacheUseTmp()
{
	UpdateUI();
}


void CConfigDlg::OnBnClickedCheckPackHdCacheUseTmp()
{
	UpdateUI();
}

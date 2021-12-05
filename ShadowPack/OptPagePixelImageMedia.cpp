// PixelImageMediaOptionDlg.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPagePixelImageMedia.h"
#include "afxdialogex.h"
#include "PackCipher.h"
#include "PackUtils.h"


// COptPagePixelImageMedia 对话框

IMPLEMENT_DYNAMIC(COptPagePixelImageMedia, CMFCPropertyPage)

COptPagePixelImageMedia::COptPagePixelImageMedia(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_PIXELMEDIA)
	, m_strPasswd1(_T(""))
	, m_strPasswd2(_T(""))
	, m_nCrypto(0)
	, m_nBytePerPixel(0)
	, m_nTotalBlocks(0)
	, m_nUsedBytes(0)
{

}

COptPagePixelImageMedia::~COptPagePixelImageMedia()
{
}

void COptPagePixelImageMedia::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWD1, m_strPasswd1);
	DDX_Text(pDX, IDC_EDIT_PASSWD2, m_strPasswd2);
	DDX_Control(pDX, IDC_COMBO_CRYPTO, m_ctlCrypto);
	DDX_Radio(pDX, IDC_RADIO_1BP1P, m_nBytePerPixel);
	if (pDX->m_bSaveAndValidate && m_strPasswd1.Compare(m_strPasswd2)) {
		AfxMessageBox(_T("Not Match!"));
		pDX->Fail();
	}

	if (pDX->m_bSaveAndValidate) {
		m_nCrypto = m_ctlCrypto.GetCurSel();
	}

	if (pDX->m_bSaveAndValidate && m_nCrypto != 0 && m_strPasswd1.GetLength() == 0) {
		AfxMessageBox(_T("Password can not be null!"));
		pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(COptPagePixelImageMedia, CMFCPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_CRYPTO, &COptPagePixelImageMedia::OnCbnSelchangeComboCrypto)
	ON_BN_CLICKED(IDC_RADIO_1BP4P, &COptPagePixelImageMedia::OnBnClickedRadio1bp4p)
	ON_BN_CLICKED(IDC_RADIO_1BP3P, &COptPagePixelImageMedia::OnBnClickedRadio1bp3p)
	ON_BN_CLICKED(IDC_RADIO_1BP2P, &COptPagePixelImageMedia::OnBnClickedRadio1bp2p)
	ON_BN_CLICKED(IDC_RADIO_1BP1P, &COptPagePixelImageMedia::OnBnClickedRadio1bp1p)
END_MESSAGE_MAP()

void COptPagePixelImageMedia::SetBPPInfo()
{
	INT nID = GetCheckedRadioButton(IDC_RADIO_1BP1P, IDC_RADIO_1BP4P);
	UINT nBPP = 1;
	CString strTotal, strUsed;
	if (nID != 0) {
		nBPP = (nID - IDC_RADIO_1BP1P) + 1;
		TRACE(_T("SetBPPEnable nBPP = %d\n"), nBPP);
		CPackUtils::TranslateSize(m_nTotalBlocks / nBPP, strTotal);
		CPackUtils::TranslateSize(m_nUsedBytes, strUsed);
		GetDlgItem(IDC_STATIC_TOTAL_BYTE)->SetWindowText(strTotal);
		GetDlgItem(IDC_STATIC_USED_BYTE)->SetWindowText(strUsed);
	}
}

BOOL COptPagePixelImageMedia::OnInitDialog()
{
	if (CMFCPropertyPage::OnInitDialog()) {
		for (INT i = 0; i < CPackCipher::GetCipherCount(); i++) {
			m_ctlCrypto.AddString(CPackCipher::GetCipherName(i));
		}
		m_ctlCrypto.SetCurSel(m_nCrypto);
		

		GetDlgItem(IDC_EDIT_PASSWD1)->EnableWindow(m_nCrypto != CPackCipher::CIPHER_NONE);
		GetDlgItem(IDC_EDIT_PASSWD2)->EnableWindow(m_nCrypto != CPackCipher::CIPHER_NONE);

		GetDlgItem(IDC_RADIO_1BP1P)->EnableWindow(m_nTotalBlocks >= m_nUsedBytes);
		GetDlgItem(IDC_RADIO_1BP2P)->EnableWindow(m_nTotalBlocks / 2 >= m_nUsedBytes);
		GetDlgItem(IDC_RADIO_1BP3P)->EnableWindow(m_nTotalBlocks / 3 >= m_nUsedBytes);
		GetDlgItem(IDC_RADIO_1BP4P)->EnableWindow(m_nTotalBlocks / 4 >= m_nUsedBytes);

		SetBPPInfo();

		return TRUE;
	}
	return FALSE;
}
// COptPagePixelImageMedia 消息处理程序


void COptPagePixelImageMedia::OnCbnSelchangeComboCrypto()
{
	CComboBox* p = dynamic_cast<CComboBox*> (GetDlgItem(IDC_COMBO_CRYPTO));
	GetDlgItem(IDC_EDIT_PASSWD1)->EnableWindow(p->GetCurSel() != CPackCipher::CIPHER_NONE);
	GetDlgItem(IDC_EDIT_PASSWD2)->EnableWindow(p->GetCurSel() != CPackCipher::CIPHER_NONE);
}


void COptPagePixelImageMedia::OnBnClickedRadio1bp4p()
{
	SetBPPInfo();
}


void COptPagePixelImageMedia::OnBnClickedRadio1bp3p()
{
	SetBPPInfo();
}


void COptPagePixelImageMedia::OnBnClickedRadio1bp2p()
{
	SetBPPInfo();
}


void COptPagePixelImageMedia::OnBnClickedRadio1bp1p()
{
	SetBPPInfo();
}

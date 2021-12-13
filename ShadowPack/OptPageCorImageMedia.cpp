// OptPageCorImageMedia.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPageCorImageMedia.h"
#include "afxdialogex.h"
#include "PackUtils.h"
#include "PackCipher.h"


// COptPageCorImageMedia 对话框

IMPLEMENT_DYNAMIC(COptPageCorImageMedia, CMFCPropertyPage)

COptPageCorImageMedia::COptPageCorImageMedia(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDD_PROPPAGE_CORMEDIA)
	, m_strPasswd1(_T(""))
	, m_strPasswd2(_T(""))
	, m_nCrypto(0)
	, m_nBytePer2Cor(0)
	, m_nTotalBlocks(0)
	, m_nUsedBytes(0)
{

}

COptPageCorImageMedia::~COptPageCorImageMedia()
{
}

BOOL COptPageCorImageMedia::OnInitDialog()
{
	return 0;
}

void COptPageCorImageMedia::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWD1, m_strPasswd1);
	DDX_Text(pDX, IDC_EDIT_PASSWD2, m_strPasswd2);
	DDX_Control(pDX, IDC_COMBO_CRYPTO, m_ctlCrypto);
	DDX_Radio(pDX, IDC_RADIO_1BP2C, m_nBytePer2Cor);
	if (pDX->m_bSaveAndValidate && m_strPasswd1.Compare(m_strPasswd2)) {
		AfxMessageBox(IDS_PASSWORD_CAN_NOT_MATCH);
		pDX->Fail();
	}

	if (pDX->m_bSaveAndValidate) {
		m_nCrypto = m_ctlCrypto.GetCurSel();
	}

	if (pDX->m_bSaveAndValidate && m_nCrypto != 0 && m_strPasswd1.GetLength() == 0) {
		AfxMessageBox(IDS_PASSWORD_CAN_NOT_NULL);
		pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(COptPageCorImageMedia, CMFCPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_CRYPTO, &COptPageCorImageMedia::OnCbnSelchangeComboCrypto)
	ON_BN_CLICKED(IDC_RADIO_1BP8C, &COptPageCorImageMedia::OnBnClickedRadio1bp8c)
	ON_BN_CLICKED(IDC_RADIO_1BP6C, &COptPageCorImageMedia::OnBnClickedRadio1bp6c)
	ON_BN_CLICKED(IDC_RADIO_1BP4C, &COptPageCorImageMedia::OnBnClickedRadio1bp4c)
	ON_BN_CLICKED(IDC_RADIO_1BP2C, &COptPageCorImageMedia::OnBnClickedRadio1bp2c)
END_MESSAGE_MAP()


// COptPageCorImageMedia 消息处理程序
void COptPageCorImageMedia::OnCbnSelchangeComboCrypto()
{
	CComboBox* p = dynamic_cast<CComboBox*> (GetDlgItem(IDC_COMBO_CRYPTO));
	GetDlgItem(IDC_EDIT_PASSWD1)->EnableWindow(p->GetCurSel() != CPackCipher::CIPHER_NONE);
	GetDlgItem(IDC_EDIT_PASSWD2)->EnableWindow(p->GetCurSel() != CPackCipher::CIPHER_NONE);
}

void COptPageCorImageMedia::SetBPCInfo()
{
	INT nID = GetCheckedRadioButton(IDC_RADIO_1BP2C, IDC_RADIO_1BP8C);
	UINT nBPC = 1;
	CString strTotal, strUsed;
	if (nID != 0) {
		nBPC = (nID - IDC_RADIO_1BP2C) + 1;
		TRACE(_T("SetBPCEnable nBPC = %d\n"), nBPC);
		CPackUtils::TranslateSize(m_nTotalBlocks / nBPC / 2, strTotal);
		CPackUtils::TranslateSize(m_nUsedBytes, strUsed);
		GetDlgItem(IDC_STATIC_TOTAL_BYTE)->SetWindowText(strTotal);
		GetDlgItem(IDC_STATIC_USED_BYTE)->SetWindowText(strUsed);
	}
}

void COptPageCorImageMedia::OnBnClickedRadio1bp8c()
{
	SetBPCInfo();
}


void COptPageCorImageMedia::OnBnClickedRadio1bp6c()
{
	SetBPCInfo();
}


void COptPageCorImageMedia::OnBnClickedRadio1bp4c()
{
	SetBPCInfo();
}


void COptPageCorImageMedia::OnBnClickedRadio1bp2c()
{
	SetBPCInfo();
}

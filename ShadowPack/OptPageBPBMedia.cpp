// OptPageBPBMedia.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPageBPBMedia.h"
#include "afxdialogex.h"
#include "PackCipher.h"
#include "PackUtils.h"
#include "resource.h"

// COptPageBPBMedia 对话框

IMPLEMENT_DYNAMIC(COptPageBPBMedia, CMFCPropertyPage)

COptPageBPBMedia::COptPageBPBMedia(CWnd* pParent, UINT nIDCaption)
	: CMFCPropertyPage(IDD_PROPPAGE_BYTE_PER_BLOCK_MEDIA, nIDCaption)
{

}

COptPageBPBMedia::~COptPageBPBMedia()
{

}

void COptPageBPBMedia::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWD1, m_strPasswd1);
	DDX_Text(pDX, IDC_EDIT_PASSWD2, m_strPasswd2);
	DDX_Control(pDX, IDC_COMBO_CRYPTO, m_ctlCrypto);
	DDX_Radio(pDX, IDC_RADIO_1BP1B, m_nBytePerBlock);
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

void COptPageBPBMedia::SetBPBInfo()
{
	INT nID = GetCheckedRadioButton(IDC_RADIO_1BP1B, IDC_RADIO_1BP4B);
	UINT nBPP = 1;
	CString strTotal, strUsed;
	if (nID != 0) {
		nBPP = (nID - IDC_RADIO_1BP1B) + 1;
		TRACE(_T("SetBPPEnable nBPP = %d\n"), nBPP);
		CPackUtils::TranslateSize(m_nTotalBlocks / nBPP - m_nHeaderSize, strTotal);
		CPackUtils::TranslateSize(m_nUsedBytes, strUsed);
		GetDlgItem(IDC_STATIC_TOTAL_BYTE)->SetWindowText(strTotal);
		GetDlgItem(IDC_STATIC_USED_BYTE)->SetWindowText(strUsed);
	}
}

BOOL COptPageBPBMedia::OnInitDialog()
{
	if (CMFCPropertyPage::OnInitDialog()) {
		for (UINT i = 0; i < CPackCipher::GetCipherCount(); i++) {
			m_ctlCrypto.AddString(CPackCipher::GetCipherName(i));
		}
		m_ctlCrypto.SetCurSel(m_nCrypto);
		

		GetDlgItem(IDC_EDIT_PASSWD1)->EnableWindow(m_nCrypto != CPackCipher::CIPHER_NONE);
		GetDlgItem(IDC_EDIT_PASSWD2)->EnableWindow(m_nCrypto != CPackCipher::CIPHER_NONE);

		GetDlgItem(IDC_RADIO_1BP1B)->EnableWindow(m_nTotalBlocks - m_nHeaderSize >= m_nUsedBytes);
		GetDlgItem(IDC_RADIO_1BP2B)->EnableWindow(m_nTotalBlocks / 2 - m_nHeaderSize >= m_nUsedBytes);
		GetDlgItem(IDC_RADIO_1BP3B)->EnableWindow(m_nTotalBlocks / 3 - m_nHeaderSize >= m_nUsedBytes);
		GetDlgItem(IDC_RADIO_1BP4B)->EnableWindow(m_nTotalBlocks / 4 - m_nHeaderSize >= m_nUsedBytes);

		SetBPBInfo();

		CString strText;
		strText.Format(IDS_BPB, 1, m_strBlockUnit);
		GetDlgItem(IDC_RADIO_1BP1B)->SetWindowText(strText);
		strText.Format(IDS_BPB, 2, m_strBlockUnit);
		GetDlgItem(IDC_RADIO_1BP2B)->SetWindowText(strText);
		strText.Format(IDS_BPB, 3, m_strBlockUnit);
		GetDlgItem(IDC_RADIO_1BP3B)->SetWindowText(strText);
		strText.Format(IDS_BPB, 4, m_strBlockUnit);
		GetDlgItem(IDC_RADIO_1BP4B)->SetWindowText(strText);

		return TRUE;
	}
	return FALSE;
}

BEGIN_MESSAGE_MAP(COptPageBPBMedia, CMFCPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_CRYPTO, &COptPageBPBMedia::OnCbnSelchangeComboCrypto)
	ON_BN_CLICKED(IDC_RADIO_1BP4B, &COptPageBPBMedia::OnBnClickedRadio1bp4b)
	ON_BN_CLICKED(IDC_RADIO_1BP3B, &COptPageBPBMedia::OnBnClickedRadio1bp3b)
	ON_BN_CLICKED(IDC_RADIO_1BP2B, &COptPageBPBMedia::OnBnClickedRadio1bp2b)
	ON_BN_CLICKED(IDC_RADIO_1BP1B, &COptPageBPBMedia::OnBnClickedRadio1bp1b)
END_MESSAGE_MAP()


// COptPageBPBMedia 消息处理程序
void COptPageBPBMedia::OnCbnSelchangeComboCrypto()
{
	CComboBox* p = dynamic_cast<CComboBox*> (GetDlgItem(IDC_COMBO_CRYPTO));
	GetDlgItem(IDC_EDIT_PASSWD1)->EnableWindow(p->GetCurSel() != CPackCipher::CIPHER_NONE);
	GetDlgItem(IDC_EDIT_PASSWD2)->EnableWindow(p->GetCurSel() != CPackCipher::CIPHER_NONE);
}


void COptPageBPBMedia::OnBnClickedRadio1bp4b()
{
	SetBPBInfo();
}


void COptPageBPBMedia::OnBnClickedRadio1bp3b()
{
	SetBPBInfo();
}


void COptPageBPBMedia::OnBnClickedRadio1bp2b()
{
	SetBPBInfo();
}


void COptPageBPBMedia::OnBnClickedRadio1bp1b()
{
	SetBPBInfo();
}

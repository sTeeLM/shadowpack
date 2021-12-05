// PixelImageMediaOptionDlg.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "OptPagePixelImageMedia.h"
#include "afxdialogex.h"
#include "PackCipher.h"


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
	if (m_strPasswd1.Compare(m_strPasswd2)) {
		AfxMessageBox(_T("Not Match!"));
		pDX->Fail();
	}
}


BEGIN_MESSAGE_MAP(COptPagePixelImageMedia, CMFCPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_CRYPTO, &COptPagePixelImageMedia::OnCbnSelchangeComboCrypto)
END_MESSAGE_MAP()

BOOL COptPagePixelImageMedia::OnInitDialog()

{
	if (CMFCPropertyPage::OnInitDialog()) {
		for (INT i = 0; i < CPackCipher::GetCipherCount(); i++) {
			m_ctlCrypto.AddString(CPackCipher::GetCipherName(i));
		}
		m_ctlCrypto.SetCurSel(m_nCrypto);

		GetDlgItem(IDC_EDIT_PASSWD1)->EnableWindow(m_nCrypto != CPackCipher::CIPHER_NONE);
		GetDlgItem(IDC_EDIT_PASSWD2)->EnableWindow(m_nCrypto != CPackCipher::CIPHER_NONE);

		GetDlgItem(IDC_RADIO_1BP1P)->EnableWindow(m_bBPP1);
		GetDlgItem(IDC_RADIO_1BP1P)->EnableWindow(m_bBPP2);
		GetDlgItem(IDC_RADIO_1BP1P)->EnableWindow(m_bBPP3);
		GetDlgItem(IDC_RADIO_1BP1P)->EnableWindow(m_bBPP4);

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

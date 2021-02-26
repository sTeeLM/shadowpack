#pragma once

#include "Resource.h"
#include "OptionDialog.h"

// CRAWPPOptionDialog �Ի���

class CRAWPPOptionDialog : public COptionDialog
{
	DECLARE_DYNAMIC(CRAWPPOptionDialog)

public:
	CRAWPPOptionDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRAWPPOptionDialog();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_OPTION_RAWPP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nFormat;
	BOOL m_bEnable1PP;
	BOOL m_bEnable2PP;
	BOOL m_bEnable3PP;
};

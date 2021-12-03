#pragma once


// CMediaOptionDlg

class CMediaOptionDlg : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CMediaOptionDlg)

public:
	CMediaOptionDlg();
	virtual ~CMediaOptionDlg();
	BOOL OnInitDialog();

protected:
	DECLARE_MESSAGE_MAP()
};



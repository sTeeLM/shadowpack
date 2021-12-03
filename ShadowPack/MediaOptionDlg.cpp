// MediaOptionDlg.cpp: 实现文件
//

#include "pch.h"
#include "ShadowPack.h"
#include "MediaOptionDlg.h"


// CMediaOptionDlg

IMPLEMENT_DYNAMIC(CMediaOptionDlg, CMFCPropertySheet)

CMediaOptionDlg::CMediaOptionDlg()
{

}

CMediaOptionDlg::~CMediaOptionDlg()
{
}

BOOL CMediaOptionDlg::OnInitDialog()
{
    BOOL bResult = CMFCPropertySheet::OnInitDialog();

    int ids[] = { IDOK, IDCANCEL }; //, ID_APPLY_NOW, IDHELP };

     // Hide Apply and Help buttons
    CWnd* pWnd = GetDlgItem(ID_APPLY_NOW);
    pWnd->ShowWindow(FALSE);
    pWnd = GetDlgItem(IDHELP);
    pWnd->ShowWindow(FALSE);

    CRect rectBtn;
    int nSpacing = 6;         // space between two buttons...

    for (int i = 0; i < sizeof(ids) / sizeof(int); i++)
    {
        GetDlgItem(ids[i])->GetWindowRect(rectBtn);

        ScreenToClient(&rectBtn);
        int btnWidth = rectBtn.Width();
        rectBtn.left = rectBtn.left + (btnWidth + nSpacing) * 2;
        rectBtn.right = rectBtn.right + (btnWidth + nSpacing) * 2;

        GetDlgItem(ids[i])->MoveWindow(rectBtn);
    }

    return bResult;
}


BEGIN_MESSAGE_MAP(CMediaOptionDlg, CMFCPropertySheet)
END_MESSAGE_MAP()



// CMediaOptionDlg 消息处理程序



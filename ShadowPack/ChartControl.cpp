// PackChart.cpp : 实现文件
//
#include "pch.h"
#include "framework.h"
#include "ShadowPack.h"
#include "ChartControl.h"
#include "math.h"


// CChartControl

IMPLEMENT_DYNAMIC(CChartControl, CStatic)

CChartControl::CChartControl() :
	m_nPercent(100)
	, m_bEnable(FALSE)
{

}

CChartControl::~CChartControl()
{
}


BEGIN_MESSAGE_MAP(CChartControl, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CChartControl 消息处理程序

void CChartControl::Initialize(CWnd * pParent, UINT nID)
{
	SubclassDlgItem(nID, pParent);

	LONG lEx = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);

	lEx |= SS_OWNERDRAW;

	lEx = ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, lEx);
}

void CChartControl::SetFreePercent(INT nPercent, BOOL bEnable/*  = TRUE */)
{
	if(nPercent >= 0 && nPercent <= 100) {
		m_nPercent = nPercent;
	}
	m_bEnable = bEnable;
	Invalidate();
}

void CChartControl::OnPaint( )
{
	PAINTSTRUCT ps;
	CRect rect;

	CBrush brushBlue, brushRed, brushGray;
	CBrush* pOldBrush;

	double x, y, free;
	double pi = 3.1415926535;
	INT nX, nY;

	GetClientRect(&rect);

	brushBlue.CreateSolidBrush(RGB(0, 0, 255));
	brushRed.CreateSolidBrush( RGB(255, 0, 255));
	brushGray.CreateSolidBrush( RGB(128, 128, 128));

	CDC* pDC = CWnd::BeginPaint(&ps);

	if(m_bEnable) {
		pOldBrush = pDC->SelectObject(&brushBlue);

		pDC->Ellipse(&rect);

		pDC->SelectObject(&brushRed);

		free = 2 * pi * m_nPercent / 100;
		if(free == 0.0) {
			free = 0.01;
		}

		x = ::cos(free) * 200;
		y = ::sin(free) * 200;

		//shift
		nX = rect.Width() / 2 + (int)x;
		nY = rect.Height() / 2 - (int)y;


		pDC->Pie(&rect, CPoint(rect.Width(),rect.Height() / 2),CPoint(nX,nY));

		pDC->SelectObject(pOldBrush);
	} else {
		pOldBrush = pDC->SelectObject(&brushGray);
		pDC->Ellipse(&rect);
		pDC->SelectObject(pOldBrush);

	}
	CWnd::EndPaint(&ps);
}
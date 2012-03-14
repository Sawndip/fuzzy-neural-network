// MyScrollChart.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "MyScrollChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyScrollChart

IMPLEMENT_DYNCREATE(CMyScrollChart, CView)

CMyScrollChart::CMyScrollChart()
{
}

CMyScrollChart::~CMyScrollChart()
{
}


BEGIN_MESSAGE_MAP(CMyScrollChart, CView)
	//{{AFX_MSG_MAP(CMyScrollChart)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_MESSAGE	  (WM_NEWVERTLIMITS,OnNewLimits)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyScrollChart drawing

void CMyScrollChart::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMyScrollChart diagnostics

#ifdef _DEBUG
void CMyScrollChart::AssertValid() const
{
	CView::AssertValid();
}

void CMyScrollChart::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyScrollChart message handlers
void CMyScrollChart::SetScrollChart(int method)
{
	mySChart.Create(WS_VISIBLE | GS_NOTIFY,CRect(60,0,460,160),this,500);

	mySChart.SetBkColor(0xf0f0f0);
	mySChart.SetGrid(FALSE);

	// error curve
	mySChart.AddLine(1, RGB(255,0,0), LS_HORZAUTO);
	mySChart.SetAutoinc(1,1);

	// error limit
	mySChart.AddLine(2, RGB(0,0,255), LS_HORZAUTO);
	mySChart.SetAutoinc(2,1);

	m_TextVerMax.Create("9999",SS_RIGHT|WS_CHILD | WS_VISIBLE,CRect(0,0,60,20),this);
	m_TextVerMin.Create("0",SS_RIGHT|WS_CHILD | WS_VISIBLE,CRect(0,140,60,160),this);
	
	m_TextHozMin.Create("0",SS_LEFT|WS_CHILD | WS_VISIBLE,CRect(60,160,100,180),this);         
	m_TextHozMax.Create("9999",SS_RIGHT|WS_CHILD | WS_VISIBLE,CRect(400,160,460,180),this);         

	fontText.CreatePointFont(85,"Courier New",GetDC( ) );
	
	m_TextHozMax.SetFont(&fontText);
	m_TextVerMin.SetFont(&fontText);
	m_TextVerMax.SetFont(&fontText);
	m_TextHozMin.SetFont(&fontText);
}

void CMyScrollChart::OnNewLimits(WPARAM wParam, LPARAM lParam)
{
	CString strTmp;
	
	float *min, *max;

	if ( (GetCurrentMessage()->message) == WM_NEWVERTLIMITS)
	{
		min = (float*)wParam;
		max = (float*)lParam;
		
		strTmp.Format("%.4f", *min);
		m_TextVerMin.SetWindowText(strTmp);
		strTmp.Format("%.4f", *max);
		m_TextVerMax.SetWindowText(strTmp);
	}
	else
	{
		strTmp.Format("%d", wParam);
		m_TextHozMin.SetWindowText(strTmp);
		strTmp.Format("%d", lParam);
		m_TextHozMax.SetWindowText(strTmp);
	}
}
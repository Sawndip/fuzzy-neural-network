// DJdummyView.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "DJdummyView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DJdummyView

IMPLEMENT_DYNCREATE(DJdummyView, CView)

DJdummyView::DJdummyView()
{
}

DJdummyView::~DJdummyView()
{
}


BEGIN_MESSAGE_MAP(DJdummyView, CView)
	//{{AFX_MSG_MAP(DJdummyView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DJdummyView drawing

void DJdummyView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// DJdummyView diagnostics

#ifdef _DEBUG
void DJdummyView::AssertValid() const
{
	CView::AssertValid();
}

void DJdummyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DJdummyView message handlers
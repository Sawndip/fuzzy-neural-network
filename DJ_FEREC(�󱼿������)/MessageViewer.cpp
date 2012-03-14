// MessageViewer.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "MessageViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageViewer

IMPLEMENT_DYNCREATE(CMessageViewer, CScrollView)

CMessageViewer::CMessageViewer()
{
	SetScrollSizes(MM_TEXT, CSize(100, 100));
}

CMessageViewer::~CMessageViewer()
{
}


BEGIN_MESSAGE_MAP(CMessageViewer, CScrollView)
	//{{AFX_MSG_MAP(CMessageViewer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageViewer drawing

void CMessageViewer::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CMessageViewer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMessageViewer diagnostics

#ifdef _DEBUG
void CMessageViewer::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMessageViewer::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMessageViewer message handlers
void CMessageViewer::MessageView(CString str)
{
	SetWindowText(str);
}

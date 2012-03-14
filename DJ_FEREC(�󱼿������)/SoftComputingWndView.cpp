// SoftComputingWndView.cpp : implementation of the CSoftComputingWndView class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"

#include "SoftComputingWndDoc.h"
#include "SoftComputingWndView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndView

IMPLEMENT_DYNCREATE(CSoftComputingWndView, CView)

BEGIN_MESSAGE_MAP(CSoftComputingWndView, CView)
	//{{AFX_MSG_MAP(CSoftComputingWndView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndView construction/destruction

CSoftComputingWndView::CSoftComputingWndView()
{
	// TODO: add construction code here

}

CSoftComputingWndView::~CSoftComputingWndView()
{
}

BOOL CSoftComputingWndView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndView drawing

void CSoftComputingWndView::OnDraw(CDC* pDC)
{
	CSoftComputingWndDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndView diagnostics

#ifdef _DEBUG
void CSoftComputingWndView::AssertValid() const
{
	CView::AssertValid();
}

void CSoftComputingWndView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSoftComputingWndDoc* CSoftComputingWndView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoftComputingWndDoc)));
	return (CSoftComputingWndDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndView message handlers

/*
void CSoftComputingWndView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CSoftComputingWndDoc* myDoc = GetDocument();

	switch(nChar)
	{
		case '0':
			myDoc->SetID("B");
			break;
		case 'a':
			myDoc->GrabImage(1);
			break;
		case '2':
			myDoc->GrabImage(2);
			break;
		case '3':
			myDoc->GrabImage(3);
			break;
		case '4':
			myDoc->GrabImage(4);
			break;
		case '5':
			myDoc->GrabImage(5);
			break;
		case '6':
			myDoc->GrabImage(6);
			break;
		case '7':
			myDoc->GrabImage(7);
			break;
		default:
			break;
	}
	
	CView::OnChar(nChar, nRepCnt, nFlags);
}
*/

#include "stdafx.h"
#include "HMMDemo.h"

#include "HMMDemoDoc.h"
#include "HMMDemoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHMMDemoView

IMPLEMENT_DYNCREATE(CHMMDemoView, CView)

BEGIN_MESSAGE_MAP(CHMMDemoView, CView)
	//{{AFX_MSG_MAP(CHMMDemoView)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHMMDemoView construction/destruction

CHMMDemoView::CHMMDemoView()
{
    m_sel = m_tmp_sel = CRect(0,0,0,0);
    m_imageList = 0;
    m_TestPath = "";  
}

CHMMDemoView::~CHMMDemoView()
{
    SetImageList(0);
}

BOOL CHMMDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CHMMDemoView drawing

void CHMMDemoView::OnDraw(CDC* pDC)
{
	CHMMDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
    
    HDC dstDC = pDC->m_hDC;
    CImage& frame = m_camera.GetFrame();

    CRect r;
    GetClipBox( dstDC, &r );

    if( m_imageList && m_imageList->GetCount() > 1 ) //several images was selected
    {
        pDC->Draw3dRect(0,0,frame.Width(),frame.Height(),RGB(0,0,0),RGB(128,128,128));
    }

    frame.Show( dstDC, r.left, r.top, r.Width(), r.Height(), r.left, r.top ); 

    if( m_sel.Width() != 0 && m_sel.Height() != 0 )
    {
        HGDIOBJ old = SelectObject( dstDC, GetStockObject( WHITE_PEN ));
        POINT pt[] = {
            { m_sel.left - r.left, m_sel.top - r.top },
            { m_sel.right - r.left - 1, m_sel.top - r.top },
            { m_sel.right - r.left - 1, m_sel.bottom - r.top - 1},
            { m_sel.left - r.left, m_sel.bottom - r.top - 1},
            { m_sel.left - r.left, m_sel.top - r.top }
        };

        Polyline( dstDC, pt, 5 );
        SelectObject( dstDC, old );
    }

    //::ReleaseDC( m_hWnd, dstDC );
}

/////////////////////////////////////////////////////////////////////////////
// CHMMDemoView message handlers

void CHMMDemoView::OnInitialUpdate() 
{
    CView::OnInitialUpdate();

    if( m_camera.Initialize( 320, 240, -1, m_hWnd ) == 0 )
    {
        MessageBox("Can't initialize camera. Try to change format","Error", MB_OK|MB_ICONERROR );
    }
    Camera().Start();
}

void CHMMDemoView::OnDestroy() 
{
    m_camera.Uninitialize();
    CView::OnDestroy();
}

void CHMMDemoView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    CView::OnLButtonDown(nFlags, point);                                       
    /*(CPoint&)m_sel = */(CPoint&)m_tmp_sel.left = (CPoint&)m_tmp_sel.right = point;
    CheckUpdate();
}

void CHMMDemoView::OnLButtonUp(UINT nFlags, CPoint point) 
{
    CView::OnLButtonUp(nFlags, point);
    (CPoint&)m_tmp_sel.right = point;
    CheckUpdate();
}

void CHMMDemoView::OnMouseMove(UINT nFlags, CPoint point) 
{
    CView::OnMouseMove(nFlags, point);
    if( nFlags & MK_LBUTTON )
    {
        (CPoint&)m_tmp_sel.right = point;
        CheckUpdate();
    }
}


void CHMMDemoView::CheckUpdate()
{
    m_sel = NormalizeRect( m_tmp_sel );
    if( !Camera().IsRunning() )
    {
        CImage& img = Camera().GetFrame();
        CRect ir( 0, 0, img.Width(), img.Height() );

        InvalidateRect(&ir, FALSE);
        UpdateWindow();
    }
}

BOOL CHMMDemoView::OnEraseBkgnd(CDC* pDC) 
{
	CImage& img = Camera().GetFrame();
    CRect ir( 0, 0, img.Width(), img.Height() );
    CRect r;
    pDC->GetClipBox( &r );

    if( ir.PtInRect( r.TopLeft()) && ir.PtInRect( r.BottomRight()))
        return TRUE;

    return CView::OnEraseBkgnd(pDC);
}

void CHMMDemoView::SetSelection( RECT* sel )
{
    if( sel )
    {
        m_tmp_sel = NormalizeRect( *sel );
    }
    else
    {
        CImage& img = Camera().GetFrame();
        m_tmp_sel = CRect( 0, 0, img.Width(), img.Height() );
    }
    CheckUpdate();
}

void  CHMMDemoView::SetImageList( CStringList* imageList )
{
    if( m_imageList ) delete m_imageList;
    m_imageList = imageList;
}


BOOL CHMMDemoView::SetTestPath(char* path)
{
   m_TestPath = path;
   return TRUE;
}


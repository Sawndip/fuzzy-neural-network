// MyCamView.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "MyCamView.h"

#include "MainFrm.h"
#include "SoftComputingWndDoc.h"
//#include "cv.h"
//#include "highgui.h"

#include "FRTK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyCamView

IMPLEMENT_DYNCREATE(CMyCamView, CView)

CMyCamView::CMyCamView()
{
}

CMyCamView::~CMyCamView()
{
}


BEGIN_MESSAGE_MAP(CMyCamView, CView)
	//{{AFX_MSG_MAP(CMyCamView)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyCamView drawing

void CMyCamView::OnDraw(CDC* pDC)
{
	HDC dstDC = pDC->m_hDC;
    CImage& frame = m_camera.GetFrame();

    CRect r;
    GetClipBox( dstDC, &r );

	/*
    if( m_imageList && m_imageList->GetCount() > 1 ) //several images was selected
    {
        pDC->Draw3dRect(0,0,frame.Width(),frame.Height(),RGB(0,0,0),RGB(128,128,128));
    }
	*/

    frame.Show( dstDC, r.left, r.top, r.Width(), r.Height(), r.left, r.top );
	
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	((CSoftComputingWndDoc*)mainframe->GetActiveDocument())->m_image = frame.GetImage();

	/*
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
	*/

}

/////////////////////////////////////////////////////////////////////////////
// CMyCamView diagnostics

#ifdef _DEBUG
void CMyCamView::AssertValid() const
{
	CView::AssertValid();
}

void CMyCamView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyCamView message handlers

void CMyCamView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
    if( m_camera.Initialize( 160, 120, -1, m_hWnd ) == 0 )
    {
        MessageBox("Can't initialize camera. Try to change format","Error", MB_OK|MB_ICONERROR );
    }
}

void CMyCamView::OnDestroy() 
{
    m_camera.Uninitialize();
    CView::OnDestroy();
}

void CMyCamView::CameraOn()
{
	Camera().Start();
}

void CMyCamView::CameraOff()
{
	Camera().Stop();
}

void CMyCamView::CameraFormat()
{
	Camera().VideoFormatDlg();
}

void CMyCamView::CameraSetting()
{
	Camera().VideoSourceDlg();
}

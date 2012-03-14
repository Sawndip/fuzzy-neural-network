#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "ImageBaseView.h"
#include "cv.h"
#include "highgui.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageBaseView

IMPLEMENT_DYNCREATE(CImageBaseView, CScrollView)

CImageBaseView::CImageBaseView()
{
    m_canvas.Create( GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 8 );
    m_rgbcanvas.Create( GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 24 );
    
    IplImage* rgb = m_rgbcanvas.GetImage();
    strcpy( rgb->colorModel, "RGB" );
    strcpy( rgb->channelSeq, "BGR" );

    IplImage* gray = m_canvas.GetImage();
    strncpy( gray->colorModel, "GRAY", 4 );
    strncpy( gray->channelSeq, "GRAY", 4 );


    m_base_size = CSize( 100, 120 );
    m_log_shift = 0;
    RecalcPictureSize();
    m_index = -1;
    m_active = -1;
    m_pen = CreatePen( PS_SOLID, 3, RGB(255,0,255));

    m_view_mode = 0;

    m_count = 0;
    m_nx = m_ny = 0;

	clear_flag = TRUE;
}

CImageBaseView::~CImageBaseView()
{
	imageList.RemoveAll();

    DeleteObject( m_pen );
}

void CImageBaseView::SetImagePlane(int nx, int ny)
{
	m_nx = nx;
	m_ny = ny;

	for (int i=0; i<m_nx*m_ny; i++)
	{
		CImage* img = new CImage;
		imageList.AddTail((CImage*)img);
	}
}

BEGIN_MESSAGE_MAP(CImageBaseView, CView)
    //{{AFX_MSG_MAP(CImageBaseView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageBaseView drawing

void  CImageBaseView::RefreshView()
{
	/*
    CHMMDemoDoc* doc = GetDocument();
    CSize virt_size = CSize( 10, 10 );

    if( doc )
    {
        CRect win_rect;
        if( m_index < 0 )
        {
            m_count = doc->GetFaceBase().GetPersonList().GetCount();
        }
        else
        {
            CPerson* person = doc->GetFaceBase().GetPerson( m_index );
            if( !person )
            {
                ASSERT(0);
                return;
            }
            m_count = person->GetImgList().GetCount();
        }

        GetClientRect( &win_rect );

        m_margin = CSize((m_pic_delta.cx - m_pic_size.cx)/2,
                         (m_pic_delta.cy - m_pic_size.cy)/2);

        m_nx = (win_rect.Width() - m_margin.cx +
                m_pic_delta.cx - m_pic_size.cx)/m_pic_delta.cx;

        m_nx = MAX( m_nx, 1 );

        if( m_nx != 0 )
        {
            m_ny = (m_count + m_nx - 1)/m_nx;
            virt_size.cx = win_rect.Width();
            virt_size.cy = MAX( m_ny, 1 )*m_pic_delta.cy;
        }
    }
    SetScrollSizes( MM_TEXT, virt_size );
	*/
    InvalidateRect(0);
}

void CImageBaseView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
    m_count = m_nx = m_ny = 0;
    m_active = -1;
    m_index = -1;


    //RefreshView();
	InvalidateRect(0);
}

void CImageBaseView::OnDraw(CDC* pDC)
{
    HDC dc;
	// get DC
    dc = ::GetDC( m_hWnd );

	// client_win_size
    CRect win_rect;
    CSize win_size;
    GetClientRect( &win_rect );
    win_size = CSize( win_rect.Width(), win_rect.Height() );

	if ((!m_nx && !m_ny) || clear_flag)
	{
		// fill canvas
		m_canvas.Fill( GetSysColor( COLOR_WINDOW ));

		// draw canvas
		IplImage* src = m_canvas.GetImage();
		IplImage* dst = m_rgbcanvas.GetImage();
		CvRect sr = cvGetImageROI( src ), dr = cvGetImageROI( dst );

		cvSetImageROI( src, RectToCvRect( win_rect ));
		cvSetImageROI( dst, RectToCvRect( win_rect ));

		cvCvtColor( src, dst, CV_GRAY2BGR );
		cvSetImageROI( src, sr );
		cvSetImageROI( dst, dr );

		m_rgbcanvas.Show( dc, 0, 0, win_size.cx, win_size.cy, 0, 0 );

		clear_flag = FALSE;
	}

	if (m_nx && m_ny)
	{
		for (int j=0; j<m_ny; j++)
			for (int i=0; i<m_nx; i++)
			{
				CImage *img = (CImage*)imageList.GetAt(imageList.FindIndex(i + j*m_nx));

				// draw image
				CPoint pt = CPoint(i*(win_size.cx/m_nx), j*(win_size.cy/m_ny));
				CSize size = CSize(win_size.cx/m_nx, win_size.cy/m_ny);
				myrect.SetRect(pt.x, pt.y, pt.x + size.cx, pt.y + size.cy);

				SetStretchBltMode( dc, COLORONCOLOR );
				img->DrawToHDC(dc, &myrect);

				//delete img;
			}
	}

	// releaseDC
    ::ReleaseDC( m_hWnd, dc );
}

void CImageBaseView::ShowImage(bool clear)
{
	clear_flag = clear;

	InvalidateRect(0);
}

void CImageBaseView::AddImage(const int idx, const int idy, IPLIMAGE inimage)
{
	if ((idx > -1 && idx < m_nx) && (idy > -1 && idy < m_ny))
	{
		CImage *img = new CImage;
		img->CopyOf(inimage);

		//CImage *tmp = (CImage*) imageList.GetAt(imageList.FindIndex(idx + idy*m_nx));
		imageList.SetAt( imageList.FindIndex(idx + idy*m_nx), (CImage*)img );
		//delete tmp;
	}
	else 
		return;
}

/////////////////////////////////////////////////////////////////////////////
// CImageBaseView message handlers

void  CImageBaseView::RecalcPictureSize()
{
    double scale = pow( sqrt(2), m_log_shift );
    m_pic_size.cx = (int)(m_base_size.cx*scale + 0.5);
    m_pic_size.cy = (int)(m_base_size.cy*scale + 0.5);
    m_pic_delta.cx = m_pic_size.cx*5/4;
    m_pic_delta.cy = m_pic_size.cy*5/4;
}

void  CImageBaseView::Zoom( bool zoom_in )
{
    if( zoom_in )
    {
        m_log_shift = MIN( ZOOM_MAX, m_log_shift + 1 );
    }
    else
    {
        m_log_shift = MAX( ZOOM_MIN, m_log_shift - 1 );
    }
    RecalcPictureSize();
    RefreshView();
    InvalidateRect(0);
}


void CImageBaseView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);
    RefreshView();
}


BOOL CImageBaseView::OnEraseBkgnd(CDC* /*pDC*/)
{
    return TRUE;
}


CRect CImageBaseView::GetRectByIndex( int index )
{
    CRect rect = CRect(0,0,0,0);
    if( (unsigned)index >= (unsigned)m_count ) return rect;

    rect.left = index % m_nx;
    rect.top = index / m_nx;

    rect.left = rect.left*m_pic_delta.cx + m_margin.cx;
    rect.top = rect.top*m_pic_delta.cy + m_margin.cy;

    rect.right = rect.left + m_pic_size.cx;
    rect.bottom = rect.top + m_pic_size.cy;

    return rect;
}

/*
int  CImageBaseView::GetIndexByPoint( CPoint pt )
{
    int y_pos = GetScrollPosition().y;

    int x = (pt.x - m_margin.cx)/m_pic_delta.cx;
    int y = (pt.y + y_pos - m_margin.cy)/m_pic_delta.cy;
    int index;

    if( x >= m_nx || y >= m_ny ) return -1;

    if( pt.x > (x+1)*m_pic_delta.cx - m_margin.cx ||
        y_pos + pt.y > (y+1)*m_pic_delta.cy - m_margin.cy ) return -1;

    index = y*m_nx + x;
    if( index >= m_count ) return -1;

    return index;
}
*/

/*
void CImageBaseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CScrollView::OnLButtonDown(nFlags, point);

    int index = GetIndexByPoint( point );
    if( index >= 0 )
    {
        m_active = index;
    }
    InvalidateRect(0);
}
*/


void CImageBaseView::SwitchMode( int index, bool to_personal )
{
	/*
    CHMMDemoDoc* doc = GetDocument();

    if( doc )
    {
        CPerson* person = doc->GetFaceBase().GetPerson( index );
        if( !person )
        {
            ASSERT(0);
            return;
        }
        if( to_personal )
        {
            person->LoadRest();
            m_index = index;
            m_active = -1;
            m_view_mode = VIEW_PERSON;
        }
        else
        {
            if( person->IsModified())
            {
                person->Save();
            }
            person->UnloadRest();
            m_index = -1;
            m_active = index;
            m_view_mode = VIEW_BASE;
        }
        RefreshView();
    }
	*/
}

/*
void CImageBaseView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    CScrollView::OnLButtonDblClk(nFlags, point);

    int index = GetIndexByPoint( point );
    if( index >= 0 && m_index == -1 )
    {
        SwitchMode( index, true );
    }
    else if( index < 0 && m_index >= 0 )
    {
        SwitchMode( m_index, false );
    }
}
*/

int CImageBaseView::GetMode()
{
    return m_view_mode;
}
#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "ImageBaseView.h"

#include "cv.h"
#include "highgui.h"
#include <math.h>

#include "MainFrm.h"
#include "SoftComputingWndDoc.h"

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

	m_X = m_Y = 1.0f;

	ptcount = 0;

	m_bMagnify = m_bRegionSelect = FALSE;
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
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
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

	//CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	if (m_nx && m_ny)
	{
		for (int j=0; j<m_ny; j++)
			for (int i=0; i<m_nx; i++)
			{
				CImage *img = (CImage*)imageList.GetAt(imageList.FindIndex(i + j*m_nx));

				if ((img->GetImage() == NULL) ||
					((img->GetImage())->width <= 0) || 
					((img->GetImage())->height <= 0))	return;

				m_X = 1.0f/((float)win_size.cx/(float)((img->GetImage())->width));
				m_Y = 1.0f/((float)win_size.cy/(float)((img->GetImage())->height));

				// draw image
				CPoint pt = CPoint(i*(win_size.cx/m_nx), j*(win_size.cy/m_ny));
				CSize size = CSize(win_size.cx/m_nx, win_size.cy/m_ny);
				myrect.SetRect(pt.x, pt.y, pt.x + size.cx, pt.y + size.cy);

				CvPoint pt1 = cvPoint(m_Tracker.m_rect.TopLeft().x * m_X, m_Tracker.m_rect.TopLeft().y * m_Y);
				CvPoint pt2 = cvPoint(m_Tracker.m_rect.BottomRight().x * m_X, m_Tracker.m_rect.BottomRight().y * m_Y);

				cvRectangle(img->GetImage(), pt1, pt2, 0x00, 2);

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

void CImageBaseView::ShowSingleImage(IplImage* inimage, bool clear)
{
	clear_flag = clear;

	CImage *img = new CImage;
	img->CopyOf(inimage);

	imageList.SetAt( imageList.FindIndex(0), (CImage*)img );

	InvalidateRect(0);
}

IPLIMAGE CImageBaseView::GetSingleImage()
{
	CImage *img = (CImage*)imageList.GetAt(imageList.FindIndex(0));

	return (img->GetImage());
}

void CImageBaseView::AddImage(const int idx, const int idy, IplImage* inimage)
{
	if ((idx > -1 && idx < m_nx) && (idy > -1 && idy < m_ny))
	{
		CImage *img = new CImage;
		img->CopyOf(inimage);

		//CvImage *tmp = (CvImage*) imageList.GetAt(imageList.FindIndex(idx + idy*m_nx));
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

void CImageBaseView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    CView::OnLButtonDblClk(nFlags, point);

	/*
	CMainFrame* main = (CMainFrame*)AfxGetMainWnd();
	CvImage *img = (CvImage*)imageList.GetAt(imageList.FindIndex(0));

	IPLIMAGE cimage = cvCreateImage(cvSize((img->GetImage())->width, (img->GetImage())->height), IPL_DEPTH_8U, 3);
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	if ((img->GetImage())->nChannels == 1)
	{
		cvvConvertImage(img->GetImage(), cimage);
		iplCopy(cimage, image);
	}
	else
		iplCopy(img->GetImage(), image);
		*/
}

void CImageBaseView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMainFrame* main = (CMainFrame*)AfxGetMainWnd();
	CImage *img = (CImage*)imageList.GetAt(imageList.FindIndex(0));

	IPLIMAGE cimage = cvCreateImage(cvSize((img->GetImage())->width, (img->GetImage())->height), IPL_DEPTH_8U, 3);
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	if ((img->GetImage())->nChannels == 1)
	{
		cvvConvertImage(img->GetImage(), cimage);
		iplCopy(cimage, image);
	}
	else
		iplCopy(img->GetImage(), image);

	(main->GetMemoryImage(0))->SetImagePlane(1,1);
	(main->GetMemoryImage(0))->ShowSingleImage(image);
	
	CView::OnRButtonDown(nFlags, point);
}

BOOL CImageBaseView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bRegionSelect)
	{
		if(pWnd == this && m_Tracker.SetCursor(this, nHitTest))
			return TRUE;
		else
			::SetCursor(::LoadCursor(NULL, IDC_CROSS));
	}
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	return TRUE;
	//return CView::OnSetCursor(pWnd, nHitTest, message);
}

#define	RECTSIZE	160

void CImageBaseView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bMagnify)
	{
		CClientDC dc(this);
		CDC MemDC;

		MemDC.CreateCompatibleDC(&dc);
		CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&m_bmpMagnify);
			
		dc.BitBlt(m_ptOld.x-RECTSIZE/2,m_ptOld.y-RECTSIZE/2,RECTSIZE,RECTSIZE,
			&MemDC, 0, 0, SRCCOPY);

		m_bmpMagnify.DeleteObject();
		MemDC.SelectObject(pOldBitmap);
		m_bMagnify = FALSE;	
		ShowCursor(TRUE);
		ClipCursor(NULL);
	}
	
	CView::OnLButtonUp(nFlags, point);
}

void CImageBaseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	CDC MemDC;
	CPen redPen(PS_SOLID,1,RGB(255,0,0)), *pOldPen;
	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	ClipCursor(&rect);

	if ((m_bRegionSelect) && (ptcount < 14))
	{	
		if (m_Tracker.HitTest(point) < 0)
		{
			m_Tracker.TrackRubberBand(this, point, TRUE);
		}
		else 
		{
			m_Tracker.Track(this, point, TRUE);
		}
		m_Tracker.m_rect.NormalizeRect();

		UpdatePoint();

		ClipCursor(NULL);
		InvalidateRect(NULL);
	}
	else
	{
		ShowCursor(FALSE);

		MemDC.CreateCompatibleDC(&dc);
		m_bmpMagnify.CreateCompatibleBitmap(&dc, RECTSIZE, RECTSIZE);
		CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&m_bmpMagnify);
			
		MemDC.BitBlt(0,0,RECTSIZE,RECTSIZE,&dc, point.x-RECTSIZE/2, point.y-RECTSIZE/2, SRCCOPY);
			
		pOldPen = (CPen *)dc.SelectObject(&redPen);
		dc.SelectObject(GetStockObject(NULL_BRUSH));
			
		dc.StretchBlt(point.x-RECTSIZE/2,point.y-RECTSIZE/2,RECTSIZE,RECTSIZE,&MemDC, RECTSIZE/4, RECTSIZE/4, RECTSIZE/2, RECTSIZE/2, SRCCOPY);
			
		dc.Rectangle(point.x-RECTSIZE/2,point.y-RECTSIZE/2,point.x+RECTSIZE/2,point.y+RECTSIZE/2);
				
		dc.SelectObject(pOldPen);
		MemDC.SelectObject(pOldBitmap);
		m_ptOld = point;
		m_bMagnify = TRUE;	
	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CImageBaseView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bMagnify)
	{
		CClientDC dc(this);
		CDC MemDC;
		CPen redPen(PS_SOLID,1,RGB(255,0,0)), *pOldPen;
		MemDC.CreateCompatibleDC(&dc);
		CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&m_bmpMagnify);
			
		pOldPen = (CPen *)dc.SelectObject(&redPen);
		dc.SelectObject(GetStockObject(NULL_BRUSH));
			
		dc.BitBlt(m_ptOld.x-RECTSIZE/2,m_ptOld.y-RECTSIZE/2,RECTSIZE,RECTSIZE,&MemDC, 0, 0, SRCCOPY);

		MemDC.BitBlt(0,0,RECTSIZE,RECTSIZE,
			&dc, point.x-RECTSIZE/2, point.y-RECTSIZE/2, SRCCOPY);

		dc.StretchBlt(point.x-RECTSIZE/2,point.y-RECTSIZE/2,RECTSIZE,RECTSIZE,	&MemDC, RECTSIZE/4, RECTSIZE/4, RECTSIZE/2, RECTSIZE/2, SRCCOPY);
		
		dc.Rectangle(point.x-RECTSIZE/2,point.y-RECTSIZE/2,point.x+RECTSIZE/2,point.y+RECTSIZE/2);
		dc.SelectObject(pOldPen);
		MemDC.SelectObject(pOldBitmap);
		
		m_ptOld = point;
	}
	
	ShowPixelInfo(point);

	CView::OnMouseMove(nFlags, point);
}

void CImageBaseView::ShowPixelInfo(CPoint point)
{
	point.x = (long)((float)point.x * m_X);
	point.y = (long)((float)point.y * m_Y);

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	CString strText;

	if (imageList.IsEmpty())	return;

	CImage *image = (CImage*)imageList.GetAt(imageList.FindIndex(0));
	IPLIMAGE img = image->GetImage();

	if (img == NULL) return;

	if(img->nChannels == 1)
	{
		strText.Format("%d x %d, Grayscale", img->width, img->height);
	}
	else
	{
		strText.Format("%d x %d, TRUE color", img->width, img->height);
	}
	pFrame->m_wndStatusBar.SetPaneText(3, strText);

	if(point.x < img->width && point.y < img->height)
	{
		strText.Format("(%d, %d)", point.x, point.y);
		pFrame->m_wndStatusBar.SetPaneText(1, strText);

		if(img->nChannels == 1)
		{
			uchar pixel;

			pixel = ((uchar*)(img->imageData + img->widthStep*point.y))[point.x];

			strText.Format("Brightness = %d", pixel);
			pFrame->m_wndStatusBar.SetPaneText(2, strText);
		}
		else
		{
			uchar* pixel = &((uchar*)(img->imageData + img->widthStep*point.y))[point.x*3];

			strText.Format("RGB(%d, %d, %d)", pixel[0], pixel[1], pixel[2]);
			pFrame->m_wndStatusBar.SetPaneText(2, strText);
		}
	}
	else
	{
		pFrame->m_wndStatusBar.SetPaneText(1, "");
		pFrame->m_wndStatusBar.SetPaneText(2, "");
	}
}

void CImageBaseView::UpdatePoint()
{
	CMainFrame *main = (CMainFrame *)AfxGetMainWnd();

	if ((m_bRegionSelect) && (ptcount<14))
	{
		pt[ptcount].x = m_Tracker.m_rect.TopLeft().x * m_X;
		pt[ptcount].y = m_Tracker.m_rect.TopLeft().y * m_Y;
		pt[ptcount+1].x = m_Tracker.m_rect.BottomRight().x * m_X;
		pt[ptcount+1].y = m_Tracker.m_rect.BottomRight().y * m_Y;

		ptcount += 2;

		CString strText;
		strText.Format("PtCnt = %d", ptcount);
		main->m_wndStatusBar.SetPaneText(0, strText);

		/*
		if (ptcount > 11)
		{
			ptcount = 0;

			CLogView* logview = main->GetDJMessage();
			CString str, strTmp;
			str.Format("");

			for (int i=0; i<12; i++)
			{
				strTmp.Format("[%3d,%3d] ", pt[i].x, pt[i].y);
				str += strTmp;
			}
			
			CFileDialog *pDlg = new CFileDialog( TRUE, NULL, NULL, OFN_READONLY, 
 		         "얼굴패턴파일 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

			if( pDlg->DoModal() )
			{
				CString filename = pDlg->GetFileName();
				
				if( filename != "" )  
				{
					FILE *fp;

					fp = fopen(filename, "wt");

					fprintf(fp, "12 \n");

					for (int j=0; j<12; j++)
					{
						fprintf(fp, "%d %d \n", pt[j].x, pt[j].y);
					}

					fclose(fp);
				}
			}
		
			logview->AddLine(str);
		}
		*/
	}
}

void CImageBaseView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	register int i, j;

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CImage *img = (CImage*)imageList.GetAt(imageList.FindIndex(0));
	IPLIMAGE image = img->GetImage();
	IPLIMAGE grayimg = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);

	CFileDialog *pDlg = new CFileDialog( FALSE, "jpg", NULL, OFN_READONLY, 
		"템플릿영상 *.jpg|*.jpg|모든파일 *.*|*.*|", NULL );

	CFileDialog *pDlg2 = new CFileDialog( FALSE, "dat", NULL, OFN_READONLY, 
		"얼굴패턴파일 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

	CFileDialog *pDlg3 = new CFileDialog( FALSE, "jpg", NULL, OFN_READONLY, 
		"JPG영상 *.jpg|*.jpg|모든파일 *.*|*.*|", NULL );

	switch(nChar)
	{
		case 'a':
			if (pDlg->DoModal())
			{
				CString filename = pDlg->GetFileName();

				if (filename != "")
				{
					cvvSaveImage(filename, image);
				}
			}
			break;
		case 's':
			cvvConvertImage(image, grayimg);
			for (i=0; i<ptcount; i+=2)
			{
				if (pDlg->DoModal())
				{
					CString filename = pDlg->GetFileName();
					if (filename != "")
					{
						IPLIMAGE subimg = cvCreateImage(cvSize(pt[i+1].x-pt[i].x+1, pt[i+1].y-pt[i].y+1), IPL_DEPTH_8U, 1);
						IPLIMAGE subimgc = cvCreateImage(cvSize(pt[i+1].x-pt[i].x+1, pt[i+1].y-pt[i].y+1), IPL_DEPTH_8U, 3);

						cvSetImageROI(grayimg, cvRect(pt[i].x, pt[i].y, pt[i+1].x-pt[i].x+1, pt[i+1].y-pt[i].y+1));
						iplCopy(grayimg, subimg);

						cvvConvertImage(subimg, subimgc);
						cvvSaveImage(filename, subimgc);

						cvReleaseImage(&subimgc);
						cvReleaseImage(&subimg);
					}
				}
			}
			break;
		case 'f':
			if( pDlg2->DoModal() )
			{
				CString filename = pDlg2->GetFileName();
				
				if( filename != "" )  
				{
					FILE *fp;

					fp = fopen(filename, "wt");

					fprintf(fp, "%d \n", ptcount);

					for (j=0; j<ptcount; j++)
					{
						fprintf(fp, "%d %d \n", pt[j].x, pt[j].y);
					}

					ptcount = 0;

					fclose(fp);
				}
			}
			break;
		case '1':
			(pFrame->GetMemoryImage(1))->SetImagePlane(1,1);
			(pFrame->GetMemoryImage(1))->ShowSingleImage(image);
			break;
		case '2':
			(pFrame->GetMemoryImage(2))->SetImagePlane(1,1);
			(pFrame->GetMemoryImage(2))->ShowSingleImage(image);
			break;
		case '3':
			(pFrame->GetMemoryImage(3))->SetImagePlane(1,1);
			(pFrame->GetMemoryImage(3))->ShowSingleImage(image);
			break;
		case 'r':
			m_bRegionSelect = !m_bRegionSelect;
			if (m_bRegionSelect)
				::SetCursor(::LoadCursor(NULL, IDC_CROSS));
			break;
		case 'm':
			m_bMagnify = !m_bMagnify;
			if (m_bMagnify)
				::SetCursor(::LoadCursor(NULL, IDC_APPSTARTING));
			break;
		default:
			break;
	}

	cvReleaseImage(&grayimg);

	CView::OnChar(nChar, nRepCnt, nFlags);
}

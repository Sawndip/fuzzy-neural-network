// ImageView.cpp : implementation of the CPicView class
//

#include "stdafx.h"
#include "Pic.h"

#include "Mainfrm.h"
#include "PicDoc.h"
#include "PicView.h"
#include "Image.h"
#include "ImagePixel.h"
#include "MyPrintPreview.h"
#include "afxcoll.h"
#include "cv.h"

#define RECTSIZE 160
#define	DEFAULT_PERIOD	30

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicView

IMPLEMENT_DYNCREATE(CPicView, CScrollView)

BEGIN_MESSAGE_MAP(CPicView, CScrollView)
	//{{AFX_MSG_MAP(CPicView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_ORIGINALSIZE, OnOriginalsize)
	ON_COMMAND(ID_MAG2, OnMag2)
	ON_COMMAND(ID_MAG4, OnMag4)
	ON_COMMAND(ID_MAG8, OnMag8)
	ON_COMMAND(ID_MAGN2, OnMagn2)
	ON_COMMAND(ID_MAGN4, OnMagn4)
	ON_COMMAND(ID_MAGN8, OnMagn8)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_DYNAMIC_INPUT, OnDynamicInput)
	ON_COMMAND(ID_DYNAMIC_CAPTURE, OnDynamicCapture)
	ON_COMMAND(ID_EDIT_POINT, OnEditPoint)
	ON_WM_TIMER()
	ON_COMMAND(ID_DYNAMIC_CAM_SELECT, OnDynamicCamSelect)
	ON_COMMAND(ID_DYNAMIC_CAM_SETTING, OnDynamicCamSetting)
	ON_COMMAND(ID_DYNAMIC_OFF, OnDynamicOff)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_MESSAGE(WM_DOREALIZE, OnDoRealize)
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_MESSAGE(PREVIEW_IMAGE_RECT_CHANGED, OnPreviewImageRectChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicView construction/destruction

CPicView::CPicView()
{
	m_bMagnify = FALSE;
	m_nMul = m_nDiv = 1;
	m_ptrSave = FALSE;
	m_bVideo = FALSE;
}

CPicView::~CPicView()
{
	//m_camera.Uninitialize();
	//if (m_bVideo)//		m_pVideo.Close();

	if (!ptrList.IsEmpty())
		ptrList.RemoveAll();

	ASSERT(ptrList.IsEmpty());
}

/////////////////////////////////////////////////////////////////////////////
// CPicView drawing
void CPicView::OnDraw(CDC* pDC)
{
	CPicDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// normal image
	if (( !pDoc->m_Image.IsDataNull() ) && !m_bVideo)
	{
		CRect rcDIB;
		rcDIB.top = rcDIB.left = 0;
		rcDIB.right = pDoc->m_Image.GetWidth();
		rcDIB.bottom = pDoc->m_Image.GetHeight();
		CRect rcDest;
		if (pDC->IsPrinting())   // printer DC
		{
			CSize sizeTmp = CSize( pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

			rcDest.SetRect( (int)(sizeTmp.cx*m_previewParam.fRectLeft),
							(int)(sizeTmp.cy*m_previewParam.fRectTop),
							(int)(sizeTmp.cx*m_previewParam.fRectRight),
							(int)(sizeTmp.cy*m_previewParam.fRectBottom));
		}
		else   // not printer DC
		{
			rcDest = rcDIB;
			rcDest.left = rcDest.left * m_nMul / m_nDiv;
			rcDest.top = rcDest.top * m_nMul / m_nDiv;
			rcDest.right= rcDest.right * m_nMul / m_nDiv;
			rcDest.bottom= rcDest.bottom * m_nMul / m_nDiv;
		}

		pDoc->m_Image.Draw(pDC->m_hDC,&rcDIB,&rcDest);
		if(pDoc->m_bRegionSelectMode) 
		{
			CRect track = pDoc->m_Tracker.m_rect;
						
			pDoc->m_Tracker.m_rect.left = pDoc->m_Tracker.m_rect.left * m_nMul / m_nDiv;
			pDoc->m_Tracker.m_rect.top = pDoc->m_Tracker.m_rect.top * m_nMul / m_nDiv;
			pDoc->m_Tracker.m_rect.right= pDoc->m_Tracker.m_rect.right * m_nMul / m_nDiv;
			pDoc->m_Tracker.m_rect.bottom= pDoc->m_Tracker.m_rect.bottom * m_nMul / m_nDiv;
			pDoc->m_Tracker.m_rect.OffsetRect(-GetScrollPosition());

			pDoc->m_Tracker.Draw(pDC);	
			pDoc->m_Tracker.m_rect = track ;
		}

		//cvvShowImage("Loaded Image", pDoc->fileimg);
	}

	// CP Image
	if ((pDoc->dispCPimg) != NULL)
	{
		if (((pDoc->dispCPimg)->imageData) && m_bVideo)
		{
			/*

			HDC dstDC = pDC->m_hDC;
			HDC srcDC = m_camera.GetFrame().GetDC();

			CRect r, sr;
			GetClipBox( dstDC, &r );
			GetClipBox( srcDC, &sr );

			m_camera.GetFrame().Show( dstDC, r.left, r.top, r.Width(), r.Height(),
									  r.left, r.top ); 
									  */

			//myCPWnd.DrawIplImage((IplImage *)pDoc->dispCPimg, FALSE);
			//myCPWnd.Invalidate(FALSE);
		}
	}

	// Background Image
	if ((pDoc->dispBGimg) != NULL) 
	{
		if (((pDoc->dispBGimg)->imageData) && m_bVideo)
		{
			//myBGWnd.DrawIplImage((IplImage *)pDoc->dispBGimg, FALSE);
			//myBGWnd.Invalidate(FALSE);
		}
	}

	// face component Image
	if ((pDoc->dispFCimg) != NULL)
	{
		if (((pDoc->dispFCimg)->imageData) && m_bVideo)
		{
			//myFCWnd.DrawIplImage((IplImage *)pDoc->dispFCimg, FALSE);
			//myFCWnd.Invalidate(FALSE);
		}
	}

	// real background Image
	if ((pDoc->dispRBGimg) != NULL)
	{
		if (((pDoc->dispRBGimg)->imageData) && m_bVideo)
		{
			//myRBGWnd.DrawIplImage((IplImage *)pDoc->dispRBGimg, FALSE);
			//myRBGWnd.Invalidate(FALSE);
		}
	}

	// LPM Image
	if ((pDoc->dispLPimg) != NULL)
	{
		if (((pDoc->dispLPimg)->imageData) && m_bVideo)
		{
			//myLPWnd.DrawIplImage((IplImage *)pDoc->dispLPimg, FALSE);
			//myLPWnd.Invalidate(FALSE);
		}
	}
}

LRESULT CPicView::OnDoRealize(WPARAM wParam, LPARAM)
{
	ASSERT(wParam != NULL);
	CPicDoc* pDoc = GetDocument();
	if (pDoc->m_Image.GetHandle() == NULL)
		return 0L;  // must be a new document

	CPalette* pPal = pDoc->m_Image.GetPalette();
	if (pPal != NULL)
	{
		CMainFrame* pAppFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		ASSERT(pAppFrame->IsKindOf(RUNTIME_CLASS( CMainFrame )));

		CClientDC appDC(pAppFrame);
		// All views but one should be a background palette.
		// wParam contains a handle to the active view, so the SelectPalette
		// bForceBackground flag is FALSE only if wParam == m_hWnd (this view)
		CPalette* oldPalette = appDC.SelectPalette(pPal, ((HWND)wParam) != m_hWnd);

		if (oldPalette != NULL)
		{
			UINT nColorsChanged = appDC.RealizePalette();
			if (nColorsChanged > 0)
				pDoc->UpdateAllViews(NULL);
			appDC.SelectPalette(oldPalette, TRUE);
		}
		else
		{
			TRACE0("\tSelectPalette failed in CDibView::OnPaletteChanged\n");
		}
	}

	return 0L;
}

void CPicView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CPicDoc* pDoc = GetDocument();

	SetScrollSizes(MM_TEXT, pDoc->m_Image.GetSize());
	ResizeParentToFit();

	/*
	myCPWnd.CreateWnd(this, "Cartesian Plane", 160, 140, 0, 0, 24);
	myBGWnd.CreateWnd(this, "BackGround Plane", 160, 140, 160, 0, 24);
	myLPWnd.CreateWnd(this, "Log-Polar Plane", 64, 148, 640, 0, 24);
	myFCWnd.CreateWnd(this, "ROI Plane", 160, 140, 320, 0, 24);
	myRBGWnd.CreateWnd(this, "Real BackGround Plane", 160, 140, 480, 0, 24);
	*/
}

/////////////////////////////////////////////////////////////////////////////
// CPicView printing
BOOL CPicView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	return DoPreparePrinting(pInfo);
}

void CPicView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	
}

void CPicView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CPicView diagnostics

#ifdef _DEBUG
void CPicView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPicView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CPicDoc* CPicView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPicDoc)));
	return (CPicDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPicView message handlers
void CPicView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if (bActivate)
	{
		ASSERT(pActivateView == this);
		OnDoRealize((WPARAM)m_hWnd, 0);   // same as SendMessage(WM_DOREALIZE);
	}
}


void CPicView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPicDoc *pDoc = GetDocument();
	CClientDC dc(this);
	CDC MemDC;
	CPen redPen(PS_SOLID,1,RGB(255,0,0)), *pOldPen;
	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	ClipCursor(&rect);

	if(pDoc->m_bRegionSelectMode)
	{	
		if (pDoc->m_Tracker.HitTest(point) < 0)
		{
			pDoc->m_Tracker.TrackRubberBand(this, point, TRUE);
		}
		else 
		{
			pDoc->m_Tracker.Track(this, point, TRUE);
		}
		pDoc->m_Tracker.m_rect.NormalizeRect();
		pDoc->m_Tracker.m_rect.OffsetRect(GetScrollPosition());

		pDoc->m_Tracker.m_rect.left = pDoc->m_Tracker.m_rect.left * m_nDiv / m_nMul;
		pDoc->m_Tracker.m_rect.top = pDoc->m_Tracker.m_rect.top * m_nDiv / m_nMul;
		pDoc->m_Tracker.m_rect.right= pDoc->m_Tracker.m_rect.right * m_nDiv / m_nMul;
		pDoc->m_Tracker.m_rect.bottom= pDoc->m_Tracker.m_rect.bottom * m_nDiv / m_nMul;

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

	if (m_ptrSave)
	{
		ptrList.AddTail((int *)(point.x));
		ptrList.AddTail((int *)(point.y));
		//ptrList.AddTail((CPoint *)(&point));

		//pOldPen = (CPen *)dc.SelectObject(&redPen);
		dc.Rectangle(point.x-2,point.y-2,point.x+2,point.y+2);
		//dc.SelectObject(pOldPen);
	}
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CPicView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bMagnify)
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
	
	ShowPixelInfo(point+GetScrollPosition());

	CScrollView::OnMouseMove(nFlags, point);
}

void CPicView::ShowPixelInfo(CPoint point)
{
	point.x = point.x * m_nDiv / m_nMul;
	point.y = point.y * m_nDiv / m_nMul;

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	CPicDoc *pDoc = GetDocument();
	CString strText;

	if(pDoc->m_Image.GetBitCount() == 8)
	{
		strText.Format("%d x %d, Grayscale", pDoc->m_Image.GetWidth(),
			pDoc->m_Image.GetHeight());
	}
	else
	{
		strText.Format("%d x %d, TRUE color", pDoc->m_Image.GetWidth(),
			pDoc->m_Image.GetHeight());
	}
	pFrame->m_wndStatusBar.SetPaneText(3, strText);

	if(point.x < pDoc->m_Image.GetWidth() && point.y < pDoc->m_Image.GetHeight())
	{
		strText.Format("(%d, %d)", point.x, point.y);
		pFrame->m_wndStatusBar.SetPaneText(1, strText);

		if(pDoc->m_Image.GetBitCount() == 8)
		{
			CPixelPtr ptr(pDoc->m_Image);
			strText.Format("Brightness = %d", (int)ptr[point.y][point.x]);
			pFrame->m_wndStatusBar.SetPaneText(2, strText);
		}
		else
		{
			CColorPixelPtr ptr(pDoc->m_Image);
			strText.Format("RGB(%d, %d, %d)", ptr[point.y][point.x].R,
				ptr[point.y][point.x].G, ptr[point.y][point.x].B);
			pFrame->m_wndStatusBar.SetPaneText(2, strText);
		}
	}
	else
	{
		pFrame->m_wndStatusBar.SetPaneText(1, "");
		pFrame->m_wndStatusBar.SetPaneText(2, "");
	}
}

void CPicView::OnLButtonUp(UINT nFlags, CPoint point) 
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
	
	CScrollView::OnLButtonUp(nFlags, point);
}

void CPicView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CPicDoc* pDoc = GetDocument();

	if(lHint)
	{
		SetScrollSizes(MM_TEXT, pDoc->m_Image.GetSize());
	}

	InvalidateRect(NULL);	
}

BOOL CPicView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPicDoc* pDoc = GetDocument();
	if (pDoc->m_bRegionSelectMode && pWnd == this && pDoc->m_Tracker.SetCursor(this, nHitTest))
		return TRUE;

	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}


void CPicView::OnOriginalsize() 
{
	CPicDoc* pDoc = GetDocument();
	m_nMul = 1;
	m_nDiv = 1;
	CSize size = pDoc->m_Image.GetSize();
	SetScrollSizes(MM_TEXT, CSize(size.cx*m_nMul/m_nDiv, size.cy*m_nMul/m_nDiv));
	InvalidateRect(NULL);
}

void CPicView::OnMag2() 
{
	CPicDoc* pDoc = GetDocument();
	m_nMul = 2;
	m_nDiv = 1;
	CSize size = pDoc->m_Image.GetSize();
	SetScrollSizes(MM_TEXT, CSize(size.cx*m_nMul/m_nDiv, size.cy*m_nMul/m_nDiv));
	InvalidateRect(NULL);
}

void CPicView::OnMag4() 
{
	CPicDoc* pDoc = GetDocument();
	m_nMul = 3;
	m_nDiv = 1;
	CSize size = pDoc->m_Image.GetSize();
	SetScrollSizes(MM_TEXT, CSize(size.cx*m_nMul/m_nDiv, size.cy*m_nMul/m_nDiv));
	InvalidateRect(NULL);
}

void CPicView::OnMag8() 
{
	CPicDoc* pDoc = GetDocument();
	m_nMul = 4;
	m_nDiv = 1;
	CSize size = pDoc->m_Image.GetSize();
	SetScrollSizes(MM_TEXT, CSize(size.cx*m_nMul/m_nDiv, size.cy*m_nMul/m_nDiv));
	InvalidateRect(NULL);
}

void CPicView::OnMagn2() 
{
	CPicDoc* pDoc = GetDocument();
	m_nMul = 1;
	m_nDiv = 2;
	CSize size = pDoc->m_Image.GetSize();
	SetScrollSizes(MM_TEXT, CSize(size.cx*m_nMul/m_nDiv, size.cy*m_nMul/m_nDiv));
	InvalidateRect(NULL);
}

void CPicView::OnMagn4() 
{
	CPicDoc* pDoc = GetDocument();
	m_nMul = 1;
	m_nDiv = 3;
	CSize size = pDoc->m_Image.GetSize();
	SetScrollSizes(MM_TEXT, CSize(size.cx*m_nMul/m_nDiv, size.cy*m_nMul/m_nDiv));
	InvalidateRect(NULL);
}

void CPicView::OnMagn8() 
{
	CPicDoc* pDoc = GetDocument();
	m_nMul = 1;
	m_nDiv = 4;
	CSize size = pDoc->m_Image.GetSize();
	SetScrollSizes(MM_TEXT, CSize(size.cx*m_nMul/m_nDiv, size.cy*m_nMul/m_nDiv));
	InvalidateRect(NULL);
}

void CPicView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GetParentFrame()->SendMessage(WM_COMMAND, ID_VIEW_TOOLBAR);
	
	CScrollView::OnRButtonDown(nFlags, point);
}

void CPicView::OnFilePrintPreview() 
{
	CPrintPreviewState* pState = new CPrintPreviewState;
	if( !DoPrintPreview(IDD_PREVIEW_TOOLBAR, this, RUNTIME_CLASS(CMyPrintPreview), pState ) )
		delete pState;
}

void CPicView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if(!pInfo->m_bPreview)
		OnDraw(pDC);
}

LPARAM CPicView::OnPreviewImageRectChanged(WPARAM wParam, LPARAM lParam)
{
	memcpy( &m_previewParam, (void*)lParam, sizeof(MyPreviewParam) );
	
	return 0;
}

void CPicView::OnDynamicInput() 
{
	/*
	if (m_camera.IsRunning())	return;

	m_bVideo = TRUE;

    if( m_camera.Initialize( 160, 120, -1, m_hWnd ) == 0 )
    {
        MessageBox("Can't initialize camera. Try to change format","Error", MB_OK|MB_ICONERROR );
    }
    
	m_camera.Start();
	SetTimer(0x1, DEFAULT_PERIOD, NULL);
	*/
}

void CPicView::OnDynamicCapture() 
{
	CMainFrame *frame = (CMainFrame *)AfxGetMainWnd();
	frame->process_start = !(frame->process_start);
	//m_pVideo.Capture();
}

void CPicView::OnEditPoint() 
{
	m_ptrSave = !m_ptrSave;	

	if (!m_ptrSave && !ptrList.IsEmpty())
	{

		CPicDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		FILE *fp;
		fp = fopen(pDoc->loadedfile, "wt");

		fprintf(fp, "%d \n", ptrList.GetCount()/2);

		for (int i=0; i<ptrList.GetCount(); i+= 2)
		{
			//CPoint *my_point = (CPoint *)(ptrList.GetAt(ptrList.FindIndex(i)));
			//fprintf(fp, "%d %d \n", my_point->x, my_point->y);
			fprintf(fp, "%d %d \n", ptrList.GetAt(ptrList.FindIndex(i)),
				ptrList.GetAt(ptrList.FindIndex(i+1)));
		}

		fclose(fp);

		ptrList.RemoveAll();
		ASSERT(ptrList.IsEmpty());
	}
}

void CPicView::OnTimer(UINT nIDEvent) 
{
	//if(!m_FrameGrabber.GetSafeHwnd())	return;

	CPicDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//get the first frame
	if (!pDoc->dispCPimg)
	{		
		/*
		pDoc->dispCPimg = iplCloneImage(m_camera.GetFrame().GetImage());
		InvalidateRect(NULL);
		*/
	}
	else
	{
		/*
		pDoc->dispCPimg = iplCloneImage(m_camera.GetFrame().GetImage());
		pDoc->ProcessImage();
		*/
	}

	CScrollView::OnTimer(nIDEvent);
}

void CPicView::OnDynamicCamSelect() 
{
	/*
	if (m_camera.IsRunning())
		m_camera.VideoSourceDlg();
		*/
}

void CPicView::OnDynamicCamSetting() 
{
	/*
	if (m_camera.IsRunning())
		m_camera.VideoFormatDlg();
		*/
}

void CPicView::OnDynamicOff() 
{
	/*
	if (m_camera.IsRunning())
		m_camera.Uninitialize();	
		*/
}

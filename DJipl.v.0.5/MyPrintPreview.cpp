// MyPrintPreview.cpp : implementation file
//

#include "stdafx.h"
#include "MyPrintPreview.h"
#include "PicDoc.h"
#include "PicView.h"
#include "Image.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPrintPreview

IMPLEMENT_DYNCREATE(CMyPrintPreview, CPreviewView)

CMyPrintPreview::CMyPrintPreview()
{
	m_tracker.m_rect.SetRect(0,0,0,0);
	m_tracker.m_nStyle = CRectTracker::resizeInside | CRectTracker::dottedLine;

	m_previewParam.fRectLeft = 0.2f;
	m_previewParam.fRectRight = 0.8f;
	m_previewParam.fRectTop = 0.2f;
	m_previewParam.fRectBottom = 0.8f;
}

CMyPrintPreview::~CMyPrintPreview()
{
}


BEGIN_MESSAGE_MAP(CMyPrintPreview, CPreviewView)
	//{{AFX_MSG_MAP(CMyPrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPrintPreview drawing

void CMyPrintPreview::OnDraw(CDC* pDC)
{
	// 기본 화면 그리기
	CPreviewView::OnDraw(pDC);

	CmyImage *pImage = &((CPicView *)m_pPrintView)->GetDocument()->m_Image;

	// 선택 영역의 위치와 크기 계산
	CRect rcPage = m_pPageInfo[0].rectScreen;
	m_tracker.m_rect.left = (int) (rcPage.Width()*m_previewParam.fRectLeft);
	m_tracker.m_rect.right = (int) (rcPage.Width()*m_previewParam.fRectRight);
	m_tracker.m_rect.top = (int) (rcPage.Height()*m_previewParam.fRectTop);
	m_tracker.m_rect.bottom = (int) (rcPage.Height()*m_previewParam.fRectBottom);
	m_tracker.m_rect.OffsetRect( rcPage.TopLeft() );

	// 이미지의 가로 세로 길이
	CRect rcDraw, rcImage;
	m_tracker.GetTrueRect(rcDraw);
	rcImage.left = rcImage.top = 0;
	rcImage.right = pImage->GetWidth();
	rcImage.bottom = pImage->GetHeight();

	// 이미지 출력
	if( pImage ) pImage->Draw(pDC->GetSafeHdc(), &rcImage, &rcDraw);

	// CRectTracker는 Device 좌표 사용하므로
	pDC->LPtoDP(m_tracker.m_rect);
	
	// CRectTracker를 그려줌
	m_tracker.Draw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CMyPrintPreview diagnostics

#ifdef _DEBUG
void CMyPrintPreview::AssertValid() const
{
	CPreviewView::AssertValid();
}

void CMyPrintPreview::Dump(CDumpContext& dc) const
{
	CPreviewView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyPrintPreview message handlers

void CMyPrintPreview::OnLButtonDown(UINT nFlags, CPoint point) 
{
		// CRectTracker가 클릭 되었는지 확인
	if(m_tracker.HitTest(point) != CRectTracker::hitNothing)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc);

		// CRectTracker 구동
		m_tracker.Track(this, point, FALSE );
		Invalidate();
		CRect rcTrackerLogical = m_tracker.m_rect;
		dc.DPtoLP( rcTrackerLogical );

		// 선택 영역이 종이 밖으로 벗어나지 않도록
		CRect rcPage = m_pPageInfo[0].rectScreen;
		m_previewParam.fRectLeft = max((float)(rcTrackerLogical.left-rcPage.left)/rcPage.Width(),0);
		m_previewParam.fRectRight = min((float)(rcTrackerLogical.right-rcPage.left)/rcPage.Width(),1);
		m_previewParam.fRectTop = max((float)(rcTrackerLogical.top-rcPage.top)/rcPage.Height(),0);
		m_previewParam.fRectBottom = min((float)(rcTrackerLogical.bottom-rcPage.top)/rcPage.Height(),1);

		// 선택 영역의 변경 사항을 알림
		if( m_pPrintView && ::IsWindow(m_pPrintView->GetSafeHwnd()) )
			m_pPrintView->SendMessage( PREVIEW_IMAGE_RECT_CHANGED, 0, (LPARAM) &m_previewParam);

		return;
	}

	CPreviewView::OnLButtonDown(nFlags, point);
}


BOOL CMyPrintPreview::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// 마우스 커서가 CRectTracker 위에 있으면 커서 모양을 변경
	if (pWnd == this && m_tracker.SetCursor(this, nHitTest))
		return TRUE;
		
	return CPreviewView::OnSetCursor(pWnd, nHitTest, message);
}

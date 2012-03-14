#if !defined(AFX_MYPRINTPREVIEW_H__9B3787F7_0168_11D2_82EA_0060975D847D__INCLUDED_)
#define AFX_MYPRINTPREVIEW_H__9B3787F7_0168_11D2_82EA_0060975D847D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define PREVIEW_IMAGE_RECT_CHANGED WM_USER+20

#include "afxpriv.h"

typedef struct MyPreviewParam {
	float	fRectLeft;
	float	fRectRight;
	float	fRectTop;
	float	fRectBottom;
} tagMyPreviewParam;

/////////////////////////////////////////////////////////////////////////////
// CMyPrintPreview view

class CMyPrintPreview : public CPreviewView
{
protected:
	CMyPrintPreview();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyPrintPreview)

// Attributes
public:
	// 영역 선택을 위한 인터페이스를 위한 클래스
	CRectTracker m_tracker;
	// 선택 영역을 저장
	MyPreviewParam m_previewParam;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPrintPreview)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyPrintPreview();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyPrintPreview)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPRINTPREVIEW_H__9B3787F7_0168_11D2_82EA_0060975D847D__INCLUDED_)

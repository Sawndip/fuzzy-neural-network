// ImageView.h : interface of the CPicView class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __IMAGEVIEW_H__
#define __IMAGEVIEW_H__

#define WM_DOREALIZE   (WM_USER + 0)
#include "MyPrintPreview.h"
//#include "MyCapVideo.h"

// 17.11.01
//#include ".\\IPL\\Camera.h"
//#include ".\\IPL\\DWnd.h"

#include "afxcoll.h"

class CPicView : public CScrollView
{
protected: // create from serialization only
	CPicView();
	DECLARE_DYNCREATE(CPicView)

// Attributes
public:
	CPicDoc* GetDocument();
	CBitmap m_bmpMagnify;		//확대경을 위한 비트맵
	BOOL	m_bMagnify;
	CPoint	m_ptOld;
	int		m_nMul, m_nDiv;
	MyPreviewParam m_previewParam;

	// USB camera 이용하기.
	// 10.11.01
	BOOL m_bVideo;
	// 13.12.01
	//CCamera m_camera;

	CPtrList ptrList;
	BOOL m_ptrSave;

	/*
	CDWnd myCPWnd;
	CDWnd myLPWnd;
	CDWnd myBGWnd;
	CDWnd myFCWnd;
	CDWnd myRBGWnd;
	*/

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowPixelInfo(CPoint point);
	virtual ~CPicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPicView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnOriginalsize();
	afx_msg void OnMag2();
	afx_msg void OnMag4();
	afx_msg void OnMag8();
	afx_msg void OnMagn2();
	afx_msg void OnMagn4();
	afx_msg void OnMagn8();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnDynamicInput();
	afx_msg void OnDynamicCapture();
	afx_msg void OnEditPoint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDynamicCamSelect();
	afx_msg void OnDynamicCamSetting();
	afx_msg void OnDynamicOff();
	//}}AFX_MSG
	afx_msg LRESULT OnDoRealize(WPARAM wParam, LPARAM lParam);  // user message
	afx_msg LPARAM OnPreviewImageRectChanged(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ImageView.cpp
inline CPicDoc* CPicView::GetDocument()
   { return (CPicDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif

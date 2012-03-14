#if !defined(AFX_IMAGEBASEVIEW_H__9391157C_5B0A_4F8F_832F_51B7306FCC2A__INCLUDED_)
#define AFX_IMAGEBASEVIEW_H__9391157C_5B0A_4F8F_832F_51B7306FCC2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageBaseView.h : header file
//

#include "afxcoll.h"
#include "highgui.h"

/////////////////////////////////////////////////////////////////////////////
// CImageBaseView view

class CImageBaseView : public CView
{
protected:
	CImageBaseView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CImageBaseView)

// Attributes
public:
	CBitmap m_bmpMagnify;		//확대경을 위한 비트맵
	BOOL m_bMagnify;
	BOOL m_bRegionSelect;
	CPoint	m_ptOld;
	float m_X, m_Y;

// Operations
public:
	int GetMode();
    //CHMMDemoDoc* GetDocument() { return (CHMMDemoDoc*)m_pDocument; };
    enum { ZOOM_MIN = -2, ZOOM_MAX = 4 };

    void  Zoom( bool zoom_in = true );
    void  SwitchMode( int index, bool to_personal );
    int   GetPersonIndex() { return m_index; };
    int   GetActive() { return m_active; };
    void  ResetActive() { m_active = -1; };
    void  RefreshView();
	CRect myrect;
	CPtrList imageList;
	bool clear_flag;

	void SetImagePlane(int nx, int ny);
	void AddImage(const int idx, const int idy, IplImage* image);
	void ShowImage(bool clear = TRUE);
	void ShowSingleImage(IplImage* image, bool clear = TRUE);
	IplImage* GetSingleImage();

	void UpdatePoint();

	CPoint pt[14];
	int ptcount;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageBaseView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowPixelInfo(CPoint point);
protected:
	virtual ~CImageBaseView();
    void    RecalcPictureSize();
    CRect   GetRectByIndex( int index );
    //int     GetIndexByPoint( CPoint pt );

    CImage  m_canvas;
    CImage  m_rgbcanvas;

    SIZE    m_base_size;
    SIZE    m_pic_size;
    SIZE    m_pic_delta;
    SIZE    m_margin;

    int     m_log_shift;
    int     m_index;
    int     m_active;

    int     m_view_mode;//VIEW_PERSON or VIEW_BASE

    HPEN    m_pen;

    int     m_count, m_nx, m_ny;

	CRectTracker m_Tracker;

	// Generated message map functions
	//{{AFX_MSG(CImageBaseView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEBASEVIEW_H__9391157C_5B0A_4F8F_832F_51B7306FCC2A__INCLUDED_)

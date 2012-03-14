#if !defined(AFX_HMMDEMOVIEW_H__80FE7EAC_1D5B_4166_B0DF_5991B563B473__INCLUDED_)
#define AFX_HMMDEMOVIEW_H__80FE7EAC_1D5B_4166_B0DF_5991B563B473__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHMMDemoView : public CView
{
protected: // create from serialization only
	CHMMDemoView();
	DECLARE_DYNCREATE(CHMMDemoView)

// Attributes
public:
    CHMMDemoDoc* GetDocument() { return (CHMMDemoDoc*)m_pDocument; };

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHMMDemoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetTestPath( char* path );
	virtual ~CHMMDemoView();
    bool IsCaptureEnabled() { m_camera.IsInitialized(); }
    void CheckUpdate();
    CCamera& Camera() { return m_camera; }
    void  SetSelection( RECT* sel );
    CRect GetSelection() { return m_sel; }
    void  SetImageList( CStringList* list );
    CStringList*  GetImageList() { return m_imageList; }
    CString GetTestPath() { return m_TestPath; }
    

    
protected:
    CRect m_sel, m_tmp_sel;
    CCamera m_camera;

    //list of files which was opened in HMMDemoView 
    CStringList* m_imageList;

// Generated message map functions
protected:
	CString m_TestPath;
	//{{AFX_MSG(CHMMDemoView)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HMMDEMOVIEW_H__80FE7EAC_1D5B_4166_B0DF_5991B563B473__INCLUDED_)

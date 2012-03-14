// SoftComputingWndView.h : interface of the CSoftComputingWndView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOFTCOMPUTINGWNDVIEW_H__5B8DCBA1_4772_45F2_A8B3_A25435D3960B__INCLUDED_)
#define AFX_SOFTCOMPUTINGWNDVIEW_H__5B8DCBA1_4772_45F2_A8B3_A25435D3960B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSoftComputingWndView : public CView
{
protected: // create from serialization only
	CSoftComputingWndView();
	DECLARE_DYNCREATE(CSoftComputingWndView)

// Attributes
public:
	CSoftComputingWndDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftComputingWndView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSoftComputingWndView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSoftComputingWndView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SoftComputingWndView.cpp
inline CSoftComputingWndDoc* CSoftComputingWndView::GetDocument()
   { return (CSoftComputingWndDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTCOMPUTINGWNDVIEW_H__5B8DCBA1_4772_45F2_A8B3_A25435D3960B__INCLUDED_)

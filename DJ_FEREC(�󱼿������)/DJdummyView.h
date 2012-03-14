#if !defined(AFX_DJDUMMYVIEW_H__6C0CDA32_DB7A_4008_9BBA_0B3FF8A302A4__INCLUDED_)
#define AFX_DJDUMMYVIEW_H__6C0CDA32_DB7A_4008_9BBA_0B3FF8A302A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DJdummyView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DJdummyView view

class DJdummyView : public CView
{
protected:
	DJdummyView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(DJdummyView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DJdummyView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~DJdummyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(DJdummyView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DJDUMMYVIEW_H__6C0CDA32_DB7A_4008_9BBA_0B3FF8A302A4__INCLUDED_)

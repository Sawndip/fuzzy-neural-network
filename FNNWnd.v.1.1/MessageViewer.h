#if !defined(AFX_MESSAGEVIEWER_H__6BA8914B_86BB_40CA_91CC_EF6B3FF10D02__INCLUDED_)
#define AFX_MESSAGEVIEWER_H__6BA8914B_86BB_40CA_91CC_EF6B3FF10D02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageViewer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessageViewer view

class CMessageViewer : public CScrollView
{
protected:
	CMessageViewer();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMessageViewer)

// Attributes
public:

// Operations
public:
	void MessageView(CString str);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageViewer)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMessageViewer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMessageViewer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEVIEWER_H__6BA8914B_86BB_40CA_91CC_EF6B3FF10D02__INCLUDED_)

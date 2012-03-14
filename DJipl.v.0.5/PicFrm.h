// PicFrm.h : interface of the CPicFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICFRM_H__BA5BA5AC_E6C1_11D1_994B_444553540000__INCLUDED_)
#define AFX_PICFRM_H__BA5BA5AC_E6C1_11D1_994B_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPicFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPicFrame)
public:
	CPicFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CPicFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICFRM_H__BA5BA5AC_E6C1_11D1_994B_444553540000__INCLUDED_)

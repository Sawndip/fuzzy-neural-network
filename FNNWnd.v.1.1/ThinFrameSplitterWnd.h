#if !defined(AFX_THINFRAMESPLITTERWND_H__04630DF1_201E_4AB9_AFFC_F4801DE327C3__INCLUDED_)
#define AFX_THINFRAMESPLITTERWND_H__04630DF1_201E_4AB9_AFFC_F4801DE327C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThinFrameSplitterWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ThinFrameSplitterWnd frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class ThinFrameSplitterWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(ThinFrameSplitterWnd)
protected:
	ThinFrameSplitterWnd();           // protected constructor used by dynamic creation

// Attributes
protected:
	CSplitterWnd    m_wndSplitter;
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ThinFrameSplitterWnd)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ThinFrameSplitterWnd();

	// Generated message map functions
	//{{AFX_MSG(ThinFrameSplitterWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THINFRAMESPLITTERWND_H__04630DF1_201E_4AB9_AFFC_F4801DE327C3__INCLUDED_)

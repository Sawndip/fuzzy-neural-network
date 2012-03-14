#if !defined(AFX_CMDDEPTH_H__C3F40C16_545C_49FF_B8AC_7161223FF027__INCLUDED_)
#define AFX_CMDDEPTH_H__C3F40C16_545C_49FF_B8AC_7161223FF027__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdDepth.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdDepth form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdDepth : public CFormView
{
protected:
	CCmdDepth();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdDepth)

// Form Data
public:
	//{{AFX_DATA(CCmdDepth)
	enum { IDD = IDD_CMD_DEPTH };
	CString	m_Method;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdDepth)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdDepth();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdDepth)
	afx_msg void OnActDepth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDDEPTH_H__C3F40C16_545C_49FF_B8AC_7161223FF027__INCLUDED_)

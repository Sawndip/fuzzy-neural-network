#if !defined(AFX_CMDEIGEN_H__E446A87D_2D59_419F_9A73_77DB088060B1__INCLUDED_)
#define AFX_CMDEIGEN_H__E446A87D_2D59_419F_9A73_77DB088060B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdEigen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdEigen form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdEigen : public CFormView
{
protected:
	CCmdEigen();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdEigen)

// Form Data
public:
	//{{AFX_DATA(CCmdEigen)
	enum { IDD = IDD_CMD_EIGEN };
	CString	m_Method;
	float	m_Ep;
	float	m_Err;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdEigen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdEigen();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdEigen)
	afx_msg void OnRecEigen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDEIGEN_H__E446A87D_2D59_419F_9A73_77DB088060B1__INCLUDED_)

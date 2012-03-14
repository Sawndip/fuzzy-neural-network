#if !defined(AFX_CMDPROJEXT_H__CA851C58_1E1A_49AE_AFDA_C675FEAE8A7F__INCLUDED_)
#define AFX_CMDPROJEXT_H__CA851C58_1E1A_49AE_AFDA_C675FEAE8A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdProjExt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdProjExt form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdProjExt : public CFormView
{
protected:
	CCmdProjExt();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdProjExt)

// Form Data
public:
	//{{AFX_DATA(CCmdProjExt)
	enum { IDD = IDD_CMD_PROJ_EXT };
	CString m_Dir;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdProjExt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdProjExt();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdProjExt)
	afx_msg void OnProjEx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDPROJEXT_H__CA851C58_1E1A_49AE_AFDA_C675FEAE8A7F__INCLUDED_)

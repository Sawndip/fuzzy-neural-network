#if !defined(AFX_CMDGF_H__119E4BFF_536D_4B20_B2FB_382CB8A6907E__INCLUDED_)
#define AFX_CMDGF_H__119E4BFF_536D_4B20_B2FB_382CB8A6907E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdGF.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdGF form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdGF : public CFormView
{
protected:
	CCmdGF();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdGF)

// Form Data
public:
	//{{AFX_DATA(CCmdGF)
	enum { IDD = IDD_CMD_GF };
	CString	m_source;
	CString	m_result;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdGF)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdGF();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdGF)
	afx_msg void OnGfExt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDGF_H__119E4BFF_536D_4B20_B2FB_382CB8A6907E__INCLUDED_)

#if !defined(AFX_CMDSF_H__A0B2DF3D_677A_46B3_9456_9459ACA3A111__INCLUDED_)
#define AFX_CMDSF_H__A0B2DF3D_677A_46B3_9456_9459ACA3A111__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdSF.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdSF form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdSF : public CFormView
{
protected:
	CCmdSF();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdSF)

// Form Data
public:
	//{{AFX_DATA(CCmdSF)
	enum { IDD = IDD_CMD_SF };
	CString m_Method;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdSF)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdSF();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdSF)
	afx_msg void OnSfExt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDSF_H__A0B2DF3D_677A_46B3_9456_9459ACA3A111__INCLUDED_)

#if !defined(AFX_CMDCOORD_H__6BC0248B_BE88_4EC8_BCA4_BC51C8656EFE__INCLUDED_)
#define AFX_CMDCOORD_H__6BC0248B_BE88_4EC8_BCA4_BC51C8656EFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdCoord.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdCoord form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdCoord : public CFormView
{
protected:
	CCmdCoord();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdCoord)

// Form Data
public:
	//{{AFX_DATA(CCmdCoord)
	enum { IDD = IDD_CMD_COORD };
	CString	m_source;
	CString	m_result;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdCoord)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdCoord();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdCoord)
	afx_msg void OnCoordTx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDCOORD_H__6BC0248B_BE88_4EC8_BCA4_BC51C8656EFE__INCLUDED_)

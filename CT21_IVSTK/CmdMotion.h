#if !defined(AFX_CMDMOTION_H__319573D2_8C7A_4710_A796_B605CE69E396__INCLUDED_)
#define AFX_CMDMOTION_H__319573D2_8C7A_4710_A796_B605CE69E396__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdMotion.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdMotion form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdMotion : public CFormView
{
protected:
	CCmdMotion();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdMotion)

// Form Data
public:
	//{{AFX_DATA(CCmdMotion)
	enum { IDD = IDD_CMD_MOTION };
	CString	m_Method;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdMotion)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdMotion();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdMotion)
	afx_msg void OnActMotion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDMOTION_H__319573D2_8C7A_4710_A796_B605CE69E396__INCLUDED_)

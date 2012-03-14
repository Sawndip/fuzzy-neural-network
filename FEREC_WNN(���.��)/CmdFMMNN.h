#if !defined(AFX_CMDFMMNN_H__3C44F619_DF55_400C_8D31_1A08555BFB3C__INCLUDED_)
#define AFX_CMDFMMNN_H__3C44F619_DF55_400C_8D31_1A08555BFB3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdFMMNN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdFMMNN form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdFMMNN : public CFormView
{
protected:
	CCmdFMMNN();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdFMMNN)

// Form Data
public:
	//{{AFX_DATA(CCmdFMMNN)
	enum { IDD = IDD_CMD_FMMNN };
	CString	m_Method;
	float	m_R;
	float	m_Theta;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdFMMNN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdFMMNN();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdFMMNN)
	afx_msg void OnRecFmmnn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDFMMNN_H__3C44F619_DF55_400C_8D31_1A08555BFB3C__INCLUDED_)

#if !defined(AFX_CMDNN_H__8947F547_F37A_4064_89A9_CD552E6AECBA__INCLUDED_)
#define AFX_CMDNN_H__8947F547_F37A_4064_89A9_CD552E6AECBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdNN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdNN form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdNN : public CFormView
{
protected:
	CCmdNN();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdNN)

// Form Data
public:
	//{{AFX_DATA(CCmdNN)
	enum { IDD = IDD_CMD_NN };
	float	m_Ep;
	float	m_Err;
	float	m_Lr;
	float	m_Mo;
	CString	m_Method;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdNN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdNN();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdNN)
	afx_msg void OnRecNn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDNN_H__8947F547_F37A_4064_89A9_CD552E6AECBA__INCLUDED_)

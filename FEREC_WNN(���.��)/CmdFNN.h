#if !defined(AFX_CMDFNN_H__83D1FBCA_674E_4A41_BD90_F44F6CCF2679__INCLUDED_)
#define AFX_CMDFNN_H__83D1FBCA_674E_4A41_BD90_F44F6CCF2679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdFNN.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdFNN form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdFNN : public CFormView
{
protected:
	CCmdFNN();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdFNN)

// Form Data
public:
	//{{AFX_DATA(CCmdFNN)
	enum { IDD = IDD_CMD_FNN };
	float	m_Ep;
	float	m_Err;
	float	m_Lr;
	CString	m_Method;
	float	m_Sp1;
	float	m_Sp2;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdFNN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdFNN();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdFNN)
	afx_msg void OnRecFnn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDFNN_H__83D1FBCA_674E_4A41_BD90_F44F6CCF2679__INCLUDED_)

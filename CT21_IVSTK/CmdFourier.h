#if !defined(AFX_CMDFOURIER_H__B488114F_AB66_4A26_B401_95F0AB3F9B0F__INCLUDED_)
#define AFX_CMDFOURIER_H__B488114F_AB66_4A26_B401_95F0AB3F9B0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdFourier.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdFourier form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdFourier : public CFormView
{
protected:
	CCmdFourier();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdFourier)

// Form Data
public:
	//{{AFX_DATA(CCmdFourier)
	enum { IDD = IDD_CMD_FTX };
	CString	m_Fourier;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdFourier)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdFourier();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdFourier)
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDFOURIER_H__B488114F_AB66_4A26_B401_95F0AB3F9B0F__INCLUDED_)

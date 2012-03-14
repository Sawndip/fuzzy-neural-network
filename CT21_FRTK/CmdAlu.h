#if !defined(AFX_CMDALU_H__1746ABF8_7B88_41C4_9043_D8B7D7E3610C__INCLUDED_)
#define AFX_CMDALU_H__1746ABF8_7B88_41C4_9043_D8B7D7E3610C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdAlu.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdAlu form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdAlu : public CFormView
{
protected:
	CCmdAlu();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdAlu)

// Form Data
public:
	//{{AFX_DATA(CCmdAlu)
	enum { IDD = IDD_CMD_ALU };
	CString m_Method;
	CString m_Const;
	BOOL m_Image1;
	BOOL m_Image2;
	BOOL m_Image3;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdAlu)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdAlu();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdAlu)
	afx_msg void OnActAlu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDALU_H__1746ABF8_7B88_41C4_9043_D8B7D7E3610C__INCLUDED_)

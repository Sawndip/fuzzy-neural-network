#if !defined(AFX_CMDCOLOR_H__5690F5A6_09B3_4FC9_9E11_239EAAE48247__INCLUDED_)
#define AFX_CMDCOLOR_H__5690F5A6_09B3_4FC9_9E11_239EAAE48247__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdColor form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdColor : public CFormView
{
protected:
	CCmdColor();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdColor)

// Form Data
public:
	//{{AFX_DATA(CCmdColor)
	enum { IDD = IDD_CMD_COLOR };
	CString	m_source;
	CString	m_result;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdColor();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdColor)
	afx_msg void OnActConvert();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDCOLOR_H__5690F5A6_09B3_4FC9_9E11_239EAAE48247__INCLUDED_)

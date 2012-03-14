#if !defined(AFX_CMDBLOB_H__A4025C77_1683_4112_BD22_9A3C53B0F598__INCLUDED_)
#define AFX_CMDBLOB_H__A4025C77_1683_4112_BD22_9A3C53B0F598__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdBlob.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdBlob form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdBlob : public CFormView
{
protected:
	CCmdBlob();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdBlob)

// Form Data
public:
	//{{AFX_DATA(CCmdBlob)
	enum { IDD = IDD_CMD_BLOB };
	CString	m_Method;
	CString	m_Morph;
	int		m_MorphNum;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdBlob)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdBlob();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdBlob)
	afx_msg void OnBlobAnal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDBLOB_H__A4025C77_1683_4112_BD22_9A3C53B0F598__INCLUDED_)

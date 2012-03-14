#if !defined(AFX_CMDVER_H__0DA939D4_5151_467E_B48B_4227BFA63394__INCLUDED_)
#define AFX_CMDVER_H__0DA939D4_5151_467E_B48B_4227BFA63394__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdVer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdVer form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdVer : public CFormView
{
protected:
	CCmdVer();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdVer)

// Form Data
public:
	//{{AFX_DATA(CCmdVer)
	enum { IDD = IDD_VERIFY };
	CSliderCtrl	m_FaceBar;
	float	m_RightEyeb;
	float	m_Nose;
	float	m_Mouth;
	float	m_LeftEyeb;
	float	m_Jaw;
	float	m_InterNm;
	float	m_InterDist;
	float	m_ForeHead;
	CString	m_FaceVer;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdVer)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdVer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdVer)
	afx_msg void OnActVer();
	afx_msg void OnReleasedcaptureFaceVer(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDVER_H__0DA939D4_5151_467E_B48B_4227BFA63394__INCLUDED_)

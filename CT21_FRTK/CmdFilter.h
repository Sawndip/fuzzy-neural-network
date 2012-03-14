#if !defined(AFX_CMDFILTER_H__8B266353_C160_4499_AE48_EFA708CCE04B__INCLUDED_)
#define AFX_CMDFILTER_H__8B266353_C160_4499_AE48_EFA708CCE04B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdFilter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdFilter form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdFilter : public CFormView
{
protected:
	CCmdFilter();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdFilter)

// Form Data
public:
	//{{AFX_DATA(CCmdFilter)
	enum { IDD = IDD_CMD_FILTER };
	CString	m_Filtering;
	CString m_MaxThr;
	CString m_MinThr;
	CString m_GaborAngleVal;
	CString m_GaborWaveVal;

	CSliderCtrl	m_MaxTh;
	CSliderCtrl	m_MinTh;
	CSliderCtrl	m_GaborAngle;
	CSliderCtrl	m_GaborWave;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdFilter)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdFilter();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdFilter)
	afx_msg void OnActFiltering();
	afx_msg void OnReleasedcaptureMinTh(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureMaxTh(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureGaborWave(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureGaborAngle(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDFILTER_H__8B266353_C160_4499_AE48_EFA708CCE04B__INCLUDED_)

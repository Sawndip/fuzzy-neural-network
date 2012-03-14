
#if !defined(AFX_MYPRSHEET_H__5F24BEA0_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_MYPRSHEET_H__5F24BEA0_32B5_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyPrSheet.h : header file
//
#include "ThresEnv.h"
#include "GaborEnv.h"
#include "ConvDlg.h"
#include "AffDlg.h"
#include "LPEnv.h"
#include "MorphEnv.h"
#include "HoughEnv.h"
#include "HLACEnv.h"
#include "RLPEnv.h"
#include "FaceEnv.h"
#include "SoftEnv.h"
#include "projEnv.h"
#include "edgeEnv.h"
#include "histEnv.h"

/////////////////////////////////////////////////////////////////////////////
// CMyPrSheet

class CMyPrSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMyPrSheet)

// Construction
public:
	CMyPrSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMyPrSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	ThresEnv m_thr;
	CAffDlg m_affine;
	CConvDlg m_conv;
	GaborEnv m_gabor;
	LPEnv m_lp;
	MorphEnv m_morph;
	HoughEnv m_hough;
	HLACEnv m_HLAC;
	RLPEnv m_rlp;
	CFaceEnv m_face;
	SoftEnv m_soft;
	projEnv m_proj;
	edgeEnv m_edge;
	histEnv m_hist;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPrSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyPrSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyPrSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPRSHEET_H__5F24BEA0_32B5_11D4_B764_00E0294BE14D__INCLUDED_)

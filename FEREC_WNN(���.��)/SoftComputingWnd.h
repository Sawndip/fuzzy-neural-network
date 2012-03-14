// SoftComputingWnd.h : main header file for the SOFTCOMPUTINGWND application
//

#if !defined(AFX_SOFTCOMPUTINGWND_H__43AC5FFC_D0FB_4EB5_ACD5_870DAFAC31E8__INCLUDED_)
#define AFX_SOFTCOMPUTINGWND_H__43AC5FFC_D0FB_4EB5_ACD5_870DAFAC31E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define	PP	0x80000000
#define FE	0x40000000
#define HP	0x20000000
#define RIP	0x08000000
#define FS	0x04000000
#define FCE	0x02000000
#define FFE 0x01000000
#define FR	0x00800000
#define FRE 0x00400000
#define IF	0x00200000
#define CC	0x00100000
#define CT	0x00080000
#define TM	0x00040000
#define BA	0x00020000
#define FV	0x00010000
#define PE	0x00008000
#define GE	0x00004000
#define GF	0x00002000
#define TF	0x00001000
#define SF	0x00000800


/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndApp:
// See SoftComputingWnd.cpp for the implementation of this class
//

class CSoftComputingWndApp : public CWinApp
{
public:
	CSoftComputingWndApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftComputingWndApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSoftComputingWndApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTCOMPUTINGWND_H__43AC5FFC_D0FB_4EB5_ACD5_870DAFAC31E8__INCLUDED_)

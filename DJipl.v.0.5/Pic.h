// IELab.h : main header file for the IELAB application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
//#include <Magick++.h>

#ifndef __IMAGEAPP_H__
#define __IMAGEAPP_H__

class CmyImage;

class CPicApp : public CWinApp
{
public:
	CPicApp();
	CmyImage *m_pNewImage;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CPicApp)
	afx_msg void OnAppAbout();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	//afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};


#ifndef		__GLOBAL_EXTERN__
#define		__GLOBAL_EXTERN__
extern CPicApp theApp;
#endif

/////////////////////////////////////////////////////////////////////////////
#endif

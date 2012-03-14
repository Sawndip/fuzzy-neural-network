// SoftComputingWndDoc.h : interface of the CSoftComputingWndDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOFTCOMPUTINGWNDDOC_H__B09095B0_E82F_4197_A697_AB95D2329D20__INCLUDED_)
#define AFX_SOFTCOMPUTINGWNDDOC_H__B09095B0_E82F_4197_A697_AB95D2329D20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define HAVE_IPL
#include <ipl.h>
#include <highgui.h>
//#include <cvaux.h>
#include <cv.h>
#include "ImageBaseView.h"

#include "FRTK.h"

class CSoftComputingWndDoc : public CDocument
{
protected: // create from serialization only
	CSoftComputingWndDoc();
	DECLARE_DYNCREATE(CSoftComputingWndDoc)

// Attributes
public:
	IPLIMAGE m_image;
	IPLIMAGE g_image;
	CImageBaseView* m_imgview;

	CFRTK m_frtk;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftComputingWndDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSoftComputingWndDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSoftComputingWndDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTCOMPUTINGWNDDOC_H__B09095B0_E82F_4197_A697_AB95D2329D20__INCLUDED_)

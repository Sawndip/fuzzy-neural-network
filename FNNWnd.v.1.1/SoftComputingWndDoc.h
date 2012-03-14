// SoftComputingWndDoc.h : interface of the CSoftComputingWndDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOFTCOMPUTINGWNDDOC_H__B09095B0_E82F_4197_A697_AB95D2329D20__INCLUDED_)
#define AFX_SOFTCOMPUTINGWNDDOC_H__B09095B0_E82F_4197_A697_AB95D2329D20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "myFNN.h"
#include "FMMNN.h"	// thanks to AKAII, 20021219

class CSoftComputingWndDoc : public CDocument
{
protected: // create from serialization only
	CSoftComputingWndDoc();
	DECLARE_DYNCREATE(CSoftComputingWndDoc)

// Attributes
public:

// Operations
public:
	myFNN m_fnn;
	CFMMNN m_fmmnn;
	IplImage* m_image;

	bool datLoaded;
	bool fnnLoaded;
	bool fmmnnLoaded;
	bool imageLoaded;
	bool bxdLoaded;

	CString filename;
	CString FNNkillnodes;
	CString FNNjustinputs;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftComputingWndDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
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
	afx_msg void OnShowImg();
	afx_msg void OnTestFnn();
	afx_msg void OnLearnFmmnn();
	afx_msg void OnParamSetFnn();
	afx_msg void OnLearnFnn();
	afx_msg void OnCamClose();
	afx_msg void OnCamFormat();
	afx_msg void OnCamInit();
	afx_msg void OnTestFmmnn();
	afx_msg void OnCamSetting();
	afx_msg void OnEditEtc();
	afx_msg void OnLearnFnn2();
	afx_msg void OnLearnIfnn();
	afx_msg void OnFsFastComparison();
	afx_msg void OnLearnFex();
	afx_msg void OnLearnStopThread();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTCOMPUTINGWNDDOC_H__B09095B0_E82F_4197_A697_AB95D2329D20__INCLUDED_)

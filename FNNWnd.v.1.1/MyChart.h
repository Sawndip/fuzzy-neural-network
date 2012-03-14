#if !defined(AFX_MYCHART_H__E8C1AEDE_A893_412E_8067_309E763B073F__INCLUDED_)
#define AFX_MYCHART_H__E8C1AEDE_A893_412E_8067_309E763B073F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyChart.h : header file
//
#include "BarGraph.h"


/////////////////////////////////////////////////////////////////////////////
// CMyChart view

class CMyChart : public CView
{
protected:
	CMyChart();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyChart)

// Attributes
public:
	CBarGraph* m_pGraph;
	int graphToDisplay;
	BOOL okToDraw;

// Operations
public:
	void MyTest();
	void DrawHistogram(float *data, int bins = 1, int subbins = 1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyChart)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyChart();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyChart)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCHART_H__E8C1AEDE_A893_412E_8067_309E763B073F__INCLUDED_)

//Graph.h - Version 4.0 (Brian Convery, November, 2002)

#if !defined(AFX_GRAPH_H__9DB68B4D_3C7C_47E2_9F72_EEDA5D2CDBB0__INCLUDED_)
#define AFX_GRAPH_H__9DB68B4D_3C7C_47E2_9F72_EEDA5D2CDBB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Graph.h : header file
//

#include "GraphDef.h"
#include "GraphDataSet.h"

/////////////////////////////////////////////////////////////////////////////
// CGraph window

class CGraph/* : public CStatic*/
{
// Construction
public:
	CGraph(BOOL display3D);

// Attributes
protected:
	CGraphDataSet *dataSet;
	CString graphTitle;
	CString legendTitle;
	CString axisXLabel;
	CString axisYLabel;
	int legendFontSize;
	int legendMaxText;
	double maxXTick, minXTick;
	double maxYTick, minYTick;
	double yTickStep, xTickStep;
	double numXTicks, numYTicks;
	int pixelsPerXTick, pixelsPerYTick;
	int xTickFontSize, yTickFontSize;
	int xAxisLocation;
	int yAxisLocation;
	BOOL horizontalAlign;
	BOOL xTicksEnabled;
	int xAxisWidth;
	int yAxisHeight;
	CString fontType;
	int quadsDisplay;

	BOOL doubleBufferingEnabled;	//CK Ng - flicker free graphs
	
	BOOL is3D;
	int depthRatio;	//for 3D axis
	int axis3DDepth;

	CObList *dataSeries;

	//print control variables
	int pGraphL, pGraphT, pGraphB, pGraphR;

	//data information for display control
private:
	double maxXvalue;
	double minXvalue;
	double maxYvalue;
	double minYvalue;
	int graphBottom;
	BOOL gridLinesEnabled;

// Operations
public:
	void SetGraphTitle(CString title);
	void SetLegendTitle(CString legend);
	void SetXAxisLabel(CString label);
	void SetYAxisLabel(CString label);
	void SetTickLimits(double minXLimit, double maxXLimit, double minYLimit, double maxYLimit);
	void SetTickStep(double xStep, double yStep);
	void EnableGridLines(BOOL enable);
	void SetHorizontalAlignment(BOOL enable);
	void SetData(double x, double y); 
	void SetData(double x, double y, COLORREF color); //sets the GraphData object with this color
	void SetData(CString xStr, double y); 
	void SetData(CString xStr, double y, COLORREF color); //sets the GraphData object with this color
	void SetColor(COLORREF color); //sets the GraphDataSet with this color
	void SetDataLegend(CString legend);
	void EnableXAxisTicks(BOOL enable);
	void SetDataSetLabel(CString label);
	void EnableDoubleBuffering(BOOL enable);	//Ck Ng - for flicker free graphs
	void SetFontType(CString font);
	void OpenDataSet();
	void CloseDataSet();
	void RemoveAllData();
	void SetPrintMargins(int top, int bottom, int left, int right, int graphTop);

protected:
	void ColorBySeries(BOOL tf);
	void ColorByGroups(BOOL tf);
	BOOL RemoveDataSet(int ds);

	void Enable3DDisplay(BOOL enable);
	void Set3DDepthRatio(int ratio);

	//drawing functions
	void DrawGraphBase(CDC *pDC);
	void DrawAxis(CDC *pDC);
	void DrawLegend(CDC *pDC);

	//printing functions
	void PrintGraphBase(CDC *pDC);
	void PrintAxis(CDC *pDC);
	void PrintLegend(CDC *pDC);

private:
	void DrawTicksGrids(CDC *pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraph)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraph();

	// Generated message map functions
/*protected:
	//{{AFX_MSG(CGraph)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()*/
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPH_H__9DB68B4D_3C7C_47E2_9F72_EEDA5D2CDBB0__INCLUDED_)

//GraphDataSet.h - Version 4.0 (Brian Convery, October, 2002)

#if !defined(__GRAPHDATASET_H__)
#define __GRAPHDATASET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// GraphDataSet.h : header file
//

#include "GraphData.h"
#include "GraphDef.h"

class CGraphDataSet
{
// Construction & Destruction
public:
	CGraphDataSet();
	~CGraphDataSet();

// Attributes
public:
	BOOL useDataLabels;
	BOOL useColor;

protected :
	COLORREF color;
	CObList *graphData;	//of type CGraphData
	CString dataSetLabel;
	CString legendText;

// Operations
public:
	void AddData(double x, double y);
	void AddData(double x, double y, COLORREF dataColor);
	void AddData(CString xLabel, double y);
	void AddData(CString xLabel, double y, COLORREF dataColor);
	void SetColor(COLORREF dataSetColor);
	void SetLegend(CString str);
	void SetLabel(CString label);
	COLORREF GetColor();
	double GetXData(int element);
	double GetYData(int element);
	CString GetXDataStr(int element);
	COLORREF GetDataColor(int element);
	int GetSize();
	CString GetLegend();
	CString GetLabel();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__GRAPHDATASET_H__)


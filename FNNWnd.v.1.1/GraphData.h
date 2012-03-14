//GraphData.h - Version 4.0 (Brian Convery, October, 2002)

#if !defined(__GRAPHDATA_H__)
#define __GRAPHDATA_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraphDef.h"

class CGraphData
{
// Construction & Destruction
public:
	CGraphData();
	~CGraphData();

// Attributes
public:

protected :
	double x;
	double y;
	CString xDataLabel;	//for normal bar type data
	COLORREF color;
	BOOL useLabel;

// Operations
public:
	BOOL GetXUsage();
	CString GetXDataLabel();
	double GetX();
	double GetY();
	void SetData(double xData, double yData);
	void SetData(CString xLabel, double yData);
	void SetColor(COLORREF plotColor);
	COLORREF GetColor();
};

#endif // !defined(__GRAPHDATA_H__)

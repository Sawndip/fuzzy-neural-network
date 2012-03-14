//GraphData.cpp - Version 4.0 (Brian Convery, October, 2002)

#include "stdafx.h"
#include "GraphData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDataSet

CGraphData::CGraphData()
{
	useLabel = FALSE;
}

CGraphData::~CGraphData()
{
}

BOOL CGraphData::GetXUsage()
{
	return useLabel;
}

CString CGraphData::GetXDataLabel()
{
	return xDataLabel;
}

double CGraphData::GetX()
{
	return x;
}

double CGraphData::GetY()
{
	return y;
}

void CGraphData::SetData(double xData, double yData)
{
	x = xData;
	y = yData;
}

void CGraphData::SetData(CString xLabel, double yData)
{
	y = yData;
	xDataLabel = xLabel;
	x = 0;
	useLabel = TRUE;
}

void CGraphData::SetColor(COLORREF plotColor)
{
	color = plotColor;
}

COLORREF CGraphData::GetColor()
{
	return color;
}


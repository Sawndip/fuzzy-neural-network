//GraphDataSet.h - Version 4.0 (Brian Convery, October, 2002)

// GraphDataSet.cpp : implementation file
//

#include "stdafx.h"
#include "GraphData.h"
#include "GraphDataSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphDataSet

CGraphDataSet::CGraphDataSet()
{
	graphData = new CObList();
	legendText = _T("");
	useDataLabels = FALSE;
	useColor = TRUE;
}

CGraphDataSet::~CGraphDataSet()
{
	//delete graphData
	POSITION pos;
 	pos = graphData->GetHeadPosition();
 	while(pos!=NULL)
 	{
  		CGraphData *curData = (CGraphData*)graphData->GetNext(pos);
  		delete curData;
 	}
 	graphData->RemoveAll();
 	delete graphData;
}

void CGraphDataSet::AddData(double x, double y)
{
	CGraphData *curData = new CGraphData();
	curData->SetData(x, y);
	graphData->AddTail((CObject*)curData);
}

void CGraphDataSet::AddData(double x, double y, COLORREF dataColor)
{
	CGraphData *curData = new CGraphData();
	curData->SetData(x, y);
	curData->SetColor(dataColor);
	useColor = FALSE;
	graphData->AddTail((CObject*)curData);
}

void CGraphDataSet::AddData(CString xLabel, double y)
{
	CGraphData *curData = new CGraphData();
	curData->SetData(xLabel, y);
	useDataLabels = TRUE;
	graphData->AddTail((CObject*)curData);
	useDataLabels = TRUE;
}

void CGraphDataSet::AddData(CString xLabel, double y, COLORREF dataColor)
{
	CGraphData *curData = new CGraphData();
	curData->SetData(xLabel, y);
	curData->SetColor(dataColor);
	useColor = FALSE;
	useDataLabels = TRUE;
	graphData->AddTail((CObject*)curData);
	useDataLabels = TRUE;
}

COLORREF CGraphDataSet::GetColor()
{
	return color;
}

COLORREF CGraphDataSet::GetDataColor(int element)
{
	POSITION pos;
	pos = graphData->GetHeadPosition();
	for(int i = 1; i < element; i++)
	{
		graphData->GetNext(pos);
	}
	CGraphData *curData = (CGraphData*) graphData->GetAt(pos);
	return curData->GetColor();
}

double CGraphDataSet::GetXData(int element)
{
	POSITION pos;
	pos = graphData->GetHeadPosition();
	for(int i = 1; i < element; i++)
	{
		graphData->GetNext(pos);
	}
	CGraphData *curData = (CGraphData*) graphData->GetAt(pos);
	return curData->GetX();
}

double CGraphDataSet::GetYData(int element)
{
	POSITION pos;
	pos = graphData->GetHeadPosition();
	for(int i = 1; i < element; i++)
	{
		graphData->GetNext(pos);
	}
	CGraphData *curData = (CGraphData*) graphData->GetAt(pos);
	return curData->GetY();
}

CString CGraphDataSet::GetXDataStr(int element)
{
	POSITION pos;
	pos = graphData->GetHeadPosition();
	for(int i = 1; i < element; i++)
	{
		graphData->GetNext(pos);
	}
	CGraphData *curData = (CGraphData*) graphData->GetAt(pos);
	return curData->GetXDataLabel();
}

void CGraphDataSet::SetLabel(CString label)
{
	dataSetLabel = label;
}

void CGraphDataSet::SetColor(COLORREF dataSetColor)
{
	color = dataSetColor;
	useColor = TRUE;
}

int CGraphDataSet::GetSize()
{
	return graphData->GetCount();
}

void CGraphDataSet::SetLegend(CString str)
{
	legendText = str;
}

CString CGraphDataSet::GetLegend()
{
	return legendText;
}

CString CGraphDataSet::GetLabel()
{
	return dataSetLabel;
}


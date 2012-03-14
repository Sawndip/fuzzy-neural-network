//Graph.cpp - Version 4.0 (Brian Convery, November, 2002)

#include "stdafx.h"
#include "afxtempl.h"
#include "math.h"
#include "Graph.h"
#include "GraphDef.h"
#include "GraphDataSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraph

CGraph::CGraph(BOOL display3D = FALSE)
{
	dataSeries = new CObList();
	legendFontSize = 12;
	legendMaxText = 0;
	minXvalue = 0;
	minYvalue = 0;
	maxXvalue = 0;
	maxYvalue = 0;
	quadsDisplay = 0;
	minXTick = 0.0;
	minYTick = 0.0;
	maxXTick = 0.0;
	maxYTick = 0.0;
	xTickStep = 1;
	yTickStep = 1;
	gridLinesEnabled = FALSE;
	horizontalAlign = FALSE;
	xTicksEnabled = TRUE;

	xTickFontSize = 12;
	yTickFontSize = 12;

	graphTitle = _T("Graph");
	legendTitle = _T("Legend");
	axisXLabel = _T("X Axis");
	axisYLabel = _T("Y Axis");

	doubleBufferingEnabled = FALSE;
	is3D = display3D;
	axis3DDepth = 0;
	depthRatio = 5;		//5% depth
	fontType = "Arial";
}

CGraph::~CGraph()
{
	if(dataSeries)
	{
		POSITION pos = dataSeries->GetHeadPosition();
		while(pos != NULL)
		{
			CGraphDataSet* curDataSet = (CGraphDataSet*)dataSeries->GetNext(pos);
			delete curDataSet;
		}
		dataSeries->RemoveAll();
		delete dataSeries;
	}
}


/*BEGIN_MESSAGE_MAP(CGraph, CStatic)
	//{{AFX_MSG_MAP(CGraph)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()*/

/////////////////////////////////////////////////////////////////////////////
// CGraph message handlers

void CGraph::ColorBySeries(BOOL tf)
{
}

void CGraph::ColorByGroups(BOOL tf)
{
}

void CGraph::OpenDataSet()
{
	dataSet = new CGraphDataSet();
}

void CGraph::CloseDataSet()
{
	dataSeries->AddTail((CObject*)dataSet);
}

void CGraph::SetData(double x, double y)
{
	dataSet->AddData(x, y);
	if(x > maxXvalue)
		maxXvalue = x;
	if(x < minXvalue)
		minXvalue = x;
	if(y > maxYvalue)
		maxYvalue = y;
	if(y < minYvalue)
		minYvalue = y;
}

void CGraph::SetData(double x, double y, COLORREF color) //if ColorBySeries
{
	dataSet->AddData(x, y, color);
	if(x > maxXvalue)
		maxXvalue = x;
	if(x < minXvalue)
		minXvalue = x;
	if(y > maxYvalue)
		maxYvalue = y;
	if(y < minYvalue)
		minYvalue = y;
}

void CGraph::SetData(CString xStr, double y)
{
	dataSet->AddData(xStr, y);
	if(xStr.GetLength() > legendMaxText)
		legendMaxText = xStr.GetLength();
	if(y > maxYvalue)
		maxYvalue = y;
	if(y < minYvalue)
		minYvalue = y;
}

void CGraph::SetData(CString xStr, double y, COLORREF color) //if ColorBySeries
{
	dataSet->AddData(xStr, y, color);
	if(xStr.GetLength() > legendMaxText)
		legendMaxText = xStr.GetLength();
	if(y > maxYvalue)
		maxYvalue = y;
	if(y < minYvalue)
		minYvalue = y;
}

void CGraph::SetDataSetLabel(CString label)
{
	dataSet->SetLabel(label);
}

void CGraph::SetColor(COLORREF color) //if ColorByGroups
{
	dataSet->SetColor(color);
}

void CGraph::SetDataLegend(CString legend)
{
	dataSet->SetLegend(legend);
	if(legend.GetLength() > legendMaxText)
		legendMaxText = legend.GetLength();
}

void CGraph::SetGraphTitle(CString label)
{
	graphTitle = label;
}

void CGraph::SetLegendTitle(CString legend)
{
	legendTitle = legend;
	if(legendTitle.GetLength() > legendMaxText)
		legendMaxText = legendTitle.GetLength();
}

void CGraph::SetXAxisLabel(CString label)
{
	axisXLabel = label;
}

void CGraph::SetYAxisLabel(CString label)
{
	axisYLabel = label;
}

void CGraph::SetTickLimits(double minXLimit, double maxXLimit, 
						   double minYLimit, double maxYLimit)
{
	minXTick = minXLimit;
	minYTick = minYLimit;
	maxXTick = maxXLimit;
	maxYTick = maxYLimit;
}

void CGraph::SetTickStep(double xStep, double yStep)
{
	//tick steps should be positive values...correct them if not

	if(xStep > 0)
		xTickStep = xStep;
	else
		xTickStep = 0 - xStep;
	if(yStep > 0)
		yTickStep = yStep;
	else
		yTickStep = 0 - yStep;
}

void CGraph::SetHorizontalAlignment(BOOL enable)
{
	horizontalAlign = enable;
}

BOOL CGraph::RemoveDataSet(int ds)
{
	BOOL retVal = FALSE;
	if((dataSeries) && (dataSeries->GetCount() >= ds))
	{
		POSITION pos;
		pos = dataSeries->GetHeadPosition();
		for(int i = 1; i < ds; i++)
		{
			dataSeries->GetNext(pos);
		}
		CGraphDataSet* curDataSet = (CGraphDataSet*)dataSeries->GetAt(pos);
		delete curDataSet;
		dataSeries->RemoveAt(pos);
		retVal = TRUE;
	}

	return retVal;
}

void CGraph::RemoveAllData()
{
	while(dataSeries->GetCount())
	{
		POSITION pos = dataSeries->GetHeadPosition();
		CGraphDataSet* curDataSet = (CGraphDataSet*)dataSeries->GetAt(pos);
		delete curDataSet;
		dataSeries->RemoveAt(pos);
	}
	delete dataSeries;
	dataSeries = new CObList();
}

void CGraph::EnableXAxisTicks(BOOL enable)
{
	xTicksEnabled = enable;
}

void CGraph::EnableDoubleBuffering(BOOL enable)
{
	doubleBufferingEnabled = enable;
}

void CGraph::SetFontType(CString font)
{
	fontType = font;
}

void CGraph::DrawGraphBase(CDC *pDC)
{
	CString tickLabel;
	CWnd* graphWnd = pDC->GetWindow();
	CRect graphRect;
	if(doubleBufferingEnabled)
		pDC->GetClipBox(&graphRect);
	else
		graphWnd->GetClientRect(&graphRect);
	TEXTMETRIC	tm;

	//reset graph to be clear background
	CBrush backBrush (WHITE);   //replace with desired background color. format: RGB(##,##,##) if your own color is desired
	CBrush* pOldBackBrush;
	pOldBackBrush = pDC->SelectObject(&backBrush);
	pDC->Rectangle(0, 0, graphRect.Width(), graphRect.Height());
	pDC->SelectObject(pOldBackBrush);

	yAxisHeight = graphRect.Height() - 20;  //for frame window and status bar
	xAxisWidth = graphRect.Width() - 5;  //for frame window
	yAxisLocation = 5;
	xAxisLocation = yAxisHeight - 5;
	graphBottom = yAxisHeight - 5;

	//draw graph title
	CFont titleFont;
	int tFontSize = 24;
	while((graphTitle.GetLength() * (tFontSize / 2)) > xAxisWidth)
	{
		tFontSize -= 2;
	}
	titleFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,fontType);
	CFont* pOldFont = (CFont*) pDC->SelectObject(&titleFont);

	pDC->GetTextMetrics(&tm);

	int charWidth = tm.tmAveCharWidth;

	pDC->TextOut((graphRect.Width() / 2) - ((graphTitle.GetLength() / 2) * charWidth), 10, graphTitle);
	pDC->SelectObject(pOldFont);

	yAxisHeight -= (20 + tm.tmHeight);
}

void CGraph::DrawAxis(CDC *pDC)
{
	TEXTMETRIC	tm;
	pDC->SetTextColor(BLACK);
	CFont sideFont, axisFont, dummyFont;
	CFont* pOldFont;
	int charWidth;
	CString tickLabel;

	//first off, determine how many quadrants are being used
	//there are 9 possible combinations for quadrants :
		//4 regular graphs, depicting each quadrant from a 4 quad graph
		//4 split quads :   __|__		rotated around center point
		//1 4 quad graph
		//resulting types are therefore (the __ and ~~ are the x axis lines)
		//I will number them in order shown below (1 - 9)
		//
		//			|__	|~~	__|	~~|
		//
		//			__|__	~~|~~	|--	--|
		//
		//			--|--
		//
	//in case of disabled ticks, define defaults
	quadsDisplay = 1;
	if((maxYvalue <= 0) && (minYvalue < 0))
		quadsDisplay = 2;

	if((maxXvalue > 0) && (minXvalue >= 0) && (maxYvalue > 0) && (minYvalue >= 0))
	{
		quadsDisplay = 1;
		if(minXTick < 0)
			minXTick = 0;
		if(minYTick < 0)
			minYTick = 0;
		if(maxXTick < 0)
			maxXTick = 0;
		if(maxYTick < 0)
			maxYTick = 0;
	}
	if((maxXvalue > 0) && (minXvalue >= 0) && (maxYvalue <= 0) && (minYvalue < 0))
	{
		quadsDisplay = 2;
		if(minXTick < 0)
			minXTick = 0;
		if(minYTick > 0)
			minYTick = 0;
		if(maxXTick < 0)
			maxXTick = 0;
		if(maxYTick > 0)
			maxYTick = 0;
	}
	if((maxXvalue <= 0) && (minXvalue < 0) && (maxYvalue > 0) && (minYvalue >= 0))
	{
		quadsDisplay = 3;
		if(minXTick > 0)
			minXTick = 0;
		if(minYTick < 0)
			minYTick = 0;
		if(maxXTick > 0)
			maxXTick = 0;
		if(maxYTick < 0)
			maxYTick = 0;
	}
	if((maxXvalue <= 0) && (minXvalue < 0) && (maxYvalue <= 0) && (minYvalue < 0))
	{	
		quadsDisplay = 4;
		if(minXTick > 0)
			minXTick = 0;
		if(minYTick > 0)
			minYTick = 0;
		if(maxXTick > 0)
			maxXTick = 0;
		if(maxYTick > 0)
			maxYTick = 0;
	}
	if((maxXvalue > 0) && (minXvalue < 0) && (maxYvalue > 0) && (minYvalue >= 0))
	{
		quadsDisplay = 5;
		if(minYTick < 0)
			minYTick = 0;
		if(maxYTick < 0)
			maxYTick = 0;
	}
	if((maxXvalue > 0) && (minXvalue < 0) && (maxYvalue <= 0) && (minYvalue < 0))
	{	
		quadsDisplay = 6;
		if(minYTick > 0)
			minYTick = 0;
		if(maxYTick > 0)
			maxYTick = 0;
	}
	if((maxXvalue > 0) && (minXvalue >= 0) && (maxYvalue > 0) && (minYvalue < 0))
	{	
		quadsDisplay = 7;
		if(minXTick < 0)
			minXTick = 0;
		if(maxXTick < 0)
			maxXTick = 0;
	}
	if((maxXvalue <= 0) && (minXvalue < 0) && (maxYvalue > 0) && (minYvalue < 0))
	{	
		quadsDisplay = 8;
		if(minXTick > 0)
			minXTick = 0;
		if(maxXTick > 0)
			maxXTick = 0;
	}
	if((maxXvalue > 0) && (minXvalue < 0) && (maxYvalue > 0) && (minYvalue < 0))
	{
		quadsDisplay = 9;
	}

	//now, the axis lines can be drawn based on the result of quads
	COLORREF axisLineColor;
	COLORREF axis3DLineColor;
	axisLineColor = BLACK;
	axis3DLineColor = DARK_GREY;
	CPen axisPen (PS_SOLID, 2, axisLineColor);
	CPen axis3DPen (PS_SOLID, 2, axis3DLineColor);
	dummyFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pOldFont = (CFont*)pDC->SelectObject(&dummyFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;

	//DOUBLE NOTE:  the following assumes a single digit after
	//the decimal.  If you desire to use integer based ticks,
	//this can be changed to .0f.  If you want to use hundredths
	//or greater decimal places, you need to change it to %2f, etc.
	tickLabel.Format("%.1f", maxYTick);
	//END DOUBLE NOTE

	switch(quadsDisplay)
	{
		case 1 :
			yAxisLocation = 5 + (tickLabel.GetLength() * charWidth) + 45; 
			xAxisLocation = graphBottom - 50;		
			yAxisHeight -= 50;
			xAxisWidth -= (yAxisLocation + 5);
			break;
		case 2 :
			yAxisLocation = 5 + (tickLabel.GetLength() * charWidth) + 45; 
			xAxisLocation = graphBottom - yAxisHeight + 50;		
			yAxisHeight -= 50;
			xAxisWidth -= (yAxisLocation + 5);
			break;
		case 3 :
			yAxisLocation = xAxisWidth - (5 + (tickLabel.GetLength() * charWidth) + 45); 
			xAxisLocation = graphBottom - 50;		
			yAxisHeight -= 50;
			xAxisWidth -= (xAxisWidth - yAxisLocation + 5);
			break;
		case 4 :
			yAxisLocation = xAxisWidth - (5 + (tickLabel.GetLength() * charWidth) + 45); 
			xAxisLocation = graphBottom - yAxisHeight + 50;		
			yAxisHeight -= 50;
			xAxisWidth -= (xAxisWidth - yAxisLocation + 5);
			break;
		case 5 :
			yAxisLocation = (5 + (tickLabel.GetLength() * charWidth)) + (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))); 
			xAxisLocation = graphBottom - 50;		
			yAxisHeight -= 50;
			xAxisWidth -= (5 + (tickLabel.GetLength() * charWidth) + 45);
			break;
		case 6 :
			//I use 20 as the base to move the tick labels to right...don't know why I need this here
			yAxisLocation = (20 + (tickLabel.GetLength() * charWidth)) + (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))); 
			xAxisLocation = graphBottom - yAxisHeight + 50;		
			yAxisHeight -= 50;
			xAxisWidth -= (5 + (tickLabel.GetLength() * charWidth) + 45);
			break;
		case 7 :
			yAxisLocation = 5 + (tickLabel.GetLength() * charWidth) + 45; 
			yAxisHeight -= 50;
			xAxisLocation = graphBottom - 50 - (yAxisHeight / 2);
			xAxisWidth -= (yAxisLocation + 5);
			break;
		case 8 :
			yAxisLocation = xAxisWidth - (5 + (tickLabel.GetLength() * charWidth) + 45); 
			yAxisHeight -= 50;
			xAxisLocation = graphBottom - 50 - (yAxisHeight / 2);
			xAxisWidth -= (xAxisWidth - yAxisLocation + 5);
			break;
		case 9 :
			yAxisLocation = (20 + (tickLabel.GetLength() * charWidth)) + (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))); 
			yAxisHeight -= 50;
			xAxisLocation = graphBottom - 50 - (yAxisHeight / 2);
			xAxisWidth -= (5 + (tickLabel.GetLength() * charWidth) + 45);
			break;
	}

	if(is3D)
	{
		axis3DDepth = (int)(yAxisHeight * depthRatio);
		yAxisHeight -= axis3DDepth;
		xAxisWidth -= axis3DDepth;
	}

	DrawTicksGrids(pDC);

	CPen* pOldAxisPen;
	pOldAxisPen = pDC->SelectObject(&axisPen);
	switch(quadsDisplay)
	{
		case 1 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation - yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation + xAxisWidth, xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 2 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation + yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation + xAxisWidth, xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth + yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation + axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation + yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 3 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation - yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(10, xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth - xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation - xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 4 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation + yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation - xAxisWidth, xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth + yAxisHeight);
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth - xAxisWidth, 
						xAxisLocation + axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation + yAxisHeight);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation - xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation + axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation + axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 5 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation - yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation + (int)(xAxisWidth * (maxXTick / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);  
				pDC->LineTo(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 6 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation + yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation + (int)(xAxisWidth * (maxXTick / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);  
				pDC->LineTo(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth + yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation + axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation + yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 7 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation - (yAxisHeight / 2));  
				pDC->LineTo(yAxisLocation, xAxisLocation + (yAxisHeight / 2));

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation + xAxisWidth, xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 8 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation - (yAxisHeight / 2));  
				pDC->LineTo(yAxisLocation, xAxisLocation + (yAxisHeight / 2));

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(10, xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth - xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation - xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 9 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation - (yAxisHeight / 2));  
				pDC->LineTo(yAxisLocation, xAxisLocation + (yAxisHeight / 2));

				//draw x axis
				pDC->MoveTo(yAxisLocation + (xAxisWidth / 2), xAxisLocation);  
				pDC->LineTo(yAxisLocation - (xAxisWidth / 2), xAxisLocation);
				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
	}
	pDC->SelectObject(pOldAxisPen);

	///////////////////////////////////

	//Axis regions first draw with adjustments for labels
	//Draw labels :
	int tFontSize;
	//draw labels
	tFontSize = 16;
	if(xAxisWidth > yAxisHeight)
	{
		while((axisYLabel.GetLength() * (tFontSize / 2)) > yAxisHeight)
		{
			tFontSize -= 2;
		}
	}
	else
	{
		while((axisXLabel.GetLength() * (tFontSize / 2)) > xAxisWidth)
		{
			tFontSize -= 2;
		}
	}
	axisFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	sideFont.CreateFont(tFontSize, 0, 900, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);

	pDC->SelectObject(&sideFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	
	//dislay y axis label
	switch(quadsDisplay)
	{
		case 1 :
			pDC->TextOut(10, (xAxisLocation - (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 2 :
			pDC->TextOut(10, (xAxisLocation + (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 3 :
			pDC->TextOut(xAxisWidth - 20 + (5 + (tickLabel.GetLength() * charWidth) + 45), (xAxisLocation - (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 4 :
			pDC->TextOut(xAxisWidth - 20 + (5 + (tickLabel.GetLength() * charWidth) + 45), (xAxisLocation + (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 5 :
			pDC->TextOut(10, (xAxisLocation - (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 6 :
			pDC->TextOut(10, (xAxisLocation + (yAxisHeight / 2)) + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 7 :
			pDC->TextOut(10, xAxisLocation + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 8 :
			pDC->TextOut(xAxisWidth - 20 + (5 + (tickLabel.GetLength() * charWidth) + 45), xAxisLocation + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 9 :
			pDC->TextOut(10, xAxisLocation + ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
	}

	pDC->SelectObject(&axisFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	//display x axis label
	switch(quadsDisplay)
	{
		case 1 :
			pDC->TextOut(yAxisLocation + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), graphBottom - 11, axisXLabel);
			break;
		case 2 :
			pDC->TextOut(yAxisLocation + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), xAxisLocation - 50, axisXLabel);
			break;
		case 3 :
			pDC->TextOut(yAxisLocation - (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), graphBottom - 11, axisXLabel);
			break;
		case 4 :
			pDC->TextOut(yAxisLocation - (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), xAxisLocation - 50, axisXLabel);
			break;
		case 5 :
			pDC->TextOut((xAxisWidth / 2) + 50 - ((axisXLabel.GetLength() / 2) * charWidth), graphBottom - 11, axisXLabel);
			break;
		case 6 :
			pDC->TextOut((xAxisWidth / 2) + 50 - ((axisXLabel.GetLength() / 2) * charWidth), xAxisLocation - 50, axisXLabel);
			break;
		case 7 :
			pDC->TextOut(yAxisLocation + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), graphBottom - 11, axisXLabel);
			break;
		case 8 :
			pDC->TextOut(yAxisLocation - (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), graphBottom - 11, axisXLabel);
			break;
		case 9 :
			pDC->TextOut((xAxisWidth / 2) + 50 - ((axisXLabel.GetLength() / 2) * charWidth), graphBottom - 11, axisXLabel);
			break;
	}

	//restore the original font
	pDC->SelectObject(pOldFont);
}

void CGraph::DrawTicksGrids(CDC *pDC)
{
	CString tickLabel;
	int charWidth;
	TEXTMETRIC	tm;

	//draw ticks
	tickLabel.Format("%.1f", maxYTick);

	if((!horizontalAlign) && (!xTicksEnabled))
		numXTicks = maxXTick - minXTick;
	else
		numXTicks = (maxXTick - minXTick) / xTickStep;
	if((horizontalAlign) && (!xTicksEnabled))
		numYTicks = maxYTick - minYTick;
	else
		numYTicks = (maxYTick - minYTick) / yTickStep;

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
	tickLabel.Format("%.1f", maxXTick);
	int maxTLabelLen = tickLabel.GetLength();
	tickLabel.Format("%.1f", minXTick);
	if(tickLabel.GetLength() > maxTLabelLen)
		maxTLabelLen = tickLabel.GetLength();
//END DOUBLE NOTE

	int x, y;	//for loop controls
	int tickYLocation, tickXLocation;

	CFont yFont;
	yFont.CreateFont(yTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pDC->SelectObject(&yFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	pixelsPerYTick = (int) (yAxisHeight / numYTicks);
	pixelsPerXTick = (int) ((xAxisWidth - (maxTLabelLen / 2 * charWidth)) / numXTicks);
	
	//adjust num ticks if negative
	if(numXTicks < 0)
		numXTicks = 0 - numXTicks;
	if(numYTicks < 0)
		numYTicks = 0 - numYTicks;

	//starting with the y axis tick marks...
	switch(quadsDisplay)
	{
		case 1 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					//draw tick mark
					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

					//draw tick label
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick + (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 10 - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);

				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 10 - (tickLabel.GetLength() * charWidth), xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 2 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 10 - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 10 - (tickLabel.GetLength() * charWidth), xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 3 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick + (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 10, tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation + 10, xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 4 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 10, tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation + 10, xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 5 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick + (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 5 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 5 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 6 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 5 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 5 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 7 :
				//starting with positive Y ticks
				for(y = 1; y <= (maxYTick / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", y * yTickStep);
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 10 - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//continue with negative Y ticks
				for(y = 1; y <= ((0 - minYTick) / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}
					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 10 - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 10 - (tickLabel.GetLength() * charWidth), xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 8 :
				//starting with positive Y ticks
				for(y = 1; y <= (maxYTick / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", y * yTickStep);
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 10, tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//continue with negative Y ticks
				for(y = 1; y <= ((0 - minYTick) / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 10, tickYLocation - (tm.tmHeight / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation + 10, xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
		case 9 :
				//starting with positive Y ticks
				for(y = 1; y <= (maxYTick / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", y * yTickStep);
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - 10 - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);
				}

				//continue with negative Y ticks
				for(y = 1; y <= ((0 - minYTick) / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 3, tickYLocation);
						pDC->LineTo(yAxisLocation + 3, tickYLocation);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - 10 - (tickLabel.GetLength() * charWidth), tickYLocation - (tm.tmHeight / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - 10 - (tickLabel.GetLength() * charWidth), xAxisLocation - (tm.tmHeight / 2), tickLabel);
			break;
	}

	//draw X axis tick marks...
	CFont xFont;
	xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	
	pDC->SelectObject(&xFont); //default x-axis font
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;

	//need an adjustment so long labels don't go off graph
	//I only do this for X axis because when the y axis is on the
	//the right side of graph, the x axis runs to far left, and the 
	//label can actually be partially off the left side of graph if
	//it is too long.  With Y axis, there's enough border reserved to
	//handle the 1/2 height of the label characters.

	switch(quadsDisplay)
	{
		case 1 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick + (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10, tickLabel);
			break;
		case 2 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick + (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 10 - tm.tmHeight, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 10 - tm.tmHeight, tickLabel);
			break;
		case 3 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10, tickLabel);
			break;
		case 4 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 10 - tm.tmHeight, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 10 - tm.tmHeight, tickLabel);
			break;
		case 5 :
				//starting with positive X ticks
				for(x = 1; x <= (maxXTick / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", x * xTickStep);
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10, tickLabel);
				}
				//now for negative X ticks
				for(x = 1; x <= ((0 - minXTick) / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10, tickLabel);
			break;
		case 6 :
				//starting with positive X ticks
				for(x = 1; x <= (maxXTick / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", x * xTickStep);
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 10 - tm.tmHeight, tickLabel);
				}
				//now for negative X ticks
				for(x = 1; x <= ((0 - minXTick) / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 10 - tm.tmHeight, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 10 - tm.tmHeight, tickLabel);
			break;
		case 7 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick + (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10 + (yAxisHeight  / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10 + (yAxisHeight  / 2), tickLabel);
			break;
		case 8 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10 + (yAxisHeight  / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 10 + (yAxisHeight  / 2), tickLabel);
			break;
		case 9 :
				//starting with positive X ticks
				for(x = 1; x <= (maxXTick / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", x * xTickStep);
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + (yAxisHeight  / 2) + 10, tickLabel);
				}
				//now for negative X ticks
				for(x = 1; x <= ((0 - minXTick) / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 3);
						pDC->LineTo(tickXLocation, xAxisLocation - 4);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + (yAxisHeight  / 2) + 10, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + (yAxisHeight  / 2) + 10, tickLabel);
			break;
	}
}

void CGraph::DrawLegend(CDC *pDC)
{
	//determine size of legend
	//12 chars per seriesSize + 6 for spacing (3 top and bottom) 
	//+ 1 set for label title(3+12+6) + rectangle (2 chars) + 3 for final bottom buffer
	int legendL, legendT, legendR, legendB, legendWidth;
	int barL, barT, barR, barB;
	int legendHeight;
	TEXTMETRIC tm;

	POSITION pos;
	CGraphDataSet *curDataSet;
	pos = dataSeries->GetHeadPosition();
	curDataSet = (CGraphDataSet*) dataSeries->GetAt(pos);
	if(!curDataSet->useDataLabels)
		legendHeight = 23 + (dataSeries->GetCount() * 18) + 3;
	else
		legendHeight = 23 + (curDataSet->GetSize() * 18) + 3;

	CFont legendFont;
	CFont* pOldFont;
	legendFont.CreateFont(legendFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pOldFont = (CFont*) pDC->SelectObject(&legendFont);
	pDC->GetTextMetrics(&tm);
	int charWidth = tm.tmAveCharWidth;
	pDC->SelectObject(pOldFont);

	legendT = (yAxisHeight / 2) - (legendHeight / 2);
	legendB = legendT + legendHeight;
	legendR = xAxisWidth - 5;
	legendL = legendR - ((legendMaxText * charWidth) + 50);
	legendWidth = legendR - legendL;

	pDC->Rectangle(legendL, legendT, legendR, legendB);
	CFont legendTitleFont;
	legendTitleFont.CreateFont(legendFontSize + 6, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pOldFont = (CFont*) pDC->SelectObject(&legendTitleFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	pDC->TextOut(legendL + (legendWidth / 2) - ((legendTitle.GetLength() / 2) * charWidth), 
					legendT + 3, legendTitle);
	pDC->SelectObject(pOldFont);
	pos = dataSeries->GetHeadPosition();
	for(int i = 0; i < dataSeries->GetCount(); i++)
	{
		//top "Legend" text will use 12 + 3 top + 6 bottom (21 total)
		//each legend label will need 3 chars on top, so the 24 in the offset
		//each label than uses 12 + 3 below + 3 above next label, so 18
		// in the i * offset.
		curDataSet = (CGraphDataSet*) dataSeries->GetAt(pos);
		if(!curDataSet->useDataLabels)
		{
			pOldFont = (CFont*) pDC->SelectObject(&legendFont);

			pDC->TextOut(legendL + 5, legendT + 24 + (i * 18) + 1, curDataSet->GetLegend());

			pDC->SelectObject(pOldFont);

			//draw bar
			COLORREF barColor;
			barColor = curDataSet->GetColor();
			CBrush brush (barColor);
			CBrush* pOldBrush;
			pOldBrush = pDC->SelectObject(&brush);

			barL = legendL + 10 + (legendMaxText * charWidth);
			barT = legendT + 24 + (i * 18) + 1;
			barR = legendR - 5;
			barB = barT + 12;
			pDC->Rectangle(barL, barT, barR, barB);

			pDC->SelectObject(pOldBrush);

			dataSeries->GetNext(pos);
		}
		else
		{
			for(int k = 0; k < curDataSet->GetSize(); k++)
			{
				pOldFont = (CFont*) pDC->SelectObject(&legendFont);

				pDC->TextOut(legendL + 5, legendT + 24 + (k * 18) + 1, curDataSet->GetXDataStr(k + 1));

				pDC->SelectObject(pOldFont);

				//draw bar
				COLORREF barColor;
				if(curDataSet->useColor)
					barColor = curDataSet->GetColor();
				else
					barColor = curDataSet->GetDataColor(k + 1);
				CBrush brush (barColor);
				CBrush* pOldBrush;
				pOldBrush = pDC->SelectObject(&brush);

				barL = legendL + 10 + (legendMaxText * charWidth);
				barT = legendT + 25 + (k * tm.tmHeight), curDataSet->GetXDataStr(k + 1).GetLength();
				barR = legendR - 5;
				barB = barT + 12;
				pDC->Rectangle(barL, barT, barR, barB);

				pDC->SelectObject(pOldBrush);
			}
			i = dataSeries->GetCount();
		}
	}
	xAxisWidth -= (legendWidth + 20);
}

void CGraph::EnableGridLines(BOOL enable)
{
	gridLinesEnabled = enable;
}

void CGraph::SetPrintMargins(int top, int bottom, int left, int right, int graphTop)
{
	//set up rectangle area for showing the graph
	pGraphL = left;
	pGraphT = graphTop;
	pGraphB = pGraphT - 5760/*7200*/;
	pGraphR = right;

	if(pGraphB < bottom)
		pGraphB = bottom; 
}

void CGraph::PrintGraphBase(CDC *pDC)
{
	TEXTMETRIC	tm;
	CString tickLabel;
	CWnd* graphWnd = pDC->GetWindow();
	CRect graphRect;
	graphWnd->GetClientRect(&graphRect);
	pDC->SetMapMode(MM_TWIPS);

	yAxisHeight = pGraphB - pGraphT;
	xAxisWidth = pGraphR - pGraphL;
	yAxisLocation = 100;
	xAxisLocation = yAxisHeight - 100;
	graphBottom = yAxisHeight - 100;

	//draw graph title
	CFont titleFont;
	int tFontSize = -480;
	while((graphTitle.GetLength() * (tFontSize / 2)) > xAxisWidth)
		tFontSize -= 40;

	titleFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH || FF_ROMAN,
			fontType);
	CFont* pOldFont = (CFont*) pDC->SelectObject(&titleFont);

	pDC->GetTextMetrics(&tm);

	int charWidth = tm.tmAveCharWidth;

	pDC->TextOut(pGraphL + (xAxisWidth / 2) - ((graphTitle.GetLength() / 2) * charWidth), pGraphT - 200, graphTitle);
	pDC->SelectObject(pOldFont);

	yAxisHeight += (400 + tm.tmHeight);

}

void CGraph::PrintAxis(CDC *pDC)
{
	TEXTMETRIC	tm;
	pDC->SetTextColor(BLACK);
	CFont sideFont, axisFont, dummyFont;
	CFont* pOldFont;
	int charWidth;
	CString tickLabel;

	//draw axis lines
	COLORREF axisLineColor;	
	COLORREF axis3DLineColor;
	axisLineColor = BLACK;
	CPen axisPen (PS_SOLID, 40, axisLineColor);
	axis3DLineColor = DARK_GREY;
	CPen axis3DPen (PS_SOLID, 40, axis3DLineColor);

	dummyFont.CreateFont(yTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pOldFont = (CFont*)pDC->SelectObject(&dummyFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;

	//DOUBLE NOTE:  the following assumes a single digit after
	//the decimal.  If you desire to use integer based ticks,
	//this can be changed to .0f.  If you want to use hundredths
	//or greater decimal places, you need to change it to %2f, etc.
	tickLabel.Format("%.1f", maxYTick);
	//END DOUBLE NOTE


	switch(quadsDisplay)
	{
		case 1 :
			yAxisLocation = pGraphL + 100 + (tickLabel.GetLength() * charWidth) + 900; 
			xAxisLocation = pGraphB + 1000;		
			yAxisHeight += 1000;
			xAxisWidth -= yAxisLocation;
			break;
		case 2 :
			yAxisLocation = pGraphL + 100 + (tickLabel.GetLength() * charWidth) + 900; 
			yAxisHeight += 1000;
			xAxisLocation = pGraphB - yAxisHeight;		
			xAxisWidth -= yAxisLocation;
			break;
		case 3 :
			yAxisLocation = pGraphL + xAxisWidth - (100 + (tickLabel.GetLength() * charWidth) + 900); 
			xAxisLocation = pGraphB + 1000;		
			yAxisHeight += 1000;
			xAxisWidth -= (xAxisWidth - yAxisLocation);
			break;
		case 4 :
			yAxisLocation = pGraphL + xAxisWidth - (100 + (tickLabel.GetLength() * charWidth) + 900); 
			yAxisHeight += 1000;
			xAxisLocation = pGraphB - yAxisHeight;		
			xAxisWidth -= (xAxisWidth - yAxisLocation);
			break;
		case 5 :
			yAxisLocation = pGraphL + (100 + (tickLabel.GetLength() * charWidth)) + (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))); 
			xAxisLocation = pGraphB + 1000;		
			yAxisHeight += 1000;
			xAxisWidth -= (100 + (tickLabel.GetLength() * charWidth) + 900);
			break;
		case 6 :
			yAxisLocation = pGraphL + (100 + (tickLabel.GetLength() * charWidth)) + (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))); 
			yAxisHeight += 1000;
			xAxisLocation = pGraphB - yAxisHeight;		
			xAxisWidth -= (100 + (tickLabel.GetLength() * charWidth) + 900);
			break;
		case 7 :
			yAxisLocation = pGraphL + 100 + (tickLabel.GetLength() * charWidth) + 900; 
			xAxisLocation = pGraphB + 1000 - (int)((yAxisHeight + 1000) * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep)));		
			yAxisHeight += 1000;
			xAxisWidth -= yAxisLocation;
			break;
		case 8 :
			yAxisLocation = pGraphL + xAxisWidth - (100 + (tickLabel.GetLength() * charWidth) + 900); 
			xAxisLocation = pGraphB + 1000 - (int)((yAxisHeight + 1000) * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep)));		
			yAxisHeight += 1000;
			xAxisWidth -= (xAxisWidth - yAxisLocation);
			break;
		case 9 :
			yAxisLocation = pGraphL + (100 + (tickLabel.GetLength() * charWidth)) + (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))); 
			xAxisLocation = pGraphB + 1000 - (int)((yAxisHeight + 1000) * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep)));		
			yAxisHeight += 1000;
			xAxisWidth -= (100 + (tickLabel.GetLength() * charWidth) + 900);
			break;
	}

	if(is3D)
	{
		axis3DDepth = (int)(yAxisHeight * depthRatio);
		yAxisHeight -= axis3DDepth;
		xAxisWidth -= axis3DDepth;
	}
	
	CPen* pOldAxisPen;
	pOldAxisPen = pDC->SelectObject(&axisPen);
	switch(quadsDisplay)
	{
		case 1 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation - yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation + xAxisWidth, xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}

			break;
		case 2 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation + yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation + xAxisWidth, xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth + yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation + axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation + yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 3 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation - yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(10, xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth - xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation - xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 4 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation + yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation - xAxisWidth, xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth + yAxisHeight);
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth - xAxisWidth, 
						xAxisLocation + axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation + yAxisHeight);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation - xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation + axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation + axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 5 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation - yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation + (int)(xAxisWidth * (maxXTick / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);  
				pDC->LineTo(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 6 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation, xAxisLocation + yAxisHeight);

				//draw x axis
				pDC->MoveTo(yAxisLocation + (int)(xAxisWidth * (maxXTick / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);  
				pDC->LineTo(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth + yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation + axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation + yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + yAxisHeight + axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation + axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 7 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation + (int)(yAxisHeight * (maxYTick / ((maxYTick - minYTick) / yTickStep))));  
				pDC->LineTo(yAxisLocation, xAxisLocation - (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))));

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(yAxisLocation + xAxisWidth, xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 8 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation - (int)(yAxisHeight * (maxYTick / ((maxYTick - minYTick) / yTickStep))));  
				pDC->LineTo(yAxisLocation, xAxisLocation + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))));

				//draw x axis
				pDC->MoveTo(yAxisLocation, xAxisLocation);  
				pDC->LineTo(10, xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation - axis3DDepth - xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation - xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation - xAxisWidth - axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
		case 9 :
				//draw y axis
				pDC->MoveTo(yAxisLocation, xAxisLocation - (int)(yAxisHeight * (maxYTick / ((maxYTick - minYTick) / yTickStep))));  
				pDC->LineTo(yAxisLocation, xAxisLocation + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))));

				//draw x axis
				pDC->MoveTo(yAxisLocation + (int)(xAxisWidth * (maxXTick / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);  
				pDC->LineTo(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))), xAxisLocation);

				if(is3D)
				{
					pDC->SelectObject(&axis3DPen);
					//shadow axis
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth - yAxisHeight);
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);
					pDC->LineTo(yAxisLocation + axis3DDepth + xAxisWidth, 
						xAxisLocation - axis3DDepth);

					//connecting line at apex
					pDC->MoveTo(yAxisLocation, xAxisLocation);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					//connecting line at height
					pDC->MoveTo(yAxisLocation, xAxisLocation - yAxisHeight);
					pDC->LineTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					
					//connecting line at width
					pDC->MoveTo(yAxisLocation + xAxisWidth, xAxisLocation);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);
										
					//top and right borders
					pDC->MoveTo(yAxisLocation + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - yAxisHeight - axis3DDepth);
					pDC->LineTo(yAxisLocation + xAxisWidth + axis3DDepth, 
						xAxisLocation - axis3DDepth);

					pDC->SelectObject(&axisPen);
				}
			break;
	}
	pDC->SelectObject(pOldAxisPen);

	///////////////////////////////////

	//Axis regions first draw with adjustments for labels
	//Draw labels :
	int tFontSize;
	//draw labels
	tFontSize = 320;

	axisFont.CreateFont(tFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	sideFont.CreateFont(tFontSize, 0, 2700, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);

	pDC->SelectObject(&sideFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	
	//dislay y axis label
	switch(quadsDisplay)
	{
		case 1 :
			pDC->TextOut(pGraphL + 200, (xAxisLocation - (yAxisHeight / 2)) - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 2 :
			pDC->TextOut(pGraphL + 200, (xAxisLocation + (yAxisHeight / 2)) - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 3 :
			pDC->TextOut(pGraphL + xAxisWidth - 500 + (tickLabel.GetLength() * charWidth), (xAxisLocation - (yAxisHeight / 2)) - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 4 :
			pDC->TextOut(pGraphL + xAxisWidth - 500 + (tickLabel.GetLength() * charWidth), (xAxisLocation + (yAxisHeight / 2)) - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 5 :
			pDC->TextOut(pGraphL + 200, (xAxisLocation - (yAxisHeight / 2)) - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 6 :
			pDC->TextOut(pGraphL + 200, (xAxisLocation + (yAxisHeight / 2)) - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 7 :
			pDC->TextOut(pGraphL + 200, xAxisLocation - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 8 :
			pDC->TextOut(pGraphL + xAxisWidth - 500 + (tickLabel.GetLength() * charWidth), xAxisLocation - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
		case 9 :
			pDC->TextOut(pGraphL + 200, xAxisLocation - ((axisYLabel.GetLength() * charWidth) / 2), axisYLabel);
			break;
	}

	pDC->SelectObject(&axisFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	//display x axis label
	switch(quadsDisplay)
	{
		case 1 :
			pDC->TextOut(yAxisLocation + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 220, axisXLabel);
			break;
		case 2 :
			pDC->TextOut(yAxisLocation + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), xAxisLocation + 1000, axisXLabel);
			break;
		case 3 :
			pDC->TextOut(yAxisLocation - (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 220, axisXLabel);
			break;
		case 4 :
			pDC->TextOut(yAxisLocation - (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), xAxisLocation + 1000, axisXLabel);
			break;
		case 5 :
			pDC->TextOut(yAxisLocation - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 220, axisXLabel);
			break;
		case 6 :
			pDC->TextOut(yAxisLocation - ((axisXLabel.GetLength() / 2) * charWidth), xAxisLocation + 1000, axisXLabel);
			break;
		case 7 :
			pDC->TextOut(yAxisLocation + (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 220, axisXLabel);
			break;
		case 8 :
			pDC->TextOut(yAxisLocation - (xAxisWidth / 2) - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 220, axisXLabel);
			break;
		case 9 :
			pDC->TextOut(yAxisLocation - ((axisXLabel.GetLength() / 2) * charWidth), pGraphB + 220, axisXLabel);
			break;
	}

	//draw ticks

	if((!horizontalAlign) && (!xTicksEnabled))
		numXTicks = maxXTick - minXTick;
	else
		numXTicks = (maxXTick - minXTick) / xTickStep;
	if((horizontalAlign) && (!xTicksEnabled))
		numYTicks = maxYTick - minYTick;
	else
		numYTicks = (maxYTick - minYTick) / yTickStep;

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
	tickLabel.Format("%.1f", maxXTick);
	int maxTLabelLen = tickLabel.GetLength();
	tickLabel.Format("%.1f", minXTick);
	if(tickLabel.GetLength() > maxTLabelLen)
		maxTLabelLen = tickLabel.GetLength();
//END DOUBLE NOTE
	
	pixelsPerYTick = (int) (yAxisHeight / numYTicks);
	pixelsPerXTick = (int) ((xAxisWidth - (maxTLabelLen / 2 * charWidth)) / numXTicks);
	
	//adjust num ticks if negative
	if(numXTicks < 0)
		numXTicks = 0 - numXTicks;
	if(numYTicks < 0)
		numYTicks = 0 - numYTicks;

	int x, y;	//for loop controls
	int tickYLocation, tickXLocation;

	CFont yFont;
	yFont.CreateFont(yTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pDC->SelectObject(&yFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	//starting with the y axis tick marks...
	switch(quadsDisplay)
	{
		case 1 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					//draw tick mark
					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

					//draw tick label
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick + (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 200 - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);

				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 200 - (tickLabel.GetLength() * charWidth), xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 2 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 200 - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 200 - (tickLabel.GetLength() * charWidth), xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 3 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick + (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 200, tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation + 200, xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 4 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 200, tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation + 200, xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 5 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 80, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick + (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 100 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 100 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 6 :
				for(y = 1; y <= numYTicks; y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 80, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxYTick - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 100 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 100 - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - (tickLabel.GetLength() * charWidth), xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 7 :
				//starting with positive Y ticks
				for(y = 1; y <= (maxYTick / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", y * yTickStep);
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 200 - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//continue with negative Y ticks
				for(y = 1; y <= ((0 - minYTick) / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation + xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - 200 - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - 200 - (tickLabel.GetLength() * charWidth), xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 8 :
				//starting with positive Y ticks
				for(y = 1; y <= (maxYTick / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", y * yTickStep);
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 200, tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//continue with negative Y ticks
				for(y = 1; y <= ((0 - minYTick) / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 60, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation, tickYLocation);
	
						pDC->LineTo(yAxisLocation - xAxisWidth, tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation + 200, tickYLocation + (tm.tmHeight / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation + 200, xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
		case 9 :
				//starting with positive Y ticks
				for(y = 1; y <= (maxYTick / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation - (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 80, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", y * yTickStep);
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - 200 - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);
				}

				//continue with negative Y ticks
				for(y = 1; y <= ((0 - minYTick) / yTickStep); y++) 
				{
					tickYLocation = (int)(xAxisLocation + (y * pixelsPerYTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(yAxisLocation - 80, tickYLocation);
						pDC->LineTo(yAxisLocation + 80, tickYLocation);
					}

					if((gridLinesEnabled) && (!horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(yAxisLocation - (xAxisWidth / 2), tickYLocation);
	
						pDC->LineTo(yAxisLocation + (xAxisWidth / 2), tickYLocation);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (y * yTickStep));
//END DOUBLE NOTE

					if(!((!xTicksEnabled) && (horizontalAlign)))
						pDC->TextOut(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - 200 - (tickLabel.GetLength() * charWidth), tickYLocation + (tm.tmHeight / 2), tickLabel);
				}
				//draw the 0 or mintick
				if(minYTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minYTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(!((!xTicksEnabled) && (horizontalAlign)))
					pDC->TextOut(yAxisLocation - (int)(xAxisWidth * ((0 - minXTick) / ((maxXTick - minXTick) / xTickStep))) - 200 - (tickLabel.GetLength() * charWidth), xAxisLocation + (tm.tmHeight / 2), tickLabel);
			break;
	}

	//draw X axis tick marks...
	CFont xFont;
	xFont.CreateFont(xTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	
	pDC->SelectObject(&xFont); //default x-axis font
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;

	//need an adjustment so long labels don't go off graph
	//I only do this for X axis because when the y axis is on the
	//the right side of graph, the x axis runs to far left, and the 
	//label can actually be partially off the left side of graph if
	//it is too long.  With Y axis, there's enough border reserved to
	//handle the 1/2 height of the label characters.

	switch(quadsDisplay)
	{
		case 1 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick + (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200, tickLabel);
			break;
		case 2 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick + (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 200 + tm.tmHeight, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 200 + tm.tmHeight, tickLabel);
			break;
		case 3 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200, tickLabel);
			break;
		case 4 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 200 + tm.tmHeight, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 200 + tm.tmHeight, tickLabel);
			break;
		case 5 :
				//starting with positive X ticks
				for(x = 1; x <= (maxXTick / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", x * xTickStep);
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200, tickLabel);
				}
				//now for negative X ticks
				for(x = 1; x <= ((0 - minXTick) / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation - yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200, tickLabel);
			break;
		case 6 :
				//starting with positive X ticks
				for(x = 1; x <= (maxXTick / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", x * xTickStep);
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 200 + tm.tmHeight, tickLabel);
				}
				//now for negative X ticks
				for(x = 1; x <= ((0 - minXTick) / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation);
	
						pDC->LineTo(tickXLocation, xAxisLocation + yAxisHeight);

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 200 + tm.tmHeight, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + 200 + tm.tmHeight, tickLabel);
			break;
		case 7 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick + (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200 + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))), tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200 + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))), tickLabel);
			break;
		case 8 :
				for(x = 1; x <= numXTicks; x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", maxXTick - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200 + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))), tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation - 200 + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))), tickLabel);
			break;
		case 9 :
				//starting with positive X ticks
				for(x = 1; x <= (maxXTick / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation + (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", x * xTickStep);
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))) - 200, tickLabel);
				}
				//now for negative X ticks
				for(x = 1; x <= ((0 - minXTick) / xTickStep); x++) 
				{
					tickXLocation = (int)(yAxisLocation - (x * pixelsPerXTick));

					if((!horizontalAlign) && (xTicksEnabled))
					{
						pDC->MoveTo(tickXLocation, xAxisLocation + 60);
						pDC->LineTo(tickXLocation, xAxisLocation - 80);
					}

					if((gridLinesEnabled) && (horizontalAlign))
					{
						//draw grid lines
						COLORREF gridLineColor;
						gridLineColor = LIGHT_GREY;
						CPen gridPen (PS_SOLID, 1, gridLineColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&gridPen);
						pDC->MoveTo(tickXLocation, xAxisLocation - (yAxisHeight / 2));
	
						pDC->LineTo(tickXLocation, xAxisLocation + (yAxisHeight / 2));

						pDC->SelectObject(pOldPen);
					}

//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", 0 - (x * xTickStep));
//END DOUBLE NOTE

					if(xTicksEnabled)
						pDC->TextOut(tickXLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))) - 200, tickLabel);
				}
				//draw the 0 or mintick
				if(minXTick > 0)
//DOUBLE NOTE:  the following assumes a single digit after
//the decimal.  If you desire to use integer based ticks,
//this can be changed to .0f.  If you want to use hundredths
//or greater decimal places, you need to change it to %.2f, etc.
					tickLabel.Format("%.1f", minXTick);
				else
					tickLabel = "0.0";
//END DOUBLE NOTE
				if(xTicksEnabled)
					pDC->TextOut(yAxisLocation - ((tickLabel.GetLength() * charWidth) / 2), xAxisLocation + (int)(yAxisHeight * ((0 - minYTick) / ((maxYTick - minYTick) / yTickStep))) - 200, tickLabel);
			break;
	}

	//restore the original font
	pDC->SelectObject(pOldFont);

}

void CGraph::PrintLegend(CDC *pDC)
{
	//determine size of legend
	//12 chars per seriesSize + 6 for spacing (3 top and bottom) 
	//+ 1 set for label title(3+12+6) + rectangle (2 chars) + 3 for final bottom buffer
	int legendL, legendT, legendR, legendB, legendWidth;
	int barL, barT, barR, barB;
	int legendHeight;
	TEXTMETRIC tm;

	POSITION pos;
	CGraphDataSet *curDataSet;
	pos = dataSeries->GetHeadPosition();
	curDataSet = (CGraphDataSet*) dataSeries->GetAt(pos);
	if(!curDataSet->useDataLabels)
		legendHeight = -500 + (dataSeries->GetCount() * -360) - 60;
	else
		legendHeight = -500 + (curDataSet->GetSize() * -360) - 60;

	CFont legendFont;
	CFont* pOldFont;
	legendFont.CreateFont(legendFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pOldFont = (CFont*) pDC->SelectObject(&legendFont);
	pDC->GetTextMetrics(&tm);
	int charWidth = tm.tmAveCharWidth;
	pDC->SelectObject(pOldFont);

	legendT = pGraphT - 500 + ((pGraphB - pGraphT) / 2) - (legendHeight / 2);
	legendB = legendT + legendHeight;
	legendR = pGraphR - 100;
	legendL = legendR - (((legendMaxText + 1) * charWidth) + 1000);
	legendWidth = legendR - legendL;

	pDC->Rectangle(legendL, legendT, legendR, legendB);
	CFont legendTitleFont;
	legendTitleFont.CreateFont((legendFontSize + 2) * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, fontType);
	pOldFont = (CFont*) pDC->SelectObject(&legendTitleFont);
	pDC->GetTextMetrics(&tm);
	charWidth = tm.tmAveCharWidth;
	pDC->TextOut(legendL + (legendWidth / 2) - ((legendTitle.GetLength() / 2) * charWidth), 
					legendT - 60, legendTitle);
	pDC->SelectObject(pOldFont);
	pos = dataSeries->GetHeadPosition();
	for(int i = 0; i < dataSeries->GetCount(); i++)
	{
		//top "Legend" text will use 12 + 3 top + 6 bottom (21 total)
		//each legend label will need 3 chars on top, so the 24 in the offset
		//each label than uses 12 + 3 below + 3 above next label, so 18
		// in the i * offset.
		curDataSet = (CGraphDataSet*) dataSeries->GetAt(pos);
		if(!curDataSet->useDataLabels)
		{
			pOldFont = (CFont*) pDC->SelectObject(&legendFont);

			pDC->TextOut(legendL + 100, legendT - 480 - (i * 360), curDataSet->GetLegend());

			pDC->SelectObject(pOldFont);

			//draw bar
			COLORREF barColor;
			barColor = curDataSet->GetColor();
			CBrush brush (barColor);
			CBrush* pOldBrush;
			pOldBrush = pDC->SelectObject(&brush);

			barL = legendL + 200 + (legendMaxText * charWidth);
			barT = legendT - 500 - (i * 360), curDataSet->GetLegend().GetLength();
			barR = legendR - 100;
			barB = barT - 240;
			pDC->Rectangle(barL, barT, barR, barB);

			pDC->SelectObject(pOldBrush);

			dataSeries->GetNext(pos);
		}
		else
		{
			for(int k = 0; k < curDataSet->GetSize(); k++)
			{
				pOldFont = (CFont*) pDC->SelectObject(&legendFont);

				pDC->TextOut(legendL + 100, legendT - 480 - (k * 360), curDataSet->GetXDataStr(k + 1));

				pDC->SelectObject(pOldFont);

				//draw bar
				COLORREF barColor;
				if(curDataSet->useColor)
					barColor = curDataSet->GetColor();
				else
					barColor = curDataSet->GetDataColor(k + 1);
				CBrush brush (barColor);
				CBrush* pOldBrush;
				pOldBrush = pDC->SelectObject(&brush);

				barL = legendL + 200 + (legendMaxText * charWidth);
				barT = legendT - 500 - (i * 360);
				barR = legendR - 100;
				barB = barT - 240;
				pDC->Rectangle(barL, barT, barR, barB);

				pDC->SelectObject(pOldBrush);
			}
			i = dataSeries->GetCount();
		}
	}
	xAxisWidth -= legendWidth;

}

void CGraph::Enable3DDisplay(BOOL enable)
{
	is3D = enable;
}

void CGraph::Set3DDepthRatio(int ratio)
{
	depthRatio = ratio;
}

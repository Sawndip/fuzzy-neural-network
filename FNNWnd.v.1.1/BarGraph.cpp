//BarGraph.cpp - Version 4.0 (Brian Convery, September, 2002)

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "math.h"
#include "BarGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBarGraph

CBarGraph::CBarGraph(BOOL display3D = FALSE):CGraph(display3D)
{
}

CBarGraph::~CBarGraph()
{
}

void CBarGraph::Draw(CDC* pDC)
{
//	CMemDC* pMemDC = NULL;
//	if(doubleBufferingEnabled)
//		pDC = pMemDC = new CMemDC(pDC);
	DrawGraphBase(pDC);
	DrawLegend(pDC);
	DrawAxis(pDC);

	//now, draw the data in bar format...

	CGraphDataSet *tmpDataSet;
	POSITION pos;
	pos = dataSeries->GetHeadPosition();

	int maxDataSetSize = 0;
	for(int x = 1; x <= dataSeries->GetCount(); x++)
	{
		tmpDataSet = (CGraphDataSet*)dataSeries->GetNext(pos);
		if(tmpDataSet->GetSize() > maxDataSetSize)
			maxDataSetSize = tmpDataSet->GetSize();
	}

	int barWidth;
	int barL, barR, barT, barB;

	//for 3D graphs...
	int red, blue, green;

	if(!horizontalAlign)
		if(xTicksEnabled)
			barWidth = (int)((pixelsPerXTick * .75) / dataSeries->GetCount());
		else
			barWidth = (int)((xAxisWidth * .75) / (dataSeries->GetCount() * maxDataSetSize));
	else
		if(xTicksEnabled)
			barWidth = (int)((pixelsPerYTick * .75) / dataSeries->GetCount());
		else
			barWidth = (int)((yAxisHeight * .75) / (dataSeries->GetCount() * maxDataSetSize));

	int tickXLocation;
	int tickYLocation;
	pos = dataSeries->GetHeadPosition();
	for(x = 1; x <= dataSeries->GetCount(); x++)
	{
		tmpDataSet = (CGraphDataSet*)dataSeries->GetNext(pos);

		if(!xTicksEnabled)
		{
			//special instance...need to manually draw the dataset label
			tickXLocation = yAxisLocation + (x * (xAxisWidth / dataSeries->GetCount())) - ((xAxisWidth / dataSeries->GetCount()) / 2);
			CFont* pOldFont;
			CFont xFont;
			TEXTMETRIC	tm;
			xFont.CreateFont(xTickFontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,"Arial");
			
			pOldFont = (CFont*)pDC->SelectObject(&xFont);
			pDC->GetTextMetrics(&tm);
			int charWidth = tm.tmAveCharWidth;
			pDC->TextOut(tickXLocation - ((tmpDataSet->GetLabel().GetLength() * charWidth) / 2), xAxisLocation + 10, tmpDataSet->GetLabel());
			pDC->SelectObject(&pOldFont);		
		}
		for(int s = 1; s <= tmpDataSet->GetSize(); s++)
		{
			double dataXValue = 0;	//in case of no data value
			double dataYValue = 0;	//in case of no data value
			if(!horizontalAlign)
			{
				if(xTicksEnabled)
					tickXLocation = yAxisLocation + (int)(minXTick * pixelsPerXTick) + ((tmpDataSet->GetSize() + 1 - s) * pixelsPerXTick);
				else
					tickXLocation = yAxisLocation + (x * (xAxisWidth / dataSeries->GetCount())) - ((xAxisWidth / dataSeries->GetCount()) / 2);

				if(xTicksEnabled)
				{
					dataXValue += tmpDataSet->GetXData(s);
//I'm commenting this condition out since no data gets displayed
//when choosing test case 10.  This may cause problems with other
//bar graphs.  I'm not confortable with this...
//					if((dataXValue == s) || ((0 - dataXValue) == s))
//					{
						dataYValue += tmpDataSet->GetYData(s);

						barL = tickXLocation - (barWidth * dataSeries->GetCount() / 2) + ((x - 1) * barWidth);
						barR = barL + barWidth;
						barT = xAxisLocation - (int)(dataYValue * pixelsPerYTick / yTickStep);
						barB = xAxisLocation;
						switch(quadsDisplay)
						{
							case 1 : barB--;
									break;
							case 2 : barB++;
									break;
							case 3 : barB--;
									break;
							case 4 : barB++;
									break;
							case 5 : barB--;
									break;
							case 6 : barB++;
									break;
							case 7 : 
									if(dataYValue > 0)
										barB--;
									else
										barB++;
									break;
							case 8 : 
									if(dataYValue > 0)
										barB--;
									else
										barB++;
									break;
							case 9 : 
									if(dataYValue > 0)
										barB--;
									else
										barB++;
									break;
						}

						COLORREF barColor;
						barColor = tmpDataSet->GetColor();
						CBrush brush (barColor);
						CBrush* pOldBrush;
						CPen rectPen(PS_SOLID, 1, barColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&rectPen);
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);

						if(is3D)
						{
							//the side - darker than front
							red = GetRValue(barColor);
							green = GetGValue(barColor);
							blue = GetBValue(barColor);
							int sideRed = red - 35;
							int sideGreen = green - 35;
							int sideBlue = blue - 35;
							if(sideRed < 0) sideRed = 0;
							if(sideGreen < 0) sideGreen = 0;
							if(sideBlue < 0) sideBlue = 0;
							COLORREF sideColor;
							sideColor = RGB(sideRed, sideGreen, sideBlue);
							CBrush sideBrush (sideColor);
							CPen sidePen (PS_SOLID, 1, sideColor);
							pDC->SelectObject(&sideBrush);
							pDC->SelectObject(&sidePen);
							CPoint sidePolyArray[4];
							CPoint sp1(barR, barT);
							CPoint sp2(barR, barB);
							CPoint sp3(barR + axis3DDepth, barB - axis3DDepth);
							CPoint sp4(barR + axis3DDepth, barT - axis3DDepth);
							sidePolyArray[0] = sp1;
							sidePolyArray[1] = sp2;
							sidePolyArray[2] = sp3;
							sidePolyArray[3] = sp4;
	
							pDC->Polygon(sidePolyArray, 4);
							
							//the top - lighter than front
							int topRed = red + 35;
							int topGreen = green + 35;
							int topBlue = blue + 35;
							if(topRed > 255) topRed = 255;
							if(topGreen > 255) topGreen = 255;
							if(topBlue > 255) topBlue = 255;
							COLORREF topColor;
							topColor = RGB(topRed, topGreen, topBlue);
							CBrush topBrush (topColor);
							CPen topPen (PS_SOLID, 1, topColor);
							pDC->SelectObject(&topBrush);
							pDC->SelectObject(&topPen);
							CPoint topPolyArray[4];
							CPoint tp1(barL, barT);
							CPoint tp2(barR, barT);
							CPoint tp3(barR + axis3DDepth, barT - axis3DDepth);
							CPoint tp4(barL + axis3DDepth, barT - axis3DDepth);
							topPolyArray[0] = tp1;
							topPolyArray[1] = tp2;
							topPolyArray[2] = tp3;
							topPolyArray[3] = tp4;

							pDC->Polygon(topPolyArray, 4);
							pDC->SelectObject(pOldPen);
						}

						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
//					}
				}
				else
				{
					dataYValue += tmpDataSet->GetYData(s);

					barL = tickXLocation - (barWidth * maxDataSetSize / 2) + ((s - 1) * barWidth);
					barR = barL + barWidth;
					barT = xAxisLocation - (int)(dataYValue * pixelsPerYTick / yTickStep);
					barB = xAxisLocation;
					switch(quadsDisplay)
					{
						case 1 : barB--;
								break;
						case 2 : barB++;
								break;
						case 3 : barB--;
								break;
						case 4 : barB++;
								break;
						case 5 : barB--;
								break;
						case 6 : barB++;
								break;
						case 7 : 
								if(dataYValue > 0)
									barB--;
								else
									barB++;
								break;
						case 8 : 
								if(dataYValue > 0)
									barB--;
								else
									barB++;
								break;
						case 9 : 
								if(dataYValue > 0)
									barB--;
								else
									barB++;
								break;
					}

					COLORREF barColor;
					if(tmpDataSet->useColor)
						barColor = tmpDataSet->GetColor();
					else
						barColor = tmpDataSet->GetDataColor(s);

					CBrush brush (barColor);
					CBrush* pOldBrush;
					CPen rectPen(PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					if(is3D)
					{
						//the side - darker than front
						red = GetRValue(barColor);
						green = GetGValue(barColor);
						blue = GetBValue(barColor);
						int sideRed = red - 35;
						int sideGreen = green - 35;
						int sideBlue = blue - 35;
						if(sideRed < 0) sideRed = 0;
						if(sideGreen < 0) sideGreen = 0;
						if(sideBlue < 0) sideBlue = 0;
						COLORREF sideColor;
						sideColor = RGB(sideRed, sideGreen, sideBlue);
						CBrush sideBrush (sideColor);
						CPen sidePen (PS_SOLID, 1, sideColor);
						pDC->SelectObject(&sideBrush);
						pDC->SelectObject(&sidePen);
						CPoint sidePolyArray[4];
						CPoint sp1(barL, barT);
						CPoint sp2(barR, barT);
						CPoint sp3(barR + axis3DDepth, barT - axis3DDepth);
						CPoint sp4(barL + axis3DDepth, barT - axis3DDepth);
						sidePolyArray[0] = sp1;
						sidePolyArray[1] = sp2;
						sidePolyArray[2] = sp3;
						sidePolyArray[3] = sp4;

						pDC->Polygon(sidePolyArray, 4);
						
						//the top - lighter than front
						int topRed = red + 35;
						int topGreen = green + 35;
						int topBlue = blue + 35;
						if(topRed > 255) topRed = 255;
						if(topGreen > 255) topGreen = 255;
						if(topBlue > 255) topBlue = 255;
						COLORREF topColor;
						topColor = RGB(topRed, topGreen, topBlue);
						CBrush topBrush (topColor);
						CPen topPen (PS_SOLID, 1, topColor);
						pDC->SelectObject(&topBrush);
						pDC->SelectObject(&topPen);
						CPoint topPolyArray[4];
						CPoint tp1(barR, barT);
						CPoint tp2(barR, barB);
						CPoint tp3(barR + axis3DDepth, barB - axis3DDepth);
						CPoint tp4(barR + axis3DDepth, barT - axis3DDepth);
						topPolyArray[0] = tp1;
						topPolyArray[1] = tp2;
						topPolyArray[2] = tp3;
						topPolyArray[3] = tp4;

						pDC->Polygon(topPolyArray, 4);
						pDC->SelectObject(pOldPen);
					}

					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}
			else
			{
				tickYLocation = xAxisLocation - (s * pixelsPerYTick);

				dataYValue += tmpDataSet->GetYData(s);
				if(dataYValue == s)
				{
					dataXValue += tmpDataSet->GetXData(s);

					barL = yAxisLocation + 1;
					barR = yAxisLocation + (int)(dataXValue * pixelsPerXTick / xTickStep);
					barT = tickYLocation + (barWidth * dataSeries->GetCount() / 2) - (x * barWidth);
					barB = barT + barWidth;
					switch(quadsDisplay)
					{
						case 1 : barL++;
								break;
						case 2 : barL++;
								break;
						case 3 : barL--;
								break;
						case 4 : barL--;
								break;
						case 5 : 
								if(dataXValue > 0)
									barL++;
								else
									barL--;
								break;
						case 6 : 
								if(dataXValue > 0)
									barL++;
								else
									barL--;
								break;
						case 7 : barL++;
								break;
						case 8 : barL--;
								break;
						case 9 : 
								if(dataXValue > 0)
									barL++;
								else
									barL--;
								break;
					}

					COLORREF barColor;
					barColor = tmpDataSet->GetColor();
					CBrush brush (barColor);
					CBrush* pOldBrush;
					CPen rectPen(PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					if(is3D)
					{
						//the side - darker than front
						red = GetRValue(barColor);
						green = GetGValue(barColor);
						blue = GetBValue(barColor);
						int sideRed = red - 35;
						int sideGreen = green - 35;
						int sideBlue = blue - 35;
						if(sideRed < 0) sideRed = 0;
						if(sideGreen < 0) sideGreen = 0;
						if(sideBlue < 0) sideBlue = 0;
						COLORREF sideColor;
						sideColor = RGB(sideRed, sideGreen, sideBlue);
						CBrush sideBrush (sideColor);
						CPen sidePen (PS_SOLID, 1, sideColor);
						pDC->SelectObject(&sideBrush);
						pDC->SelectObject(&sidePen);
						CPoint sidePolyArray[4];
						CPoint sp1(barR, barT);
						CPoint sp2(barR, barB);
						CPoint sp3(barR + axis3DDepth, barB - axis3DDepth);
						CPoint sp4(barR + axis3DDepth, barT - axis3DDepth);
						sidePolyArray[0] = sp1;
						sidePolyArray[1] = sp2;
						sidePolyArray[2] = sp3;
						sidePolyArray[3] = sp4;

						pDC->Polygon(sidePolyArray, 4);
						
						//the top - lighter than front
						int topRed = red + 35;
						int topGreen = green + 35;
						int topBlue = blue + 35;
						if(topRed > 255) topRed = 255;
						if(topGreen > 255) topGreen = 255;
						if(topBlue > 255) topBlue = 255;
						COLORREF topColor;
						topColor = RGB(topRed, topGreen, topBlue);
						CBrush topBrush (topColor);
						CPen topPen (PS_SOLID, 1, topColor);
						pDC->SelectObject(&topBrush);
						pDC->SelectObject(&topPen);
						CPoint topPolyArray[4];
						CPoint tp1(barL, barT);
						CPoint tp2(barR, barT);
						CPoint tp3(barR + axis3DDepth, barT - axis3DDepth);
						CPoint tp4(barL + axis3DDepth, barT - axis3DDepth);
						topPolyArray[0] = tp1;
						topPolyArray[1] = tp2;
						topPolyArray[2] = tp3;
						topPolyArray[3] = tp4;

						pDC->Polygon(topPolyArray, 4);
						pDC->SelectObject(pOldPen);
					}

					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}
//	if(pMemDC != NULL)
//		delete pMemDC;
}

int CBarGraph::Print(CDC *pDC)
{
	PrintGraphBase(pDC);
	PrintLegend(pDC);
	PrintAxis(pDC);

	//now, draw the data in bar format...

	CGraphDataSet *tmpDataSet;
	POSITION pos;
	pos = dataSeries->GetHeadPosition();

	int maxDataSetSize = 0;
	for(int x = 1; x <= dataSeries->GetCount(); x++)
	{
		tmpDataSet = (CGraphDataSet*)dataSeries->GetNext(pos);
		if(tmpDataSet->GetSize() > maxDataSetSize)
			maxDataSetSize = tmpDataSet->GetSize();
	}

	int barWidth;
	int barL, barR, barT, barB;
	
	//for 3d display
	int red, blue, green;

	if(!horizontalAlign)
		if(xTicksEnabled)
			barWidth = (int)((pixelsPerXTick * .75) / dataSeries->GetCount());
		else
			barWidth = (int)((xAxisWidth * .75) / (dataSeries->GetCount() * maxDataSetSize));
	else
		if(xTicksEnabled)
			barWidth = (int)((pixelsPerYTick * .75) / dataSeries->GetCount());
		else
			barWidth = (int)((yAxisHeight * .75) / (dataSeries->GetCount() * maxDataSetSize));

	int tickXLocation;
	int tickYLocation;
	pos = dataSeries->GetHeadPosition();
	for(x = 1; x <= dataSeries->GetCount(); x++)
	{
		tmpDataSet = (CGraphDataSet*)dataSeries->GetNext(pos);

		if(!xTicksEnabled)
		{
			//special instance...need to manually draw the dataset label
			tickXLocation = yAxisLocation + (x * (xAxisWidth / dataSeries->GetCount())) - ((xAxisWidth / dataSeries->GetCount()) / 2);
			CFont* pOldFont;
			CFont xFont;
			TEXTMETRIC	tm;
			xFont.CreateFont(xTickFontSize * 20, 0, 0, 0, 700, FALSE, FALSE, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,"Arial");
			
			pOldFont = (CFont*)pDC->SelectObject(&xFont);
			pDC->GetTextMetrics(&tm);
			int charWidth = tm.tmAveCharWidth;
			pDC->TextOut(tickXLocation - ((tmpDataSet->GetLabel().GetLength() * charWidth) / 2), xAxisLocation + 10, tmpDataSet->GetLabel());
			pDC->SelectObject(&pOldFont);		
		}

		for(int s = 1; s <= tmpDataSet->GetSize(); s++)
		{
			double dataXValue = 0;	//in case of no data value
			double dataYValue = 0;	//in case of no data value
			if(!horizontalAlign)
			{
				if(xTicksEnabled)
					tickXLocation = yAxisLocation + (int)(minXTick * pixelsPerXTick) + ((tmpDataSet->GetSize() + 1 - s) * pixelsPerXTick);
				else
					tickXLocation = yAxisLocation + (x * (xAxisWidth / dataSeries->GetCount())) - ((xAxisWidth / dataSeries->GetCount()) / 2);

				if(xTicksEnabled)
				{
					dataXValue += tmpDataSet->GetXData(s);
//I'm commenting this condition out since no data gets displayed
//when choosing test case 10.  This may cause problems with other
//bar graphs.  I'm not confortable with this...
//					if((dataXValue == s) || ((0 - dataXValue) == s))
//					{
						dataYValue += tmpDataSet->GetYData(s);

						barL = tickXLocation - (barWidth * dataSeries->GetCount() / 2) + ((x - 1) * barWidth);
						barR = barL + barWidth;
						barT = xAxisLocation - (int)(dataYValue * pixelsPerYTick / yTickStep);
						barB = xAxisLocation;

						COLORREF barColor;
						barColor = tmpDataSet->GetColor();
						CBrush brush (barColor);
						CBrush* pOldBrush;
						CPen rectPen(PS_SOLID, 1, barColor);
						CPen* pOldPen;
						pOldPen = pDC->SelectObject(&rectPen);
						pOldBrush = pDC->SelectObject(&brush);
						pDC->Rectangle(barL, barT, barR, barB);

						if(is3D)
						{
							//the side - darker than front
							red = GetRValue(barColor);
							green = GetGValue(barColor);
							blue = GetBValue(barColor);
							int sideRed = red - 35;
							int sideGreen = green - 35;
							int sideBlue = blue - 35;
							if(sideRed < 0) sideRed = 0;
							if(sideGreen < 0) sideGreen = 0;
							if(sideBlue < 0) sideBlue = 0;
							COLORREF sideColor;
							sideColor = RGB(sideRed, sideGreen, sideBlue);
							CBrush sideBrush (sideColor);
							CPen sidePen (PS_SOLID, 1, sideColor);
							pDC->SelectObject(&sideBrush);
							pDC->SelectObject(&sidePen);
							CPoint sidePolyArray[4];
							CPoint sp1(barL, barT);
							CPoint sp2(barR, barT);
							CPoint sp3(barR + axis3DDepth, barT - axis3DDepth);
							CPoint sp4(barL + axis3DDepth, barT - axis3DDepth);
							sidePolyArray[0] = sp1;
							sidePolyArray[1] = sp2;
							sidePolyArray[2] = sp3;
							sidePolyArray[3] = sp4;
	
							pDC->Polygon(sidePolyArray, 4);
						
							//the top - lighter than front
							int topRed = red + 35;
							int topGreen = green + 35;
							int topBlue = blue + 35;
							if(topRed > 255) topRed = 255;
							if(topGreen > 255) topGreen = 255;
							if(topBlue > 255) topBlue = 255;
							COLORREF topColor;
							topColor = RGB(topRed, topGreen, topBlue);
							CBrush topBrush (topColor);
							CPen topPen (PS_SOLID, 1, topColor);
							pDC->SelectObject(&topBrush);
							pDC->SelectObject(&topPen);
							CPoint topPolyArray[4];
							CPoint tp1(barR, barT);
							CPoint tp2(barR, barB);
							CPoint tp3(barR + axis3DDepth, barB - axis3DDepth);
							CPoint tp4(barR + axis3DDepth, barT - axis3DDepth);
							topPolyArray[0] = tp1;
							topPolyArray[1] = tp2;
							topPolyArray[2] = tp3;
							topPolyArray[3] = tp4;

							pDC->Polygon(topPolyArray, 4);
							pDC->SelectObject(pOldPen);
						}
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
//					}
				}
				else
				{
					dataYValue += tmpDataSet->GetYData(s);

					barL = tickXLocation - (barWidth * maxDataSetSize / 2) + ((s - 1) * barWidth);
					barR = barL + barWidth;
					barT = xAxisLocation - (int)(dataYValue * pixelsPerYTick / yTickStep);
					barB = xAxisLocation;

					COLORREF barColor;
					if(tmpDataSet->useColor)
						barColor = tmpDataSet->GetColor();
					else
						barColor = tmpDataSet->GetDataColor(s);

					CBrush brush (barColor);
					CBrush* pOldBrush;
					CPen rectPen(PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					if(is3D)
					{
						//the side - darker than front
						red = GetRValue(barColor);
						green = GetGValue(barColor);
						blue = GetBValue(barColor);
						int sideRed = red - 35;
						int sideGreen = green - 35;
						int sideBlue = blue - 35;
						if(sideRed < 0) sideRed = 0;
						if(sideGreen < 0) sideGreen = 0;
						if(sideBlue < 0) sideBlue = 0;
						COLORREF sideColor;
						sideColor = RGB(sideRed, sideGreen, sideBlue);
						CBrush sideBrush (sideColor);
						CPen sidePen (PS_SOLID, 1, sideColor);
						pDC->SelectObject(&sideBrush);
						pDC->SelectObject(&sidePen);
						CPoint sidePolyArray[4];
						CPoint sp1(barL, barT);
						CPoint sp2(barR, barT);
						CPoint sp3(barR + axis3DDepth, barT - axis3DDepth);
						CPoint sp4(barL + axis3DDepth, barT - axis3DDepth);
						sidePolyArray[0] = sp1;
						sidePolyArray[1] = sp2;
						sidePolyArray[2] = sp3;
						sidePolyArray[3] = sp4;

						pDC->Polygon(sidePolyArray, 4);
						
						//the top - lighter than front
						int topRed = red + 35;
						int topGreen = green + 35;
						int topBlue = blue + 35;
						if(topRed > 255) topRed = 255;
						if(topGreen > 255) topGreen = 255;
						if(topBlue > 255) topBlue = 255;
						COLORREF topColor;
						topColor = RGB(topRed, topGreen, topBlue);
						CBrush topBrush (topColor);
						CPen topPen (PS_SOLID, 1, topColor);
						pDC->SelectObject(&topBrush);
						pDC->SelectObject(&topPen);
						CPoint topPolyArray[4];
						CPoint tp1(barR, barT);
						CPoint tp2(barR, barB);
						CPoint tp3(barR + axis3DDepth, barB - axis3DDepth);
						CPoint tp4(barR + axis3DDepth, barT - axis3DDepth);
						topPolyArray[0] = tp1;
						topPolyArray[1] = tp2;
						topPolyArray[2] = tp3;
						topPolyArray[3] = tp4;

						pDC->Polygon(topPolyArray, 4);
						pDC->SelectObject(pOldPen);
					}
					
					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}
			else
			{
				tickYLocation = xAxisLocation - (s * pixelsPerYTick);

				dataYValue += tmpDataSet->GetYData(s);
				if(dataYValue == s)
				{
					dataXValue += tmpDataSet->GetXData(s);

					barL = yAxisLocation;
					barR = yAxisLocation + (int)(dataXValue * pixelsPerXTick / xTickStep);
					barT = tickYLocation + (barWidth * dataSeries->GetCount() / 2) - (x * barWidth);
					barB = barT + barWidth;

					COLORREF barColor;
					barColor = tmpDataSet->GetColor();
					CBrush brush (barColor);
					CBrush* pOldBrush;
					CPen rectPen(PS_SOLID, 1, barColor);
					CPen* pOldPen;
					pOldPen = pDC->SelectObject(&rectPen);
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Rectangle(barL, barT, barR, barB);

					if(is3D)
					{
						//the side - darker than front
						red = GetRValue(barColor);
						green = GetGValue(barColor);
						blue = GetBValue(barColor);
						int sideRed = red - 35;
						int sideGreen = green - 35;
						int sideBlue = blue - 35;
						if(sideRed < 0) sideRed = 0;
						if(sideGreen < 0) sideGreen = 0;
						if(sideBlue < 0) sideBlue = 0;
						COLORREF sideColor;
						sideColor = RGB(sideRed, sideGreen, sideBlue);
						CBrush sideBrush (sideColor);
						CPen sidePen (PS_SOLID, 1, sideColor);
						pDC->SelectObject(&sideBrush);
						pDC->SelectObject(&sidePen);
						CPoint sidePolyArray[4];
						CPoint sp1(barR, barT);
						CPoint sp2(barR, barB);
						CPoint sp3(barR + axis3DDepth, barB - axis3DDepth);
						CPoint sp4(barR + axis3DDepth, barT - axis3DDepth);
						sidePolyArray[0] = sp1;
						sidePolyArray[1] = sp2;
						sidePolyArray[2] = sp3;
						sidePolyArray[3] = sp4;

						pDC->Polygon(sidePolyArray, 4);
						
						//the top - lighter than front
						int topRed = red + 35;
						int topGreen = green + 35;
						int topBlue = blue + 35;
						if(topRed > 255) topRed = 255;
						if(topGreen > 255) topGreen = 255;
						if(topBlue > 255) topBlue = 255;
						COLORREF topColor;
						topColor = RGB(topRed, topGreen, topBlue);
						CBrush topBrush (topColor);
						CPen topPen (PS_SOLID, 1, topColor);
						pDC->SelectObject(&topBrush);
						pDC->SelectObject(&topPen);
						CPoint topPolyArray[4];
						CPoint tp1(barL, barT);
						CPoint tp2(barR, barT);
						CPoint tp3(barR + axis3DDepth, barT - axis3DDepth);
						CPoint tp4(barL + axis3DDepth, barT - axis3DDepth);
						topPolyArray[0] = tp1;
						topPolyArray[1] = tp2;
						topPolyArray[2] = tp3;
						topPolyArray[3] = tp4;

						pDC->Polygon(topPolyArray, 4);
						pDC->SelectObject(pOldPen);
					}

					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}
		}
	}

	return 1;
}

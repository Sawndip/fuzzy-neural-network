// MyChart.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "MyChart.h"

#include "BarGraph.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyChart

IMPLEMENT_DYNCREATE(CMyChart, CView)

CMyChart::CMyChart()
{
	okToDraw = FALSE;
	graphToDisplay = 0;
	m_pGraph = NULL;
}

CMyChart::~CMyChart()
{
	//if (m_pGraph)
	//	delete m_pGraph;
}


BEGIN_MESSAGE_MAP(CMyChart, CView)
	//{{AFX_MSG_MAP(CMyChart)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyChart drawing

void CMyChart::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

	if(okToDraw)
	{
		switch(graphToDisplay)
		{
			case 1 : m_pGraph->Draw(pDC);
					 break;
		}
	}
	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CMyChart diagnostics

#ifdef _DEBUG
void CMyChart::AssertValid() const
{
	CView::AssertValid();
}

void CMyChart::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyChart message handlers

/*
void CMyChart::DrawHistogram(float *data, int bins, int subbins)
{
	int i,j,k;
	CString emotions;

	if (m_pGraph == NULL)
		m_pGraph = new CGraph(BAR_GRAPH);

	m_pGraph->SetGraphTitle("Histogram of Connection Degree");
//	m_pGraph->SetGraphAlignment(HORIZONTAL_ALIGN);

	m_pGraph->SetXAxisAlignment(0);
//	m_pGraph->SetXAxisAlignment(90);
//	m_pGraph->SetXAxisAlignment(270);
//	m_pGraph->SetXAxisAlignment(45);
//	m_pGraph->SetXAxisAlignment(315);
	m_pGraph->SetXAxisLabel("Features");
	//m_pGraph->SetYAxisLabel("Scores");
	m_pGraph->SetGraphQuadType(1);
	//m_pGraph->SetGraphQuadType(3);
//	m_pGraph->SetGraphQuadType(4);
//	m_pGraph->SetTickLimits(-300, 300, 100);

	float maxHist = -1;

	for (k=0; k<subbins; k++)
	{
		maxHist = -1;

		for (j=0; j<bins; j++)
		{
			if (data[j+k*subbins] >= 0)
				maxHist = __max(maxHist, data[j+k*subbins]);
		}
	}

	m_pGraph->SetTickLimits(0, 10, 1);

	CGraphSeries* series = new CGraphSeries[bins];

	for (i=0; i<bins; i++)
	{
		for (j=0; j<subbins; j++)
			series[i].SetData(j, data[i + j*bins]);

		emotions.Format("x%d", i);
		series[i].SetLabel(emotions);
		m_pGraph->AddSeries(&(series[i]));
	}

	for (j=0; j<subbins; j++)
	{
		m_pGraph->SetColor(j, RGB(255*(subbins-j)/subbins, 0, 255*(j)/subbins));
		emotions.Format("Emotion#%d", j);
		m_pGraph->SetLegend(j, emotions);
	}
		
	graphComplete = TRUE;
	Invalidate(TRUE);
}
*/

//20040104
void CMyChart::DrawHistogram(float *data, int bins, int subbins)
{
	int i,j,k;
	CString emotions;

	if(m_pGraph != NULL)
		delete m_pGraph;

	m_pGraph = new CBarGraph(FALSE);
	m_pGraph->SetGraphTitle("Histogram of Connection Index");
	m_pGraph->SetLegendTitle("");
	m_pGraph->EnableGridLines(TRUE);
	m_pGraph->SetXAxisLabel("Features");
	m_pGraph->SetYAxisLabel("");
	m_pGraph->EnableDoubleBuffering(FALSE);
	m_pGraph->EnableXAxisTicks(FALSE);

	/*
	float maxHist = -1;

	for (k=0; k<subbins; k++)
	{
		maxHist = -1;

		for (j=0; j<bins; j++)
		{
			if (data[k+j*subbins] >= 0)
				maxHist = __max(maxHist, data[k+j*subbins]);
		}
	}
	*/

	m_pGraph->SetTickLimits(0.0f, bins-1.0f, 0.0f, 1.0f);
	m_pGraph->SetTickStep(1.0f,0.5f);

	for (i=0; i<bins; i++)
	{
		m_pGraph->OpenDataSet();
	
		for (j=0; j<subbins; j++)
		{
			emotions.Format("Emotion#%d", j);
			m_pGraph->SetDataLegend(emotions);
			m_pGraph->SetData(emotions, data[j+i*subbins], RGB(255*(subbins-j)/subbins, 0, 255*(j)/subbins));
		}

		emotions.Format("x%d", i);
		m_pGraph->SetDataSetLabel(emotions);

		m_pGraph->CloseDataSet();
	}

	graphToDisplay = 1;
	okToDraw = TRUE;
	Invalidate(TRUE);
}


//BarGraph.h - Version 4.0 (Brian Convery, September, 2002)

#if !defined(_BARGRAPH_H_)
#define _BARGRAPH_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// BarGraph.h : header file
//

#include "Graph.h"

class CBarGraph : public CGraph
{
// Construction
public:
	CBarGraph(BOOL display3D);

// Attributes
public:


private:

// Operations
public:
	void Draw(CDC* pDC);
	int Print(CDC *pDC);

private:


// Implementation
public:
	virtual ~CBarGraph();

};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_BARGRAPH_H_)

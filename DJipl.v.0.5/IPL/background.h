/*	if you use this code, please acknoledge me
	-- Ofer Achler
		University of California, San Diego
*/
// axisimageDlg.h : header file
//
//{{AFX_INCLUDES()
#include "ipl.h"
//}}AFX_INCLUDES

/////////////////////////////////////////////////////////////////////////////
// Background Differencing using Median frame and difference calculated using
// 3 color channels
#define BGFRAMES 7
#define BGCENTRAL 4
#define BGDIFFTHRESHOLD 5

typedef struct _RGB {
	char R;
	char G;
	char B;
} RGB;


class Background {
public:
	Background(IplImage* bg);	// standard constructor with the first frame
								// to be used for background generation
	Background(void);	// standard constructor with no initial background
						// image


	// Output functions
	IplImage* Difference(IplImage* cur);	// Difference cur image with
											// generated background
	IplImage* DifferenceMask(IplImage* cur); // Same as above but return mask
	IplImage* Current(void) { return iplCloneImage(bgimage); }    //Returns the current background

	// Input Functions
	void Update(IplImage* bg); // Incorporates bg into the generated background

	~Background();	// Standard Destructor

private:
	void bgupdater(void);

	IplImage* bgimage;
	IplImage* bgframe[BGFRAMES];
	int bgcount;

};

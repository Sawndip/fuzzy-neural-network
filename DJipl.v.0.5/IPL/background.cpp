/*	if you use this code, please acknoledge me
	-- Ofer Achler
		University of California, San Diego
*/
#include "stdafx.h"
#include "background.h"
//#include "ijl.h"
#include "ipl.h"
#include "cv.h"
#include "math.h"

// Precondition: None
// Postcondition: system is initialized
Background::Background(void)
{
	int i;

	bgcount=0;
	bgimage=NULL;
	for(i=0;i<BGFRAMES;i++)
	{
		bgframe[i]=NULL;
	}
}

// Precondition: bg is valid
// Postcondition: system is initialized with bg inputted into the system
Background::Background(IplImage* bg)
{
	int i;

	bgcount=1;
	for(i=bgcount;i<BGFRAMES;i++)
	{
		bgframe[i]=NULL;
	}
	bgimage=iplCloneImage(bg);

	bgframe[0]=iplCloneImage(bg);
	bgupdater();
}

// Precondition bg is valid (and same resolution as the first inputted frame)
// Postcondition: the background is regenerated
void Background::Update(IplImage* bg)
{
	bgframe[bgcount]=iplCloneImage(bg);
	bgcount=bgcount+1;
	if (bgcount >= BGFRAMES) bgcount=0;
	if (bgimage == NULL) bgimage=iplCloneImage(bg);
	bgupdater();
}

// Precondition: cur is valid (and same resolution as generated background)
// Postcondition: returns a newly allocated image that is a mask of all the
//				  regions that have a difference bigger than BGDIFFTHRESHOLD
//				  from the generated background
IplImage* Background::DifferenceMask(IplImage* cur)
{
	IplImage* tmp=NULL;

	tmp=iplCloneImage(cur);
//	double iplNorm(IplImage* bgimage, IplImage* cur, IPL_C);
	iplSubtract(cur, bgimage, tmp);

	iplThreshold(tmp,tmp,BGDIFFTHRESHOLD);
	int x,y;
	char ctmp[3];
	char max[3],min[3];
	min[0]=0;
	min[1]=0;
	min[2]=0;

	max[0]=255;
	max[1]=255;
	max[2]=255;

	for (y=0;y<bgimage->height;y++) 
	{
		for (x=0;x<bgimage->width;x++) 
		{
				iplGetPixel(tmp,x,y,ctmp);
				if (ctmp[0]==0 && ctmp[1]==0 && ctmp[2]==0)
					iplPutPixel(tmp,x,y,min);
				else iplPutPixel(tmp,x,y,max);
		}
	}
	return tmp;
}

// Precondition: cur is valid (and same resolution as generated background)
// Postcondition: returns a newly allocated image with all the
//				  regions that have a difference bigger than BGDIFFTHRESHOLD
//				  from the generated background
IplImage* Background::Difference(IplImage* cur)
{
	IplImage* tmp=NULL;

	tmp=DifferenceMask(cur);
//	double iplNorm(IplImage* bgimage, IplImage* cur, IPL_C);
	iplAnd(cur,tmp,tmp);
//	iplMultiply(cur, tmp, tmp);

	return tmp;
}

// Internal Processor
void Background::bgupdater(void)
{
	int x,y,w,h,count1,count2,numframes;
	char ctmp[3];
	RGB carr[BGFRAMES],cctmp;
		
	w = bgframe[0]->width;
	h = bgframe[0]->height;
			
	for (count1=0; ((bgframe[count1]!=NULL) && (count1<BGFRAMES)); count1++);

	numframes=count1;

	for (y=0;y<h;y++) 
	{
		for (x=0;x<w;x++) 
		{
			for (count1=0;count1<numframes;count1++)
			{
				iplGetPixel(bgframe[count1],x,y,ctmp);
				carr[count1].R=ctmp[0];
				carr[count1].G=ctmp[1];
				carr[count1].B=ctmp[2];
			}
			
			// Ordino carr // Ordering process
			for (count1=0;count1<numframes;count1++)
			{
				for (count2=count1+1;count2<numframes;count2++) 
				{
					if (( (int)carr[count1].R+carr[count1].G+carr[count1].B)>
						( (int)carr[count2].R+carr[count2].G+carr[count2].B) )
					{		
						cctmp = carr[count2];
						carr[count2] = carr[count1];
						carr[count1]= cctmp;
					}
				}
			}
			ctmp[0]=carr[BGCENTRAL].R;
			ctmp[1]=carr[BGCENTRAL].G;
			ctmp[2]=carr[BGCENTRAL].B;
			iplPutPixel(bgimage,x,y,ctmp);
		}
	}
}

//Deconstructor, overrides default since now we have to delete the 
//memory allocated
Background::~Background()
{
	//release the memory
	int i;
	for (i=0; i<BGFRAMES; i++)
	{
		iplDeallocate(bgframe[i],IPL_IMAGE_ALL);
	}
	iplDeallocate(bgimage,IPL_IMAGE_ALL);
}


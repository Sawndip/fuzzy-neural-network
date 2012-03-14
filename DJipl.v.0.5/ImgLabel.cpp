// ImgLabel.cpp: implementation of the CImgLabel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "ImgLabel.h"
#include "ImgMisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImgLabel::CImgLabel()
{

}

CImgLabel::~CImgLabel()
{
	if (map) delete map;
	if (rgns) delete rgns;
	if (areas) delete areas;
	if (rsttbl) delete rsttbl;
	if (labeledimg) delete labeledimg;
}

//
// Condition: there is no label in image boundary
// 
CImage *CImgLabel::labeling(CImage *b, int verbose)
{
	if (b == NULL){
		if(bimg == NULL) return NULL;
	} else
		bimg = b;

	CImage *out_i = NULL;
    int i,j,k;
    int ch=0;
	CImage temp = CImgMisc::MakeImg(bimg->width, bimg->height,8);
	unsigned short *sbuf = new unsigned short[bimg->width * bimg->height];
	const	int Num = 256 * 256;
    int alpha[Num], A[Num];
	unsigned char *p, *p1, *p2;
	unsigned char q, q1, q2;
	unsigned char zz = 0;
	unsigned short *s, *s1, *s2;

    for(i=0;i<Num;i++) { alpha[i]=i; A[i] = 0; }

	// result table[alpha] is sorted incremetally
	p = bimg->getBuffer(0); p1 = bimg->getBuffer(0); p2 = bimg->getBuffer(0);
	s = sbuf; s1 = sbuf; s2 = sbuf;
	int u1, u2, uM, um;
	for(j=0;j<bimg->height;j++, p++, s++){
		if (j == 1){s2 = sbuf; p2=bimg->getBuffer(0);}
		p1 = &zz;
		for(i=0;i<bimg->width;i++){
			// p1 , s1 : i == 0 (boundary)
			if (i != 1) {s1++; p1++;}
			else {s1 = s-1; p1 = p-1;}
	
			// p2, s2 : j == 0 (boundary)
			if (j != 0) {p2++; s2++;}
			else p2 = &zz;
			q = *p;
            if(q==255) {
				q1 = *p1;
				q2 = *p2;
				if ((q1==0) &&(q2==0)) *s = ch++;
				else if((q2==255) && (q1==0)) *s = *s2;
				else if((q1==255) && (q2==0)) *s = *s1;
				else if((q2==255) && (q1==255)){
					*s = *s1;
					if ( *s2 != *s1) {
						// noting equivalene to table
						u1 = alpha[*s1];
						u2 = alpha[*s2];
						uM = MAX(u1, u2);
						um = MIN(u1, u2);
						for(k=0;k<ch;k++){if(alpha[k] == uM) alpha[k] = um; }
					}
				}
				A[*s] ++;
			} else *s = 0;
        }
    }

	if (ch > 255) {
		printf ("Error in Labeling: The number of first segmented objects(%d) exceeds 256\n", ch);
	}
	if (verbose > 1)
		for (i = 0; i < ch; i++) 
			printf ("%3d %3d %3d\n", i+1, alpha[i], A[i]);

	// table sorting
	int blobnum = 0;
	for (i = 0; i < ch; i++) {
		if (alpha[i] >= i) {
			for (j = i+1; j < ch; j++) 
				if (alpha[i] == alpha[j]) alpha[j] = blobnum;
			alpha[i] = blobnum++; 
		} 
	}

	N = blobnum;
	if (verbose > 1)
		printf ("%d labels(ch = %d) generated\n", blobnum, ch);
	if (N > 255) {
		printf ("Error in Labeling: The number of final segmented objects(%d) exceeds 256\n", N);
	}

	out_i = new CImage(temp);

	// second pass
	// background = 0 label
	s = sbuf; 
	for(j=0;j<bimg->height;j++){
		for(i=0;i<bimg->width;i++, s++){
			if(bimg->getPixel(i,j)==255)
				out_i->setPixel(i,j,clip255(alpha[*s] +1)); 
			else out_i->setPixel(i,j,0);
		}
   	}

	labeledimg = out_i;

	// calulate areas simply
	if (areas == NULL)  delete areas;
	areas = new int[N];
	for (i = 0; i < N; i++)  areas[i] = 0;
	for (i = 0; i < ch; i++) areas[alpha[i]] += A[i];

	if (verbose)
		for (i = 0; i < N; i++) 
			printf ("%3d %3d %3d\n", i+1, alpha[i], areas[i]);

	delete sbuf;
	return out_i;
}

float CImgLabel::getArea(int nth)
{
	if (labeledimg == NULL) {
		fprintf (stderr,"not yet labeld\n");
		return -1;
	}
	return areas[nth];
}

int CImgLabel::calArea()
{
	if (labeledimg == NULL) {
		fprintf (stderr,"not yet labeld\n");
		return -1;
	}
	if (areas == NULL)  delete areas;
	areas = new int[N];
	int x, y, n;
	for (n = 0; n < N; n++)  areas[n] = 0;

	unsigned char *p = labeledimg->buf[0];
	for(y=0;y<labeledimg->height;y++){
		for(x=0;x<labeledimg->width;x++, p++){
			if (*p)
			areas[*p-1]++;	
		}
	}

	return 0;
}

void CImgLabel::getRegion(int nth, int &x1, int &y1, int &x2, int &y2)
{
	int x, y, n;

	CPoint *rgn_tbl = new CPoint[N];

	for (n = 0; n < N; n++) {
		rgn_tbl[n].left = labeledimg->width; rgn_tbl[n].top = labeledimg->height;
		rgn_tbl[n].right = 0; 				rgn_tbl[n].bottom = 0;
	}

	unsigned char *p = labeledimg->buf[0];
	for(y=0;y<labeledimg->height;y++){
		for(x=0;x<labeledimg->width;x++, p++){
			if (*p) {
				if (x < rgn_tbl[*p-1].left) rgn_tbl[*p-1].left = x;
				if (x > rgn_tbl[*p-1].right) rgn_tbl[*p-1].right = x;
				if (y < rgn_tbl[*p-1].top) rgn_tbl[*p-1].top = y;
				if (y > rgn_tbl[*p-1].bottom) rgn_tbl[*p-1].bottom = y;
			}
		}
	}

/*
	for (n = 0; n < N; n++) 
		printf ("%3d [%3d %3d %3d %3d]\n", n+1, rgn_tbl[n].x1, rgn_tbl[n].x2, labeledimg->height - rgn_tbl[n].y1,labeledimg->height -  rgn_tbl[n].y2);
*/

	x1 = rgn_tbl[nth].left; y1 = rgn_tbl[nth].top;
	x2 = rgn_tbl[nth].right; y2 = rgn_tbl[nth].bottom;

	rgns = rgn_tbl;
}

BinaryImage *CImgLabel::select(int min_thresh,int max_thresh, SelectionMode mode, Threshold tmode)
{
//	CImage *bin_i = new CImage(labeledimg);
	BinaryImage *bin_i = new BinaryImage(labeledimg);
	switch(mode){
	case Area:
		if (areas == NULL) getArea();
		if (map == NULL) map = new int[N+1];
		map[0] = 0;	// background
		for (int n = 0; n < N; n++) {
			if (tmode == InBound) {
				if ((areas[n] >= min_thresh) && (max_thresh == -1))
					map[n+1] = 255;
				else if ((areas[n] >= min_thresh) && (areas[n] <= max_thresh))
					map[n+1] = 255;
				else map[n+1] = 0;
			} else {	// OutOfBound
				if ((areas[n] <= min_thresh) && (max_thresh == -1))
					map[n+1] = 255;
				else if ((areas[n] <= min_thresh) || (areas[n] >= max_thresh))
					map[n+1] = 255;
				else map[n+1] = 0;
			}
		}
/*
		for (n = 0; n < N; n++) 
			printf (" %d %d (%d)\n", n+1, map[n+1], areas[n]);
*/
		unsigned char *p = bin_i->buf[0];
		for(int y=0;y<bin_i->height;y++)
			for(int x=0;x<bin_i->width;x++, p++)
				*p = map[*p];
		break;
	default:
		printf("mode selection error\n");
	}

	return bin_i;
}
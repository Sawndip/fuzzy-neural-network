//
//
//
#include <stdafx.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <math.h>
#include "cimage.h"
#include "morph.h"

static int verbose = 0;
Morphology::Morphology(int w, int h, unsigned char *buf)
{
	if (buf == NULL && w > 0 && h > 0) buf = new unsigned char[w * h];
	setStructuringElement(w, h, buf);
}

Morphology::~Morphology()
{
	delete se_buf;
}

void Morphology::setStructuringElement(int w, int h, unsigned char *buf)
{
	se_width = w; se_height = h;
	se_buf = new unsigned char[w*h];
	memcpy(se_buf, buf, w*h);
}


void  Morphology::print(FILE *fout, char *name)
{
	if (name != NULL) {
		fprintf(fout,"%s:\n",name);
	}
	for (int y = 0; y < h(); y++) {
		for (int x = 0; x < w(); x++)
			fprintf (fout, "%d", ((pel(x,y) == 255) ? 1 : (pel(x,y) == 0 ? 0 : 2)));
		fprintf(fout, "\n");
	}
}

void Morphology::complement()
{
	unsigned char *p1 = se_buf;
	int x, y;

	for (y = 0; y < se_height; y++)
		for (x = 0; x < se_width; x++, p1++) 
			*p1 = 255 - *p1; 
}

void Morphology::reflection()
{
	unsigned char *buf = new unsigned char[se_height * se_width];
	unsigned char *p1 = se_buf, *p2 = buf;
	int x, y;

	for (y = 0; y < se_height; y++)
		for (x = 0; x < se_width; x++, p2++) 
			*p2 = pel(se_width - x-1, se_height - y-1);

	p1 = se_buf; p2 = buf;
	for (y = 0; y < se_height; y++)
		for (x = 0; x < se_width; x++, p1++, p2++) 
			*p1 = *p2;

	delete buf;
}

Morphology& Morphology::operator = (const Morphology &m)
{
	if ((se_height != ((Morphology)m).h()) || (se_width != ((Morphology)m).w())) {
		delete se_buf;
		se_buf = new unsigned char [se_width * se_height];
	}
	memcpy(se_buf, ((Morphology)m).getStructuringElement(), se_width * se_height);
	return (*this);
}
// Def: 
//                   ^
// C = A + B = {x | (B)x n A != Q }
// 	C(x, y) = sum B(x - i, y-j) & A(i, j) for all i, j
//
BinaryImage &Morphology::dilation(BinaryImage &A, BinaryImage &C)
{
	int x, y, ii, jj;
	int se_width_2 = se_width/2;
	int se_height_2 = se_height/2;

	for (y = A.getROIy1(); y < A.getROIy2(); y++)
		for (x = A.getROIx1(); x < A.getROIx2(); x++) {
			unsigned char v = 0;
			unsigned char *sp = se_buf;
			for (int j = 0; j < se_height && !v; j++) {
				for (int i = 0; i < se_width && !v; i++, sp++) {
					if ((*sp) == 255 || (*sp) == 0) {
						ii = x+i-se_width_2; jj = y+j-se_height_2;
						if (ii >= 0 && ii < A.w() && 0 <= jj && jj < A.h()) {
							if ((*sp) == 255 && (A.pel(ii, jj)== 255)) 
								v = 255;
//							if ((*sp) == 255 && (A.pel(x+se_width_2-i, y+se_height_2-j)== 255) ) 
						}
					}
				}
			}
			C.pel(x, y) = v;
		}
	return C;
}

BinaryImage &Morphology::erosion(BinaryImage &A, BinaryImage &C)
{
	int x, y, ii, jj;
	int se_width_2 = se_width/2;
	int se_height_2 = se_height/2;

	for (y = A.getROIy1(); y < A.getROIy2(); y++)
		for (x = A.getROIx1(); x < A.getROIx2(); x++) {
			unsigned char v = 255;
			unsigned char *sp = se_buf;
			for (int j = 0; j < se_height && v; j++) {
				for (int i = 0; i < se_width && v; i++, sp++) 
					if ((*sp) == 255 || (*sp) == 0) {
						ii = x+i-se_width_2; jj = y+j-se_height_2;
						if (ii >= 0 && ii < A.w() && 0 <= jj && jj < A.h()) {
							if ((*sp) == 255 && A.pel(ii, jj) == 0) 
								v = 0;
						} else v = 0;
					}
			}
			C.pel(x, y) = v;
		}
	return C;
}

BinaryImage &Morphology::opening(BinaryImage &A, BinaryImage &C)
{
	BinaryImage tmp(C);
	erosion(A, tmp);
	dilation(tmp, C);	
	return C;
}

BinaryImage &Morphology::closing(BinaryImage &A, BinaryImage &C)
{
	BinaryImage tmp(C);
	dilation(A, tmp);
	erosion(tmp, C);	
	return C;
}

BinaryImage &Morphology::filling(int x, int y, BinaryImage &A, BinaryImage &C,int n)
{
	BinaryImage X(A); X = 0;
	BinaryImage t1(A); t1 = 0;
	BinaryImage Ac = ~A;
	C = 0;
	X.pel(x, y) = 255;
	if (n == -1) {
		while (X != C) {
			C = X;
			dilation(X, t1);
			intersection(t1, Ac, X);	
		}
	} else {
		for (int i = 0; i < n; i++) {
			C = X;
			dilation(X, t1);
			intersection(t1, Ac, X);	
		}
	}
	return C;
}

BinaryImage &Morphology::HitMiss(BinaryImage &A, BinaryImage &C, int w1, int h1)
{
	if (w1 < se_width) w1 = se_width;
	else if ((w1 - se_width)%2) {
		printf ("Warning:width: centering 1 pel shift\n");
	}
	if (h1 < se_height) h1 = se_height;
	else if ((h1 - se_height)%2) {
		printf ("Warning:height: centering 1 pel shift\n");
	}
	unsigned char *buf2, *p1, *p2;
/*
	buf2 = new unsigned char[se_width * se_height];	
	p2 = buf2; p1 = se_buf;
	for (int y = 0; y < se_height; y++)
		for (int x = 0; x < se_width; x++, p1++, p2++){
			*p2 = 255 - *p1;
		}
	Morphology B2(se_width, se_height, buf2);
*/
	buf2 = new unsigned char[w1 * h1];	
	p2 = buf2; p1 = se_buf;
	int x1 = (w1 - se_width)/2, x2 = se_width + (w1 - se_width)/2;
	int y1 = (h1 - se_height)/2, y2 = se_height + (h1 - se_height)/2;
	for (int y = 0; y < h1; y++)
		for (int x = 0; x < w1; x++, p2++){
			if (x1 <= x && x < x2 && y1 <= y && y < y2) {
				*p2 = 255 - *p1; p1++;
			} else  *p2 = 255;
		}
	Morphology B2(w1, h1, buf2);	// B2 = W - B
/*
	for (y = 0; y < B2.h(); y++){
		for (x = 0; x < B2.w(); x++)
			printf ("%02x", B2.pel(x,y));
		printf("\n");
	}
*/

	BinaryImage X(A), Y(A); X = 0;
	erosion(A, X);
	C = A; ~C;	// temp Z = Ac
	B2.erosion(C, Y);
	C = X & Y;
/*
	B2.reflection();
	B2.dilation(A, Y);
	C = X - Y;
*/
	return C;
}

BinaryImage &Morphology::HitMiss(BinaryImage &A, BinaryImage &C, Morphology &J, Morphology &K)
{
	// should check J & K = null

	BinaryImage X(A.w(),A.h()), Y(A.w(),A.h()); 

	J.erosion(A, X);
//	int x, y;
	if (verbose) {
		A.print(stdout,"A");
		J.print(stdout,"J SE");
		X.print(stdout,"A erosion(A,J)");
	}

	C = A; ~C;	// temp Z = Ac
	K.erosion(C, Y);
	if (verbose) {
		C.print(stdout,"A complement(~A)");
		K.print(stdout,"K SE");
		Y.print(stdout,"A complement, erosion(~A,K)");
	}

	C = X & Y;
	if (verbose)
		C.print(stdout,"A HitMiss = (A - J) n (~A - K)");
	return C;
}

/*
BinaryImage &Morphology::HitMiss_fast(BinaryImage &A, BinaryImage &C, Morphology &J, Morphology &K)
{
	// should check J & K = null

	BinaryImage X(A.w(),A.h()), Y(A.w(),A.h()); 

	J.erosion(A, X);
	C = A; ~C;	// temp Z = Ac
	K.erosion(C, Y);
	C = X & Y;

	return C;
}
*/

//
// for thinning set structuring elements
// REF: Gonzalez pp. 537
void Morphology::setStructElement8(int n, int isJ)
{
	unsigned char *p = se_buf;
	for (int y = 0; y < se_height; y++)
		for (int x = 0; x < se_width; x++, p++)
			*p = 0;
	p = se_buf;
	switch(n) {
	case 0: if (isJ) p[6] = p[7] = p[8] = p[4] = 255; 
			else p[0] = p[1] = p[2] = 255; 
			break;
	case 1: if (isJ) p[6] = p[7] = p[3] = p[4] = 255; 
			else p[5] = p[1] = p[2] = 255; 
			break;
	case 2: if (isJ) p[6] = p[3] = p[4] = p[0] = 255; 
			else p[8] = p[5] = p[2] = 255; 
			break;
	case 3: if (isJ) p[3] = p[4] = p[0] = p[1] = 255; 
			else p[7] = p[8] = p[5] = 255; 
			break;
	case 4: if (isJ) p[4] = p[0] = p[1] = p[2] = 255; 
			else p[6] = p[7] = p[8] = 255; 
			break;
	case 5: if (isJ) p[4] = p[5] = p[1] = p[2] = 255; 
			else p[6] = p[7] = p[3] = 255; 
			break;
	case 6: if (isJ) p[8] = p[4] = p[5] = p[2] = 255; 
			else p[6] = p[3] = p[0] = 255; 
			break;
	case 7: if (isJ) p[7] = p[8] = p[4] = p[5] = 255; 
			else p[3] = p[0] = p[1] = 255; 
			break;
	}
/*
	switch(n) {
	case 0: if (isJ) p[0] = p[1] = p[2] = p[4] = 255; 
			else p[6] = p[7] = p[8] = 255; 
			break;
	case 1: if (isJ) p[0] = p[1] = p[3] = p[4] = 255; 
			else p[5] = p[7] = p[8] = 255; 
			break;
	case 2: if (isJ) p[0] = p[3] = p[4] = p[6] = 255; 
			else p[2] = p[5] = p[8] = 255; 
			break;
	case 3: if (isJ) p[3] = p[4] = p[6] = p[7] = 255; 
			else p[1] = p[2] = p[5] = 255; 
			break;
	case 4: if (isJ) p[4] = p[6] = p[7] = p[8] = 255; 
			else p[0] = p[1] = p[2] = 255; 
			break;
	case 5: if (isJ) p[4] = p[5] = p[7] = p[8] = 255; 
			else p[0] = p[1] = p[3] = 255; 
			break;
	case 6: if (isJ) p[2] = p[4] = p[5] = p[8] = 255; 
			else p[0] = p[3] = p[6] = 255; 
			break;
	case 7: if (isJ) p[1] = p[2] = p[4] = p[5] = 255; 
			else p[3] = p[6] = p[7] = 255; 
			break;
	}
*/
/*
	switch(n) {
	case 0: p[4] = p[6] = p[7] = p[8] = 255; p[3] = p[5] = 127; break;
	case 1: p[3] = p[4] = p[6] = p[7] = 255; p[0] = p[8] = 127;break;
	case 2: p[0] = p[3] = p[4] = p[6] = 255; p[1] = p[7] = 127; break;
	case 3: p[0] = p[1] = p[3] = p[4] = 255; p[2] = p[6] = 127; break;
	case 4: p[0] = p[1] = p[2] = p[4] = 255; p[3] = p[5] = 127; break;
	case 5: p[1] = p[2] = p[4] = p[5] = 255; p[0] = p[8] = 127; break;
	case 6: p[2] = p[4] = p[5] = p[8] = 255; p[1] = p[7] = 127; break;
	case 7: p[4] = p[5] = p[7] = p[8] = 255; p[2] = p[6] = 127; break;
	}
*/
/*
	switch(n) {
	case 0: p[0] = p[1] = p[2] = p[3] = 255; break;
	case 1: p[0] = p[1] = p[3] = p[6] = 255; break;
	case 2: p[0] = p[3] = p[6] = p[7] = 255; break;
	case 3: p[3] = p[6] = p[7] = p[8] = 255; break;
	case 4: p[5] = p[6] = p[7] = p[8] = 255; break;
	case 5: p[2] = p[5] = p[7] = p[8] = 255; break;
	case 6: p[1] = p[2] = p[5] = p[8] = 255; break;
	case 7: p[0] = p[1] = p[2] = p[5] = 255; break;
	}
*/
}

BinaryImage &Morphology::thinning(BinaryImage &A, BinaryImage &C, int iterations)
{
	BinaryImage X(A); BinaryImage Y(A);
	BinaryImage Z(A);

	if (iterations == -1) {
		
		while (X != C) {
			for (int i = 0; i < 8; i++) {
				setStructElement8(i);
				// basic thinning
				HitMiss(X, Y, se_width, se_height);
				Z = Y; ~Z;
				C = X & Z;
			}
			X = C;
		}
	} else {
//		char buf[50];
		C = A;
		Morphology J(3, 3), K(3, 3);
		int count = 0;
		for (int n = 0; n < iterations; n++) {
			for (int i = 0; i < 8; i++) {
				X = C;
				J.setStructElement8(i, 1);	// J
				K.setStructElement8(i, 0);	// K
				// basic thinning
				HitMiss(X, Y, J, K);
				~Y; 
				C = X & Y;
				if (verbose) {
					printf("\n%d-th iteration, %d-th sequence\n", n, i);
					C.print(stdout,"C");
				}

				if(X != C) {	 
					count=0;
					printf ("%d %d: X != C\n", n, i);
				} else {
					count ++;
					printf ("%d %d: X == C\n", n, i);
				}
				if(count >= 8) {
					printf ("iteration end: %d times\n", n);
					return C;
				}
			}
		}
	}
	return C;
}

BinaryImage &Morphology::thickening(BinaryImage &A, BinaryImage &C, int iterations)
{
	BinaryImage X(A); BinaryImage Y(A);

	if (iterations == -1) {
		C = 0;	
		while (X != C) {
			X = C;
			for (int i = 0; i < 8; i++) {
				setStructElement8(i);
				// basic thickening
				HitMiss(X, Y, 3);
				C = X | Y;
			}
		}
	} else {
//		char buf[50];
		Morphology J(3, 3), K(3, 3);
		C = A;
		for (int n = 0; n < iterations; n++) {
			int count = 0;
			printf("\n%d-th iteration\n", n);
			for (int i = 0; i < 8; i++) {
				X = C;
				J.setStructElement8(i);
				K = J;
				K.reflection();
				// basic thickening
				HitMiss(X, Y, J, K);
/*
				X.print(stdout,"X");
//				Y.print(stdout,"Y = X * (J,K)");
*/
				
				C = X | Y;
/*
				C.print(stdout,"C = X | Y");
				sprintf(buf,"thick%d%d.bin",n,i);
				C.save(buf);
				if(X != C) {	 
					printf ("%d %d: X != C\n", n, i);
				} else {
					count ++;
					printf ("%d %d: X == C\n", n, i);
				}
*/
			}
/*
			if(count < 8) {
				printf ("%d iteration :X != C\n", n);
			} else
				printf ("%d iteration :X == C\n", n);
*/
		}
	}
	return C;
}

/*
BinaryImage &Morphology::convexhull(BinaryImage &A, BinaryImage &C, int iterations)
{
	BinaryImage X(A); BinaryImage Y(A);

	if (iterations == -1) {
		C = A;
		while (X != C) {
			X = C;
			for (int i = 0; i < 8; i++) {
				setStructElement8(i);
				// basic thickening
				HitMiss(X, Y, 3);
				C = X | Y;
			}
		}
	} else {
		char buf[50];
		C = A;
		for (int n = 0; n < iterations; n++) {
			X = C;
			for (int i = 0; i < 4; i++) {
				X = C;
				setStructElement4(i);
				// basic thickening
				HitMiss(X, Y, 3);
				C = A | Y;
				sprintf(buf,"convex%d%d.bin",n,i);
				C.save(buf);
			}
		}
	}
	return C;
}
*/


// dilate any point of either zero or intermediate density
GrayImage &Morphology::dilation2(GrayImage &A, GrayImage &C, int mmx, int mx, int my, int mth)
{
	int densSum;
	int i,j,k,l;
	int mx_2 = mx/2, my_2 = my/2;

	for(j=1;j<C.h()-1;j++)
		for(i=1;i<C.w()-1;i++)
		{
			densSum = 0;
			for(k=0;k<mx;k++)
				for(l=0;l<my;l++)
					densSum += (A.getPixel(i+k-mx_2,j+l-my_2) == mmx) ? 1 : 0;

			if (densSum > mth)
				C.setPixel(i,j,mmx);
			else
				C.setPixel(i,j,A.getPixel(i,j));
		}

	return C;
}

BinaryImage &Morphology::dilation3(BinaryImage &A, BinaryImage &C, int mmx, int mx, int my, int mth)
{
	int densSum;
	int i,j,k,l;//,ii,jj;
	int mx_2 = mx/2, my_2 = my/2;

	// dilate any point of either zero or intermediate density
	for(j=0;j<A.h();j++)
		for(i=0;i<A.w();i++)
		{
			if (A.getPixel(i,j) == 0)
			{
				densSum = 0;
				for(k=0;k<mx;k++)
					for(l=0;l<my;l++)
						densSum += (A.getPixel(i+k-mx_2,j+l-my_2) == mmx) ? 1 : 0;

				if (densSum > mth)
					C.setPixel(i,j,mmx);
				else
					C.setPixel(i,j,0);
			}
		}

	return C;
}

// erode any full-density point
GrayImage &Morphology::erosion2(GrayImage &A, GrayImage &C, int mmx, int mx, int my, int mth)
{
	int densSum;
	int i,j,k,l,ii,jj;
	int mx_2 = mx/2, my_2 = my/2;

	for(j=1;j<C.h()-1;j++)
		for(i=1;i<C.w()-1;i++)
		{
			densSum = 0;
			for(k=0;k<mx;k++)
				for(l=0;l<my;l++)
				{
					ii = i+k-mx_2;
					jj = j+l-my_2;
					densSum += (A.getPixel(ii,jj) == mmx) ? 1 : 0;
				}

			if (densSum < mth)
				C.setPixel(i,j,0);
			else
				C.setPixel(i,j,A.getPixel(i,j));
		}

	return C;
}

// search detected pixel
BinaryImage &Morphology::erosion3(BinaryImage &A, BinaryImage &C, int mmx, int mx, int my, int mth)
{
	int densSum;
	int i,j,k,l;
	int mx_2 = mx/2, my_2 = my/2;

	
	for(j=0;j<A.h();j++)
		for(i=0;i<A.w();i++)
		{
			if (A.getPixel(i,j) == mmx)
			{
				densSum = 0;
				for(k=0;k<mx;k++)
					for(l=0;l<my;l++)
						densSum += (A.getPixel(i+k-mx_2,j+l-my_2) == mmx) ? 1 : 0;

				if (densSum > mth)
					C.setPixel(i,j,mmx);
				else
					C.setPixel(i,j,0);
			}
		}

	return C;
}

GrayImage &Morphology::denseMap(BinaryImage &A, GrayImage &C, int sx, int sy, int mx, int my)
{
	int densSum;
	int i,j,k,l;
	int mx_2 = mx/2, my_2 = my/2;
	
	for(j=0;j<C.h();j++)
		for(i=0;i<C.w();i++)
		{
			densSum = 0;
			for(k=0;k<mx;k++)
				for(l=0;l<my;l++)
					densSum += (A.getPixel(sx*i+k-mx_2,sy*j+l-my_2) == 255) ? 1 : 0;

			C.setPixel(i,j,densSum);
		}

	return C;
}
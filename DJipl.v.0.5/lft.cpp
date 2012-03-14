#include "stdafx.h"
#include <stdio.h>
#include <iostream.h>
#include <stdlib.h>
/*
#include <Inventor/SbLinear.h>
#include <Inventor/SbPList.h>
*/
#include "lft.h"		// slice DFT routine
#include "Complex.h"	// complex variable routine
#include "image.h"
#include "ImagePixel.h"
// #include <transforms.h>

#define M_PI	3.1415926535897932384626433832795
#define PI2 M_PI*2.0
#define NEXTMX 12

int prime[NEXTMX] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 }; 

LFT::LFT(CmyImage *g, StackingMode mode)
{
	zin = NULL; zout = NULL; N= 0;
	smode = mode;
	//setImage(g, mode);
	img = g;
}

LFT::~LFT()
{
	if(zin != NULL)
	{
		delete [] zin;
		zin = NULL;
	}
	if(zout != NULL)
	{
		delete [] zout;
		zout = NULL;
	}
}

// set 'zin' as 'z1' with 'length'
// IN	z1, length
// OUT	
void  LFT::set(Complex  *z1, int length)
{
	if (N != length) {
		if (zin != NULL)
		{
			delete zin;
			zin = NULL;
		}
		if (zout != NULL)
		{
			delete zout;
			zout = NULL;
		}
		N = length;
		zin = new Complex [N];	
		zout = NULL;
	}

	int i ;
	if (smode == NotDefined) for (i = 0; i < N; i++) zin[i] = 0;
	else for (i = 0; i < N; i++) zin[i] = z1[i];
}

// return 'z1' with set as g/h sequence using given 'gimg' w.r.t. 'mode'
// IN	gimg, mode
// OUT	z1
Complex *LFT::setImage(CmyImage *gimg, StackingMode mode)
{
	if (gimg) img = gimg;
	if (img == NULL) return NULL;
	int len = img->GetWidth() * img->GetHeight();
	Complex *z1 = new Complex [len];

	int i = 0;

	if (mode != NotDefined) smode = mode;

	// pixel pointer to image
	if (img->GetBitCount() == 8)
	{
		CPixelPtr ptr(*img);

		if (mode == X|| mode == Horizontal) {
			for (int y = 0; y < img->GetHeight(); y++)
				for (int x = 0; x < img->GetWidth(); x++, i++) 
					z1[i] = (double)ptr[y][x];	
		} else if (mode == Y|| mode == Vertical) {
			for (int x = 0; x < img->GetWidth(); x++) 
				for (int y = 0; y < img->GetHeight(); y++, i++)
					z1[i] = (double)ptr[y][x];	
		}
	}
	else
	{
		CColorPixelPtr ptr(*img);

		if (mode == X|| mode == Horizontal) {
			for (int y = 0; y < img->GetHeight(); y++)
				for (int x = 0; x < img->GetWidth(); x++, i++) 
					z1[i] = (double)ptr[y][x].R;	
		} else if (mode == Y|| mode == Vertical) {
			for (int x = 0; x < img->GetWidth(); x++) 
				for (int y = 0; y < img->GetHeight(); y++, i++)
					z1[i] = (double)ptr[y][x].R;	
		}
	}

	set(z1, len);
	return z1;
}

// normalize 'z' with 'n'
// IN	z, n
// OUT	z
Complex *LFT::normalize(Complex *z, int n)
{
  double nfactor = (double) 1.0 / (double) n;
  for (int i = 0; i < n; i++)
	z[i] *= nfactor;
  return z;
}

// transform using FFT with 'gimg' w.r.t. 'mode'
// IN	gimg, mode
// OUT	zout
Complex *LFT::transform(CmyImage *gimg, StackingMode mode)
{
	Complex *z1 = setImage(gimg, mode);
	Complex *z2 = new Complex [N];

	zout = fft(z1, z2, N, 1);		
	
	if (z1 == zout)
	{
		delete [] z2;
		z2 = NULL;
	}
	else
	{
		delete [] z1;
		z1 = NULL;
	}

	zout = normalize(zout, N);
	return zout;
}

// xtransform
Complex *LFT::xtransform(CmyImage *gimg)
{
	return transform(gimg, X);
}

// ytransform
Complex *LFT::ytransform(CmyImage *gimg)
{
	return transform(gimg, Y);
}

double *LFT::getMag()
{
	double *mag = new double[N];
	for (int i = 0; i < N; i++) mag[i] = abs(zout[i]);
	return mag;
}

// get angle in radian ( -M_PI ~ M_PI )
double *LFT::getAngle()
{
	double *ang = new double[N];
	for (int i = 0; i < N; i++) ang[i] = arg(zout[i]);
	return ang;
}

double *LFT::getReal()
{
	double *d = new double[N];
	for (int i = 0; i < N; i++) d[i] = real(zout[i]);
	return d;
}

double *LFT::getImag()
{
	double *d = new double[N];
	for (int i = 0; i < N; i++) d[i] = imag(zout[i]);
	return d;
}

void LFT::show()
{
	for (int i = 0; i < N; i++) {
		printf ("%d %f %f\n", i, real(zin[i]), imag(zin[i])); 
//		cout << i << " " << zin[i] << "\n";
	}
}

// print to file with filename 'fout' using 'in' method
// IN	fout, in
// OUT
// in: 0(zin), 1(zout), 2(zin & zout)
void LFT::print(FILE *fout, int in)
{
	int i;

	switch(in) {
		case 0:
			for (i = 0; i < N; i++) 
				fprintf(fout,"%12.7f %12.7f\n", real(zin[i]), imag(zin[i]));
			break;
		case 1:
			for (i = 0; i < N; i++) 
				fprintf(fout,"%12.7f %12.7f\n", real(zout[i]), imag(zout[i]));
			break;
		case 2:
			for (i = 0; i < N; i++) 
				fprintf(fout,"%12.7f %12.7f %12.7f %12.7f\n", real(zin[i]), imag(zin[i]), real(zout[i]), imag(zout[i]));
			break;
		default:
			break;
	}
}

// print magnitude
// IN	fout, in, single, from, to, Hzflg
// OUT
// in: 
void LFT::printMag(FILE *fout, int in, int single, int from, int to, int Hzflg)
{
	int normalfactor = 
		(in == 1) ? 
			((smode == X) ? img->GetWidth(): img->GetHeight()):
			((smode == X) ? img->GetHeight(): img->GetWidth());
	if (Hzflg) {
		from *= normalfactor;
		to *= normalfactor;
	}
	if (from <0 || from > N) from = 0;
	if (to < 0 || to > N) to = N;

	int i;
	switch(in) {
	case 1:
		for (i = from; i < to; i++) {
			fprintf(fout,"%12.7f %12.7f\n",
				single == 1 ? ((double)i/(double)normalfactor):((double)(i%normalfactor)/(double)normalfactor), 
				abs(zin[i]));
			if (!single && ((i % normalfactor) == normalfactor -1))
				fprintf (fout,"\n");
		}
		break;
	case 0:
		for (i = from; i < to; i++) {
			fprintf(fout,"%12.7f %12.7f\n",
				single == 1 ? ((double)i/(double)normalfactor):((double)(i%normalfactor)/(double)normalfactor), 
			abs(zout[i]));
			if (!single && ((i % normalfactor) == normalfactor -1))
				fprintf (fout,"\n");
		}
		break;
	case 2:
		break;
	default:
		break;
	}
}

int LFT::printMag(char *filename, int in, int single, int from, int to, int Hzflg)
{
	FILE *fout;
	if (filename == NULL) fout = stdout;
	else if ((fout = fopen(filename,"w")) == NULL) return 0;
	printMag(fout, in, single, from, to, Hzflg);
	if (filename != NULL) fclose (fout);
	return 1;
}

// graph of the results
void LFT::graph(int in, int single, char *titlename)
{
	char *filenamei = "tmplfti.dat", *filenameo = "tmplfto.dat";
	char buf[400];

	if (in != 0) printMag(filenamei, 1, single);
	if (in != 1) printMag(filenameo, 0, single);

	if (in < 2) {
		char *graph_cmd = "-g 2 -I a -S 23 0.02"
			" --tick-size -.015 -f .06 --title-font-size 0.09"
			" --title-font-name Times-Roman --font-name HersheyGothic-English"
			" -X \'x\' -Y \'Magnitude\'"
			" --upward-shift .25 --right-shift .25"
			" --width-of-plot .5 --height-of-plot .5";
		sprintf (buf,"graph -T X %s %s -L '%s' %s&", 
						graph_cmd, in == 1 ? "": "-l Y", 
						smode == X ? "G(p)" : "H(q)", 
						in == 1 ? (smode == X ? "g(s)" : "h(t)"): 
								  (smode == X ? "G(p)" : "H(q)"), 
						in == 1 ? filenamei : filenameo);
	} else if (in == 2) {
		char *graph_cmd = "-g 2 -I a -S 23 0.02"
			" --tick-size -.015 -f .06 --title-font-size 0.09"
			" --title-font-name Times-Roman --font-name HersheyGothic-English"
			" -X \'x\' -Y \'Magnitude\'"
			" --upward-shift .25 --right-shift .25"
			" -w .5 -h .5";
		char titlebuf[100];
		if (titlename) sprintf (titlebuf,"(%s)", titlename);  
		else sprintf (titlebuf,"");  
		puts(titlebuf);
		sprintf (buf,"graph -T X --reposition 0 0 1 -h 0.25 -L '%s' %s"
					" --reposition 0 0.4 1 -h 0.25 -l Y -g 3 -L '%sLFT %s%s' %s&", 
						"Input", filenamei,
						smode == X ? "x" : "y", 
						"Output", 
						titlebuf,
						filenameo);
	}
	puts (buf);
	system(buf);
}

#define	LFT_NUM_FEATURES	13
/* Features
   0 : A(DC)
   1 : A(1Hz)
   2 : A(2Hz)
   3 : A(1Hz -1) / A(1Hz)
   4 : A(1Hz +1) / A(1Hz)
   5 : (w1*A(1Hz - 1) + w2* A(1Hz -2)) / A(1Hz)
   6 : (w1*A(1Hz + 1) + w2* A(1Hz +2)) / A(1Hz)
   7 : sum wi*A(1Hz -1) / A(1Hz) 
   8 : sum wi*A(1Hz +1) / A(1Hz) 
   9 : A(1Hz +1) / A(1Hz - 1) (= f[4]/f[3])
   10 : wi*A(1Hz +1) / wi*A(1Hz - 1) (= f[6]/f[5])
   11 : wi*A(1Hz +1) / wi*A(1Hz - 1) (= f[8]/f[7])
 */
int 	LFT::getFeaturesLength() {return LFT_NUM_FEATURES;}
double *LFT::getFeatures()
{
	double wgts[] = { 0.6, 0.5, 0.5, 0.5, 0.5};
	double nwgts = 0.0;
	int size = 5;
	int n = 1+size*2;
	double *features=new double[LFT_NUM_FEATURES];
	double *dv = new double[n];
	int i, j;

	for (i = 0; i < getFeaturesLength(); i++) features[i] = 0.0;
	for (i = 0, j = OneHz()-size; i < n; i++, j++) dv[i] = abs(zout[j]);
	j = 0;
	features[j++] = abs(zout[0]);
	features[j++] = abs(zout[OneHz()]);
	features[j++] = abs(zout[2*OneHz()]);
	//	features[j++] = dv[size];
	features[j++] = dv[size-1]/features[1];
	features[j++] = dv[size+1]/features[1];
	for (i = 0; i < 2; i++) nwgts += wgts[i];
	features[j++] = (wgts[0]*dv[size-1]+wgts[1]*dv[size-2])
	                 /(nwgts*features[1]);
	features[j++] = (wgts[0]*dv[size+1]+wgts[1]*dv[size+2])
	                 /(nwgts*features[1]);
	double f7 = 0.0, f8 =0.0;
	nwgts = 0.0;
	for (i = 0; i < size; i++) {
	  f7 += wgts[size - i - 1] * dv[i];
	  f8 += wgts[i] * dv[size + i + 1];
	  nwgts += wgts[i];
	}
	features[j++] = f7/(nwgts*features[1]);
	features[j++] = f8/(nwgts*features[1]);
	features[j++] = features[4]/features[3];
	features[j++] = features[6]/features[5];
	features[j++] = f8/f7;
	features[j++] = abs(zout[OneHz()]) /abs(zout[0]) *100.0;
	if (LFT_NUM_FEATURES != j) {
	  char *func = "LFT::getFeatures";
	  printf ("bug at %s\n", func);
	}
	delete dv;
	return features;
}

// FFT func.
// IN	z1, z2, n, inzee
// OUT	z1 or z2
Complex* LFT::fft(Complex *z1, Complex *z2, int n, int inzee) 
/*  Construct the discrete Fourier transform of z1 (or z2)     
	in the Cooley-Tukey way, but with a twist.   
	z1[before], z2[before].     inzee==1 means input
	in z1; inzee==2 means input in z2 */ 
{     
	int before = n;     
	int after = 1;     
	int next = 0;     
	int now;     

	do {     
		int np = prime[next];     
		if ( (before/np)*np < before ) {         
			if (++next < NEXTMX) continue;         
			now = before;         
			before = 1;
		}     
		else {         
			now = np;         
			before /= np;     
		}     
		if (inzee == 1)    
			fftstp(z1, after, now, before, z2);     
		else
			fftstp(z2, after, now, before, z1);     
		inzee = 3 - inzee;     
		after *= now;     
	} while (1 < before)    ; 
	return (inzee==1) ? z1 : z2; 
} 

// fftstp
// IN	zin, after, now, before, zout
// OUT
void LFT::fftstp(Complex* zin, int after, int now, int before, Complex* zout) 
/*  zin(after,before,now) zout(after,now,before)     
	there is ample scope for optimization */ 
{     
	double angle = PI2/(now*after);
    
	Complex omega, arg, value;

	omega = Complex(cos(angle), -sin(angle));
	arg = 1;

    for (int j=0; j<now; j++) {
    for (int ia=0; ia<after; ia++) {
        for (int ib=0; ib<before; ib++) {
             // value = zin(ia,ib,now)
			value = zin[ia + ib*after + (now-1)*before*after]; 
        for (int in=now-2; 0<=in; in--) {
         // value = value*arg + zin(ia,ib,in)
            value *= arg;
            value += zin[ia + ib*after + in*before*after];
        }
         // zout(ia,j,ib) = value
        zout[ia + j*after + ib*now*after] = value;
        }
        arg *= omega;
    } 
    }
}

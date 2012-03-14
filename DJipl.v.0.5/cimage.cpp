#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <math.h>
#include "ieeefp.h"
#include <sys/types.h>
#include <malloc.h>
#include "cimage.h"
#include "ImagePixel.h"
#include "ImgMisc.h"

#define M_PI	3.1415926535897932384626433832795
int debug;
extern void printMem();
//extern void printImageInfo(IMAGE *image, int level = 0);

#ifdef _CIMAGE_ALL_DEBUG_
int image_num = 0;

int getAllocImageNum() { return image_num;}

int printAllocedImageNum(int n)
{
	if (n == 1)
		printf ("%s: no. of alloced image = %d\n", "constructing",image_num);
	else
		printf ("%s: no. of alloced image = %d\n", "destructing", image_num);
	return image_num;
}
#endif 

/*
IMAGE *iopen(char *file, char *mode)
{
	return iopen(file, mode, 0, 0, 0, 0, 0);
}
*/
//
//	ColorImage
//

// constructor 

ColorImage::ColorImage (int w, int h, int d, unsigned char *r, 
						unsigned char *g, unsigned char *b) : width(w), height(h), depth(d) 
{
	if (w > 0 && h > 0) {
		unsigned char *bf[3];
		bf[0] = r; bf[1] = g; bf[2] = b;
		for (int k = 0; k < d; k++) {
			buf[k] = new unsigned char[w * h];
			memcpy(buf[k], bf[k], width * height);
		}
	}
	resetROI();
#ifdef _CIMAGE_ALL_DEBUG_
	image_num ++;
	printAllocedImageNum(1);
#endif 
}

ColorImage::ColorImage (int w, int h, int d, unsigned char *bf[3]) 
{
	ColorImage (w, h, d, bf[0], bf[1], bf[2]);
}

ColorImage::ColorImage () : width(0), height(0), depth(0)
{
	buf[0]=NULL;buf[1]=NULL;buf[2]=NULL; resetROI();
#ifdef _CIMAGE_ALL_DEBUG_
	image_num ++;
	printAllocedImageNum(1);
#endif 
} 

ColorImage::ColorImage (int w, int h, int d) : width(w), height(h), depth(d) 
{
	if (w > 0 && h > 0) 
		for (int k = 0; k < d; k++) buf[k] = new unsigned char[w * h];
	else
		for (int k = 0; k < d; k++) buf[k] = NULL;
	x1=0;x2=w;y1=0;y2=h;
#ifdef _CIMAGE_ALL_DEBUG_
	image_num ++;
	printAllocedImageNum(1);
#endif 
}

ColorImage::ColorImage (int w, int h, int d, unsigned char r, 
						unsigned char g, unsigned char b) : width(w), height(h), depth(d) 
{
	int v[3];
	v[0] = r; v[1] = g; v[2] = b;
	for (int k = 0; k < depth; k++) {
		buf[k] = new unsigned char[width * height];
		memset(buf[k], v[k], width * height);
	}
	x1=0;x2=w;y1=0;y2=h;
#ifdef _CIMAGE_ALL_DEBUG_
	image_num ++;
	printAllocedImageNum(1);
#endif 
}

ColorImage::ColorImage (const ColorImage & c)
{
	width = c.width; height = c.height; depth = c.depth;
	for (int k = 0; k < c.depth; k++) {
		buf[k] = new unsigned char[width * height];
		memcpy(buf[k], c.buf[k], width * height);
	}
	x1=c.x1;x2=c.x2;y1=c.y1;y2=c.y2;
//	x1=c.getROIx1();x2=c.getROIx2();y1=c.getROIy1();y2=c.getROIy2();
#ifdef _CIMAGE_ALL_DEBUG_
	image_num ++;
	printAllocedImageNum(1);
#endif 
}

ColorImage::ColorImage(char *filename)
{
	width = 0; height = 0; depth = 0;
	buf[0] = NULL; buf[1] = NULL; buf[2] = NULL;
	if(load(filename)) 
		fprintf(stderr,"cannot create ColorImage\n");
	x1=0;x2=width;y1=0;y2=height;
#ifdef _CIMAGE_ALL_DEBUG_
	image_num ++;
	printAllocedImageNum(1);
#endif 
}

ColorImage::ColorImage(GrayImage *r, GrayImage *g, GrayImage *b)
{
	if (r == NULL) width = height = 0;
	else { 
		width = r->width; height = r->height; 
		buf[0] = new unsigned char[width * height];
		memcpy(buf[0], r->buf[0], width * height);
	}
	if (g != NULL && b != NULL) {
		depth = 3;
		buf[1] = new unsigned char[width * height];
		memcpy(buf[1], g->buf[0], width * height);
		buf[2] = new unsigned char[width * height];
		memcpy(buf[2], b->buf[0], width * height);
	} else depth = 1;
	x1=r->getROIx1();x2=r->getROIx2();y1=r->getROIy1();y2=r->getROIy2();
#ifdef _CIMAGE_ALL_DEBUG_
	image_num ++;
	printAllocedImageNum(1);
#endif 
}

ColorImage::~ColorImage()
{
	for (int k = 0; k < depth; k++) delete [] buf[k];
#ifdef _CIMAGE_ALL_DEBUG_
	image_num --;
	printAllocedImageNum(0);
#endif 
}

ColorImage& ColorImage::resize(const ColorImage &c)
{ 
	width = c.width; height = c.height; depth = c.depth;
	for (int k = 0; k < depth; k++) {
		delete buf[k];
		buf[k] = new unsigned char[c.width * c.height];
	}
	return (*this);
}

void ColorImage::Realloc(int w, int h)
{
	width = w; height = h;
	for (int k = 0; k < depth; k++) 
		buf[k] = (unsigned char *)realloc(buf[k], w*h*sizeof(unsigned char));
}

void ColorImage::copyFrom(int ww, int hh, int dd,  unsigned char *r, unsigned char *g, unsigned char *b)
{
	if (ww > 0 && hh > 0 && r != NULL) {
		unsigned char *bf[3];
		bf[0] = r; bf[1] = g; bf[2] = b;
		for (int k = 0; k < dd; k++) {
			buf[k] = (unsigned char *) realloc(buf[k], sizeof(unsigned char) * ww * hh);
			if (buf[k] == NULL) {
				printf ("Error in ColorImage:\n");
			} else {
				memcpy(buf[k], bf[k], ww * hh);
			}
		}
	} 
	width = ww; height = hh; depth = dd;
}

void ColorImage::copyFrom(ColorImage *ci)
{
	if (w() == ci->w() && h() == ci->h() && d() == ci->d() && ci->buf[0] != NULL) {
		unsigned char *bf[3];
		bf[0] = ci->buf[0]; bf[1] = ci->buf[1]; bf[2] = ci->buf[2];
		for (int k = 0; k < depth; k++) {
				memcpy(buf[k], bf[k], w() * h());
		}
		width = ci->w(); height = ci->h(); depth = ci->d();
	} else 
		copyFrom(ci->width, ci->height, ci->depth, ci->buf[0], ci->buf[1], ci->buf[2]);
}

int ColorImage::copyTo(unsigned char *r, unsigned char *g, unsigned char *b)
{
	if (width > 0 && height > 0 && depth > 0) {
		unsigned char *bf[3]; 
		bf[0] = r; bf[1] = g; bf[2] = b;
		for (int k = 0; k < depth; k++) {
			bf[k] = (unsigned char *)realloc(bf[k], sizeof(unsigned char) * width * height);
			memcpy(bf[k], buf[k], width * height);
		}
		return 0;
	} else {
		return -1;
	}
}

int ColorImage::load(char *filename, int verbose)
{
	CmyImage image;
	int i,j,k,l;
	char str[100];

	switch(verbose)
	{
	case OUTPUT:
		sprintf(str,"%s%s",OUTPUT_STR,filename);
		break;
	case TEST:
		sprintf(str,"%s%s",TEST_STR,filename);
		break;
	case TRAINING:
		sprintf(str,"%s%s",TRAINING_STR,filename);
		break;
	default:
		sprintf(str,"%s", filename);
		break;
	}

	image.Load(str);
	
	width = image.GetWidth();
	height = image.GetHeight();
	depth = image.GetBitCount()/8;

	l = 0;
	// gray
	if (depth == 1)
	{
		CPixelPtr ptr(image);
		for (k = 0; k < depth; k++) {
			buf[k] = (unsigned char *)realloc(buf[k], sizeof(unsigned char)*width*height);
			for (j=0;j<height;j++)
				for (i=0;i<width;i++,l++)
					*(buf[k]+l) = ptr[j][i];
		}
	}
	// color
	else
	{
		CColorPixelPtr ptr(image);
		for (k = 0; k < depth; k++) {
			buf[k] = (unsigned char *)realloc(buf[k], sizeof(unsigned char)*width*height);
			l=0;
			for (j=0;j<height;j++)
				for (i=0;i<width;i++,l++)
				{
					switch(k)
					{
					case 0:
						*(buf[k]+l) = ptr[j][i].R;
						break;
					case 1:
						*(buf[k]+l) = ptr[j][i].G;
						break;
					case 2:
						*(buf[k]+l) = ptr[j][i].B;
						break;
					}
				}
		}
	}

	//if (verbose) printf ("image loaded: %s(%dx%d)\n", filename, width, height);
	return 0;
}

int ColorImage::save(char *filename, int verbose)
{
	char str[100];
	int i,j,k;
	CmyImage img = CImgMisc::MakeImg(width,height,depth*8);

	k = 0;
	if (depth == 1)
	{
		CPixelPtr ptr(img);
		for (j=0;j<height;j++)
			for (i=0;i<width;i++,k++)
				ptr[j][i] = *(buf[0]+k);
	}
	else
	{
		CColorPixelPtr ptr(img);
		for (j=0;j<height;j++)
			for (i=0;i<width;i++,k++)
			{
				ptr[j][i].R = *(buf[0]+k);
				ptr[j][i].G = *(buf[1]+k);
				ptr[j][i].B = *(buf[2]+k);
			}
	}

	switch(verbose)
	{
	case OUTPUT:
		sprintf(str,"%s%s",OUTPUT_STR,filename);
		break;
	case TEST:
		sprintf(str,"%s%s",TEST_STR,filename);
		break;
	case TRAINING:
		sprintf(str,"%s%s",TRAINING_STR,filename);
		break;
	default:
		sprintf(str,"%s%s",OUTPUT_STR,filename);
		break;
	}
	
	img.Save(str);

	//if (verbose) printf ("image loaded: %s(%dx%d)\n", filename, width, height);
	return 0;
}
/*
int ColorImage::load(char *filename, int verbose)
{
	CImage *image;

	if ((image->Load(filename)) == NULL) {
		fprintf (stderr,"can't open input file: %s\n", filename);
		return -1;
	}

	/* allocate buffers for image data 
	unsigned short *bufs[3], *q; 
	unsigned char *p[3];
	if (!(image->GetBitCount() == 8 || image->GetBitCount() == 24)) return -1;

	for (int k = 0; k < image->GetBitCount()/8; k++) {
		bufs[k] = new unsigned short [image->GetWidth()];
		buf[k] = (unsigned char *)realloc(buf[k], 
				sizeof(unsigned char) * image->GetWidth() * image->GetHeight());
		if (buf[k] == NULL || bufs[k] == NULL) {
			//fprintf (stderr,"error in memory alloc\n");
			//iclose(image);

			if (buf[k]) { delete buf[k]; buf[k] = NULL;}
			if (bufs[k]) delete bufs[k];
			while(--k >= 0) {
				delete buf[k]; delete bufs[k];
				buf[k] = NULL;
			}
			return -1;
		}
		p[k] = buf[k]; 
	}

	for(int y=0; y<image->GetHeight(); y++) {
		for (k = 0; k < image->GetBitCount()/8; k++) {
			getrow(image, bufs[k],y,k);
			q = bufs[k];
			for(int x=0; x<image->GetWidth(); x++)
				*p[k]++ = (unsigned char) (*q++);
		}
	}
	width = image->GetWidth(); height = image->GetHeight(); depth = image->GetBitCount()/8;

	//iclose(image);
	for (k = 0; k < image->GetBitCount()/8; k++) delete  bufs[k]; 

	if (verbose) printf ("image loaded: %s(%dx%d)\n", filename, width, height);
	return 0;
}

int ColorImage::save(char *filename, int verbose)
{
	IMAGE *image;
	int k, x, y;
	unsigned int dim;
	if (depth == 3) dim = 3;
	else dim = 2;
	if (width <=0 || height <= 0 || (depth != 1 && depth !=3)) return -1;
	if((image=iopen(filename,"w",RLE(1),dim,width,height,depth)) == NULL) {
		fprintf (stderr,"can't open input file: %s\n", filename);
		return -1;
	}

	/* allocate buffers for image data 
	unsigned short *bufs, *q; 
	unsigned char *p[3];
	bufs = new unsigned short [width];
	if (bufs == NULL) {
		fprintf (stderr,"error in memory alloc\n");
		iclose(image);
		return -1;
	}

	for (k = 0; k < depth; k++) p[k] = buf[k];

	for(y=0; y<height; y++) {
		for (k = 0; k < depth; k++) {
			for(x=0, q = bufs; x<width; x++) 
				*q++ = (unsigned short) *p[k]++; 
			putrow(image,bufs,y,k);
		}
	}

	iclose(image);
	delete bufs;

	if(verbose) printf ("image saved: %s\n", filename);
	return 0;
}
*/
ColorImage *ColorImage::getSubImage(int sx, int sy, int ex, int ey)
{
	int siWidth = ex - sx;	
	int siHeight = ey - sy;	
	int k;

	if (!(siWidth > 0 && siHeight > 0)) return  NULL;

	unsigned char *sibuf[3];
	for (k = 0; k < depth; k++) 
		sibuf[k] = new unsigned char[siWidth * siHeight];

	unsigned char *p1, *p2;
	for (k = 0; k < depth; k++) {
		p1 = buf[k] + sx + sy * width; p2 = sibuf[k];
		for (int y = 0; y < siHeight; y++, p1 += width, p2 += siWidth)
			memcpy(p2, p1, siWidth);
	}

	ColorImage *ci = new ColorImage(siWidth, siHeight, depth, sibuf[0], sibuf[1], sibuf[2]);

	for (k = 0; k < depth; k++) delete sibuf[k];
	return ci;
}

int ColorImage::setSubImage(const ColorImage &ci, int x, int y)
{
	int k;
	unsigned char *p, *q;
	if (ci.width + x > width || ci.width > width) return -1;
	if (ci.height + y > height || ci.height > height) return -1;

	for (k = 0; k < depth; k++) {
		p = ci.buf[k]; q = buf[k] + x + y * width;
		for (int y = 0; y < ci.height; y++, p += ci.width, q += width)
			memcpy(q, p, ci.width);
	}
	return 0;
}

/*
const unsigned char *ColorImage::getrow(int y, int len, unsigned long *d)
const unsigned char *ColorImage::gethline(int x, int y, int len, int rgb,
unsigned char *d)
{
	d = (unsigned char *)realloc(d, len);
	memcpy(d, getp(x,y, rgb), len);
	return d;
}

void ColorImage::sethline(int x, int y, int len, unsigned long *d)
{
	memcpy(getp(x,y), d, len * sizeof(unsigned long));
}

unsigned long *ColorImage::getvline(int x, int y, int len, unsigned long *d)
{
	if (!d) d = new unsigned long[len];
	unsigned long *dp = getp(x, y); 
	for (int i = 0; i < len; i++) {
		*dp = *d++;
		dp += width;
	}
	return d;
}

void ColorImage::setvline(int x, int y, int len, unsigned long *d)
{
	unsigned long *dp = getp(x, y); 
	for (int i = 0; i < len; i++) {
		*dp = *d++;
		dp += width;
	}
}
*/

//
//	XBGR format
//
int ColorImage::from3Gray(GrayImage *r, GrayImage *g, GrayImage *b)
{
	// size check
	if (!(r->width == g->width && r->width == b->width) ||
		!(r->height == g->height && r->height == b->height)) {
		fprintf (stderr, "error in gray image size\n");
		fprintf (stderr, "red:%dx%d\n", r->width, r->height);
		fprintf (stderr, "green:%dx%d\n", g->width, g->height);
		fprintf (stderr, "blue:%dx%d\n", b->width, b->height);
		return -1;
	}
	copyFrom(width, height, 3, r->buf[0], g->buf[0], b->buf[0]);
	return 0;
}

int ColorImage::to3Gray(GrayImage *r, GrayImage *g, GrayImage *b)
{
	if( !r || !g || !b) return -1;
	if (!(width == r->width && height == r->height)) 
		r->Realloc(width, height);
	if (!(width == g->width && height == g->height)) 
		g->Realloc(width, height);
	if (!(width == b->width && height == b->height)) 
		b->Realloc(width, height);

	return copyTo(r->buf[0], g->buf[0], b->buf[0]);
}

BinaryImage *ColorImage::threshold (int r1, int r2, Threshold rmode,
							int g1, int g2, Threshold gmode,
							int b1, int b2, Threshold bmode)
{
	unsigned char *p[3];
	int th1[3], th2[3];
	Threshold modes[3];
	for (int k = 0; k < depth; k++) p[k] = buf[k];
	th1[0] = r1; th2[0] = r2; modes[0] = rmode;
	th1[1] = g1; th2[1] = g2; modes[1] = gmode;
	th1[2] = b1; th2[2] = b2; modes[2] = bmode;
	BinaryImage *bin = new BinaryImage(width, height);
	unsigned char *q = bin->getBuffer();

	register int flg = 0;
	for (int y = y1; y < y2; y++){
		for (k = 0; k < depth; k++) p[k] = getp(x1, y, k); 
		for (int x = x1; x < x2; x++, q++){
			flg = 0;
			for (k = 0; k < depth; k++) {
				if ( modes[k] == InBound && 
					 ((*p[k]) >= th1[k]) && ((*p[k]) <= th2[k]) ) {
						flg ++;
				} else if ( modes[k] == OutOfBound && 
							(((*p[k]) <= th1[k]) || ((*p[k]) >= th2[k])) ) {
						flg ++;
				}
			}
			if (flg == 3) 
				*q = WHITE;//BinaryImage::WHITE;
			else 
				*q = BLACK;//BinaryImage::BLACK;
			for (k = 0; k < depth; k++) p[k]++;
		}
	}
	return bin;
}

GrayImage *ColorImage::getLumImg ()
{
	unsigned char *p[3];
	p[0] = buf[0]; p[1] = buf[1]; p[2] = buf[2];
	
	GrayImage *lum_img = NULL;
	lum_img = new GrayImage(width, height);
	if(depth !=3) {
		lum_img->copyFrom(this);
		return lum_img;
	}
	unsigned char *q = lum_img->getBuffer();

	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++, q++, p[0]++, p[1]++, p[2]++){
			*q = ((int)(*p[0]) + (int)(*p[1]) + (int)(*p[2])) / 3;
		}
	}
	return lum_img;
}

void ColorImage::info()
{
	printf("Image x and y size in pixels: %d %d\n",width, height);
	printf("Image zsize in channels: %d\n",depth);
}

void ColorImage::printx(char *filename, int from_y, int to_y, int a)
{
	unsigned char *p[3];
	FILE *fout;
	int k, x, y;

	char tmpstr[200];
	sprintf (tmpstr, "%s", filename);
	if (!strcmp(tmpstr,"stdout")) fout = stdout;
	else fout = fopen(tmpstr,"w");
	if (from_y < 0) from_y = 0;
	if (to_y > width) to_y = width;

	for(x=0; x < a; x++) {
		if (!x) fprintf (fout, "        ");
		fprintf (fout, "%02x ", x);
	}

	fprintf(fout,"\n");
	for(y=to_y-1; y>=from_y; y--) {
		fprintf (fout,"%3d row:", h() - 1 - y);
		for (k = 0; k < depth; k++) { 
			p[k] = getp(0, y, k);
			for(x=0; x<width; x++) {
				fprintf (fout, "%02x", *p[k]++);
				if (x%a == a-1 && a == width) fprintf (fout, "\n");
				else if (x%a == a-1) fprintf (fout, "\n      ");
				else fprintf (fout, " ");
			}
		}
		if (a != width) fprintf (fout, "\n");
	}

	if (fout != stdout) fclose(fout);
}

//
// operators
//
ColorImage&  ColorImage::operator =(const ColorImage &c)
{
	int k;
	if (height == c.height) {
		if (width == c.width) {
			if (depth == c.depth) {	
				for (k = 0; k < depth; k++)
					memcpy(buf[k], c.buf[k], width * height);
				return (*this);
			} else {
				printf ("error in operator = \n");
			}
		} else {
			printf ("error in operator = \n");
		}
	} else {
		resize(c);
		for (k = 0; k < depth; k++)
			memcpy(buf[k], c.buf[k], width * height);
		return (*this);
	}
	return (*this);
}

ColorImage&  ColorImage::operator = (const unsigned char a)
{
	for (int k = 0; k < depth; k++) 
		memset(buf[k], (int)a, width * height);
	return *this;
}

ColorImage  ColorImage::operator -() const
{
	unsigned char ucv = 255;
	unsigned char *p;
	for (int k = 0; k < depth; k++) {
		p = buf[k]; 
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++, p++) 
				*p = ucv - *p;
	}
	return *this;
}

// not logical operator
ColorImage  ColorImage::operator ~() const
{
	unsigned char *p;
	for (int k = 0; k < depth; k++) {
		p = buf[k]; 
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++, p++) 
				*p = ~(*p);
	}
	return *this;
}

ColorImage &	ColorImage::operator +=(const ColorImage &c)
{return (*this = *this + c);}

ColorImage &	ColorImage::operator -=(const ColorImage &c)
{return (*this = *this - c);}

ColorImage operator +(const ColorImage &c1, const ColorImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = c1.depth < c2.depth ? c1.depth:c2.depth;

	ColorImage d1(w, h, d), d2(w, h, d), d3(w, h, d);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	int v;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) {
				v = (int)(*p1++) + (int)(*p2++);		
				*p3++ = (unsigned char)((v+1)/2);	
			}
	}
	return ColorImage(d3);
}


ColorImage operator +(const ColorImage &c, const unsigned char a)
{
	unsigned char *b[3];
	int v[3], tmp;
	for (int k = 0; k < c.depth; k++) {
		b[k] = new unsigned char[c.width * c.height];
		v[k] = a;
	}

	unsigned char *p, *q;
	for (k = 0; k < c.depth; k++) {
		p = c.buf[k]; 
		q = b[k]; 
		for (int y = 0; y < c.height; y++)
			for (int x = 0; x < c.width; x++) {
				tmp = (int)(*p++) + v[k];
				tmp = clip255(tmp);
				*q++ = (unsigned char)tmp;	
			}
	}
	return ColorImage(c.width, c.height, c.depth, b);
}

ColorImage operator +(const unsigned char a, const ColorImage &c) 
{
	return operator +(c, a);
}

//
// 128+(inimage1-inimage2)
//
ColorImage operator -(const ColorImage &c1, const ColorImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = c1.depth < c2.depth ? c1.depth:c2.depth;

	ColorImage d1(w, h, d), d2(w, h, d), d3(w, h, d);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	int v;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) {
				v = (int)128 + (int)(*p1++) - (int)(*p2++);		
				*p3++ = (unsigned char)(v > 255 ? 255: (v < 0 ? 0 : v));
			}
	}
	return ColorImage(d3);
}

// inimage - a;
ColorImage operator -(const ColorImage &c, const unsigned char a)
{
	unsigned char *b[3];
	int v[3], tmp;
	for (int k = 0; k < c.depth; k++) {
		b[k] = new unsigned char[c.width * c.height];
		v[k] = a;
	}

	unsigned char *p, *q;
	for (k = 0; k < c.depth; k++) {
		p = c.buf[k]; q = b[k]; 
		for (int y = 0; y < c.height; y++)
			for (int x = 0; x < c.width; x++) {
				tmp = (int)(*p++) - v[k];
				tmp = clip0(tmp);
				*q++ = (unsigned char)tmp;	
			}
	}
	return ColorImage(c.width, c.height, c.depth, b);
}

ColorImage operator -(const unsigned char a, const ColorImage &c) 
{
	unsigned char *b[3];
	int v[3], tmp;
	for (int k = 0; k < c.depth; k++) {
		b[k] = new unsigned char[c.width * c.height];
		v[k] = a;
	}

	unsigned char *p, *q;
	for (k = 0; k < c.depth; k++) {
		p = c.buf[k]; q = b[k]; 
		for (int y = 0; y < c.height; y++)
			for (int x = 0; x < c.width; x++) {
				tmp = v[k] - (int)(*p++);
				tmp = clip0(tmp);
				*q++ = (unsigned char)tmp;	
			}
	}
	return ColorImage(c.width, c.height, c.depth, b);
}

int operator == (const ColorImage &c1, const ColorImage &c2)
{
	if ((c1.width != c2.width) || (c1.height != c2.height) || 
		(c1.depth != c2.depth)) return 0;

	for (int k = 0; k < c1.depth; k++) 
		if(memcmp(c1.buf[k], c2.buf[k], c1.width * c1.height)) return 0;
	return 1;
}

// logical operator
ColorImage operator & (const ColorImage &c1, const ColorImage &c2)
{
	if(debug) printf ("logical and operator\n");
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = c1.depth < c2.depth ? c1.depth:c2.depth;

	ColorImage d1(w, h, d), d2(w, h, d), d3(w, h, d);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) 
				*p3++ = (*p1++) & (*p2++);		
	}
	return ColorImage(d3);
}

ColorImage operator &( const ColorImage &c, unsigned long a)
{
	if(debug) printf ("logical and operator\n");
	unsigned char *b[3];
	unsigned char v[3];
	for (int k = 0; k < c.depth; k++) {
		b[k] = new unsigned char[c.width * c.height];
		v[k] = (unsigned char)(a & 0xff);
		a >>= 8;
	}

	unsigned char *p, *q;
	for (k = 0; k < c.depth; k++) {
		p = c.buf[k]; q = b[k]; 
		for (int y = 0; y < c.height; y++)
			for (int x = 0; x < c.width; x++) 
				*q++ = v[k] & (*p++); 
	}
	return ColorImage(c.width, c.height, c.depth, b);
}

ColorImage operator &(unsigned long a, const ColorImage &c) 
{
	return operator &(c, a);
}


ColorImage operator | (const ColorImage &c1, const ColorImage &c2)
{
	if(debug) printf ("logical or operator\n");
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = c1.depth < c2.depth ? c1.depth:c2.depth;

	ColorImage d1(w, h, d), d2(w, h, d), d3(w, h, d);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) 
				*p3++ = (*p1++) | (*p2++);		
	}
	return ColorImage(d3);
}

ColorImage operator |( const ColorImage &c, unsigned long a)
{
	if(debug) printf ("logical or operator\n");
	unsigned char *b[3];
	unsigned char v[3];
	for (int k = 0; k < c.depth; k++) {
		b[k] = new unsigned char[c.width * c.height];
		v[k] = (unsigned char)(a & 0xff);
		a >>= 8;
		printf ("0x%x ", v[k]);
	}

	unsigned char *p, *q;
	for (k = 0; k < c.depth; k++) {
		p = c.buf[k]; q = b[k]; 
		for (int y = 0; y < c.height; y++)
			for (int x = 0; x < c.width; x++) 
				*q++ = v[k] | (*p++); 
	}
	return ColorImage(c.width, c.height, c.depth, b);
}

ColorImage operator |(unsigned long a, const ColorImage &c) 
{
	return operator |(c, a);
}


/////////////////////////////////////////////////////////////////////////
// GrayImage
/////////////////////////////////////////////////////////////////////////

void GrayImage::copyFrom(ColorImage *ci, int num)
{
	if (w() == ci->w() && h() == ci->h() && ci->buf[0] != NULL) {
		memcpy(buf[0], ci->getBuffer(num), ci->w() * ci->h());
		width = ci->w(); height = ci->h();
	} else 
		copyFrom(ci->w(), ci->h(), ci->getBuffer(num));
}

void GrayImage::copyFrom(GrayImage *gi)
{
	if (w() == gi->w() && h() == gi->h() && gi->d()== 1 && gi->buf[0] != NULL) {
		memcpy(buf[0], gi->getBuffer(), gi->w() * gi->h());
		width = gi->w(); height = gi->h();
	} else 
		copyFrom(gi->w(), gi->h(), gi->getBuffer());
}

void GrayImage::copyFrom(int ww, int hh, unsigned char *r)
{
	if (ww > 0 && hh > 0 && r != NULL) {
		unsigned char *ucbuf = (unsigned char *) realloc(buf[0], sizeof(unsigned char) * ww * hh);
		if (ucbuf == NULL) {
			printf ("Error in ColorImage:\n");
		} else {
			buf[0] = ucbuf;	
			memcpy(buf[0], r, ww * hh);
		}
	} 
	width = ww; height = hh; 
}

// offset
// by DJ
// peripheral region counts.
GrayImage *GrayImage::convolution(int kernel_width, int kernel_height, int *kernel, int div, int offset)
{
	GrayImage *rst = new GrayImage(this);
	unsigned char *p = buf[0], *q = rst->buf[0];
	int v;
	int xx, yy;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++, q++) {
			// modified version by DJ.
				v = 0;
				int *kp = kernel;
				yy = y - kernel_height/2;
				for (int ky = 0; ky < kernel_height; ky++, yy++)
				{
					xx = x - kernel_width/2;
					for (int kx = 0; kx < kernel_width; kx++, kp++, xx++)
					{
						// exception for out-of-bound pixel.
						if (xx < 0)
							xx += width;
						else if (xx >= width)
							xx -= width;
						if (yy < 0)
							yy += height;
						else if (yy >= height)
							yy -= height;

						v +=(int) getPixel(xx, yy) * (*kp);
					}
				}
				*q = clips(v / div + offset);
		}

	return rst;
}

GrayImage *GrayImage::convolutionf(int kernel_width, int kernel_height, float *kernel, float scale, float offset)
{
	GrayImage *rst = new GrayImage(this);
	unsigned char *p = buf[0], *q = rst->buf[0];
	float vf;
	int xx, yy;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++, q++) {
			vf = 0.0;
			float *kp = kernel;
			yy = y - kernel_height/2;
			for (int ky = 0; ky < kernel_height; ky++, yy++)
			{
				xx = x - kernel_width/2;
				for (int kx = 0; kx < kernel_width; kx++, kp++, xx++)
				{
					// exception for out-of-bound pixel.
					if (xx < 0)
						xx += width;
					else if (xx >= width)
						xx -= width;
					if (yy < 0)
						yy += height;
					else if (yy >= height)
						yy -= height;

					vf +=(float) getPixel(xx, yy) * (*kp);
				}
			}
			*q = clips((int)(vf *scale + offset));
		}
	return rst;
}

// 2000.10.11.
// convolution with double type coeff.
// return double type coeff.
double *GrayImage::convolutionf0(int kernel_width, int kernel_height, double *kernel, double scale, double offset)
{
	unsigned char *p = buf[0];
	double vf;
	int xx, yy;

	double *rst_img = new double[width*height];
	int z;

	int x,y;

	z=0;
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++, p++, z++) {
			vf = 0.0;
			double *kp = kernel;
			yy = y - kernel_height/2;
			for (int ky = 0; ky < kernel_height; ky++, yy++)
			{
				xx = x - kernel_width/2;
				for (int kx = 0; kx < kernel_width; kx++, kp++, xx++)
				{
					// exception for out-of-bound pixel.
					if (xx < 0)
						xx = -xx;
					else if (xx > width)
						xx = 2*width - 1 - xx;
					if (yy < 0)
						yy = -yy;
					else if (yy > height)
						yy = 2*height - 1 - yy;
					//if ((xx > kernel_width && xx < width - kernel_width - 1) && (yy > kernel_height && yy < height - kernel_height - 1))
					//{
						vf += (double)getPixel(xx, yy) * (*kp);
					//}
				}
			}
			rst_img[z] = vf*scale + offset;
		}

	return rst_img;
}

// 20010710@13:44@DJ
// point convolution
double *GrayImage::point_convolution(int kernel_width, int kernel_height, double *kernel, CPoint *point, int point_num, double scale, double offset)
{
	unsigned char *p = buf[0];
	double vf;
	int xx, yy;

	double *rst_img = new double[width*height];
	int z;
	int x, y;
	int k = 0;

	for (k=0; k<point_num; k++)
	{
		z=0;
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++, p++, z++) {
				if (point[k].x == x && point[k].y == y)
				{
					vf = 0.0;
					double *kp = kernel;
					yy = y - kernel_height/2;
					for (int ky = 0; ky < kernel_height; ky++, yy++)
					{
						xx = x - kernel_width/2;
						for (int kx = 0; kx < kernel_width; kx++, kp++, xx++)
						{
							// exception for out-of-bound pixel.
							if (xx < 0)
								xx = -xx;
							else if (xx > width)
								xx = 2*width - 1 - xx;
							if (yy < 0)
								yy = -yy;
							else if (yy > height)
								yy = 2*height - 1 - yy;
							//if ((xx > kernel_width && xx < width - kernel_width - 1) && (yy > kernel_height && yy < height - kernel_height - 1))
							//{
								vf += (double)getPixel(xx, yy) * (*kp);
							//}
						}
					}
					rst_img[z] = vf*scale + offset;

					break;
				}
			}
	}

	return rst_img;
}

// under programming
GrayImage *GrayImage::edgeDetect(int kernel_width, int kernel_height, int *kernel_h, int *kernel_v, int div, int offset)
{
	GrayImage *rst = new GrayImage(this);
	unsigned char *p = buf[0], *q = rst->buf[0], qtheta;
	int xx, yy;
	int hv, vv;

	if (kernel_width%2 == 0) {
		printf ("kernel width(%d): not odd\n", kernel_width);
	}
	if (kernel_height%2 == 0) {
		printf ("kernel height(%d): not odd\n", kernel_height);
	}
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++, q++) {
			if ((y < kernel_height/2) || (y >= (height - kernel_height/2))
			   || (x < kernel_width/2) || (x >= (width - kernel_width/2))){
				*q = clips(*p + offset);
			} else {
				hv = 0;
				vv = 0;
				int *kh = kernel_h;
				int *kv = kernel_v;
				yy = y - kernel_height/2;
				for (int ky = 0; ky < kernel_height; ky++, yy++) {
					xx = x - kernel_width/2;
					for (int kx = 0; kx < kernel_width; kx++, kh++, kv++, xx++) {
						hv +=(int) getPixel(xx, yy) * (*kh);
						vv +=(int) getPixel(xx, yy) * (*kv);
					}
				}
				hv /= div;
				vv /= div;
				*q = clips((int) sqrt((hv * hv + vv * vv)/2.));
				qtheta = clips ((int)(atan2(vv, hv) * 127 / M_PI  + 128)) ;
			}
		}
	return rst;
}

//
// Input: 2 kernels : horizontal & vertical
// Output: Magnitude & Angle Image
//  	  (or Vertical & Horizontal Image)
//
GrayImage *GrayImage::edgeDetect(int kernel_width, int kernel_height, float *kernel_h, float *kernel_v, int offset, int mode)
{
//	GrayImage *rst = new GrayImage[2](this);
//	GrayImage *rstAngle = new GrayImage(this);
	GrayImage *rstArray = new GrayImage[2];
	GrayImage *rst = rstArray;
	GrayImage *rstAngle = rstArray+1;
	*rst = (GrayImage)this;
	*rstAngle = (GrayImage)this;
	unsigned char *p = buf[0], *q = rst->buf[0], *qtheta=rstAngle->buf[0];
	int xx, yy;
	double hv, vv;
	float *kh , *kv ;

	if (kernel_width%2 == 0) {
		printf ("kernel width(%d): not odd\n", kernel_width);
	}
	if (kernel_height%2 == 0) {
		printf ("kernel height(%d): not odd\n", kernel_height);
	}
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++, q++, qtheta++) {
			if ((y < kernel_height/2) || (y >= (height - kernel_height/2))
			   || (x < kernel_width/2) || (x >= (width - kernel_width/2))){
				*q = clips(*p + offset);
			} else {
				hv = 0; vv = 0;
				kh = kernel_h; kv = kernel_v;
				yy = y - kernel_height/2;
				for (int ky = 0; ky < kernel_height; ky++, yy++) {
					xx = x - kernel_width/2;
					for (int kx = 0; kx < kernel_width; kx++, kh++, kv++, xx++) {
						hv +=(float) getPixel(xx, yy) * (*kh);
						vv +=(float) getPixel(xx, yy) * (*kv);
					}
				}
				if (mode) {
					*q = clips((unsigned char) hv + offset);
					*qtheta = clips((unsigned char) vv + offset);
				} else {
					*q = clips((int) hypot(hv, vv) + offset);
					*qtheta = clips ((int)((atan2(vv, hv) +(double)M_PI)* (double)128.0/(double)M_PI)) ;
				}
			}
		}
	return rstArray;
}

BinaryImage *GrayImage::edgeMap(int kernel_width, int kernel_height, float *kernel_h, float *kernel_v, int offset, int mode)
{
	BinaryImage *rstArray = new BinaryImage[2];
	BinaryImage *rst = rstArray;
	BinaryImage *rstAngle = rstArray+1;
	*rst = this;
	*rstAngle = this;
	unsigned char *p = buf[0], *q = rst->buf[0], *qtheta=rstAngle->buf[0];
	int xx, yy;
	double hv, vv;
	float *kh , *kv ;

	if (kernel_width%2 == 0) {
		printf ("kernel width(%d): not odd\n", kernel_width);
	}
	if (kernel_height%2 == 0) {
		printf ("kernel height(%d): not odd\n", kernel_height);
	}
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++, q++, qtheta++) {
			if ((y < kernel_height/2) || (y >= (height - kernel_height/2))
			   || (x < kernel_width/2) || (x >= (width - kernel_width/2))){
				*q = clips(*p + offset);
			} else {
				hv = 0; vv = 0;
				kh = kernel_h; kv = kernel_v;
				yy = y - kernel_height/2;
				for (int ky = 0; ky < kernel_height; ky++, yy++) {
					xx = x - kernel_width/2;
					for (int kx = 0; kx < kernel_width; kx++, kh++, kv++, xx++) {
						hv +=(float) getPixel(xx, yy) * (*kh);
						vv +=(float) getPixel(xx, yy) * (*kv);
					}
				}
				if (mode) {
					*q = (abs((int)hv) > abs((int)vv)) ? ((abs((int)hv) < offset) ? 0 : 255) : 0;
					*qtheta = (abs((int)hv) < abs((int)vv)) ?((abs((int)vv) < offset) ? 0:255) : 0;
				} else {
					if (abs((int)hv) > offset) 
					*q = (abs((int)hv) > offset) ? 255 : 0;
					*qtheta = (vv > offset) ? 255 : 0;
				}
			}
		}
	return rstArray;
}

// operator
GrayImage&  GrayImage::operator =(const GrayImage &c)
{
	if (width != c.width || height != c.height)
		resize(c);

	memcpy(buf[0], c.buf[0], width * height);
	return (*this);
}

GrayImage&  GrayImage::operator =(const BinaryImage &c)
{
	if (width != c.width || height != c.height)
		resize(c);

	memcpy(buf[0], c.buf[0], width * height);
	return (*this);
}

GrayImage&  GrayImage::operator =(const ColorImage &c)
{
	if (height == c.height) {
		if (width == c.width) {
			memcpy(buf[0], c.buf[0], width * height);
			return (*this);
		} else {
			printf ("error in operator = \n");
		}
	} else {
		resize(c);
		memcpy(buf[0], c.buf[0], width * height);
		return (*this);
	}
	return (*this);
}

GrayImage&  GrayImage::operator = (const unsigned char a)
{
	memset(buf[0], (int)a, width * height);
	return *this;
}

GrayImage  GrayImage::operator -() const
{
	unsigned char ucv = 255;
	unsigned char *p = buf[0]; 
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++) 
			*p = ucv - *p;
	return *this;
}

// not logical operator
GrayImage  GrayImage::operator ~() const
{
	unsigned char *p = buf[0]; 
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++) 
			*p = ~(*p);
	return *this;
}

/////////////////////////////////////////////////////////////////

GrayImage &	GrayImage::operator +=(const GrayImage &c)
{return (*this = *this + c);}

GrayImage &	GrayImage::operator -=(const GrayImage &c)
{return (*this = *this - c);}

GrayImage operator +(const GrayImage &c1, const GrayImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;

	GrayImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	int v;
	p1 = d1.buf[0]; p2 = d2.buf[0]; p3 = d3.buf[0];
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++) {
			v = (int)(*p1++) + (int)(*p2++);		
			*p3++ = (unsigned char)((v+1)/2);	
		}
	return GrayImage(d3);
}


GrayImage operator +(const GrayImage &c, const unsigned char a)
{
	unsigned char *b;
	int v, tmp;
	b = new unsigned char[c.width * c.height];
	v = a;

	unsigned char *p, *q;
	p = c.buf[0]; 
	q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) {
			tmp = (int)(*p++) + v;
			tmp = clip255(tmp);
			*q++ = (unsigned char)tmp;	
		}
	return GrayImage(c.width, c.height, b);
}

GrayImage operator +(const unsigned char a, const GrayImage &c) 
{
	return operator +(c, a);
}

//
// 128+(inimage1-inimage2)
//
GrayImage operator -(const GrayImage &c1, const GrayImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = 1;

	GrayImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	int v;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) {
				v = (int)128 + (int)(*p1++) - (int)(*p2++);		
				*p3++ = (unsigned char)(v > 255 ? 255: (v < 0 ? 0 : v));
			}
	}
	return GrayImage(d3);
}

// inimage - a;
GrayImage operator -(const GrayImage &c, const unsigned char a)
{
	unsigned char *b;
	int v, tmp;
	b = new unsigned char[c.width * c.height];
	v = a;

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) {
			tmp = (int)(*p++) - v;
			tmp = clip0(tmp);
			*q++ = (unsigned char)tmp;	
		}
	return GrayImage(c.width, c.height, b);
}

GrayImage operator -(const unsigned char a, const GrayImage &c) 
{
	unsigned char *b;
	int v, tmp;
	b = new unsigned char[c.width * c.height];
	v = a;

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) {
			tmp = v - (int)(*p++);
			tmp = clip0(tmp);
			*q++ = (unsigned char)tmp;	
		}
	return GrayImage(c.width, c.height, b);
}

int operator == (const GrayImage &c1, const GrayImage &c2)
{
	if ((c1.width != c2.width) || (c1.height != c2.height) || 
		(c1.depth != c2.depth)) return 0;

	for (int k = 0; k < c1.depth; k++) 
		if(memcmp(c1.buf[k], c2.buf[k], c1.width * c1.height)) return 0;
	return 1;
}

// logical operator
GrayImage operator & (const GrayImage &c1, const GrayImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = 1;

	GrayImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) 
				*p3++ = (*p1++) & (*p2++);		
	}
	return GrayImage(d3);
}

GrayImage operator &( const GrayImage &c, unsigned long a)
{
	unsigned char *b, v;
	b = new unsigned char[c.width * c.height];
	v = (unsigned char)(a & 0xff);
	a >>= 8;

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) 
			*q++ = v & (*p++); 
	return GrayImage(c.width, c.height, b);
}

GrayImage operator &(unsigned long a, const GrayImage &c) 
{
	return operator &(c, a);
}


GrayImage operator | (const GrayImage &c1, const GrayImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;

	GrayImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	p1 = d1.buf[0]; p2 = d2.buf[0]; p3 = d3.buf[0];
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++) 
			*p3++ = (*p1++) | (*p2++);		
	return GrayImage(d3);
}

GrayImage operator |( const GrayImage &c, unsigned long a)
{
	unsigned char *b, v;
	b = new unsigned char[c.width * c.height];
	v = (unsigned char)(a & 0xff);
	a >>= 8;
	if (debug) printf ("0x%x ", v);

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) 
			*q++ = v | (*p++); 
	return GrayImage(c.width, c.height, b);
}

GrayImage operator |(unsigned long a, const GrayImage &c) 
{
	return operator |(c, a);
}

// 20010516@0529a
// xor logical operator
GrayImage operator ^ (const GrayImage &c1, const GrayImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = 1;

	GrayImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) 
				*p3++ = (*p1++) ^ (*p2++);		
	}
	return GrayImage(d3);
}

GrayImage operator ^( const GrayImage &c, unsigned long a)
{
	unsigned char *b, v;
	b = new unsigned char[c.width * c.height];
	v = (unsigned char)(a & 0xff);
	a >>= 8;

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) 
			*q++ = v ^ (*p++); 
	return GrayImage(c.width, c.height, b);
}

GrayImage operator ^(unsigned long a, const GrayImage &c) 
{
	return operator ^(c, a);
}
// DJ

int GrayImage::saveraw(char *filename, int flag, int verbose)
{
	FILE *fout;
	int k, x, y;
	unsigned int dim;
	if (depth == 3) dim = 3;
	else dim = 2;
	if (width <=0 || height <= 0 || (depth != 1 && depth !=3)) return -1;


	if ((fout = fopen(filename,"w")) == NULL) {
		fprintf (stderr,"can't open input file: %s\n", filename);
		return -1;
	}

	/* allocate buffers for image data */
	unsigned char *q;

	for (k = 0; k < depth; k++) {
		q = buf[k]; 
		if ((flag & FORMATDirectionMask) == FORMATDirectionX) {
			for(y=0; y<height; y++) {
				for(x=0; x<width; x++) {
					if ((flag & FORMATDecMask) == FORMATDec){
						fprintf (fout, "%3d ", *q++); 
					}else {	//default
						fprintf (fout, "%02x ", *q++);
					}
					if (((flag & FORMAT2DMask) != FORMAT2D) && ((flag & FORMAT1DColMask) == FORMAT1DCol)){
						fprintf (fout, "\n");
					}
				}
				if ((flag & FORMAT2DMask) == FORMAT2D)
					fprintf (fout, "\n");
			}
		} else { // y-direction
			for(x=0; x<width; x++) {
				q = buf[k] + x;
				for(y=0; y<height; y++) {
					if ((flag & FORMATDecMask) == FORMATDec)
						fprintf (fout, "%3d ", *q); 
					else 	//default
						fprintf (fout, "%02x ", *q);
					if (((flag & FORMAT2DMask) != FORMAT2D) 
						&& ((flag & FORMAT1DColMask) == FORMAT1DCol))
						fprintf (fout, "\n");
					q += width;
				}
				if ((flag & FORMAT2DMask) == FORMAT2D)
					fprintf (fout, "\n");
			}
		}
	}

	fclose(fout);

	if(verbose) printf ("image saved: %s\n", filename);
	return 0;
}

BinaryImage *GrayImage::threshold(int min_value, int max_value, Threshold mode)
{
	BinaryImage *bin = new BinaryImage(width, height);
	unsigned char *q = bin->getBuffer();
	unsigned char *p = buf[0];
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++, p++, q++){
			if (mode == InBound) {
				if (((*p) >= min_value) && ((*p) <= max_value))
					*q = 255;
				else *q = 0;
			} else {
				if (((*p) <= min_value) || ((*p) >= max_value))
					*q = 255;
				else *q = 0;
			}
		}
	}
	return bin;
}

void GrayImage::thresholdSelf(int min_value, int max_value, Threshold mode)
{
	unsigned char *p = buf[0];
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++, p++){
			if (mode == InBound) {
				if (((*p) >= min_value) && ((*p) <= max_value))
					*p = 255;
				else *p = 0;
			} else {
				if (((*p) <= min_value) || ((*p) >= max_value))
					*p = 255;
				else *p = 0;
			}
		}
	}
}
int * GrayImage::proj(ProjMode mode, int x_1, int y_1, int x_2, int y_2)
{
	if (x_2 < 0 || y_2 < 0) { x_1 = x1; x_2 = x2; y_1 = y1; y_2 = y2;}
	else { x1 = x_1; x2 = x_2; y1 = y_1; y2 = y_2;}
	int x, y, n, num;
	int *bins;
#ifdef _DEBUG_
	if (x_2 < x_1) { int tmp = x_1; x_1 = x_2; x_2 = tmp;}
	if (y_2 < y_1) { int tmp = y_1; y_1 = y_2; y_2 = tmp;}
#endif

	switch(mode){
	case X:			// proj. into x-axis
		num = x2 - x1;
		bins = new int[num];
		for (n = 0 ; n < num; n++) bins[n] = 0;
		for (x = x1, n = 0; x < x2; x++, n++) 
			for (y = y1; y < y2; y++) 
				bins[n] += getPixel(x, y);
		break;
	case Y:			// proj. into y-axis
		num = y2 - y1;
		bins = new int[num];
		for (n = 0 ; n < num; n++) bins[n] = 0;
		for (y = y1, n = 0; y < y2; y++, n++) 
			for (x = x1; x < x2; x++) 
				bins[n] += getPixel(x, y);
		break;
	case XY:			// proj. into y-axis
		num = y2 - y1 + x2 - x1;
		bins = new int[num];
		for (n = 0 ; n < num; n++) bins[n] = 0;
		for (x = x1, n = 0; x < x2; x++, n++) 
			for (y = y1; y < y2; y++) 
				bins[n] += getPixel(x, y);
		for (y = y1; y < y2; y++, n++) 
			for (x = x1; x < x2; x++) 
				bins[n] += getPixel(x, y);
		break;
	default:
		printf ("not defined projection mode in GrayImage::proj\n");
		return NULL;
	}

	return bins;
}

//////////////////////////////////////////////////////////////////////////// 
//
//	Binary Image Class
int * BinaryImage::proj(ProjMode mode, int x_1, int y_1, int x_2, int y_2)
{
	if (x_2 < 0 || y_2 < 0) { x_1 = x1; x_2 = x2; y_1 = y1; y_2 = y2;}
	else { x1 = x_1; x2 = x_2; y1 = y_1; y2 = y_2;}
	int x, y, n, num;
	int *bins;
#ifdef _DEBUG_
	if (x_2 < x_1) { int tmp = x_1; x_1 = x_2; x_2 = tmp;}
	if (y_2 < y_1) { int tmp = y_1; y_1 = y_2; y_2 = tmp;}
#endif

	switch(mode){
	case X:			// proj. into x-axis
		num = x2 - x1;
		bins = new int[num];
		for (n = 0 ; n < num; n++) bins[n] = 0;
		for (x = x1, n = 0; x < x2; x++, n++) {
			for (y = y1; y < y2; y++) {
				if (IsWhite(x, y)) bins[n] ++;
			}
		}
		break;
	case Y:			// proj. into y-axis
		num = y2 - y1;
		bins = new int[num];
		for (n = 0 ; n < num; n++) bins[n] = 0;
		for (y = y1, n = 0; y < y2; y++, n++) {
			for (x = x1; x < x2; x++) {
				if (IsWhite(x, y)) bins[n] ++;
			}
		}
		break;
	case XY:			// proj. into y-axis
		num = y2 - y1 + x2 - x1;
		bins = new int[num];
		for (n = 0 ; n < num; n++) bins[n] = 0;
		for (x = x1, n = 0; x < x2; x++, n++) {
			for (y = y1; y < y2; y++) {
				if (IsWhite(x, y)) bins[n] ++;
			}
		}
		for (y = y1; y < y2; y++, n++) {
			for (x = x1; x < x2; x++) {
				if (IsWhite(x, y)) bins[n] ++;
			}
		}
		break;
	default:
		printf ("error in BinaryImage::proj\n");
		return NULL;
	}

	return bins;
}

BinaryImage *BinaryImage::extractConnectedRegion(int x0, int y0, int *area, int verbose)
{
	BinaryImage *outImg = NULL;
    int i,j,k;
    int ch=0;
	unsigned short *sbuf = new unsigned short[width * height];
	const	int Num = 256 * 256;
    int alpha[Num], A[Num]; // for area calculation
	unsigned char *p, *p1, *p2;
	unsigned char q1, q2;
	unsigned short *s, *s1, *s2;
	unsigned char zz = 0;
	int u1, u2, uM, um;

    for(i=0;i<Num;i++) { alpha[i]=i; A[i] = 0; }

	// result table[alpha] is sorted incremetally
	// p1  p
	// 	   p2
	p = getBuffer(0); p1 = getBuffer(0); p2 = getBuffer(0); 
	s= sbuf; s1= sbuf; s2= sbuf;
    for(j=0;j<height;j++){
		if (j == 1) {s2 = sbuf; p2 = getBuffer(0);}
		p1 = &zz;
		for(i=0;i<width;i++, p++, s++){
			// p1, s1 : i == 0
			if (i != 1) {s1++; p1++;}
			else {s1 = s-1; p1 = p-1;} 

			// p2, s2 : j == 0
			if (j != 0) {p2++; s2++;}
			else p2 = &zz; 

            if(*p==255) {
				q1 = *p1; q2 = *p2;
				if ((q1==0) &&(q2==0)) 			*s = ch++;
				else if((q2==255) && (q1==0)) 	*s = *s2;
				else if((q1==255) && (q2==0)) 	*s = *s1;
				else if((q2==255) && (q1==255)){
					*s = *s1;
					if (*s2 != *s1) {
						// noting equivalene to table
						u1 = alpha[*s1]; 	u2 = alpha[*s2];
						uM = MAX(u1, u2); 	um = MIN(u1, u2);
						for(k=0;k<ch;k++){ if(alpha[k] == uM) alpha[k] = um; }
					}
				}
				A[*s] ++;
			} else *s = 0;
        }
    }

	if (ch > Num) {
		printf ("The number of first labeled objects exceeds buffer size(%d):(%d)\n", Num, ch);
	}
	if (verbose) 
		printf ("The number of first labeled objects : (%d)\n", ch);
	if (verbose>1)
		for (i = 0; i < ch; i++) 
			printf ("%3d %3d %3d\n", i, alpha[i], A[i]);

	// table sorting
	int blobnum = 0;
	for (i = 0; i < ch; i++) {
		if (alpha[i] >= i) {
			for (j = i+1; j < ch; j++) 
				if (alpha[i] == alpha[j]) alpha[j] = blobnum;
			alpha[i] = blobnum++; 
		} 
	}

	int theBlob = alpha[sbuf[x0+y0*width]]; 
	if (verbose>1) 
		printf ("%d labels(ch = %d) generated, %dth blob selected\n", 
				blobnum, ch, theBlob);

	outImg = new BinaryImage(width, height);

	// second pass
	s= sbuf; 
	for(j=0;j<height;j++){
		for(i=0;i<width;i++, s++){
			if((getPixel(i,j)==255) && (theBlob == alpha[*s])) 
				outImg->set(i,j); 
			else outImg->unset(i,j);
		}
   	}

	// calulate areas simply
	if (area) {
		area = 0;
		for (i = 0; i < ch; i++) {
			if (alpha[i] == theBlob) area += A[i];
		}
	}

	delete sbuf;
	return outImg;
}

void  BinaryImage::print(FILE *fout, char *name)
{
	if (name != NULL) {
		fprintf(fout,"%s:\n",name);
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
			fprintf (fout, "%d", ((pel(x,y) == 255) ? 1 : (pel(x,y) == 0 ? 0 : 2)));
		fprintf(fout, "\n");
	}
}

// operator
BinaryImage&  BinaryImage::operator =(const BinaryImage &c)
{
	if (width != c.width || height != c.height)
		resize(c);

	memcpy(buf[0], c.buf[0], width * height);
	return (*this);
}

BinaryImage&  BinaryImage::operator = (const unsigned char a)
{
	memset(buf[0], (int)(a == BLACK ? BLACK: WHITE), width * height);
	return *this;
}

BinaryImage  BinaryImage::operator -() const
{
	unsigned char ucv = 255;
	unsigned char *p = buf[0]; 
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++) 
			*p = ucv - *p;
	return *this;
}

// logical operator : NOT
BinaryImage  BinaryImage::operator ~() const
{
	unsigned char *p = buf[0]; 
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++, p++) 
			*p = ~(*p);
	return *this;
}

/////////////////////////////////////////////////////////////////

BinaryImage &	BinaryImage::operator +=(const BinaryImage &c)
{return (*this = *this + c);}

BinaryImage &	BinaryImage::operator -=(const BinaryImage &c)
{return (*this = *this - c);}

BinaryImage operator +(const BinaryImage &c1, const BinaryImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;

	BinaryImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	int v;
	p1 = d1.buf[0]; p2 = d2.buf[0]; p3 = d3.buf[0];
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++) {
			v = (int)(*p1++) + (int)(*p2++);		
			*p3++ = (unsigned char)clips(v);
		}
	return BinaryImage(d3);
}


BinaryImage operator +(const BinaryImage &c, const unsigned char a)
{
	unsigned char *b;
	int v, tmp;
	b = new unsigned char[c.width * c.height];
	v = a;

	unsigned char *p, *q;
	p = c.buf[0]; 
	q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) {
			tmp = (int)(*p++) + v;
			tmp = clip255(tmp);
			*q++ = (unsigned char)tmp;	
		}
	return BinaryImage(c.width, c.height, b);
}

BinaryImage operator +(const unsigned char a, const BinaryImage &c) 
{
	return operator +(c, a);
}

//
// (inimage1-inimage2)
// Note: different from Gray/Color operator
//
BinaryImage operator -(const BinaryImage &c1, const BinaryImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = 1;

	BinaryImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	int v;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) {
				v = (int)(*p1++) - (int)(*p2++);		
				*p3++ = (unsigned char)clips(v);
			}
	}
	return BinaryImage(d3);
}

// inimage - a;
BinaryImage operator -(const BinaryImage &c, const unsigned char a)
{
	unsigned char *b;
	int v, tmp;
	b = new unsigned char[c.width * c.height];
	v = a;

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) {
			tmp = (int)(*p++) - v;
			tmp = clip0(tmp);
			*q++ = (unsigned char)tmp;	
		}
	return BinaryImage(c.width, c.height, b);
}

BinaryImage operator -(const unsigned char a, const BinaryImage &c) 
{
	unsigned char *b;
	int v, tmp;
	b = new unsigned char[c.width * c.height];
	v = a;

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) {
			tmp = v - (int)(*p++);
			tmp = clip0(tmp);
			*q++ = (unsigned char)tmp;	
		}
	return BinaryImage(c.width, c.height, b);
}

int operator == (const BinaryImage &c1, const BinaryImage &c2)
{
	if ((c1.width != c2.width) || (c1.height != c2.height) || 
		(c1.depth != c2.depth)) return 0;

	for (int k = 0; k < c1.depth; k++) 
		if(memcmp(c1.buf[k], c2.buf[k], c1.width * c1.height)) return 0;
	return 1;
}

// logical operator
BinaryImage operator & (const BinaryImage &c1, const BinaryImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;
	int d = 1;

	BinaryImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	for (int k = 0; k < d; k++) {
		p1 = d1.buf[k]; p2 = d2.buf[k]; p3 = d3.buf[k];
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) 
				*p3++ = (*p1++) & (*p2++);		
	}
	return BinaryImage(d3);
}

BinaryImage operator &( const BinaryImage &c, unsigned long a)
{
	unsigned char *b, v;
	b = new unsigned char[c.width * c.height];
	v = (unsigned char)(a & 0xff);
	a >>= 8;

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) 
			*q++ = v & (*p++); 
	return BinaryImage(c.width, c.height, b);
}

BinaryImage operator &(unsigned long a, const BinaryImage &c) 
{
	return operator &(c, a);
}


BinaryImage operator | (const BinaryImage &c1, const BinaryImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;

	BinaryImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	p1 = d1.buf[0]; p2 = d2.buf[0]; p3 = d3.buf[0];
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++) 
			*p3++ = (*p1++) | (*p2++);		
	return BinaryImage(d3);
}

BinaryImage operator |( const BinaryImage &c, unsigned long a)
{
	unsigned char *b, v;
	b = new unsigned char[c.width * c.height];
	v = (unsigned char)(a & 0xff);
	a >>= 8;
	if (debug) printf ("0x%x ", v);

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) 
			*q++ = v | (*p++); 
	return BinaryImage(c.width, c.height, b);
}

BinaryImage operator |(unsigned long a, const BinaryImage &c) 
{
	return operator |(c, a);
}

// 20010516@0531a
// xor logical operator
BinaryImage operator ^ (const BinaryImage &c1, const BinaryImage &c2)
{
	// size check
	int w = c1.width > c2.width ? c1.width:c2.width;
	int h = c1.height > c2.height ? c1.height:c2.height;

	BinaryImage d1(w, h), d2(w, h), d3(w, h);
	d1.setSubImage(c1, 0, 0);
	d2.setSubImage(c2, 0, 0);

	unsigned char *p1, *p2, *p3;
	p1 = d1.buf[0]; p2 = d2.buf[0]; p3 = d3.buf[0];
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++) 
			*p3++ = (*p1++) ^ (*p2++);		
	return BinaryImage(d3);
}

BinaryImage operator ^( const BinaryImage &c, unsigned long a)
{
	unsigned char *b, v;
	b = new unsigned char[c.width * c.height];
	v = (unsigned char)(a & 0xff);
	a >>= 8;
	if (debug) printf ("0x%x ", v);

	unsigned char *p, *q;
	p = c.buf[0]; q = b; 
	for (int y = 0; y < c.height; y++)
		for (int x = 0; x < c.width; x++) 
			*q++ = v ^ (*p++); 
	return BinaryImage(c.width, c.height, b);
}

BinaryImage operator ^(unsigned long a, const BinaryImage &c) 
{
	return operator ^(c, a);
}
// DJ

//
////
// Color
//

//////////////////////////////////////////////////////////////////////////// 
//
//	Color Transform
//
float ColorSpaceTransform::value(float n1, float n2, float hue)
{
   if (hue>360)
    hue-=360;
    if (hue<0)
    hue+=360;
    if (hue<60)
    return (float)(n1+(n2-n1)*(hue/60.0));
    if (hue<180)
    return(n2);
    if (hue<240)
    return (float)(n1+(n2-n1)*((240-hue)/60.0));
    return n1;
}

void ColorSpaceTransform::hsi_to_rgb(float h,float s,float i,float &r,float &g,float &b)
{
    float *l, *m, *n;
    h*=360.0;
    if (h < 120.0){
        l = &b; m = &r; n = &g;
    } else if (h < 240.0){
        l = &r; m = &g; n = &b;
		h -= 120.0;
    } else {
        l = &g; m = &b; n = &r;
		h -= 240.0;
    }
    *l = (float)((1-s)/3.0);
    *m = (float)((1.0 + s * cos( h * M_PI/180)/ cos ((60-h)*M_PI/180))/3.0);
    *n = 1 - (*l+*m);
    *l *= (float)(i*3.0);
    *m *= (float)(i*3.0);
    *n *= (float)(i*3.0);
}

void ColorSpaceTransform::hls_to_rgb(float h,float l,float s,float & r,float &g,float &b)
{
    float   m1, m2;

    h *= 360.0;
    if (l<=0.5) m2 = (float)(l*(1.0+s));
    else m2 = l+s-l*s;
    m1 = 2*l-m2;
    if (s==0) r = g = b = l;
    else {
		r = value(m1,m2,h+(float)120.0);
		g = value(m1,m2,h);
		b = value(m1,m2,h-(float)120.0);
    }
}

void ColorSpaceTransform::hsv_to_rgb(float h,float s,float v,float & r,float &g,float &b)
{
    int i;
    float f, p, q, t;

    h *= 360.0;
    if (s==0) {
		r = v; g = v; b = v;
    } else {
		if (h==360) h = 0;
		h /= 60;
		i = (int)floor(h);
		f = h - i;
		p = (float)(v*(1.0-s));
		q = (float)(v*(1.0-(s*f)));
		t = (float)(v*(1.0-(s*(1.0-f))));
		switch (i) {
		case 0 : r = v; g = t; b = p; break;
		case 1 : r = q; g = v; b = p; break;
		case 2 : r = p; g = v; b = t; break;
		case 3 : r = p; g = q; b = v; break;
		case 4 : r = t; g = p; b = v; break;
		case 5 : r = v; g = p; b = q; break;
		}
    }
}

void ColorSpaceTransform::yiq_to_rgb(float y,float i,float q,float &r,float &g,float &b)
{
    r = (float)(1.0*y + 0.948262*i + 0.624013*q);
    g = (float)(1.0*y + -0.276066*i + -0.63981*q);
    b = (float)(1.0*y + -1.10545*i + 1.72986*q);
}

void ColorSpaceTransform::rgb_to_hsi(float r,float g,float b,float &h,float &s,float &i)
{
    float maxcol, mincol;
    float sum;

    maxcol = MAX(r,MAX(g,b));
    mincol = MIN(r,MIN(g,b));
    sum = r+g+b;
    i = (float)(sum/3.0);                /* intensity */
    if (i != 0.) s = 1 - mincol / i; /* saturation */
    else {
        s = 0.0; /* meaningless*/
        h = 0.0;
        return ;
    }

    if (s == 0.) h = 0; /* meaningless */
    else {
        double rg, rb, gb;
        if (r == g && r == b) h = 0.0;
        else {
            rg = r - g; rb = r - b; gb = g - b; 
            errno=0;
			if (b > g) { 
				h = (float)acos(0.5 * (rg + rb)/sqrt(rb*rb - rg*gb));
				h = (float)(M_PI * 2.0 - h);
			} else 
				h = (float)acos(0.5 * (rg + rb)/sqrt(rg*rg + rb*gb));
/*            if (isnand(h))
                fprintf (stderr,"NaN !! (r, g, b) = (%4.1f, %4.1f, %4.1f)\n",
                            r, g, b);
            if (errno)
                perror("error in rgb_to_hsi");*/
        }
    }

    h /= (float)(M_PI*2.0);
    if (debug)
    if(h>1.0 || s > 1.0 || h < 0.0 || s < 0.0)
        fprintf (stderr, "hsi = %6.3f %6.3f %6.3f\n", h, s, i);
}

void ColorSpaceTransform::rgb_to_hls(float r,float g,float b,float &h,float &l,float &s)
{
    float rc, gc, bc;
    float maxcol, mincol, cdelta;

    maxcol = MAX(r,MAX(g,b));
    mincol = MIN(r,MIN(g,b));
    l = (mincol+maxcol)/2;     /* lightness */
    if (maxcol==mincol) {       /* achromatic case */
		s = 0;     /* h is undefined in the achromatic case */
		h = 0;
    } else {
		if (l<=0.5) s = (maxcol-mincol)/(maxcol+mincol);
		else s = (maxcol-mincol)/(2-maxcol-mincol);
	/* find hue */
		cdelta = maxcol-mincol;
		rc = (maxcol-r)/cdelta;
		gc = (maxcol-g)/cdelta;
		bc = (maxcol-b)/cdelta;
		if (r==maxcol) h = bc-gc;
		else if (g==maxcol) h = 2+rc-bc;
		else h = 4+gc-rc;
		h *= 60.0;
		if (h<0.0) h += 360.0;
		h /= 360.0;
    }
}

void ColorSpaceTransform::
		rgb_to_hsv(float r,float g,float b,float &lh,float &ls,float &lv)
{
    float h, s, v;
    float cmax, cmin, cdelta;
    float rc, gc, bc;

    /* find the cmax and cmin of r g b */
    cmax = r; cmin = r;
    cmax = (g>cmax ? g:cmax);
    cmin = (g<cmin ? g:cmin);
    cmax = (b>cmax ? b:cmax);
    cmin = (b<cmin ? b:cmin);
    v = cmax;       /* value */
    if (cmax!=0.0) s = (cmax - cmin)/cmax;
    else { s = 0.0; h = 0.0; }
    if (s == 0.0) h = -1.0;
    else {
		cdelta = cmax-cmin;
		rc = (cmax-r)/cdelta;
		gc = (cmax-g)/cdelta;
		bc = (cmax-b)/cdelta;
		if (r==cmax) h = bc-gc;
		else if (g==cmax) h = (float)2.0+rc-bc;
		else h = (float)4.0+gc-rc;
		h = (float)(h*60.0);
		if (h<0.0) h += 360.0;
    }
    ls = s;
    lh = (float)(h/360.0);
    lv = v;
}

void ColorSpaceTransform::setcolorsys(ColorSys sys)
{
    switch(sys) {
    case RGB: tofunc = rgb_to_rgb; fromfunc = rgb_to_rgb; colorsys = sys; break;
    case CMY: tofunc = cmy_to_rgb; fromfunc = rgb_to_cmy; colorsys = sys; break;
    case HSV: tofunc = hsv_to_rgb; fromfunc = rgb_to_hsv; colorsys = sys; break;
    case HLS: tofunc = hls_to_rgb; fromfunc = rgb_to_hls; colorsys = sys; break;
    case YIQ: tofunc = yiq_to_rgb; fromfunc = rgb_to_yiq; colorsys = sys; break;
    case HSI: tofunc = hsi_to_rgb; fromfunc = rgb_to_hsi; colorsys = sys; break;
    case rgI: tofunc = rgI_to_rgb; fromfunc = rgb_to_rgI; colorsys = sys; break;
    case gbI: tofunc = gbI_to_rgb; fromfunc = rgb_to_gbI; colorsys = sys; break;
    case brI: tofunc = brI_to_rgb; fromfunc = rgb_to_brI; colorsys = sys; break;
    case rgb: tofunc = nrgb_to_rgb; fromfunc = rgb_to_nrgb; colorsys = sys; break;
	case I1I2I3: tofunc = irg_to_rgb; fromfunc = rgb_to_i1i2i3; colorsys = sys; break;
	case I1I2I3N: tofunc = irg_to_rgb; fromfunc = rgb_to_i1i2i3n; colorsys = sys; break;
	case YCRCB: tofunc = irg_to_rgb; fromfunc = rgb_to_YCrCb; colorsys = sys; break;
	case I1I2I3NN: tofunc = irg_to_rgb; fromfunc = rgb_to_i1i2i3nn; colorsys = sys; break;
	case STV: tofunc = irg_to_rgb; fromfunc = rgb_to_stv; colorsys = sys; break;
	case I1I2I3N3: tofunc = irg_to_rgb; fromfunc = rgb_to_i1i2i3n3; colorsys = sys; break;
    default: 
		fprintf(stderr,"ColorSpaceTransform::setcolorsys :  bad color system no %d\n",sys);
        tofunc = rgb_to_rgb;
        fromfunc = rgb_to_rgb;
        colorsys = sys;
        break;
    }
}

void ColorSpaceTransform::fromtransform(ColorImage &c)
{
	int x, y;
    int ir, ig, ib;
	unsigned char *rp = c.buf[0], *gp = c.buf[1], *bp = c.buf[2];
 	float cf1[3], cf2[3];

    for (y = 0; y < c.height; y++)
        for (x = 0; x < c.width; x++, rp++, gp++, bp++) {
            irgb_to_rgb(*rp, *gp, *bp, cf1[0], cf1[1], cf1[2]);
            fromrgb(cf1[0], cf1[1], cf1[2], cf2[0], cf2[1], cf2[2]);
            rgb_to_irgb(cf2[0], cf2[1], cf2[2], ir, ig, ib);
			*rp = (unsigned char)clips(ir);
			*gp = (unsigned char)clips(ig);
			*bp = (unsigned char)clips(ib);
        }
}

void ColorSpaceTransform::totransform(ColorImage &c)
{
	int x, y;
    int ir, ig, ib;
	unsigned char *rp = c.buf[0], *gp = c.buf[1], *bp = c.buf[2];
 	float cf1[3], cf2[3];

    for (y = 0; y < c.height; y++)
        for (x = 0; x < c.width; x++, rp++, gp++, bp++) {
            irgb_to_rgb(*rp, *gp,  *bp, cf1[0], cf1[1], cf1[2]);
            torgb(cf1[0], cf1[1], cf1[2], cf2[0], cf2[1], cf2[2]);
            rgb_to_irgb(cf2[0], cf2[1], cf2[2], ir, ig, ib);
			*rp = (unsigned char)clips(ir);
			*gp = (unsigned char)clips(ig);
			*bp = (unsigned char)clips(ib);
        }
}

char *ColorSpaceTransform::getTransformName()
{
    char *titlename;
    static char title[10];
    switch(colorsys) {
    case ColorSpaceTransform::RGB:
        titlename = "RGB-RGB";
        break;
    case ColorSpaceTransform::CMY:
        titlename = "RGB-CMY";
        break;
    case ColorSpaceTransform::HSV:
        titlename = "RGB-HSV";
        break;
    case ColorSpaceTransform::HLS:
        titlename = "RGB-HLS";
        break;
    case ColorSpaceTransform::YIQ:
        titlename = "RGB-YIQ";
        break;
    case ColorSpaceTransform::HSI:
        titlename = "RGB-HSI";
        break;
    case ColorSpaceTransform::rgI:
        titlename = "RGB-rgI";
        break;
    case ColorSpaceTransform::gbI:
        titlename = "RGB-gbI";
        break;
    case ColorSpaceTransform::brI:
        titlename = "RGB-brI";
        break;
    case ColorSpaceTransform::rgb:
        titlename = "RGB-rgb";
        break;
    default:
        fprintf(stderr,"imageTransform():bad color system no %d\n",colorsys);
        break;
    }

    sprintf (title,"%s",titlename);
    return title;
}

char *ColorSpaceTransform::getName()
{
    char *titlename;
    static char title[10];
    switch(colorsys) {
    case ColorSpaceTransform::RGB:
        titlename = "RGB";
        break;
    case ColorSpaceTransform::CMY:
        titlename = "CMY";
        break;
    case ColorSpaceTransform::HSV:
        titlename = "HSV";
        break;
    case ColorSpaceTransform::HLS:
        titlename = "HLS";
        break;
    case ColorSpaceTransform::YIQ:
        titlename = "YIQ";
        break;
    case ColorSpaceTransform::HSI:
        titlename = "HSI";
        break;
    case ColorSpaceTransform::rgI:
        titlename = "rgI";
        break;
    case ColorSpaceTransform::gbI:
        titlename = "gbI";
        break;
    case ColorSpaceTransform::brI:
        titlename = "brI";
        break;
    case ColorSpaceTransform::rgb:
        titlename = "rgb";
        break;
    default:
        fprintf(stderr,"imageTransform():bad color system no %d\n",colorsys);
        break;
    }

    sprintf (title,"%s",titlename);
    return title;
}



//////////////////////////////////////////////////////////////////////////// 
// 					utility

// slow alorithm
ImageLabeling::~ImageLabeling() 
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
GrayImage *ImageLabeling::labeling(GrayImage *b, int verbose)
{
	if (b == NULL){
		if(bimg == NULL) return NULL;
	} else
		bimg = b;

	GrayImage *out_i = NULL;
    int i,j,k;
    int ch=0;
	GrayImage temp(bimg->width, bimg->height);
	unsigned short *sbuf = new unsigned short[bimg->width * bimg->height];
	//const	int Num = 256 * 256;
	int Num = bimg->width * bimg->height;
    //int alpha[Num], A[Num];
	int *alpha = new int[Num];
	int *A = new int[Num];

	unsigned char *p, *p1, *p2;
	unsigned char q, q1, q2;
	unsigned char zz = 0;
	unsigned short *s, *s1, *s2;

    for(i=0;i<Num;i++) { alpha[i]=i; A[i] = 0; }

	// result table[alpha] is sorted incremetally
	p = bimg->getBuffer(0); p1 = bimg->getBuffer(0); p2 = bimg->getBuffer(0);
	s = sbuf; s1 = sbuf; s2 = sbuf;
	int u1, u2, uM, um;
	//for(j=0;j<bimg->height;j++, p++, s++){
	for(j=0;j<bimg->height;j++)
	{
		if (j == 1){s2 = sbuf; p2=bimg->getBuffer(0);}
		p1 = &zz;
	//	for(i=0;i<bimg->width;i++){
		for(i=0;i<bimg->width;i++, p++, s++)
		{
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
				A[*s]++;
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

	out_i = new GrayImage(temp);

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

		delete []A;
		delete []alpha;
	delete []sbuf;
	return out_i;
}

float ImageLabeling::getArea(int nth)
{
	if (labeledimg == NULL) {
		fprintf (stderr,"not yet labeld\n");
		return -1;
	}
	return (float)areas[nth];
}

int ImageLabeling::calArea()
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

void ImageLabeling::getRegion(int nth, int &x1, int &y1, int &x2, int &y2)
{
	int x, y, n;

	CRect *rgn_tbl = new CRect[N];

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
		printf ("%3d [%3d %3d %3d %3d]\n", n+1, rgn_tbl[n].left, rgn_tbl[n].right, labeledimg->height - rgn_tbl[n].top,labeledimg->height -  rgn_tbl[n].bottom);
*/

	x1 = rgn_tbl[nth].left; y1 = rgn_tbl[nth].top;
	x2 = rgn_tbl[nth].right; y2 = rgn_tbl[nth].bottom;

	rgns = rgn_tbl;
}

BinaryImage *ImageLabeling::select(int min_thresh,int max_thresh, SelectionMode mode, Threshold tmode)
{
	int n,x,y;
	unsigned char *p;
//	GrayImage *bin_i = new GrayImage(labeledimg);
	BinaryImage *bin_i = new BinaryImage(labeledimg);
	switch(mode){
	case Area:
		if (areas == NULL) getArea();
		if (map == NULL) map = new int[N+1];
		map[0] = 0;	// background
		for (n = 0; n < N; n++) {
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
		p = bin_i->buf[0];
		for(y=0;y<bin_i->height;y++)
			for(x=0;x<bin_i->width;x++, p++)
				*p = map[*p];
		break;
	default:
		printf("mode selection error\n");
	}

	return bin_i;
}

//////////////////////////////////////////////////////////////////////////////////
// class ProfileData

int
ProfileData::getMaxMin(int *max_ns, int &max_num, int *min_ns, int &min_num)	
{
	int i;
	int *x = X;
	int min_value = X[0], max_value = X[0];

	for (i = 0; i < N; i++, x++) {
		if ((*x) < min_value) { min_value = *x;}
		if ((*x) > max_value) { max_value = *x;}
	}
	min_num = getValue(min_value, min_ns);	
	max_num = getValue(max_value, max_ns);	
	return (min_num + max_num);
}

int
ProfileData::getMax(int *max_ns, int *max_num)
{
	int i;
	int *x = X;
	int max_value = X[0];

	for (i = 0; i < N; i++, x++) {
		if ((*x) > max_value) { max_value = *x;}
	}
	if (max_num) *max_num = getValue(max_value, max_ns);	
	return (max_value);
}

int
ProfileData::getMin(int *min_ns, int *min_num)
{
	int i;
	int *x = X;
	int min_value = X[0];

	for (i = 0; i < N; i++, x++) {
		if ((*x) < min_value) { min_value = *x;}
	}
	if (min_num) *min_num = getValue(min_value, min_ns);	
	return (min_value);
}

int 
ProfileData::getPercent(int *ns, float percent, float tol)	
{
	int i, k = 0;
	int *x = X;
	int max_value = getMax();
	int min_value = getMin();

	int v1 = (int)((float) (max_value - min_value) * (percent - tol));
	int v2 = (int)((float) (max_value - min_value) * (percent + tol));
	v1 += min_value; v2 += min_value;
	if (debug) {
		printf ("min : %d, max : %d\n", min_value, max_value);
		printf ("finding :[%d,%d]\n", v1, v2);
	}
	int flag = 0;
	for (i = 0; i < N; i++, x++) 
		if ((*x) >= v1 && (*x) <= v2) {
			if (ns) ns[k++] = i;
			else k++;
			flag = 0;
		} else if ((*x) < v1) {
			if (flag > 0) {
				if (ns) {
					if ((v1 - *x) < (*(x-1) - v2))
						ns[k++] = i;
					else 
						ns[k++] = i-1;
				}
				else k++;
			}
			flag = -1;
		} else if ((*x) > v2) {
			if (flag < 0) {
				if (ns) {
					if ((v1 - *(x-1)) > (*x - v2))
						ns[k++] = i;
					else 
						ns[k++] = i-1;
				}
				else k++;
			}
			 flag = 1;
		}
	if (debug) 
		printf (" num = %d\n", k);
	return k;
}

int 
ProfileData::getPulse(int *ns, float percent, PulseType p_pulse, float tol,
int verbose)	
{
	int i, k = 0;
	int *x = X;
	int max_value = getMax();
	int min_value = getMin();

	int v1 = (int)((float) (max_value - min_value) * (percent - tol));
	v1 += min_value; 
	if (debug) {
		printf ("min : %d, max : %d\n", min_value, max_value);
		printf ("thresholing at %d\n", v1);
	}
	if (v1 == min_value) {
		printf ("Warning in ProfileData::getPulse : ");
		printf ("the thresholding value is selected at minimum\n");
	}

	// 		----\___________/-----------\________
	// flag :111 00000000000 11111111111 0000000
	//
	int flag = 0;
	for (i = 0; i < N; i++, x++) {
		switch(flag) {
		case 0:
			if ((*x) >= v1) { // rising edge
				if( (k%2 == 0 && p_pulse == Positive) || 
					(k%2 == 1 && p_pulse == Negative) ) {
					if (ns) ns[k++] = i;
					else k++;
				}
			}
			break;
		case 1:
			if ((*x) < v1) { // falling edge
				if ((k%2 == 1 && p_pulse == Positive) ||
					(k%2 == 0 && p_pulse == Negative) ) {
					if (ns) ns[k++] = i;
					else k++;
				}
			}
			break;
		default:
			break;
		}
		if ((*x) >= v1) flag = 1;
		else flag = 0;
	}

	if (verbose) {
		printf (" num = %d(thresh = %d): ", k, v1);
		for (i = 0; i < k; i++) {
			if (i%2 == 0) printf ("[%d,", ns[i]);
			else  printf ("%d] ", ns[i]);
		}
		printf ("\n");
	}
	return k;
}

int
ProfileData::getValue(int value, int *ns)
{
	int i, k = 0;
	int *x = X;

	for (i = 0; i < N; i++, x++) 
		if (value == (*x)) ns[k++] = i;
	ns [k] = 0;
	return k;
}

int
ProfileData::selectPulse(int *outi, int *ini, int num, int min_width, int max_width)
{
	int w;
	int i, j = 0;
	if (min_width == -1) min_width = 0;
	for (i = 0; i < num/2; i++) {
		w = abs(ini[2*i] - ini[2*i+1]);	
		if (max_width == -1 && w >= min_width) {
			outi[2*j] = ini[2*i];
			outi[2*j+1] = ini[2*i+1];
			j++; 
		} else if (w >= min_width && w <=max_width) {
			outi[2*j] = ini[2*i];
			outi[2*j+1] = ini[2*i+1];
			j++; 
		}
	} 

	if (debug) {
		printf (" num = %d: ", j);
		for (i = 0; i < j; i++) {
			printf ("[%d,%d] ", outi[2*i], outi[2*i+1]);
		}
		printf ("\n");
	}
	return 2*j;
}

int
ProfileData::save(char *filename, int verbose)
{
	int i;

	FILE *fout;

	if ((fout = fopen(filename,"w")) == NULL) {
		return -1;
	}
	if (verbose) {
		printf ("save to file : %s\n", filename);
	}
	for (i = 0; i < N; i++) 
		fprintf(fout,"%d\n", X[i]);
	fclose(fout);
	return 0;
}

int icmp(const void *x, const void *y)
{
	int xx = *(int *)x, yy = *(int *)y;
	return (xx -yy); 
//	return ((*(int *)x)-(*(int *)y));
}

ProfileData *
ProfileData::medianFilter(int size)
{
	ProfileData *med = new ProfileData(N, new int[N]);
	int *base = new int[size];	
	if (!(size%2)) size ++;

	for (int i = 0; i < N; i++) {
		if (i < size/2 || i > (N-size/2)) (*med)[i] = X[i];
		else {
			for (int j = 0; j < size; j++)
				base[j] = X[i+j-size/2];	
			qsort(base, size, sizeof(int), icmp);
			(*med)[i] = base[size/2];
		}
	}
	delete base;
	return med;
}

//
// find local max, min list 
// in the sense of scale space
// dx1 (t) = 			  x(t) - x(t-1)
// dx2 (t) = 		    x(t+1) - x(t-1)
// dx3 (t) = {x(t+2) + x(t+1)} - {x(t-1) + x(t-2)}
// -- remove critical point 
int 
ProfileData::findLocalMaxMin(int *maxpt, int &maxnum, int *minpt, int &minnum,
					int tolHeight, int dx1Thresh, int dx2Thresh, int dx3Thresh, int verbose)
{
	int dx1, dx2, dx3;
	int increasing = 1;
	int znum = 0, 
		prestate = 0;	// previous extremum point's max(+1)/min(-1) status
						
	int etpt=-1,		// extremum point
		 st_max=-1, en_max=-1, st_min=-1, en_min=-1;
	char msg[100];

	if (verbose) 
		printf ("  i : X ,inc,znum,      dx,        etpt,    max,   min, prestate, msg\n"); 
	// find all extrema
	maxnum = 0; minnum = 0;
	for (int i = 2; i < N-3; i++) {
		sprintf (msg, "");
		dx1 = X[i] - X[i-1];
		dx2 = X[i+1] - X[i-1];
		dx3 = X[i+2] + X[i+1] - X[i-2] - X[i-1];
		if (dx1 > dx1Thresh && dx2 > dx2Thresh && dx3 > dx3Thresh) { // increasing
			if (etpt != -1 && prestate != -1) { 
//			if (etpt != -1 && (prestate == 0 || 
//	 		(prestate == 1 && maxnum > 0 &&  X[maxpt[maxnum-1]] >= X[etpt] + tolHeight))) { 
				switch(increasing){
				case -3:	// min
					prestate = -1; 	// current point is minimum
					minpt[minnum++] = etpt;
					if (verbose) sprintf (msg, "Min(%d)", etpt);
					etpt = -1;
					break;
				case 0:
					en_min = etpt ;
					if (st_min != -1) {
						if (verbose) sprintf (msg, "Min(%d)", etpt);
						prestate = -1;
						if (en_min == st_min) // single points
							minpt[minnum++] = en_min;
						else {
							if (verbose) sprintf (msg, "Mins**(%d, %d)", st_min, en_min);
							minpt[minnum++] = st_min;
							minpt[minnum++] = en_min;
						}
					} else if (prestate ==0) {// first point
						prestate = -1; 	// current point is minimum
						minpt[minnum++] = etpt;
						if (verbose) sprintf (msg, "Min(%d)", etpt);
					}
					st_min = en_min = -1;
					st_max = en_max = -1;
					etpt = -1;
					break;
				}
			} 
			increasing = 3;
			znum = 0;
		} else if (dx1 < -dx1Thresh && dx2 < -dx2Thresh && dx3 < -dx3Thresh) { // decreasing
			if (etpt != -1 && prestate != 1) { 
//			if (etpt != -1 && (prestate == 0 || 
//			(prestate == -1 && minnum > 0 
//				&&  X[minpt[minnum-1]] +tolHeight <= X[etpt]))) { 
				switch(increasing){
				case 3:
					if (verbose) sprintf (msg, "Max(%d)", etpt);
					maxpt[maxnum++] = etpt;
					prestate = 1; 	// current point is maximum
					etpt = -1;
					break;
				case 0:
					en_max = etpt;
					if (st_max != -1 ) {
						prestate = 1;
						if (verbose) sprintf (msg, "Max(%d)", etpt);
						if (en_max == st_max) // single points
							maxpt[maxnum++] = en_max;
						else {
							if (verbose) sprintf (msg, "Maxs**(%d, %d)", st_max, en_max);
							maxpt[maxnum++] = st_max;
							maxpt[maxnum++] = en_max;
						}
					} else if (prestate ==0) {// first point
						if (verbose) sprintf (msg, "Max(%d)", etpt);
						maxpt[maxnum++] = etpt;
						prestate = 1; 	// current point is maximum
						etpt = -1;
					}
					st_min = en_min = -1;
					st_max = en_max = -1;
					etpt = -1;
					break;
				default:
					break;
				}
			} 
			increasing = -3;
			znum = 0;
//		} else if (dx1 == 0 && dx2 == 0 && dx3 == 0) { // decreasing
		} else if (abs(dx1) <= dx1Thresh && abs(dx2) <= dx2Thresh && abs(dx3) <= dx3Thresh) { // decreasing
			switch(increasing){
			case 3:
				st_max = i;
				break;
			case -3:
				st_min = i;
				break;
			default:
				break;
			}
			etpt = i; 
			znum = 7;
			increasing = 0;
		} else if (znum < 7) {
			// complex due to noise
			// mark most suitable etpt 
			if (abs(dx1) <= dx1Thresh && abs(dx2)<= dx2Thresh && znum <= 6)  {
				etpt = i; znum = 6;
			} else if (abs(dx1) <= dx1Thresh && abs(dx3) <= dx3Thresh && znum <= 5)  {
				etpt = i; znum = 5;
			} else if (abs(dx1) <= dx1Thresh && znum <= 4)  {
				etpt = i; znum = 4;
			} else if (abs(dx2)<= dx2Thresh && abs(dx3) <= dx3Thresh && znum <= 3) {
				etpt = i; znum = 3;
			} else if (abs(dx2)<= dx2Thresh && znum <= 2) {
				etpt = i; znum = 2;
			} else if (abs(dx3) <= dx3Thresh && znum <= 1) {
				etpt = i; znum = 1;
			} else {
				if (verbose) sprintf(msg,"don't know");
			}
		} else {
/*
			printf ("BUG at %d(%d) :znum = %d, increasing = %d, prestate = %d, ", 
					i, X[i], znum, increasing, prestate);
			printf ("dx = [%d, %d, %d]\n", dx1, dx2, dx3);
			printf ("max = [%d, %d], ", st_max, en_max);
			printf ("min = [%d, %d]\n", st_min, en_min);
*/
		}
		if (verbose) 
			printf ("%3d:%3d, %2d, %d, [%3d, %3d, %3d], %3d, [%3d,%3d],[%3d,%3d],%d, %s\n", 
					i, X[i], increasing, znum, dx1, dx2, dx3, etpt, st_max, en_max, st_min, en_min, prestate, msg);
	}

	if(increasing == 0 && znum == 7 ) {
		if (st_min != -1 && prestate == 1)
			minpt[minnum++] = st_min;
		else if (st_max != -1 && prestate == -1)
			maxpt[maxnum++] = st_max;
	}

	return (maxnum+minnum);
}

int 
ProfileData::findLocalMaxMinAlternate(int *maxpt, int &maxnum, int *minpt, int &minnum,
					int tolHeight, int dx1Thresh, int dx2Thresh, int dx3Thresh, int verbose)
{
	int dx1, dx2, dx3;
	int increasing = 1;
	int znum = 0, 
		prestate = 0;	// previous extremum point's max(+1)/min(-1) status
						
	int etpt=-1,		// extremum point
		 st_max=-1, en_max=-1, st_min=-1, en_min=-1;
	char msg[100];

	if (verbose) 
		printf ("  i : X ,inc,znum,      dx,        etpt,    max,   min, prestate, msg\n"); 
	// find all extrema
	maxnum = 0; minnum = 0;
	for (int i = 2; i < N-3; i++) {
		sprintf (msg, "");
		dx1 = X[i] - X[i-1];
		dx2 = X[i+1] - X[i-1];
		dx3 = X[i+2] + X[i+1] - X[i-2] - X[i-1];
		if (dx1 > dx1Thresh && dx2 > dx2Thresh && dx3 > dx3Thresh) { // increasing
			if (etpt != -1 && prestate != -1) { 
//			if (etpt != -1 && (prestate == 0 || 
//	 		(prestate == 1 && maxnum > 0 &&  X[maxpt[maxnum-1]] >= X[etpt] + tolHeight))) { 
				switch(increasing){
				case -3:	// min
					prestate = -1; 	// current point is minimum
					minpt[minnum++] = etpt;
					if (verbose) sprintf (msg, "Min(%d)", etpt);
					etpt = -1;
					break;
				case 0:
					en_min = etpt ;
					if (st_min != -1) {
						if (verbose) sprintf (msg, "Min(%d)", etpt);
						prestate = -1;
						if (en_min == st_min) // single points
							minpt[minnum++] = en_min;
						else {
							if (verbose) sprintf (msg, "Mins**(%d, %d)", st_min, en_min);
							minpt[minnum++] = (st_min +en_min)/2;
						}
					} else if (prestate ==0) {// first point
						prestate = -1; 	// current point is minimum
						minpt[minnum++] = etpt;
						if (verbose) sprintf (msg, "Min(%d)", etpt);
					}
					st_min = en_min = -1;
					st_max = en_max = -1;
					etpt = -1;
					break;
				}
			} 
			increasing = 3;
			znum = 0;
		} else if (dx1 < -dx1Thresh && dx2 < -dx2Thresh && dx3 < -dx3Thresh) { // decreasing
			if (etpt != -1 && prestate != 1) { 
//			if (etpt != -1 && (prestate == 0 || 
//			(prestate == -1 && minnum > 0 
//				&&  X[minpt[minnum-1]] +tolHeight <= X[etpt]))) { 
				switch(increasing){
				case 3:
					if (verbose) sprintf (msg, "Max(%d)", etpt);
					maxpt[maxnum++] = etpt;
					prestate = 1; 	// current point is maximum
					etpt = -1;
					break;
				case 0:
					en_max = etpt;
					if (st_max != -1 ) {
						prestate = 1;
						if (verbose) sprintf (msg, "Max(%d)", etpt);
						if (en_max == st_max) // single points
							maxpt[maxnum++] = en_max;
						else {
							if (verbose) sprintf (msg, "Maxs**(%d, %d)", st_max, en_max);
							maxpt[maxnum++] = (st_max +en_max)/2;
						}
					} else if (prestate ==0) {// first point
						if (verbose) sprintf (msg, "Max(%d)", etpt);
						maxpt[maxnum++] = etpt;
						prestate = 1; 	// current point is maximum
						etpt = -1;
					}
					st_min = en_min = -1;
					st_max = en_max = -1;
					etpt = -1;
					break;
				default:
					break;
				}
			} 
			increasing = -3;
			znum = 0;
//		} else if (dx1 == 0 && dx2 == 0 && dx3 == 0) { // decreasing
		} else if (abs(dx1) <= dx1Thresh && abs(dx2) <= dx2Thresh && abs(dx3) <= dx3Thresh) { // decreasing
			switch(increasing){
			case 3:
				st_max = i;
				break;
			case -3:
				st_min = i;
				break;
			default:
				break;
			}
			etpt = i; 
			znum = 7;
			increasing = 0;
		} else if (znum < 7) {
			// complex due to noise
			// mark most suitable etpt 
			if (abs(dx1) <= dx1Thresh && abs(dx2)<= dx2Thresh && znum <= 6)  {
				etpt = i; znum = 6;
			} else if (abs(dx1) <= dx1Thresh && abs(dx3) <= dx3Thresh && znum <= 5)  {
				etpt = i; znum = 5;
			} else if (abs(dx1) <= dx1Thresh && znum <= 4)  {
				etpt = i; znum = 4;
			} else if (abs(dx2)<= dx2Thresh && abs(dx3) <= dx3Thresh && znum <= 3) {
				etpt = i; znum = 3;
			} else if (abs(dx2)<= dx2Thresh && znum <= 2) {
				etpt = i; znum = 2;
			} else if (abs(dx3) <= dx3Thresh && znum <= 1) {
				etpt = i; znum = 1;
			} else {
				if (verbose) sprintf(msg,"don't know");
			}
		} else {
/*
			printf ("BUG at %d(%d) :znum = %d, increasing = %d, prestate = %d, ", 
					i, X[i], znum, increasing, prestate);
			printf ("dx = [%d, %d, %d]\n", dx1, dx2, dx3);
			printf ("max = [%d, %d], ", st_max, en_max);
			printf ("min = [%d, %d]\n", st_min, en_min);
*/
		}
		if (verbose) 
			printf ("%3d:%3d, %2d, %d, [%3d, %3d, %3d], %3d, [%3d,%3d],[%3d,%3d],%d, %s\n", 
					i, X[i], increasing, znum, dx1, dx2, dx3, etpt, st_max, en_max, st_min, en_min, prestate, msg);
	}

	if(increasing == 0 && znum == 7 ) {
		if (st_min != -1 && prestate == 1)
			minpt[minnum++] = st_min;
		else if (st_max != -1 && prestate == -1)
			maxpt[maxnum++] = st_max;
	}

	return (maxnum+minnum);
}

int 	
ProfileData::
removeFromMaxMinAlternate (int *maxpt, int &maxnum, int *minpt, int &minnum,
					int tolHeight, int MinList)
{
	int *ptlist1, *ptlist2;
	int num, start;
	int cnt = 0;
	if (MinList == 1) {
		ptlist1 = minpt; ptlist2 = maxpt; num = minnum;
		start = (maxpt[0] < minpt[0]) ? 0 : 1;
	} else {
		ptlist1 = maxpt; ptlist2 = minpt; num = maxnum;
		start = (maxpt[0] > minpt[0]) ? 0 : 1;
	}
	for (int i = start; i < num-1; i++) {
		// remove ptlist1, i
		if (abs(X[ptlist2[i-start]] - X[ptlist1[i]]) <= tolHeight &&
			abs(X[ptlist2[i+1-start]] - X[ptlist1[i]]) <= tolHeight) { 
			if (MinList == 1) {
				ptlist2[i-start] = 
					(X[ptlist2[i-start]]> X[ptlist2[i+1-start]]) ?
							ptlist2[i-start]: ptlist2[i+1-start];
				for (int j = i-start+1; j < maxnum-1; j++)
					maxpt[j] = maxpt[j+1];
			} else {
				ptlist2[i-start] = 
						(X[ptlist2[i-start]]< X[ptlist2[i+1-start]]) ?
							ptlist2[i-start]: ptlist2[i+1-start];
				for (int j = i-start+1; j < minnum-1; j++)
					minpt[j] = minpt[j+1];
			}
			
			for (int j = i; j < num-1; j++){
				ptlist1[j] = ptlist1[j+1];
			}
			i--;
			cnt ++; num --;
		}
	}

/* start/end points
	if (start) {
		if (abs(ptlist2[i] - ptlist1[i]) <= tolHeight)
			for (j = i; j < num-1; j++){
				ptlist1[j] = plist1[j+1];
			}
	}
*/
	if (MinList == 1) 	{ minnum = num; maxnum -= cnt; } 
	else 				{ maxnum = num; minnum -= cnt; }
	return cnt;
}

// just debug
void ProfileData::
printLocalMaxMin(int *maxpt, int maxnum, int *minpt, int minnum, int offset, FILE *fout)
{
	fprintf (fout, "max(%d): ", maxnum); 
	for (int i = 0; i < maxnum; i++) fprintf (fout, "%3d ", maxpt[i] + offset);
	fprintf(fout, "\n");
	fprintf (fout, "min(%d): ", minnum);
	for (i = 0; i < minnum; i++) fprintf (fout, "%3d ", minpt[i] + offset);
	fprintf(fout, "\n");
}

// pt : given 
// find an interval enclosing pt(max, min)
//
int 
ProfileData::findEnclInterval(int *x1, int *x2, int *ptlist, int num, int pt)
{
	int done = 0;
	for (int i = 1; i < num && !done; i++) {
		if (ptlist[i-1] < pt && pt < ptlist[i]) {
			*x1 = ptlist[i-1]; *x2 = ptlist[i];
		}
	}
	if (!done) return -1;

	return 0;
}

/*
int 
ProfileData::findEnclInterval(int *x1, int *x2, int *maxpt, int &maxnum, int *minpt, int &minnum, int pt, int mininterval, int verbose)
{
	if (mininterval) {
		for (int i = 1; i < minnum; i++) {
			if (minpt[i-1] < pt && pt < minpt[i]) {
				*x1 = minpt[i-1]; *x2 = minpt[i];
			}
		}
	} else {
	}
}
*/
// GradImage

GradImage::GradImage(Type tt, GrayImage *mImg, GrayImage *aImg)
{
	magImage = mImg; angImage = aImg; t = tt;
}

GradImage::~GradImage()
{
	if (magImage) delete magImage; 
	if (angImage) delete angImage;
}

// Sobel Edge
double GradImage::getEdgeness_2(GrayImage &I, int x, int y)
{
    int A[8];
    int K = 2;
    double Gc, Gr;
    A[0] = I.getPixel(x-1, y+1);
    A[1] = I.getPixel(x,   y+1);
    A[2] = I.getPixel(x+1, y+1);
    A[3] = I.getPixel(x+1, y);
    A[4] = I.getPixel(x+1, y-1);
    A[5] = I.getPixel(x,   y-1);
    A[6] = I.getPixel(x-1, y-1);
    A[7] = I.getPixel(x-1, y);
    Gr = (double)((A[2]+K*A[3]+A[4]) - (A[0]+K*A[7]+A[6]))/(double)(K+2);
    Gc = (double)((A[0]+K*A[1]+A[2]) - (A[6]+K*A[5]+A[4]))/(double)(K+2);
    return (Gr*Gr + Gc * Gc);
}

void GradImage::performAtPel(GrayImage *inputImage, int x, int y, 
							double *Gx, double *Gy)
{
    int A[8];
    int K = 2;
    A[0] = inputImage->getPixel(x-1, y+1);
    A[1] = inputImage->getPixel(x,   y+1);
    A[2] = inputImage->getPixel(x+1, y+1);
    A[3] = inputImage->getPixel(x+1, y);
    A[4] = inputImage->getPixel(x+1, y-1);
    A[5] = inputImage->getPixel(x,   y-1);
    A[6] = inputImage->getPixel(x-1, y-1);
    A[7] = inputImage->getPixel(x-1, y);

	switch(t) {
	case FiniteDifference:
		printf ("not implemented\n");
		break;
	case Sobel:
	default:
		K = 2;
		break;
	}
    *Gy = (double)((A[2]+K*A[3]+A[4]) - (A[0]+K*A[7]+A[6]))/(double)(K+2);
    *Gx = (double)((A[0]+K*A[1]+A[2]) - (A[6]+K*A[5]+A[4]))/(double)(K+2);
}

GrayImage *GradImage::perform(GrayImage *inputImage, double scalingMag)
{
  int hh = inputImage->getROIy2() - inputImage->getROIy1();
  int ww = inputImage->getROIx2() - inputImage->getROIx1();
	if (magImage) magImage->Realloc(ww, hh);
	else magImage= new GrayImage(ww, hh);
	if (angImage) angImage->Realloc(ww, hh);
	else angImage= new GrayImage(ww, hh);

	unsigned char *p = inputImage->buf[0], *q = magImage->buf[0], *qtheta=angImage->buf[0];
	double gy, gx;

	if (scalingMag < 0.0) scalingMag = 1.0 / sqrt(2.0);
	double angScale = (double)255.0/(double)(2.*M_PI) ;
	double tmp;
	for (int y = inputImage->getROIy1(); y < inputImage->getROIy2(); y++) {
		p = inputImage->getp(inputImage->getROIx1(), y); 
		for (int x = inputImage->getROIx1(); x < inputImage->getROIx2(); x++, 
			   q++, qtheta++) {
			if ((y < 1) || (y >= (inputImage->h() - 1))
			   || (x < 1) || (x >= (inputImage->w() - 1))){
				*q = 0;
				*qtheta = 0;
			} else {
				performAtPel(inputImage, x, y, &gy, &gx);
				*q = clips((int) (hypot(gx, gy) * scalingMag));
				tmp  = atan2(gy, gx);
				*qtheta = clips ((int)((tmp > 0 ? tmp: (tmp + 2.0*(double)M_PI))* 							angScale)); 
			}
		}
	}
	return magImage;
}

GrayImage * GradImage::getHImage(double scaling, int offset)
{
	if (!magImage ||!angImage) {
		printf ("Waning: grad image not defined\n");
		return NULL;
	}
	GrayImage *hImage= new GrayImage(magImage->w(), magImage->h());

	unsigned char *p = magImage->buf[0], *ptheta=angImage->buf[0];
	unsigned char *q = hImage->buf[0]; 
	double magScale = 1.0, mag;
	double angScale = (double)(2.*M_PI)/(double)255.0;

	if (scaling < 0.0 && offset == 0) {	// absolute, no offset, clipping
		for (int y = magImage->getROIy1(); y < magImage->getROIy2(); y++) {
			q = hImage->getp(magImage->getROIx1(), y); 
			p = magImage->getp(magImage->getROIx1(), y); 
			ptheta = angImage->getp(magImage->getROIx1(), y);
			for (int x = magImage->getROIx1(); x < magImage->getROIx2(); x++, p++, q++, ptheta++) {
					mag = (double)(*p);
					*q = clips(abs((int)(mag*cos((double)(*ptheta)* angScale))));
			}
		}
		return hImage;
	} else if (scaling < 0.0 && offset == -1) {
		switch(t) {
		case Sobel: 
			magScale = sqrt(2.0);
			offset  = 127;
			break;
		default:
			magScale = 1;
			offset  = 127;
			break;
		}
	} else {
		magScale = scaling;
	}

	for (int y = magImage->getROIy1(); y < magImage->getROIy2(); y++) {
		q = hImage->getp(magImage->getROIx1(), y); 
		p = magImage->getp(magImage->getROIx1(), y); 
		ptheta = angImage->getp(magImage->getROIx1(), y);
		for (int x = magImage->getROIx1(); x < magImage->getROIx2(); x++, p++, q++, ptheta++) {
			if ((y < 1) || (y >= (magImage->h() - 1))
			   || (x < 1) || (x >= (magImage->w() - 1))){
				*q = 0;
				*ptheta = 0;
			} else {
				mag = (double)(*p) * magScale;
				*q = clips((int)(mag*cos((double)(*ptheta)* angScale)) + offset);
			}
		}
	}
	return hImage;
}

GrayImage * GradImage::getVImage(double scaling, int offset)
{
	if (!magImage ||!angImage) {
		printf ("Waning: grad image not defined\n");
		return NULL;
	}
	GrayImage *vImage= new GrayImage(magImage->w(), magImage->h());

	unsigned char *p = magImage->buf[0], *ptheta=angImage->buf[0];
	unsigned char *q = vImage->buf[0]; 
	double magScale = 1.0, mag;
	double angScale = (double)(2.*M_PI)/(double)255.0;

	if (scaling < 0.0 && offset == 0) {	// absolute, no offset, clipping
		for (int y = magImage->getROIy1(); y < magImage->getROIy2(); y++){
			q = vImage->getp(magImage->getROIx1(), y); 
			p = magImage->getp(magImage->getROIx1(), y); 
			ptheta = angImage->getp(magImage->getROIx1(), y);
			for (int x = magImage->getROIx1(); x < magImage->getROIx2(); x++, p++, q++, ptheta++) {
					mag = (double)(*p);
					*q = clips(abs((int)(mag*sin((double)(*ptheta)* angScale))));
			}
		}
		return vImage;
	} else if (scaling < 0.0 && offset == -1) {
		switch(t) {
		case Sobel: 
			magScale = sqrt(2.0);
			offset  = 127;
			break;
		default:
			magScale = 1;
			offset  = 127;
			break;
		}
	} else {
		magScale = scaling;
	}

	for (int y = magImage->getROIy1(); y < magImage->getROIy2(); y++) {
		q = vImage->getp(magImage->getROIx1(), y); 
		p = magImage->getp(magImage->getROIx1(), y); 
		ptheta = angImage->getp(magImage->getROIx1(), y);
		for (int x = magImage->getROIx1(); x < magImage->getROIx2(); x++, p++, q++, ptheta++) {
				mag = (double)(*p) * magScale;
				*q = clips((int)(mag*sin((double)(*ptheta)* angScale)) + offset);
		}
	}
	return vImage;
}

/*
void printMemoryInfo()
{
	struct mallinfo mi = mallinfo();

	printf ("Memory Information\n");
	printf ("%s : %d\n", "           total space in arena", mi.arena);
	printf ("%s : %d\n", "      number of ordinary blocks", mi.ordblks);
	printf ("%s : %d\n", "         number of small blocks", mi.smblks);
	printf ("%s : %d\n", " space in holding block headers", mi.hblkhd);
	printf ("%s : %d\n", "       number of holding blocks", mi.hblks);
	printf ("%s : %d\n", "   space in small blocks in use", mi.usmblks);
	printf ("%s : %d\n", "     space in free small blocks", mi.fsmblks);
	printf ("%s : %d\n", "space in ordinary blocks in use", mi.uordblks);
	printf ("%s : %d\n", "  space in free ordinary blocks", mi.fordblks);
	printf ("%s : %d\n", "                 space penalty ", mi.keepcost);
}
*/
// Dominance Edge Map

BinaryImage * GradImage::getYGradDomMap(
					GrayImage *iImg, int thresh,
					GrayImage *mImg, GrayImage *aImg, int mthresh, 
					int verbose) 
{
	if (!mImg ||!aImg) {
		if (!magImage ||!angImage) {
			printf ("Waning: grad image not defined\n");
			return NULL;
		} else {
			if (verbose) printf ("\tChange to default grad images\n");
			mImg = magImage;
			aImg = angImage;
		}
	}
	BinaryImage *vImg= new BinaryImage(mImg->w(), mImg->h());

	unsigned char *pm = mImg->buf[0], *pa=aImg->buf[0];
	unsigned char *pi = NULL;
	double mag, xx, yy;
	double angScale = (double)(2.*M_PI)/(double)255.0;

	if (iImg == NULL || thresh <= 0) {	// fast
		for (int y = mImg->getROIy1(); y < mImg->getROIy2(); y++){
			pm = mImg->getp(mImg->getROIx1(), y); 
			pa = aImg->getp(mImg->getROIx1(), y);
			for (int x = mImg->getROIx1(); x < mImg->getROIx2(); 
													x++, pm++, pa++) {
				if ((mag = (double)(*pm)) < mthresh) vImg->reset(x, y);
				else {
					yy = fabs(mag*sin((double)(*pa)* angScale));
					xx = fabs(mag*cos((double)(*pa)* angScale));
					if ( xx > yy) vImg->reset(x, y);
					else vImg->set(x, y);
				}
			}
		}
	} else {
		pi = iImg->buf[0];
		for (int y = mImg->getROIy1(); y < mImg->getROIy2(); y++){
			pm = mImg->getp(mImg->getROIx1(), y); 
			pa = aImg->getp(mImg->getROIx1(), y);
			pi = iImg->getp(iImg->getROIx1(), y);
			for (int x = mImg->getROIx1(); x < mImg->getROIx2(); 
													x++, pm++, pa++, pi++) {
				if ((*pi) < thresh) vImg->reset(x, y);
				else if ((mag = (double)(*pm)) < mthresh) vImg->reset(x, y);
				else {
					yy = fabs(mag*sin((double)(*pa)* angScale));
					xx = fabs(mag*cos((double)(*pa)* angScale));
					if ( xx > yy) vImg->reset(x, y);
					else vImg->set(x, y);
				}
			}
		}
	}
	return vImg;
}

BinaryImage * GradImage::getXGradDomMap(
					GrayImage *iImg, int thresh,
					GrayImage *mImg, GrayImage *aImg, int mthresh,
					int verbose) 
{
	if (!mImg ||!aImg) {
		if (!magImage ||!angImage) {
			printf ("Waning: grad image not defined\n");
			return NULL;
		} else {
			if (verbose) printf ("\tChange to default grad images\n");
			mImg = magImage;
			aImg = angImage;
		}
	}
	BinaryImage *hImg= new BinaryImage(mImg->w(), mImg->h());

	unsigned char *pm = mImg->buf[0], *pa=aImg->buf[0];
	unsigned char *pi = NULL;
	double mag, xx, yy;
	double angScale = (double)(2.*M_PI)/(double)255.0;

	if (iImg == NULL || thresh <= 0) {	// fast
		for (int y = mImg->getROIy1(); y < mImg->getROIy2(); y++){
			pm = mImg->getp(mImg->getROIx1(), y); 
			pa = aImg->getp(mImg->getROIx1(), y);
			for (int x = mImg->getROIx1(); x < mImg->getROIx2(); 
													x++, pm++, pa++) {
				if ((mag = (double)(*pm)) < mthresh) hImg->reset(x, y);
				else {
					yy = fabs(mag*sin((double)(*pa)* angScale));
					xx = fabs(mag*cos((double)(*pa)* angScale));
					if ( xx < yy) hImg->reset(x, y);
					else hImg->set(x, y);
				}
			}
		}
	} else {
		pi = iImg->buf[0];
		for (int y = mImg->getROIy1(); y < mImg->getROIy2(); y++){
			pm = mImg->getp(mImg->getROIx1(), y); 
			pa = aImg->getp(mImg->getROIx1(), y);
			pi = iImg->getp(iImg->getROIx1(), y);
			for (int x = mImg->getROIx1(); x < mImg->getROIx2(); 
													x++, pm++, pa++, pi++) {
				if ((*pi) < thresh) hImg->reset(x, y);
				else if ((mag = (double)(*pm)) < mthresh) hImg->reset(x, y);
				else {
					yy = fabs(mag*sin((double)(*pa)* angScale));
					xx = fabs(mag*cos((double)(*pa)* angScale));
					if ( xx < yy) hImg->reset(x, y);
					else hImg->set(x, y);
				}
			}
		}
	}
	return hImg;
}


void ColorImage::drawLine(int x1, int y1, int x2, int y2, int d, unsigned char gray)
{
	

}

void ColorImage::drawRect(int x1, int y1, int x2, int y2, int d, unsigned char gray)
{
	int i;

	if ((x1 < x2) && (y1 < y2)
		&& (x1 >= 0 && x2 <= width-1)
		&& (y1 >= 0 && y2 <= height-1))
	{
		for (i=x1; i<x2; i++)
		{
			*(buf[d]+i+y1*width) = gray;
			*(buf[d]+i+y2*width) = gray;
		}

		for (i=y1; i<y2; i++)
		{
			*(buf[d]+x1+i*width) = gray;
			*(buf[d]+x2+i*width) = gray;
		}
	}
}

void ColorImage::drawRectAll(int x1, int y1, int x2, int y2, unsigned char gray)
{
	int i;

	for (i=0; i<depth; i++)
		drawRect(x1, y1, x2, y2, i, gray);
}

void GrayImage::drawRect(int x1, int y1, int x2, int y2, unsigned char gray)
{
	ColorImage::drawRect(x1, y1, x2, y2, 0, gray);
}

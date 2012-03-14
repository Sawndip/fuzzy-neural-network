//
//	cimage.h
//  colorimage.h
//	programmed by Gyu-tae Park
//
#ifndef _ColorImage_
#define _ColorImage_

#define M_PI	3.1415926535897932384626433832795

#define	OUTPUT	0
#define	TEST	1
#define	TRAINING	2

#define	OUTPUT_STR	"d:\\djkim\\research\\figure\\output\\"
#define	TEST_STR	"d:\\djkim\\research\\figure\\test\\"
#define	TRAINING_STR	"d:\\djkim\\research\\figure\\training\\"

#define FORMATHexMask		(1 << 0)
#define FORMATDecMask		(1 << 0)
#define FORMAT2DMask		(1 << 1)
#define FORMAT1DMask		(1 << 1)
#define FORMAT1DRowMask		(1 << 2)
#define FORMAT1DColMask		(1 << 2)
#define FORMATDirectionMask	(1 << 3)
#define FORMATHex			(0 << 0)
#define FORMATDec			(1 << 0) 
#define FORMAT2D			(1 << 1)
#define FORMAT1D			(0 << 1)
#define FORMAT1DRow			(1 << 2)
#define FORMAT1DCol			(0 << 2)
#define FORMATDirectionX	(0 << 3)
#define FORMATDirectionY	(1 << 3)

#include <math.h>

inline	int clip255(int x)	{ return (x > 255 ? 255: x); }
inline	int clip0  (int x) 	{ return (x <   0 ?   0: x); }
inline	int clips  (int x) 	{ return clip255(clip0(x));  }
#ifndef MAX
	#define MAX(a, b)	((a)>(b) ? (a):(b))
#endif
#ifndef MIN
	#define MIN(a, b)	((a)>(b) ? (b):(a))
#endif

class GrayImage;
class BinaryImage;

class ColorImage{
public:
	void drawRectAll(int x1, int y1, int x2, int y2, unsigned char gray = 255);
	void drawRect(int x1, int y1, int x2, int y2, int d, unsigned char gray = 255);
	void drawLine(int x1, int y1, int x2, int y2, int d, unsigned char gray = 255);
	unsigned char *	buf[3];
	int depth;
	int	width, height;
	int x1, x2, y1, y2;	// roi

	//
	// contructor and destructor
	//
	ColorImage 	(int w, int h, int d, unsigned char *r, 
				unsigned char *g, unsigned char *b);
	ColorImage 	(int w, int h, int d, unsigned char *bf[3]);
	ColorImage 	(int w, int h, int d, unsigned char r, 
				unsigned char g = 0, unsigned char b = 0);
	ColorImage 	(const ColorImage & c);
	ColorImage 	(char *filename);
	ColorImage 	(GrayImage *r, GrayImage *g, GrayImage *b);
	ColorImage 	(int w, int h, int d);
	ColorImage 	();
	~ColorImage	();

	//
	// methods
	//
	//void drawDot(int x, int y, int d, int gray) { *(buf[d]+x+y*width) = gray; };

	static ColorImage black(int w, int h, int d)
	{ return ColorImage	(w, h, d, (unsigned char)0, 
						 (unsigned char)0, (unsigned char)0); }
	static ColorImage white(int w, int h, int d)
	{ return ColorImage	(w, h, d, (unsigned char)255, 
						 (unsigned char)255, (unsigned char)255); }

	void Realloc(int w, int h);
	int load(char *filename, int verbose = 0);
	int save(char *filename, int verbose = 0);
	ColorImage& resize(const ColorImage &c);

	int w()	{return width;}
	int h()	{return height;}
	int d()	{return depth;}
	unsigned char *getBuffer(int d=0) {return buf[d];}

	// ROI : region of interest
	int getROIWidth(){ return x2 - x1;}
	int getROIHeight(){ return y2 - y1;}
	int getROIx1(){ return x1;}
	int getROIx2(){ return x2;}
	int getROIy1(){ return y1;}
	int getROIy2(){ return y2;}
	void setROI(int xx1, int yy1, int xx2, int yy2) 
			{if (xx1 > xx2) {x1 = xx2; x2 = xx1;} else  {x1 = xx1; x2 = xx2;} 
			 if (yy1 > yy2) {y1 = yy2; y2 = yy1;} else {y1 = yy1; y2 = yy2;} }
	void resetROI() {x1=0;x2=width;y1=0;y2=height;}
	// for debug
	void info();
	void printx(char *filename, int from_y=0, int to_y=5, int a = 9);
	void copyFrom(ColorImage *ci);
	void copyFrom(int w=0, int h=0, int d=3,  unsigned char *r = NULL, 
				unsigned char *g = NULL, unsigned char *b = NULL);
	int copyTo(unsigned char *r, unsigned char *g, unsigned char *b);

	ColorImage *getSubImage(int sx, int sy, int ex, int ey); 	
	int setSubImage(const ColorImage &ci, int sx, int sy); 	

	//unsigned char *getp(int x, int y, int z) { return(buf[z]+x+y*width);}
	unsigned char *getp(int x, int y, int z) 
	{
		if (y<0) y += height;
		if (y>height) y -= height;
		if (x<0) x += width;
		if (x>width) x -= width;
		return(buf[z]+x+y*width);
	}
	unsigned char &pelAt(int x, int y, int z) { return(*(buf[z]+x+y*width));}

	unsigned char 	getPixel(int x, int y, int z) { return *getp(x, y, z);}
	int 			getValue(int x, int y, int z) 
											{ return (int)(*getp(x, y, z));}
	void 			setPixel(int x, int y, unsigned long v) 
					{*getp(x, y, 0) = (unsigned char)(v & 0xff); 
					if (depth == 3) { *getp(x, y, 1) = (unsigned char)((v >> 8) & 0xff); 
								*getp(x, y, 2) = (unsigned char)((v >> 16) & 0xff);}}
	void 			setPixel(int x, int y, unsigned char *vs) 
					{*getp(x, y, 0) = vs[0]; 
					if(depth==3){*getp(x, y, 1) = vs[1]; *getp(x, y, 2) = vs[2];}}
	void 			setPixel(int x, int y, unsigned char r, 
							unsigned char g, unsigned char b) 
					{*getp(x, y, 0) = r; 
					if(depth==3){*getp(x, y, 1) = g; *getp(x, y, 2) = b;}}

	int to3Gray(GrayImage *r, GrayImage *g, GrayImage *b);
	int from3Gray(GrayImage *r, GrayImage *g, GrayImage *b);

	enum Threshold {InBound, OutOfBound};
	BinaryImage *	threshold(int r1=0, int r2 = 255, Threshold rmode= InBound,
							int g1=0, int g2 = 255, Threshold gmode= InBound,
							int b1=0, int b2 = 255, Threshold bmode= InBound);
	GrayImage *getLumImg();

	//
	// Image operator
	//

	// Unary plus
	ColorImage operator +() const;
	// Unary negation
	ColorImage operator -() const;
	// not logical operator
	ColorImage operator ~() const;
	// Simple assignment
	ColorImage&  operator = (const ColorImage &c);
	ColorImage&  operator = (const unsigned char a);

	// Destructive addition
	ColorImage &		operator +=(const ColorImage &c);

	// Destructive substraction
	ColorImage &		operator -=(const ColorImage &c);
};

// operators
inline ColorImage ColorImage::operator +() const
{ return *this; }
// add (plus)
ColorImage operator +(const ColorImage &c, const unsigned char a);
ColorImage operator +(const unsigned char a, const ColorImage &c); 
ColorImage operator +(const ColorImage &c1, const ColorImage &c2);
// substract (minus)
ColorImage operator -(const ColorImage &c, const unsigned char a);
ColorImage operator -(const unsigned char a, const ColorImage &c); 
ColorImage operator -(const ColorImage &c1, const ColorImage &c2);
// equal
int operator == (const ColorImage &c1, const ColorImage &c2);
// not equal
inline int operator != (const ColorImage &c1, const ColorImage &c2)
{ return !(c1 == c2);}
// logical operator
// and
ColorImage operator &(const ColorImage &c1, const ColorImage &c2);
ColorImage operator &(unsigned long a, const ColorImage &c2);
ColorImage operator &(const ColorImage &c1, unsigned long a);
// or
ColorImage operator |(const ColorImage &c1, const ColorImage &c2);
ColorImage operator |(unsigned long a, const ColorImage &c2);
ColorImage operator |(const ColorImage &c1, unsigned long a);
// xor
ColorImage operator ^(const ColorImage &c1, const ColorImage &c2);
ColorImage operator ^(unsigned long a, const ColorImage &c2);
ColorImage operator ^(const ColorImage &c1, unsigned long a);

class GrayImage: public ColorImage {
public:
	void drawRect(int x1, int y1, int x2, int y2, unsigned char gray = 255);
	enum ProjMode {X, Y, XY};
	enum Threshold {InBound, OutOfBound};

	// contructor and destructor
	GrayImage (int w, int h, unsigned char *d) 
										: ColorImage (w, h, 1, d, NULL, NULL) {}
	GrayImage (int w, int h, unsigned char d) 
										: ColorImage 	(w, h, 1, d, 0, 0) {} 
	GrayImage (const GrayImage & c) : ColorImage 	(c) {}
	GrayImage (char *filename) : ColorImage (filename) {}
	GrayImage (GrayImage *g) : ColorImage (g, NULL, NULL) {}
	GrayImage (int w=0, int h=0) : ColorImage (w, h, 1) {}

	// methods
	void copyFrom(GrayImage *ci);
	void copyFrom(int w=0, int h=0, unsigned char *r = NULL);
	void copyFrom(ColorImage *ci, int num = 0);
	unsigned char *getp(int x, int y) { return ColorImage::getp(x, y, 0);}
	unsigned char 	getPixel(int x, int y) { return *getp(x, y);}
	int 			getValue(int x, int y) 
											{ return (int)(*getp(x, y));}
	void 			setPixel(int x, int y, unsigned char v) 
										{unsigned char *p = buf[0]; p[x + y*width] = v;}
	unsigned char &	pel(int x, int y) { return *getp(x, y);}
	int saveraw(char *filename, int flag, int verbose);
	GrayImage *getSubImage(int sx, int sy, int ex, int ey) 	
	{return (GrayImage *)ColorImage::getSubImage(sx, sy, ex, ey);} 	

	BinaryImage *	threshold		(int min_value, int max_value = 255, 
									Threshold mode= InBound);
	void			thresholdSelf	(int min_value, int max_value = 255, 
									Threshold mode= InBound);

	int * proj(ProjMode mode = X,int x_1=0, int y_1=0, int x_2=-1, int y_2=-1);
	GrayImage *convolution	(int kernel_width, int kernel_height, int *kernel, 
							int div, int offset = 0);
	GrayImage *convolutionf	(int kernel_width, int kernel_height, float *kernel,
							float scale = 1.0, float offset = 0);
	GrayImage *convolutionf2	(int kernel_width, int kernel_height, float *kernel,
							float scale = 1.0, float offset = 0);
	double *point_convolution(int kernel_width, int kernel_height, double *kernel, CPoint *point, int point_num, double scale = 1.0, double offset = 0);
	double *convolutionf0	(int kernel_width, int kernel_height, double *kernel,
		double scale = 1.0, double offset = 0);


	// Simple assignment
	GrayImage&  operator = (const GrayImage &c);
	GrayImage&  operator = (const ColorImage &c);
	GrayImage&  operator = (const BinaryImage &c);
	GrayImage&  operator = (const unsigned char a); 
	// Unary plus
	GrayImage operator +() const;
	// Unary negation
	GrayImage operator -() const;
	// not logical operator
	GrayImage operator ~() const;

	// Destructive addition
	GrayImage &		operator +=(const GrayImage &c);

	// Destructive substraction
	GrayImage &		operator -=(const GrayImage &c);

	// For Binary Image
	// logical operator

	GrayImage *edgeDetect(int kernel_width, int kernel_height, int *kernel_h,
							int *kernel_v,  int div, int offset);
	GrayImage *edgeDetect(int kernel_width, int kernel_height, 
					float *kernel_h, float *kernel_v, int offset, int mode=0);
	BinaryImage *edgeMap(int kernel_width, int kernel_height, float *kernel_h, 
					float *kernel_v, int offset=0, int mode=1);
//	binOp(const GrayImage &g, int *table);
};

// operators
inline GrayImage GrayImage::operator +() const
{ return *this; }
// add (plus)
GrayImage operator +(const GrayImage &c, const unsigned char a);
GrayImage operator +(const unsigned char a, const GrayImage &c); 
GrayImage operator +(const GrayImage &c1, const GrayImage &c2);
// substract (minus)
GrayImage operator -(const GrayImage &c, const unsigned char a);
GrayImage operator -(const unsigned char a, const GrayImage &c); 
GrayImage operator -(const GrayImage &c1, const GrayImage &c2);
// equal
int operator == (const GrayImage &c1, const GrayImage &c2);
// not equal
inline int operator != (const GrayImage &c1, const GrayImage &c2)
{ return !(c1 == c2);}
// logical operator
// and
GrayImage operator &(const GrayImage &c1, const GrayImage &c2);
GrayImage operator &(unsigned long a, const GrayImage &c2);
GrayImage operator &(const GrayImage &c1, unsigned long a);
// or
GrayImage operator |(const GrayImage &c1, const GrayImage &c2);
GrayImage operator |(unsigned long a, const GrayImage &c2);
GrayImage operator |(const GrayImage &c1, unsigned long a);
// xor
GrayImage operator ^(const GrayImage &c1, const GrayImage &c2);
GrayImage operator ^(unsigned long a, const GrayImage &c2);
GrayImage operator ^(const GrayImage &c1, unsigned long a);

//
// Binary Image
//

#define WHITE	255
#define BLACK	0

class BinaryImage: public GrayImage {
public:

	enum ProjMode {X, Y, XY};
	// contructor and destructor
	BinaryImage (int w, int h, unsigned char *d) : GrayImage (w, h, d) {}
	BinaryImage (int w, int h, unsigned char d) : GrayImage (w, h, d)  {}
	BinaryImage (const BinaryImage & c) : GrayImage 	(c)  {}
	BinaryImage (char *filename) : GrayImage (filename)  {}
	BinaryImage (BinaryImage *g) : GrayImage (g)  {}
	BinaryImage (GrayImage *g) : GrayImage (g)  {}
	BinaryImage (int w=0, int h=0) : GrayImage (w, h)  {}

	// methods
	int IsWhite(int x, int y) 	{ return (getPixel(x, y) == WHITE) ? 1: 0; }
	int IsBlack(int x, int y) 	{ return (getPixel(x, y) == BLACK) ? 1: 0; }
	int isSet(int x, int y) 	{ return (getPixel(x, y) == WHITE) ? 1: 0; }
	int isUnset(int x, int y) 	{ return (getPixel(x, y) == BLACK) ? 1: 0; }
	int isReset(int x, int y) 	{ return isUnset(x, y);}
	void set(int x, int y) 		{ pel(x,y) = WHITE;}
	void unset(int x, int y) 	{ pel(x,y) = BLACK;}
	void reset(int x, int y) 	{ unset(x,y);}

	BinaryImage *getSubImage(int sx, int sy, int ex, int ey)
	{return (BinaryImage *)ColorImage::getSubImage(sx, sy, ex, ey);} 	
//	int setSubImage(const BinaryImage &bi, int sx, int sy)  	
//	{ return ColorImage::setSubImage((ColorImage *)&bi, sx, sy); }

	int * proj(ProjMode mode = X,int x_1=0, int y_1=0, int x_2=-1, int y_2=-1);
	BinaryImage *extractConnectedRegion(int x0, int y0, int *area = NULL, int verbose=0);

	void  print(FILE *fout = stdout, char *name = NULL);

	// Simple assignment
	BinaryImage&  operator = (const BinaryImage &c);
	BinaryImage&  operator = (const unsigned char a); 
	// Unary plus
	BinaryImage operator +() const;
	// Unary negation
	BinaryImage operator -() const;
	// not logical operator
	BinaryImage operator ~() const;

	// Destructive addition
	BinaryImage &		operator +=(const BinaryImage &c);

	// Destructive substraction
	BinaryImage &		operator -=(const BinaryImage &c);

	// For Binary Image
	// logical operator

//	binOp(const BinaryImage &g, int *table);
};

// operators
inline BinaryImage BinaryImage::operator +() const
{ return *this; }
// add (plus)
BinaryImage operator +(const BinaryImage &c, const unsigned char a);
BinaryImage operator +(const unsigned char a, const BinaryImage &c); 
BinaryImage operator +(const BinaryImage &c1, const BinaryImage &c2);
// substract (minus)
BinaryImage operator -(const BinaryImage &c, const unsigned char a);
BinaryImage operator -(const unsigned char a, const BinaryImage &c); 
BinaryImage operator -(const BinaryImage &c1, const BinaryImage &c2);
// equal
int operator == (const BinaryImage &c1, const BinaryImage &c2);
// not equal
inline int operator != (const BinaryImage &c1, const BinaryImage &c2)
{ return !(c1 == c2);}
// logical operator
// and
BinaryImage operator &(const BinaryImage &c1, const BinaryImage &c2);
BinaryImage operator &(unsigned long a, const BinaryImage &c2);
BinaryImage operator &(const BinaryImage &c1, unsigned long a);
// or
BinaryImage operator |(const BinaryImage &c1, const BinaryImage &c2);
BinaryImage operator |(unsigned long a, const BinaryImage &c2);
BinaryImage operator |(const BinaryImage &c1, unsigned long a);
// xor
BinaryImage operator ^(const BinaryImage &c1, const BinaryImage &c2);
BinaryImage operator ^(unsigned long a, const BinaryImage &c2);
BinaryImage operator ^(const BinaryImage &c1, unsigned long a);

//
// transforms between different color models
//
class ColorSpaceTransform {
public:
	enum ColorSys {RGB=1, CMY, HSV, HLS, YIQ, HSI, rgI, gbI, brI, rgb, I1I2I3, I1I2I3N, YCRCB, I1I2I3NN, STV, I1I2I3N3, rb, rg, ab};
private:
	ColorSys colorsys;
	void (*tofunc)(float, float, float, float &, float &, float &);
	void (*fromfunc)(float, float, float, float &, float &, float &);
	
public:
	void setcolorsys(ColorSys sys);
	ColorSys	 getcolorsys() {return colorsys;}
	void fromtransform(ColorImage &c);
	void totransform(ColorImage &c);

	void torgb(float r,float g,float b,float &tr,float &tg,float &tb)
				{ tofunc(r,g,b,tr,tg,tb); }
	void fromrgb(float r,float g,float b,float &tr,float &tg,float &tb)
				{ fromfunc(r,g,b,tr,tg,tb); }
	void rgbcomplement(float r,float g,float b,float &cr,float &cg,float &cb)
				{ float h, s, v;
					rgb_to_hsv(r,g,b,h,s,v);
					v = v+(float)0.5;
					if(v>1.0) v -= 1.0;
					hsv_to_rgb(h,s,v,cr,cg,cb);
				}

	void irgb_to_rgb(int ir,int ig,int ib,float &r,float &g,float &b)	
					{ r = (float)(ir/255.0); g = (float)(ig/255.0); b = (float)(ib/255.0); }

	// transform 
	// Convert FROM rgb TO rgb
static	void rgb_to_rgb(float fr,float fg,float fb,float &tr,float &tg,float &tb)
					{ tr = fr; tg = fg; tb = fb; }

	// float to float
static	float value(float n1, float n2, float hue);
static	void cmy_to_rgb(float c,float m,float y,float &r,float &g,float &b)
					{ r = (float)1.0-c; g = (float)1.0-m; b = (float)1.0-y; }
static	void hsi_to_rgb(float h,float s,float i,float &r,float &g,float &b);
static	void hls_to_rgb(float h,float l,float s,float &r,float &g,float &b);
static	void hsv_to_rgb(float h,float s,float v,float &r,float &g,float &b);
static	void hsb_to_rgb(float h,float s,float v,float &r,float &g,float &b)
					{ hsv_to_rgb(h,s,v,r,g,b); }
static	void yiq_to_rgb(float y,float i,float q,float &r,float &g,float &b);
static	void rgI_to_rgb(float nr,float ng,float I,float &r,float &g,float &b)
					{ r = (float)3.0 * nr * I; g = (float)3.0 * ng * I; b = (float)3.0 * (1 - ng - nr ) * I;}
static	void gbI_to_rgb(float ng,float nb,float I,float &r,float &g,float &b)
					{ r = (float)(3.0 * (1.0 - ng  - nb) * I); g = (float)3.0 * ng * I; b = (float)3.0 * nb * I;}
static	void brI_to_rgb(float nb,float nr,float I,float &r,float &g,float &b)
					{ r = (float)(3.0 * nr * I); g = (float)(3.0 * (1.0 - nb - nr) * I); b = (float)(3.0 * nb * I);}
static	void nrgb_to_rgb(float nr,float ng,float nb,float &r,float &g,float &b)
					{ r = nr ; g = ng; b = nb;}
static	void irg_to_rgb(float nr,float ng,float nb,float &r,float &g,float &b)
					{ r = nr ; g = ng; b = nb;}

	//  Convert TO various formats FROM rgb.
static	void rgb_to_cmy(float r,float g,float b,float &c,float &m,float &y)
					{ c = (float)1.0-r; m = (float)1.0-g; y = (float)1.0-b; }
static	void rgb_to_irgb(float r,float g,float b, int &ir,int &ig,int &ib)
			{ ir = (int)((r*255.0)+0.5); ig = (int)((g*255.0)+0.5); ib = (int)((b*255.0)+0.5); }
static	void rgb_to_irg(float r,float g,float b, float &ii,float &ir,float &ig);
static	void rgb_to_hsi(float r,float g,float b,float &h,float &s,float &i);
static	void rgb_to_hls(float r,float g,float b,float &h,float &l,float &s);
static	void rgb_to_hsv(float r,float g,float b,float &lh,float &ls,float &lv);
static	void rgb_to_hsb(float r,float g,float b,float &lh,float &ls,float &lv)
					{ rgb_to_hsv(r,g,b,lh,ls,lv); }
static	void rgb_to_yiq(float r,float g,float b,float &y,float &i,float &q)
					{ 	y = (float)(0.3*r+0.59*g+0.11*b);
						i = (float)(0.6*r-0.28*g-0.32*b);
						q = (float)(0.21*r-0.52*g+0.31*b);
					}
static	void rgb_to_rgI(float r,float g,float b,float &nr,float &ng,float &I)
					{ 	I = (r+g+b);
						if (I <= (float)0.0001) { nr = ng = (float)(1.0/3.0); }
						else { nr = r/I; ng = g/I;}
						I /= (float)3.0;
					}
static	void rgb_to_gbI(float r,float g,float b,float &ng,float &nb,float &I)
					{ 	I = (r+g+b);
						if (I <= (float)0.0001) { ng = nb = (float)(1.0/3.0); }
						else { ng = g/I; nb = b/I;}
						I /= (float)3.0;
					}
static	void rgb_to_brI(float r,float g,float b,float &nb,float &nr,float &I)
					{ 	I = (r+g+b);
						if (I <= (float)0.0001) { nb = nr = (float)(1.0/3.0); }
						else { nb = b/I; nr = r/I;}
						I /= (float)3.0;
					}
static	void rgb_to_nrgb(float r,float g,float b,float &nr,float &ng,float &nb)
					{ 	float I = (r+g+b);
						if (I < (float)(1.0e-5)) { nb = nr = ng = (float)(1.0/3.0);}
						else { ng = g/I; nr = r/I; nb = (float)1.0 - nr - ng;}
					}
static void rgb_to_i1i2i3(float r,float g,float b, float &i1,float &i2,float &i3)
					{
						i1 = (float)((r+g+b)/3.);
						i2 = (float)((r-b)/2.);
						i3 = (float)((2.*g-r-b)/4.);
					}
static void rgb_to_i1i2i3n(float r,float g,float b, float &i1,float &i2,float &i3)
					{
						i1 = (float)((r+g+b)/3.);
						//i2 = (float)(r-b);
						i2 = (float)((r-b)/2.+0.5);
						//i3 = (float)((2.*g-r-b)/2.);
						i3 = (float)((2.*g-r-b)/4.+0.5);
					}

static void rgb_to_i1i2i3nn(float r,float g,float b, float &i1,float &i2,float &i3)
					{
						// org. values
						//i1 = (float)(0.3655*r+0.3091*g+0.3253*b);
						//i2 = (float)(-0.4432*r-0.0303*g+0.5265*b);
						//i3 = (float)(-0.2718*r+0.5299*g-0.1983*b);
						// mod. values
						i1 = (float)(.322*r+.315*g+.363*b);
						i2 = (float)(-.398*r-.133*g+.469*b+.531);
						i3 = (float)(-.341*r+.516*g-.143*b+.484);
					}

static void rgb_to_i1i2i3n3(float r,float g,float b, float &i1,float &i2,float &i3)
					{
						// org. values
						//i1 = (float)(0.3655*r+0.3091*g+0.3253*b);
						//i2 = (float)(-0.4432*r-0.0303*g+0.5265*b);
						//i3 = (float)(-0.2718*r+0.5299*g-0.1983*b);
						// mod. values
						i1 = (float)(.316*r+.351*g+.333*b);
						i2 = (float)(-.515*r+.068*g+.417*b+.515);
						i3 = (float)(.197*r-.482*g+.321*b+.482);
					}

static void rgb_to_stv(float r,float g,float b, float &i1,float &i2,float &i3)
					{
						i1 = (float)sqrt(9./5.*((r-1./3.)*(r-1./3.)+(g-1./3.)*(g-1./3.)));
						i2 = (float)(atan((r-1./3.)/(g-1./3.))/M_PI + 0.5);
						i3 = (float)((r+g+b)/3.);
					}

/*
Y = 0.299 R + 0.587 G + 0.114 B
Cr = ((B-Y)/2) + 0.5
Cb = ((R-Y)/1.6) + 0.5
*/
static void rgb_to_YCrCb(float r,float g,float b, float &y,float &cr,float &cb)
					{
						y = (float)(0.299*r+0.587*g+0.114*b);
						cr = (float)((b-y)/2.+0.5);
						cb = (float)((r-y)/1.6+0.5);
					}

static void rgb_to_rb(float r,float g,float b, float &rbr,float &rbg,float &rbb)
					{
						rbr = __min((float)(r/(g+0.001f)), 1.0f);
						rbg = (float)(0.0f);
						rbb = __min((float)(b/(g+0.001f)), 1.0f);
					}

static void rgb_to_rg(float r,float g,float b, float &rgr,float &rgg,float &rgb)
					{
						rgr = __min((float)(r/(b+0.001f)), 1.0f);
						rgg = __min((float)(g/(b+0.001f)), 1.0f);
						rgb = (float)(0.0f);
					}
/*
static void rgb_to_ab(float r,float g,float b, float &abr,float &abg,float &abb)
					{
						abr = (float)(0.299*r+0.587*g+0.114*b);
						abg = (float)((b-y)/2.+0.5);
						abb = (float)((r-y)/1.6+0.5);
					}

*/

	char *getName();
	char *getTransformName();

	// Constructor and Destructor
	ColorSpaceTransform(ColorSys sys=RGB){setcolorsys(sys);}
	~ColorSpaceTransform(){}
};

class ImageLabeling {
public:
	enum Threshold {InBound, OutOfBound};
private:
	int N;
	int	*map;
	CRect *rgns;
	int *areas;
	int	*rsttbl;
	GrayImage *bimg;
	GrayImage *labeledimg;
	int x0, y0, W, H; 	// region of interest
public:
	enum ProjMode {X, Y};
	enum SelectionMode {Area};

	ImageLabeling(GrayImage *b = NULL) { bimg = b; map = NULL; areas = NULL;
rsttbl = NULL; rgns = NULL; labeledimg = NULL;}
	~ImageLabeling();
		
	// methods
	int getLength() {return N;}
	void setImage(GrayImage *b) { bimg = b;}
	GrayImage *labeling(GrayImage *b = NULL, int verbose=0);
	int *getAreas()				{return areas;}
	float getArea(int nth = 0);
	int calArea();
	void getRegion(int nth, int &x1, int &y1, int &x2, int &y2);

	BinaryImage *select(int min_thresh=0,int max_thresh=255, SelectionMode mode=Area, Threshold tmode=InBound);
};

class ProfileData{
public:
	enum PulseType {Negative, Positive};
private:
	int	*X;
	int N;
public:
	ProfileData(int n, int *x) 	{ X = NULL; N= -1; set(n, x);}
	~ProfileData()				{X=NULL;}//delete X;}

	// operator
	int&	operator [] (int i)	{return X[i];}

	// methods
	void	set(int n, int *x) 	{if (X) delete []X; N = n; X = x;}
	int 	getLength()			{return N;}
	int&	get(int n) 			{return X[n];}
	int 	getValue(int value, int *ns);	
	int 	getMax(int *ns=NULL, int *num=NULL);
	int 	getMin(int *ns=NULL, int *num=NULL);	
	int 	getMaxMin(int *max_ns, int &max_num, int *min_ns, int &min_num);	
	int 	getPercent(int *ns = NULL, float percent = 100.0, float tol = 1.0e-5);	
	int 	getPulse(int *ns = NULL, float percent = 100.0, 
					PulseType p_pulse=Positive, float tol = 1.0e-5, 
					int verbose = 0);
	int 	selectPulse(int *outi, int *ini, int num, int min_width=-1, int max_width=-1);
	int 	save(char *filename, int verbose = 0);
	ProfileData *medianFilter(int size = 3);
	int 	findLocalMaxMin(int *maxpt, int &maxnum, int *minpt, int &minnum, 
							int tolHeight=0,
							int dx1Thresh=0, int dx2Thresh=0, int dx3Thresh=0, 
							int verbose =0);
	int 	findLocalMaxMinAlternate(
							int *maxpt, int &maxnum, int *minpt, int &minnum, 
							int tolHeight=0,
							int dx1Thresh=0, int dx2Thresh=0, int dx3Thresh=0, 
							int verbose =0);
	int 	removeFromMaxMinAlternate(
							int *maxpt, int &maxnum, int *minpt, int &minnum, 
							int tolHeight=0, int MinList=0);
	void 	printLocalMaxMin(int *maxpt, int maxnum, int *minpt, int minnum, 
							int offset=0, FILE *fout=stdout);
	int findEnclInterval(int *x1, int *x2, int *ptlist, int num, int pt);
};

//
// Gradient 
//
class GradImage {
public:
	enum Type {Sobel, FiniteDifference};
private:
	GrayImage *magImage;
	GrayImage *angImage;
	Type t;
	static double getEdgeness_2(GrayImage &I, int x, int y);
public:
	GradImage(Type tt = Sobel, GrayImage *mImg = NULL, GrayImage *aImg = NULL);
	~GradImage();

	GrayImage *getMagImage(){return magImage;}
	GrayImage *getAngImage(){return angImage;}
	GrayImage *getVImage(double scaling = 1.0, int offset = 0);
	GrayImage *getHImage(double scaling = 1.0, int offset = 0);
	GrayImage *perform(GrayImage *inputImage, double scalingMag = 1.0);
	void performAtPel(GrayImage *inputImage, int x, int y, 
						double *Gx, double *Gy);
	// Dominance Edge Map
	BinaryImage *getYGradDomMap(
		GrayImage *iImg=NULL, int thresh=0,
		GrayImage *mImg=NULL, GrayImage *aImg=NULL, int mthresh=0, int verbose = 0);
	BinaryImage *getXGradDomMap(
		GrayImage *iImg=NULL, int thresh=0,
		GrayImage *mImg=NULL, GrayImage *aImg=NULL, int mthresh=0, int verbose = 0); 

	BinaryImage *getHEdgeDomMap(
		GrayImage *iImg=NULL, int thresh=0,
		GrayImage *mImg=NULL, GrayImage *aImg=NULL, int mthresh=0, int verbose = 0)
	{ return getYGradDomMap( iImg, thresh, mImg, aImg, mthresh, verbose ); }
	BinaryImage *getVEdgeDomMap(
		GrayImage *iImg=NULL, int thresh=0,
		GrayImage *mImg=NULL, GrayImage *aImg=NULL, int mthresh=0, int verbose = 0) 
	{ return  getXGradDomMap(iImg, thresh, mImg, aImg, mthresh, verbose); }

	void setType(Type tt) {t = tt;}
	Type getType() {return t;}
};

// functions
extern GrayImage *color2gray(ColorImage &C, 
							unsigned char *c1, unsigned char *c2, double N,
							int verbose = 0);
extern void printMemoryInfo();

// external variables
extern int debug;
#endif

//
// lft.h
// programmed by Gyu-tae Park
// modified by DJ
// 2000.3.21
//
#ifndef _LFT_
#define	_LFT_
#include "Complex.h"
#include "image.h"

class LFT {
public:
	enum StackingMode {NotDefined, X, Y, Horizontal, Vertical};
private:
	int	N;
	CmyImage *img;
	Complex *zin, *zout;
	Complex *setImage(CmyImage *g, StackingMode mode = X);
	StackingMode smode;
public:
	Complex* fft(Complex *z1, Complex *z2, int n, int inzee);
	void fftstp(Complex* zin, int after, int now, int before, Complex* zout);
	void 	set(Complex *z1, int length);
	int	getLength() {return N;}
	Complex get(int i){return zout[i];}
	Complex *gets(){return zout;}
	Complex *getInput()	{return zin;}
	double *getMag();
	double *getAngle();
	double *getImag();
	double *getReal();
	CmyImage *getImage()	{return img;}
	Complex *normalize(Complex *z, int n);
	Complex *transform(CmyImage *g=NULL, StackingMode mode = NotDefined);
	Complex *xtransform(CmyImage *gimg = NULL);
	Complex *ytransform(CmyImage *gimg = NULL);

	int htUnitLength() { return (img->GetHeight()); }
	int gsUnitLength() { return (img->GetWidth()); }
	int GpUnitLength() { return (img->GetHeight()); }
	int HqUnitLength() { return (img->GetWidth()); }
	int UnitLength()   { return ((smode == X) ? GpUnitLength(): 
						    HqUnitLength());}
	int OneHz()	   {return UnitLength();}
	int 	getFeaturesLength();
	double	*getFeatures();
	void	printMag(FILE *out = stdout, int in = 0, int single = 1, int from = -1, int to=-1, int Hzflg = 1);
	int printMag(char *filename=NULL, int in=0, int single=1, int from=-1, int to=-1, int Hzflg = 1);
	void	print(FILE *out = stdout, int in = 0);
	void	graph(int in = 0, int single = 1, char *titlename=NULL);
	void	show();

	// operator
	Complex & operator[](int i) {return zout[i];}

	LFT(CmyImage *g=NULL, StackingMode mode = NotDefined);
	~LFT();
};
#endif
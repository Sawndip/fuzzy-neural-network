#include <stdio.h>
#include <float.h>
#include <math.h>

double fclips (double x) 
{
	return (x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 :x));
}

double tri(double x, double a, double b, double = 0.0, double = 0.0)
{
	double  v = (fabs(b) < DBL_MIN)	?  1.0 : (1.0 - fabs((a - x)/b));
	
	return fclips(v);
}

double IntegralTri(double from, double to, double a, double b)
{
	double sgn = 1.0;
	double v = 0.0;
	if (from > to) {double tmp = from; from=to; to=tmp; sgn = -1.0;}
	if (from < 0.0) from = 0;
	if (to > 1.0) to = 1.0;
//	if (a < 0) a = -a;
	if (b < 0) b = -b;

	double S1 =	0.5 *b;
	if (to < a - b)	 return 0.0;
	if (from > a + b)	 return 0.0;
	if (from < a - b) {
		if (to < a) v = ((to-a+b)*(1.0+(to-a)/b)*0.5);
		else if (to < a + b) v = (S1 + 0.5*(to-a)*(2.0-(to-a)/b));
		else v = b;
	} else if (from < a) {
		if (to < a) v = (0.5*(to-from)*(2.0+(to+from-2.0*a)/b));
		else if (to < a + b) v = (0.5*(a-from)*(2.0+(from-a)/b) 
									+ 0.5*(to-a)*(2.0-(to-a)/b));
		else v = (0.5*(a-from)*(2.0+(from-a)/b) + S1); 
	} else if (from < a + b) {
		if (to < a + b) v = (0.5*(to-from)*(2.0-(to+from-2.0*a)/b));
		else v = (0.5*(a+b-from)*(2.0-(b+from-a)/b));
	} else v = 0.0;

	return _copysign(v, sgn);
}

double IntegrateFunc(double from, double to, double (*f)(double, double, double, double, double), int N, double p1, double p2, double p3, double p4)
{
	if (from < 0.0) from = 0.0;
	if (to > 1.0) to = 1.0;
	double dx = (to-from)/N;
	double xi = from;
	double s = 0.0;
	for (int i = 0; i < N; i++, xi+= dx) 
		s += (*f)(xi, p1, p2, p3, p4);

	return (s * dx);
}

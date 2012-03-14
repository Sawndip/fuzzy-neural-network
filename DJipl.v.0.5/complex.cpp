#include "stdafx.h"
#include "complex.h"
#include <stdio.h>

Complex   operator /  (const Complex& x, const Complex& y)
{                                   
	return x*conj(y)/(y.real()*y.real() + y.imag()*y.imag());
}

Complex   operator /  (const Complex& x, double y)
{
	return Complex(x.real()/y,x.imag()/y);
}

Complex   operator /  (double   x, const Complex& y)
{
	return x*conj(y)/(y*conj(y));
}

Complex   cos(const Complex& x)
{
	return 0.5*( exp((Complex(0,1)*x)) + exp((Complex(0,-1)*x)) );
}

Complex   sin(const Complex& x)
{
	return (0.5/Complex(0,1))*( exp((Complex(0,1)*x)) - exp((Complex(0,-1)*x)) );
}

Complex   cosh(const Complex& x)
{
	return 0.5*( exp(x) + exp(-x) );
}

Complex   sinh(const Complex& x)
{
	return 0.5*( exp(x) - exp(-x) );
}

Complex   exp(const Complex& x)
{
	return exp(x.real())*(cos(x.imag()) + Complex(0,1)*sin(x.imag()));
}

Complex   log(const Complex& x)
{
	double r = abs(x);
	double t = arg(x);
	return log(r) + Complex(0,1)*t;
}

Complex   pow(const Complex& x, int p)
{
	int i;
	Complex out;
	out = Complex(1,0);

	for(i=0;i<p;i++)
	{
		out *= x;
	}
	return out;
}


Complex   pow(const Complex& x, double y)
{
	double r = abs(x);
	double t = arg(x);
	double n = 1./y;
	return pow(r,n)*( cos(t/n) + Complex(0,1)*sin(t/n) );
}

Complex   sqrt(const Complex& x)
{
	double r = abs(x);
	double t = arg(x);
	return sqrt(r)*( cos(t/2) + Complex(0,1)*sin(t/2) );
}
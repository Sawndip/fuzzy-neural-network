//
//  Complex number Class
//
//	This program is for "ANSI C++"  include Visual C++, Turbo C, Boland C, and etc
//
//
//  1996. 7. 29.
//
//  Optoelectronics Lab.   Dept. of EMDE      Inha Univ
//
//  Programmed by  Kyung il Lee
//
//
#ifndef _Complex_h

#define _Complex_h 1

#include "stdafx.h"
#include <iostream.h>
#include <math.h>

class Complex
{
protected:

  double           re;
  double           im;

public:

  double           real() const;
  double           imag() const;

                   Complex();
                   Complex(const Complex& y);
                   Complex(double r, double i=0);

                  ~Complex();

  Complex&         operator =  (const Complex& y);

  Complex&         operator += (const Complex& y);
  Complex&         operator += (double y);
  Complex&         operator -= (const Complex& y);
  Complex&         operator -= (double y);
  Complex&         operator *= (const Complex& y);
  Complex&         operator *= (double y);

  Complex&         operator /= (const Complex& y); 
  Complex&         operator /= (double y); 

  void             error(const char* msg) const;
};


// non-inline functions

Complex   operator /  (const Complex& x, const Complex& y);
Complex   operator /  (const Complex& x, double y);
Complex   operator /  (double   x, const Complex& y);

Complex   cos(const Complex& x);
Complex   sin(const Complex& x);

Complex   cosh(const Complex& x);
Complex   sinh(const Complex& x);

Complex   exp(const Complex& x);
Complex   log(const Complex& x);

Complex   pow(const Complex& x, int p);
Complex   pow(const Complex& x, double y);
Complex   sqrt(const Complex& x);
   

// inline members

inline double  Complex::real() const { return re; }
inline double  Complex::imag() const { return im; }

inline Complex::Complex() { re = 0.0; im = 0.0; }
inline Complex::Complex(const Complex& y) :re(y.real()), im(y.imag()) {}
inline Complex::Complex(double r, double i) :re(r), im(i) {}

inline Complex::~Complex() {}

inline Complex&  Complex::operator =  (const Complex& y) 
{ 
	re = y.real(); im = y.imag(); return *this; 
} 

inline Complex&  Complex::operator += (const Complex& y)
{ 
  re += y.real();  im += y.imag(); return *this; 
}

inline Complex&  Complex::operator += (double y)
{ 
  re += y; return *this; 
}

inline Complex&  Complex::operator -= (const Complex& y)
{ 
  re -= y.real();  im -= y.imag(); return *this; 
}

inline Complex&  Complex::operator -= (double y)
{ 
  re -= y; return *this; 
}

inline Complex&  Complex::operator *= (const Complex& y)
{  
  double r = re * y.real() - im * y.imag();
  im = re * y.imag() + im * y.real(); 
  re = r; 
  return *this; 
}

inline Complex&  Complex::operator *= (double y)
{  
  re *=  y; im *=  y; return *this; 
}


//  functions

inline int  operator == (const Complex& x, const Complex& y)
{
  return x.real() == y.real() && x.imag() == y.imag();
}

inline int  operator == (const Complex& x, double y)
{
  return x.imag() == 0.0 && x.real() == y;
}

inline int  operator != (const Complex& x, const Complex& y)
{
  return x.real() != y.real() || x.imag() != y.imag();
}

inline int  operator != (const Complex& x, double y)
{
  return x.imag() != 0.0 || x.real() != y;
}

inline Complex  operator - (const Complex& x)
{
  return Complex(-x.real(), -x.imag());
}

inline Complex  conj(const Complex& x)
{
  return Complex(x.real(), -x.imag());
}

inline Complex  operator + (const Complex& x, const Complex& y)
{
  return Complex(x.real() + y.real(), x.imag() + y.imag());
}

inline Complex  operator + (const Complex& x, double y)
{
  return Complex(x.real() + y, x.imag());
}

inline Complex  operator + (double x, const Complex& y)
{
  return Complex(x + y.real(), y.imag());
}

inline Complex  operator - (const Complex& x, const Complex& y)
{
  return Complex(x.real() - y.real(), x.imag() - y.imag());
}

inline Complex  operator - (const Complex& x, double y)
{
  return Complex(x.real() - y, x.imag());
}

inline Complex  operator - (double x, const Complex& y)
{
  return Complex(x - y.real(), -y.imag());
}

inline Complex  operator * (const Complex& x, const Complex& y)
{
  return Complex(x.real() * y.real() - x.imag() * y.imag(), 
                 x.real() * y.imag() + x.imag() * y.real());
}

inline Complex  operator * (const Complex& x, double y)
{
  return Complex(x.real() * y, x.imag() * y);
}

inline Complex  operator * (double x, const Complex& y)
{
  return Complex(x * y.real(), x * y.imag());
}

inline double  real(const Complex& x)
{
  return x.real();
}

inline double  imag(const Complex& x)
{
  return x.imag();
}

inline double  abs(const Complex& x)
{
  return hypot(x.real(), x.imag());
}

inline double  norm(const Complex& x)
{
  return (x.real() * x.real() + x.imag() * x.imag());
}

inline double  arg(const Complex& x)
{
  return atan2(x.imag(), x.real());
}

inline Complex  polar(double r, double t)
{
  return Complex(r * cos(t), r * sin(t));
}

#endif
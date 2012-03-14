# g++ -I../include -L../lib -O -o tMatrix tMatrix.C -lMatrix -lg++     -lm
# CC  -I../include -L../lib -O -o tMatrix tMatrix.C -lMatrix -lcomplex -lm
#include <complex.Matrix.h>
#define matrix complexMatrix

int main() {
  matrix x(2, 2, complex(1.0, sqrt(3.0)));
  cout	<<"     x "<< "\n"
  	<<      x  << "\n"
  	<<"sqrt(x)"<< "\n"
  	<< sqrt(x) << "\n"
  	<<"conj(x)"<< "\n"
  	<< conj(x) << "\n"
  	<<" abs(x)"<< "\n"
  	<<  abs(x) << "\n"
 	<<"norm(x)"<< "\n"
 	<< norm(x) << "\n"
	<<" arg(x)"<< "\n"
	<<  arg(x) << "\n"
	<<"real(x)"<< "\n"
	<< real(x) << "\n"
	<<"imag(x)"<< "\n"
	<< imag(x) << "\n"
	;
  }


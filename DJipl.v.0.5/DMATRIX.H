#ifndef MATRIX_H
#define MATRIX_H

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef __IOSTREAM_H
#include <iostream.h>
#endif

#ifndef __MATH_H
#include <math.h>
#endif

#ifndef eps
#define eps 1e-14
#endif

#ifndef tol
#define tol 1e-7
#endif

#ifdef NO_BOOLEAN
enum bool {false=0,true=1};
#endif

#  define max(a,b)    (((a) > (b)) ? (a) : (b))
#  define min(a,b)    (((a) < (b)) ? (a) : (b))

// change "double" to "float" or "long double" if you wish
typedef double T;

class dmatrix;
typedef dmatrix Matrix;

class dvector;
typedef dvector Vector;

class dpoly;
typedef dpoly Polynm;

//////////////////// matrix class ///////////////////////
class dmatrix
{
private:
  T *Val;
  size_t Row, Col;
  T *SetupMat(size_t r, size_t c);
  void PurgeMat(void);

public :
  // Value extraction method
  size_t Rsize () const { return Row; }
  size_t Csize () const { return Col; }

  // indexing
  T& operator() (size_t i, size_t j) const;
  T& operator() (size_t i) const;

  // constructors and destructor
  dmatrix();
  dmatrix(size_t r);
  dmatrix(size_t rr, size_t cc);
  dmatrix(const dmatrix& m);
  dmatrix(size_t r, size_t c, T x);
  dmatrix(size_t r, size_t c, T *x);
  dmatrix& operator = (const dmatrix& m);
  dmatrix& operator = (T *x);
  ~dmatrix();
  dmatrix& ReSize(const size_t r, const size_t c);

  // io
  friend istream& operator >> (istream& s, const dmatrix& m);
  friend ostream& operator << (ostream& s, const dmatrix& m);

  // addition
  friend dmatrix operator + (const dmatrix& a, const dmatrix& b);
  friend dmatrix operator + (const dmatrix& a, T b);
  friend dmatrix operator + (T b, const dmatrix& a);
  dmatrix& operator += (const dmatrix& a);
  dmatrix& operator += (T b);

  // subtraction
  friend dmatrix operator - (const dmatrix& a, const dmatrix& b);
  friend dmatrix operator - (const dmatrix& a, T b);
  friend dmatrix operator - (T b, const dmatrix& a);
  dmatrix& operator -= (const dmatrix& a );
  dmatrix& operator -= (T b);
  friend dmatrix operator - (const dmatrix& a); // unary minus

  // multiplication
  friend dmatrix operator * (const dmatrix& a, const dmatrix& b);
  friend dmatrix operator * (const dmatrix& a, T b);
  friend dmatrix operator * (T b, const dmatrix& a);
  dmatrix& operator *= (const dmatrix& a );
  dmatrix& operator *= (T b);
  dmatrix& operator ^= (const size_t& pow) ;
  friend dmatrix operator ^ (const dmatrix& m, const size_t& pow);

  // division
  friend dmatrix operator / (const dmatrix& a, const dmatrix& b);
  friend dmatrix operator / (const dmatrix& a, T b);
  friend dmatrix operator / (T b, const dmatrix& a);
  dmatrix& operator /= (T b);

  // Identity and Null matrices
  friend dmatrix Ident(const size_t r);
  friend dmatrix Null(const size_t r);
  friend dmatrix Null(const size_t r, const size_t c);

  // Type of matrices
  bool IsSquare () const { return (Row == Col); }
  bool IsSingular () const;
  bool IsDiagonal () const;
  bool IsNull () const;
  bool IsSymmetric () const;
  bool IsSkewSymmetric () const;
  bool IsUpperTriangular () const;
  bool IsLowerTriangular () const;

  // swap rows of a matrix
  friend void SwapRow(const dmatrix& m, int row1, int row2);

  // submatrix
  friend dmatrix Submat(const dmatrix& m,int rl,int ru,int cl,int cu);
  friend dmatrix Mrow(const dmatrix& m,size_t r);
  friend dmatrix Mcol(const dmatrix& m,size_t r);

  // combine two matrices
  friend dmatrix JoinRow(const dmatrix& m1, const dmatrix& m2);
  friend dmatrix JoinCol(const dmatrix& m1, const dmatrix& m2);

  // miscellaneous operation
  T Cofact (size_t row, size_t col) const;// cofactor
  int pivot (size_t r) const;             // pivot
  friend dmatrix Tran (const dmatrix& m); // transpose
  friend dmatrix Inv (const dmatrix& m);  // inverse
  friend dmatrix Adj(const dmatrix& m);   // adjoint
  friend dmatrix Conj(const dmatrix& m);  // conjugate
  friend dmatrix Pinv(const dmatrix& m);  // psuedo-inverse
  friend T Det (const dmatrix& m);        // determinant
  friend T Norm (const dmatrix& m);       // norm
  friend T Cond (const dmatrix& m);       // condition number
  friend int Rank(const dmatrix& m);      // rank
  friend dmatrix Diag(const dmatrix& m);  // diagnal elements
};

//// Simultaneous equations solver
dmatrix Solve (const dmatrix& m, const dmatrix& v);

//// LU decomposition (Doolittle's algorithm)
void lu(const dmatrix& m, dmatrix& L, dmatrix& U);

//// LU decomposition (Crout's algorithm)
void lud(const dmatrix& m, dmatrix& L, dmatrix& U);

//// Choleskey factorization
dmatrix chol(const dmatrix& m);

//// Givens transformation matrix
dmatrix givens(const dmatrix& v, int j, int k);

//// Householde transformation matrix
dmatrix house(const dmatrix& v, int j, int k);

//// QR factorization
void qr(const dmatrix& m, dmatrix& Q, dmatrix& R);

//// Hessenberg form
void hess(const dmatrix& m, dmatrix& H, dmatrix& P);

//// SVD decomposition
int svd(const dmatrix& A, dmatrix& U, dmatrix& S, dmatrix& V);

//// eigenvalues and eigenvectors
void eig(const dmatrix& a, dmatrix& wr, dmatrix& wi,
				dmatrix& vr, dmatrix& vi);
void eig(const dmatrix& a, dmatrix& wr, dmatrix& wi);


////////////// vector class ////////////////////
class dvector : public dmatrix
{
private:
  size_t size;
public:
  // constructors
  dvector(size_t r) : dmatrix(r,1), size(r) { }
  dvector() : dmatrix(4,1), size(4) { }
  dvector(const dvector& a) : dmatrix(a), size(a.Len()) { }
  dvector(const dmatrix& a) : dmatrix(a), size(a.Rsize()) { }
  dvector& operator = (T *x);
  ~dvector() { }

  // dot product
  friend T Vdot( const dvector& a, const dvector& b);

  // norm
  friend double Vnorm( const dvector& a);
  friend double Vnorminf( const dvector& a);
  friend double Vnorm1( const dvector& a);

  /// miscellaneous
  size_t Len() const {return size; }
  friend dvector Subvec(const dvector& v, int j, int k);
  friend dmatrix Vmul(const dvector& a, const dvector& b);
};


////////////////// poly class ///////////////////////
// [an ... a1 a0] for polynomial an*s^n+...+a1*s+a0
class dpoly : public dmatrix
{
private:
  size_t deg;       // highest degree
public:
  // constructors
  dpoly(size_t r) : dmatrix(1,r+1), deg(r) { }
  dpoly() : dmatrix(1,5), deg(4) { }
  dpoly(const dpoly& a) : dmatrix(a), deg(a.Deg()) { }
  dpoly& operator = (const dpoly& a);
  dpoly& operator = (T *x);
  ~dpoly() { }

  // indexing
  T& operator() (size_t i) const;

  // convolution and power
  friend dpoly operator * (const dpoly& a, const dpoly& b);
  dpoly& operator *= (const dpoly& b);
  friend dpoly operator ^ (const dpoly& a, const size_t& pow);
  dpoly& operator ^= (const size_t& pow);

  // addition and substraction
  friend dpoly operator + (const dpoly& a, const dpoly& b);
  friend dpoly operator - (const dpoly& a, const dpoly& b);
  dpoly& operator += (const dpoly& b);
  dpoly& operator -= (const dpoly& b);

  /// miscellaneous
  size_t Deg() const {return deg;}
};

///// polynomial whose roots are given by a vector
dpoly Char_poly(const dmatrix& wr,const dmatrix& wi);

///// character polynomial for a matrix
dpoly Char_poly(const dmatrix& a);

///// roots of a polynomial
void Roots(const dpoly& v,dmatrix& wr, dmatrix& wi);

#endif

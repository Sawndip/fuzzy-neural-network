#if !defined(AFX_ALGEBRA_H__ECC965F5_235E_11D2_912F_00104B6A0945__INCLUDED_)
#define AFX_ALGEBRA_H__ECC965F5_235E_11D2_912F_00104B6A0945__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Algebra.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlgebra document

class CAlgebra : public CDocument
{
protected:
	CAlgebra();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CAlgebra)

// Attributes
public:

// Operations
public:

	static float*  vect_add(float* a, float* b, int);
	static double* vect_add_double(double* a, double* b, int k);
	static float   vect_inner(float* a, float* b, int);
	static float*  vect_cross(float* a, float* b );
	static float**  vect_cross2(float* a, float* b, int n );
	static float*  vect_mat_mul(float * a, float** b, int bx, int by);
	static float** mat_add(float** a, float** b, int nx, int ny);
	static float** mat_sub(float** a, float** b, int nx, int ny);
	static float** mat_multiply(float** a, int, int, float**, int, int);
	static double** mat_multiply_double(double** a, int, int, double**, int, int);
	static float*  mat_vect_mul(float** a, int ax, int ay, float* b);
	static double* mat_vect_mul_double(double** a, int ax, int ay, double* b);
	static float** mat_transpose(float** a, int, int);
	static double** mat_transpose_double(double** a, int, int);
	static float** mat_inv(float **a, int n, float *det_val);
	static float** mat_new_forRecipe(int nx, int ny);
	static float** mat_copy_fromRecipe(float **in, int nx, int ny);
	static float*  vec_new_forRecipe(int n);
	static double* vec_new_forRecipeDouble(int n);
	static float*  vec_copy_fromRecipe(float *in, int n);

	static void ludcmp(float **a, int n, int *indx, float *d);
    static void lubksb(float **a, int n, int *indx, float b[]);
	static float** mat_new_float(int nx, int ny);
	static double** mat_new_double(int nx, int ny);
	static void jacobi(float **a, int n, float d[], float **v, int *nrot);
	static void jacobi1(double **A, int N, double EIGEN[], double **T);
	static void jacobi2(float *a, int n, float *d, float *v, int *nrot);

	static void eigsrt(float d[], float **v, int n);
	static void eigsrt_double(double d[], double **v, int n);
	static void svdcmp(float **a, int m, int n, float w[], float **v);
	static float pythag(float a, float b);
    static void  tred2(float **a, int n, float d[], float e[]);
	static void  tqli(float d[], float e[], int n, float **z);
	static void  elmhes(float **a, int n);
	static void  hqr(float **a, int n, float wr[], float wi[]);
	static void  Myhqr(float **a, int n, float wr[], float **v);

	static void mat_copy_double(double **,int,int,double **,int,int);
	static void mat_copy_float(float **,int,int,float **,int,int);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlgebra)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAlgebra();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CAlgebra)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALGEBRA_H__ECC965F5_235E_11D2_912F_00104B6A0945__INCLUDED_)

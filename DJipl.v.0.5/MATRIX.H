#ifndef _FCOMPLEX_DECLARE_T_
typedef struct FCOMPLEX {float r,i;} fcomplex;
#define _FCOMPLEX_DECLARE_T_
#endif /* _FCOMPLEX_DECLARE_T_ */

//--------------------------------------------------------------
// complex�� ó���ϱ� ���� �Լ� 
//--------------------------------------------------------------
fcomplex Cadd(fcomplex a, fcomplex b);
fcomplex Csub(fcomplex a, fcomplex b);
fcomplex Cmul(fcomplex a, fcomplex b);
fcomplex Complex(float re, float im);
fcomplex Conjg(fcomplex z);
fcomplex Cdiv(fcomplex a, fcomplex b);
float Cabs(fcomplex z);
fcomplex Csqrt(fcomplex z);
fcomplex RCmul(float x, fcomplex a);

//--------------------------------------------------------------
// ���� �Լ� 
//--------------------------------------------------------------
void free_dvector(double *v, long nl, long nh);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);
float *vector(long nl, long nh);
int *ivector(long nl, long nh);
void free_vector(float *v, long nl, long nh);
void free_ivector(int *v, long nl, long nh);
float **matrix(long nrl, long nrh, long ncl, long nch);
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);
double *dvector(long nl, long nh);
double **dmatrix(long nrl, long nrh, long ncl, long nch);

//--------------------------------------------------------------
// SVD ���� ���α׷� 
//--------------------------------------------------------------
void svdcmp(double  **a, int m, int n, double  w[], double  **v);
void svbksb(double  **u, double  w[], double  **v, int m, int n, double  b[], double  x[]);
double  pythag(double  a, double  b);

void svd_linear_sol(double **AA,double *BB,double *sol,int num_par);
void svd_linear_sol_mn(double **AA,double *BB,double *sol,int numdata,int numpar);                                   
void svd_linear_6x6(double AA[6][6],double BB[],double sol[],int num_par);

void solve_linear_eqn_using_svd(double *AA,double *BB,double *sol,int numdata,int numpar);                                   

//--------------------------------------------------------------
// Nonlinear Optimization ���� ���α׷� 
// Levenberg-Marquardt 
//--------------------------------------------------------------
void mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ma,
  	        int lista[],int mfit,float **covar, float **alpha, float *chisq,						  
	        void (*funcs)(float [], float [], float *, float []),float *alamda);
void mrqcof(float x[], float y[], float sig[], int ndata, float a[],int ma,
			int lista[],int mfit,float **alpha, float beta[], float *chisq,						  
			void (*funcs)(float [], float [], float *, float []));					  
void covsrt(float **covar, int ma, int ia[], int mfit);
void gaussj(float **a, int n, float **b, int m);
void funcs(float x[],float a[],float *y,float dyda[]);
void fdis1(float x[],float a[],float *y,float dyda[]);
void fdis2(float x[],float a[],float *y,float dyda[]);

//--------------------------------------------------------------
// Eigenvalue, Eigenvector ���ϴ� ���α׷� - Symmetric Matrix
// �� eigenvalue, egienvector
//--------------------------------------------------------------
void get_eig(float **in,float *sol,int mrow,int mcol);
void jacobi(float **a, int n, float d[], float **v, int *nrot);
void eigsrt(float d[], float **v, int n);

//--------------------------------------------------------------
// eigenvalue of real nonsymmetric matrix
//--------------------------------------------------------------
void get_eigenvale_nxn_nonsymmetric(float in[],float *sol,int *num_sol,int num);
void elmhes(float **a, int n);
void balanc(float **a, int n);
void hqr(float **a, int n, float wr[], float wi[]);

//--------------------------------------------------------------
// ���׽��� ���� ���ϴ� ���α׷� 
//--------------------------------------------------------------
void qroot(float p[], int n, float *b, float *c, float eps);
void poldiv(float u[], int n, float v[], int nv, float q[], float r[]);

//--------------------------------------------------------------
// ���׽��� ���� ���ϴ� ���α׷� : laguer()
//--------------------------------------------------------------
void laguer(fcomplex a[], int m, fcomplex *x, int *its);

//--------------------------------------------------------------
// ���׽��� ���� ���ϴ� ���α׷� : laguer()
//--------------------------------------------------------------
void get_inverse_matrix(float in[],float sol[],int num_par);                                   



			   

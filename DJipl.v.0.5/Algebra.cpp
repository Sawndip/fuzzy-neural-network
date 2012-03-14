// Algebra.cpp : implementation file
//

#include "stdafx.h"
#include "Algebra.h"

#include "nrutil.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlgebra

IMPLEMENT_DYNCREATE(CAlgebra, CDocument)

CAlgebra::CAlgebra()
{
}

BOOL CAlgebra::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CAlgebra::~CAlgebra()
{
}


BEGIN_MESSAGE_MAP(CAlgebra, CDocument)
	//{{AFX_MSG_MAP(CAlgebra)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlgebra diagnostics

#ifdef _DEBUG
void CAlgebra::AssertValid() const
{
	CDocument::AssertValid();
}

void CAlgebra::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAlgebra serialization

void CAlgebra::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAlgebra commands
float* CAlgebra::vect_add(float* a, float* b, int k)
{
	int i;
	float * out = new float[k];

	for(i=0; i < k; i++) {
       out[i] = a[i]+b[i];
	}
	
	return(out);
}
double* CAlgebra::vect_add_double(double* a, double* b, int k)
{
	int i;
	double * out = new double[k];

	for(i=0; i < k; i++) {
       out[i] = a[i]+b[i];
	}
	
	return(out);
}

//
//
float** CAlgebra::mat_inv(float **a, int n, float* det_val)
{
	// input : **a, n,
	// output : **c, det_val
	// ludcmp와 lubksb함수를 필요로 함
	float d, *col;
	int i, j, *indx;
	float ** c;

	c = mat_new_float(n,n);
	col = new float[n];
	indx = new int[n];

	ludcmp(a,n,indx,&d);

	for(j=0; j < n; j++) d *= a[j][j];
	*det_val = d;

	//===================================
	//imp code 추가
	if(d==0){
//		AfxMessageBox("INV singular",MB_OK);
		delete []col; delete []indx;
		return(NULL);
	}

	for(j=0; j < n; j++){
		for(i=0; i < n; i++) col[i]=0.0f;
		col[j] = 1.0f;
		lubksb(a,n,indx,col);
		for(i=0; i < n; i++) c[i][j]=col[i];
	}

	delete []col; delete []indx;

	return(c);
}
//
//
void CAlgebra::ludcmp(float **a, int n, int *indx, float *d)
{
	#define TINY 1.0e-20;

	int i,imax,j,k;
	float big,dum,sum,temp;
	float *vv;

	vv = new float[n];

	*d=1.0f;
	for (i=0;i<n;i++) {
		big=0.0f;
		for (j=0;j<n;j++)
			if ((temp=(float)fabs(a[i][j])) > big) big=temp;
		if (big == 0.0f) TRACE("Singular matrix in routine ludcmp\n");
		vv[i]=1.0f/big;
	}
	for (j=0;j<n;j++) {
		for (i=0;i<j;i++) {
			sum=a[i][j];
			for (k=0;k<i;k++) sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
		}
		big=0.0f;
		for (i=j;i<n;i++) {
			sum=a[i][j];
			for (k=0;k<j;k++)
				sum -= a[i][k]*a[k][j];
			a[i][j]=sum;
			if ( (dum=vv[i]*(float)fabs(sum)) >= big) {
				big=dum;
				imax=i;
			}
		}
		if (j != imax) {
			for (k=0;k<n;k++) {
				dum=a[imax][k];
				a[imax][k]=a[j][k];
				a[j][k]=dum;
			}
			*d = -(*d);
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if (a[j][j] == 0.0f) a[j][j]=(float)TINY;
		if (j != (n-1)) {
			dum=1.0f/(a[j][j]);
			for (i=j+1;i<n;i++) a[i][j] *= dum;
		}
	}
	delete []vv;
}
//
//
void CAlgebra::lubksb(float **a, int n, int *indx, float b[])
{
	int i,ii=-1,ip,j;
	float sum;

	for (i=0; i < n; i++) {
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii >= 0)
			for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
		else if (sum) ii=i;
		b[i]=sum;
	}
	for (i=n-1;i>=0;i--) {
		sum=b[i];
		for (j=i+1;j<=n-1;j++) sum -= a[i][j]*b[j];
		b[i]=sum/a[i][i];
	}
}
//
//
float** CAlgebra::mat_new_float(int nx, int ny)
{
	int i;				     //  ------> x
	float **m;				 //	 |
  	m = new float *[ny];	 //	 |
							 //	 y
	
	for (i=0; i < ny; i++)
		m[i] = new float[nx];

	return m;
}
//
double** CAlgebra::mat_new_double(int nx, int ny)
{
	int i;				     //  ------> x
	double **m;				 //	 |
  	m = new double *[ny];	 //	 |
							 //	 y
	
	for (i=0; i < ny; i++)
		m[i] = new double[nx];
	
	return m;
}
//
float** CAlgebra::mat_new_forRecipe(int nx, int ny)
{
	int i;				     //  ------> x
	float **m;				 //	 |
  	m = new float *[ny+1];	 //	 |
							 //	 y
	
	for (i=0; i < ny+1; i++)
		m[i] = new float[nx+1];
	
	return m;
}
//
//
float** CAlgebra::mat_copy_fromRecipe(float **in, int nx, int ny)
{
	int i,j;				     
	float **m;				 

	m = new float *[ny];	 
	for (i=0; i < ny; i++)
		m[i] = new float[nx];
	
	for (i=1; i<=ny; i++)
		for (j=1; i<=nx; j++)
			m[i-1][j-1] = in[i][j];

	delete []in;
	return m;
}
//
//
float* CAlgebra::vec_new_forRecipe(int n)
{
	float *m;				 //	 |
  	m = new float[n+1];	 //	 |

	return m;
}

double* CAlgebra::vec_new_forRecipeDouble(int n)
{
	double *m;				 //	 |
  	m = new double[n+1];	 //	 |

	return m;
}

//
float* CAlgebra::vec_copy_fromRecipe(float *in, int n)
{
	int i;				     
	float *m;				 
  	m = new float[n];
	
	for (i=1; i<=n; i++)
		m[i-1] = in[i];

	delete []in;
	return m;
}
//
//
float CAlgebra::vect_inner(float* a, float* b, int n)
{
	int i;
	float out;

	out=0.0f;
	for(i=0; i < n; i++)
		out += a[i]*b[i];

	return(out);
}
//
//
float*  CAlgebra::vect_cross(float* a, float* b )
{
	float *out = new float[3];

	out[0] = a[1]*b[2]-a[2]*b[1]; 
	out[1] = a[2]*b[0]-a[0]*b[2];
	out[2] = a[0]*b[1]-a[1]*b[0];

	return(out);
}
//
//
float**  CAlgebra::vect_cross2(float* a, float* b, int n )
{
	int i,j;
	float **out;
	
	out = mat_new_forRecipe(n,n);

	for(j=1;j<=n;j++)
		for(i=1;i<=n;i++)
			out[j][i]=a[i]*b[j];

	return(out);
}
//
//
float** CAlgebra::mat_add(float** a, float** b, int nx, int ny)
{
	int i, j;
	float ** out;

	out = mat_new_float(nx,ny);
	for(j=0; j<ny; j++)
		for(i=0; i<nx; i++)
			out[j][i] = a[j][i]+b[j][i];

	return(out);
}
//
//
float** CAlgebra::mat_sub(float** a, float** b, int nx, int ny)
{
	// out = a - b
	int i, j;
	float ** out;

	out = mat_new_float(nx,ny);
	for(j=0; j<ny; j++)
		for(i=0; i<nx; i++)
			out[j][i] = a[j][i]-b[j][i];

	return(out);
}
//
//
float** CAlgebra::mat_transpose(float** a, int nx, int ny)
{
	int i, j;
	float ** out;

	out = mat_new_float(ny,nx);

	for(i=0; i < ny; i++)
		for(j=0; j < nx; j++)
			out[j][i] = a[i][j];

	return(out);
}

double** CAlgebra::mat_transpose_double(double ** a, int nx, int ny)
{
	int i, j;
	double ** out;

	out = mat_new_double(ny,nx);

	for(i=0; i < ny; i++)
		for(j=0; j < nx; j++)
			out[j][i] = a[i][j];

	return(out);
}

//
//
float** CAlgebra::mat_multiply(float** a, int ax, int ay, float** b, int bx, int by)
{
	int i, j, k, m;
	float ** out;

	if (ax == by) {
		m = ax;
		out = mat_new_float(bx,ay);
		for(i=0; i < ay; i++)
			for(j=0; j < bx; j++) {
				out[i][j] = 0.0f;
				for(k=0; k < m; k++)  {
					out[i][j] += a[i][k]*b[k][j];
		//			TRACE("%f\n", out[i][j]);
				}
		}
	}
	else TRACE("We can not multiply these matrcies!\n");

	return(out);
}

double** CAlgebra::mat_multiply_double(double** a, int ax, int ay, double** b, int bx, int by)
{
	int i, j, k, m;
	double ** out;

	if (ax == by) {
		m = ax;
		out = mat_new_double(bx,ay);
		for(i=0; i < ay; i++)
			for(j=0; j < bx; j++) {
				out[i][j] = 0.0f;
				for(k=0; k < m; k++)  {
					out[i][j] += a[i][k]*b[k][j];
		//			TRACE("%f\n", out[i][j]);
				}
		}
	}
	else TRACE("We can not multiply these matrcies!\n");

	return(out);
}
void CAlgebra::mat_copy_float(float **a,int ax,int ay,float **b,int bx,int by)
{
	if(ax==bx && ay==by){
		
		for(int m=0; m<ay; m++)
		for(int n=0; n<ax; n++){
			b[m][n]=a[m][n];
		}
	}
	else TRACE("We can not copy these matrcies!\n");
		
}

void CAlgebra::mat_copy_double(double **a,int ax,int ay,double **b,int bx,int by)
{
	if(ax==bx && ay==by){
		
		for(int m=0; m<ay; m++)
		for(int n=0; n<ax; n++){
			b[m][n]=a[m][n];
		}
	}
	else TRACE("We can not multiply these matrcies!\n");
		
}
//
//
float* CAlgebra::mat_vect_mul(float** a, int ax, int ay, float* b)
{
	int i, j;
	float *out = new float[ay];

	for(i=0; i < ay; i++) {
		out[i] = 0.0f;
		for(j=0; j < ax; j++) 
			out[i] += a[i][j]*b[j];
	}
	return(out);
}
double* CAlgebra::mat_vect_mul_double(double** a, int ax, int ay, double* b)
{
	int i, j;
	double *out = new double[ay];

	for(i=0; i < ay; i++) {
		out[i] = 0.0f;
		for(j=0; j < ax; j++) 
			out[i] += a[i][j]*b[j];
	}
	return(out);
}

//
//
float* CAlgebra::vect_mat_mul(float * a, float** b, int bx, int by)
{
	int i, j;
	float *out = new float[bx];

	for(j=0; j < bx; i++) {
		out[j] = 0.0f;
		for(i=0; i < by; i++)  out[j] += a[i]*b[i][j];
	}
	return(out);
}
//
//
#define EPS 0.000000001

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);a[k][l]=h+s*(g-h*tau);

void CAlgebra::jacobi(float **a, int n, float d[], float **v, int *nrot)
{
	// Original Source를 그대로 이용 (i=1부터 시작)
	// Symmetry Matrix에 대한 고유값(d[]) 및 고유벡터(**v)의 계산
	// 내림차순으로 Sorting됨
	int j,iq,ip,i;//, k;
	float tresh,theta,tau,t,sm,s,h,g,c,*b,*z;//, p;

	b = vec_new_forRecipe(n);
	z = vec_new_forRecipe(n);

	for (ip=1; ip<=n; ip++) {
		for (iq=1; iq<=n; iq++) v[ip][iq]=0.0;
		v[ip][ip]=1.0;
	}
	for (ip=1; ip<=n; ip++) {
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0;
	}
	*nrot=0;
	for (i=0; i<50; i++) {
		sm=0.0;
		for (ip=1; ip<=n-1; ip++) {
			for (iq=ip+1; iq<=n; iq++)
				sm += (float)fabs(a[ip][iq]);
		}
		if (sm == 0.0) {
/*			// sorting 추가 (eigsrt)
			for (i=1; i<n; i++) {
				p=d[k=i];
				for (j=i+1; j<=n; j++)
					if (d[j] >= p) p=d[k=j];
				if (k != i) {
					d[k]=d[i];
					d[i]=p;
					for (j=1; j<=n; j++) {
						p=v[j][i];
						v[j][i]=v[j][k];
						v[j][k]=p;
					}
				}
			}*/
			delete []z;
			delete []b;
			return;
		}
		if (i < 4)
			tresh=(float)(0.2*sm/(n*n));
		else
			tresh=0.0;
		
		for (ip=1; ip<=n-1; ip++) {
			for (iq=ip+1; iq<=n; iq++) {
				g=100.0f*(float)fabs(a[ip][iq]);
				if (i > 4 && (float)(fabs(d[ip])+g) == (float)fabs(d[ip])
					&& (float)(fabs(d[iq])+g) == (float)fabs(d[iq]))
					a[ip][iq]=0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if ((float)(fabs(h)+g) == (float)fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5f*h/(a[ip][iq]);
						t=1.0f/((float)fabs(theta)+(float)sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0f/(float)sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0f+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0;
					for (j=1; j<=ip-1; j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1; j<=iq-1; j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1; j<=n; j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1; j<=n; j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++(*nrot);
				}
			}
		}
		for (ip=1; ip<=n; ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0.0;
		}
	}
	AfxMessageBox("Too many iterations in routine jacobi", MB_OK);

	delete []z;
	delete []b;
	return;
}

//
//
void CAlgebra::jacobi1(double **A, int N, double EIGEN[], double **T)
{
    int i, j, k;
	int ITER, ITMAX, NM1, IP1;
	double EPS1, EPS2, EPS3;
	double S, SIGMA1, SIGMA2, OFFDSQ, HOLDKI, HOLDIK;
	double P, SPQ, CSA, SNA, Q;
	double *AIK;

	AIK = new double[N+1];

	for(j=1; j <= N; j++)
		for(i=1; i <= N; i++) 
			T[i][j] = 0.0;
	for(j=1; j <= N-1; j++)
		for(i=j+1; i <= N; i++) 
			A[i][j] = 0.0;

//	TRACE("A = \n");
//	for(j=1; j <= N; j++) {
//		for(i=1; i <= N; i++) 
//			TRACE("%12.2e", A[j][i]);
//		TRACE("\n");
//	}
	//
	//
	ITMAX =	50;
	EPS1 = 1.0e-50;
	EPS2 = 1.0e-50;
	EPS3 = 1.0e-25;
	//
	NM1 = N-1;
	//
	// setup initial matrix
	//
	SIGMA1 = 0.0;
	OFFDSQ = 0.0;
	for (i=1; i<=N; i++) {
		SIGMA1 = SIGMA1 + A[i][i]*A[i][i];
		T[i][i] = 1.0;
		IP1 = i+1;
		if (i >= N) break;
		for (j=IP1; j<=N; j++)
			OFFDSQ = OFFDSQ + A[i][j]*A[i][j];
	}
	S = 2.0f*OFFDSQ + SIGMA1;
	//
	//
	for (ITER=1; ITER<=ITMAX; ITER++) {
		for (i=1; i <= NM1; i++) {
			IP1 = i+1;
			for (j=IP1; j<=N; j++) {
				Q = fabs(A[i][i] - A[j][j]);
				if(Q <= EPS1) {
					CSA = 1.0/sqrt(2.0);
					SNA = CSA;
				}
				else if (fabs(A[i][j]) <= EPS2)  {
					A[i][j] = 0.0;
					continue;
				}
				else {
					P = 2.0*A[i][j]*Q/(A[i][i] - A[j][j]);
					SPQ = sqrt(P*P + Q*Q);
					CSA = sqrt((1.0+Q/SPQ)/2.0);
					SNA = P/(2.0*CSA*SPQ);
				}
								 					
				for (k=1; k<=N; k++) {
		     		HOLDKI = T[k][i];
		     		T[k][i] = HOLDKI*CSA + T[k][j]*SNA;
		     		T[k][j] = HOLDKI*SNA - T[k][j]*CSA;
		     	}
		     	
		     	for (k=i; k<=N; k++) {
					if (k > j) {
			     		HOLDIK = A[i][k];
			     		A[i][k] = HOLDIK*CSA + A[j][k]*SNA;
			     		A[j][k] = HOLDIK*SNA - A[j][k]*CSA;
						continue;
					}
		     		AIK[k] = A[i][k];
					A[i][k] = CSA*AIK[k] + SNA*A[k][j];
					if (k != j) continue;
					A[j][k] = SNA*AIK[k] - CSA*A[j][k];
				}

				AIK[j] = SNA*AIK[i] - CSA*AIK[j];

				for (k=1; k <=j; k++) {
					if (k <= i) {
			     		HOLDKI = A[k][i];
			     		A[k][i] = HOLDKI*CSA + A[k][j]*SNA;
			     		A[k][j] = HOLDKI*SNA - A[k][j]*CSA;
						continue;
					}
		     		A[k][j] = SNA*AIK[k] - A[k][j]*CSA;
				}
				A[i][j] = 0.0;
			}  // end of j
		} // end of i

		TRACE("ITER = %d\n", ITER);
		TRACE("A = \n");
		for(j=1; j <= 5; j++) {
			for(i=1; i <= 5; i++) 
				TRACE("%12.2e", A[j][i]);
			TRACE("\n");
		}

		SIGMA2 = 0.0f;
		for (i=1; i <=N; i++) {
			EIGEN[i] = A[i][i];
			SIGMA2 = SIGMA2 + EIGEN[i]*EIGEN[i];
		}

		TRACE("Sigma : %12.2e %12.2e %12.2e\n", SIGMA1, SIGMA2, SIGMA1/SIGMA2);

		if ( (float)fabs(1.0f-SIGMA1/SIGMA2) >= EPS3) {
			SIGMA1 = SIGMA2;
			continue;
		}
		else return;
	}    
	TRACE("No Convergence!\n");
	return;
}
//
//

//
//
#define ROTATE2(a, i, j, k, l, n) g=a[i*n+j]; h=a[k*n+l]; a[i*n+j] = g-s*(h+g*tau); \
  a[k*n+l] = h+s*(g-h*tau);

void CAlgebra::jacobi2(float *a, int n, float *d, float *v, int *nrot)
/* 
  Computes all eigenvalues and eigenvectors of a real symmetric matrix A
  On output, elements of A above the diagonal are destroyed,
  D returns the eigenvalues of A, and V is a matrix whose columns are the
  eigenvectors of A.
*/
  {
  
  int j,iq,ip,ip_times_n,i ;
  float tresh,theta,tau,t,sm,s,h,g,c,*b,*z,*vector();
  
  b = (float *)malloc(n*sizeof(float));
  z = (float *)malloc(n*sizeof(float));

  
  for(ip_times_n=0, ip=0; ip<n; ++ip, ip_times_n+=n)  
      {
      
      /* Initialize the identity matrix */
      for(iq=0; iq<n; ++iq)v[ip_times_n + iq] = 0.0 ;
      v[ip_times_n + ip] = 1.0 ;
      
      /* Initailize b and d to be diagonal of a */
      b[ip] = d[ip] = a[ip_times_n + ip];
      z[ip] = 0.0 ;
      }
      
  *nrot = 0 ;
  for(i=0;i<50;++i)
      {
      /* Sum off-diagonal elements */
      sm=0.0 ;

      for(ip_times_n=0,ip=0;ip<n-1;ip++,ip_times_n+=n)
	for(iq=ip+1;iq<n;iq++)
	  sm += (float)fabs(a[ip_times_n + iq]);
	  
      /*  If we have converged,  free the working vectors and return. */
      if(sm == 0.0)
	  {
	  free(b);
	  free(z);
	  return;
	  }
      
      /* tresh is different on first three iterations...*/
      tresh = (float)((i<3) ? 0.2*sm/(n*n) : 0.0);
      
      for(ip_times_n=0,ip=0;ip<n-1;ip++,ip_times_n+=n)
	  {
	  for(iq=ip+1;iq<n;++iq)
	      {
	      g= (float)(100.0*fabs(a[ip_times_n + iq]));

	      /* After four sweeps, skip the rotation if the off-diagonal element is small */
	      /* This test is taken directly from the text and looks a little suspect to me... */

	      if(i > 3 && g < EPS)
		a[ip_times_n + iq] = 0.0 ;

	      else if(fabs(a[ip_times_n+iq]) > tresh) 
		  {
		  h=d[iq]-d[ip];
		  if(g < EPS)
		    t = (float)((fabs(a[ip_times_n+iq]) > EPS) ? (a[ip_times_n+iq])/h : 0.0); 
		  else
		      { 
		      theta=(float)((fabs(h) < EPS) ? 0.0 : 0.5*h/(a[ip_times_n+iq]));
		      t=(float)(1.0/(fabs(theta)+sqrt(1.0+theta*theta)));
		      if(theta < 0.0)
			t = -t ; 
		      } 
		  c=(float)(1.0/sqrt(1.0+t*t));
		  s=t*c;
		  tau=(float)(s/(1.0+c));
		  
		  h=t*a[ip_times_n+iq];
		  z[ip] -= h;
		  z[iq] += h;
		  d[ip] -= h;
		  d[iq] += h;
		  a[ip_times_n+iq]=0.0;
		  
		  for(j=0;j<ip;j++)
		      {
		      ROTATE2(a,j,ip,j,iq,n);
		      }
		  for(j=ip+1;j<iq;j++)
		      {
		      ROTATE2(a,ip,j,j,iq,n);
		      }
		  for(j=iq+1;j<n;j++)
		      {
		      ROTATE2(a,ip,j,iq,j,n);
		      }
		  for(j=0;j<n;j++)
		      {
		      ROTATE2(v,j,ip,j,iq,n);
		      }
		  ++(*nrot);
		  }
	      }
	  }
      for(ip=0;ip<n;++ip)
	  {
	  b[ip] += z[ip];
	  d[ip]=b[ip];
	  z[ip]=0.0;
	  }
      }

  /* Failed to converge!! What to do ??? */
  /* Well, let's at least free up memory and return without a murmur */

  free(b);
  free(z);
  return;
  
  }

//
//


void CAlgebra::eigsrt(float d[], float **v, int n)
{
	// Jacobi에 대한 고유값의 Sorting
	int k,j,i;
	float p;

	for (i=1; i<n; i++) {
		p=d[k=i];
		for (j=i+1; j<=n; j++)
			if (d[j] >= p) p=d[k=j];
		if (k != i) {
			d[k]=d[i];
			d[i]=p;
			for (j=1; j<=n; j++) {
				p=v[j][i];
				v[j][i]=v[j][k];
				v[j][k]=p;
			}
		}
	}
}
//
void CAlgebra::eigsrt_double(double d[], double **v, int n)
{
	// Jacobi에 대한 고유값의 Sorting
	int k,j,i;
	double p;

	for (i=1; i<n; i++) {
		p=d[k=i];
		for (j=i+1; j<=n; j++)
			if (d[j] >= p) p=d[k=j];
		if (k != i) {
			d[k]=d[i];
			d[i]=p;
			for (j=1; j<=n; j++) {
				p=v[j][i];
				v[j][i]=v[j][k];
				v[j][k]=p;
			}
		}
	}
}
//
//
void CAlgebra::svdcmp(float **a, int m, int n, float w[], float **v)
{
	// SVD의 계산
	// Recipe의 original Source를 그대로 이용 (i=1서부터 시작)
	// w(eigen values)와 v(eigen vectors)는 외부에서 memory allocation
	// "pythag"함수 필요함
	// A = U*W*Vt
	// "eigstr"을 이용하여 W(singular value)와 Vt를 큰순서에서 작은 sorting할 수 있음
	
	int flag,i,its,j,jj,k,l,nm;
	float anorm,c,f,g,h,s,scale,x,y,z,*rv1;

	rv1=vec_new_forRecipe(n);
	g=scale=anorm=0.0;
	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += (float)fabs(a[k][i]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f=a[i][i];
				g = -(float)SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][i]=f-g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
					f=s/h;
					for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
				}
				for (k=i;k<=m;k++) a[k][i] *= scale;
			}
		}
		w[i]=scale *g;
		g=s=scale=0.0;
		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += (float)fabs(a[i][k]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g = -(float)SIGN(sqrt(s),f);
				h=f*g-s;
				a[i][l]=f-g;
				for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
					for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
				}
				for (k=l;k<=n;k++) a[i][k] *= scale;
			}
		}
		anorm=(float)FMAX(anorm,((float)fabs(w[i])+(float)fabs(rv1[i])));
	}
	for (i=n;i>=1;i--) {
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++)
					v[j][i]=(a[i][j]/a[i][l])/g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
					for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
				}
			}
			for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
		}
		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}
	for (i=IMIN(m,n);i>=1;i--) {
		l=i+1;
		g=w[i];
		for (j=l;j<=n;j++) a[i][j]=0.0;
		if (g) {
			g=1.0f/g;
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
				f=(s/a[i][i])*g;
				for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
			}
			for (j=i;j<=m;j++) a[j][i] *= g;
		} else for (j=i;j<=m;j++) a[j][i]=0.0;
		++a[i][i];
	}
	for (k=n;k>=1;k--) {
		for (its=1;its<=30;its++) {
			flag=1;
			for (l=k;l>=1;l--) {
				nm=l-1;
				if ((float)(fabs(rv1[l])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((float)(fabs(w[nm])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if ((float)(fabs(f)+anorm) == anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=1.0f/h;
					c=g*h;
					s = -f*h;
					for (j=1;j<=m;j++) {
						y=a[j][nm];
						z=a[j][i];
						a[j][nm]=y*c+z*s;
						a[j][i]=z*c-y*s;
					}
				}
			}
			z=w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j=1;j<=n;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			if (its == 30) AfxMessageBox("no convergence in 30 svdcmp iterations", MB_OK);
			x=w[l];
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0f*h*y);
			g=pythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+(float)SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=1;jj<=n;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}
				z=pythag(f,h);
				w[j]=z;
				if (z) {
					z=1.0f/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=1;jj<=m;jj++) {
					y=a[jj][j];
					z=a[jj][i];
					a[jj][j]=y*c+z*s;
					a[jj][i]=z*c-y*s;
				}
			}
			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
	delete []rv1;
}
//
//
float CAlgebra::pythag(float a, float b)
{
	float absa,absb;
	absa=(float)fabs(a);
	absb=(float)fabs(b);
	if (absa > absb) return absa*(float)sqrt(1.0+(float)SQR(absb/absa));
	else return (absb == 0.0f ? 0.0f : absb*(float)sqrt(1.0f+(float)SQR(absa/absb)));
}
//
//
void CAlgebra::tred2(float **a, int n, float d[], float e[])
{
	int l,k,j,i;
	float scale,hh,h,g,f;

	for (i=n;i>=2;i--) {
		l=i-1;
		h=scale=0.0;
		if (l > 1) {
			for (k=1;k<=l;k++)
				scale += (float)fabs(a[i][k]);
			if (scale == 0.0)
				e[i]=a[i][l];
			else {
				for (k=1;k<=l;k++) {
					a[i][k] /= scale;
					h += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g=(f >= 0.0f ? -(float)sqrt(h) : (float)sqrt(h));
				e[i]=scale*g;
				h -= f*g;
				a[i][l]=f-g;
				f=0.0;
				for (j=1;j<=l;j++) {
					a[j][i]=a[i][j]/h;
					g=0.0;
					for (k=1;k<=j;k++)
						g += a[j][k]*a[i][k];
					for (k=j+1;k<=l;k++)
						g += a[k][j]*a[i][k];
					e[j]=g/h;
					f += e[j]*a[i][j];
				}
				hh=f/(h+h);
				for (j=1;j<=l;j++) {
					f=a[i][j];
					e[j]=g=e[j]-hh*f;
					for (k=1;k<=j;k++)
						a[j][k] -= (f*e[k]+g*a[i][k]);
				}
			}
		} else
			e[i]=a[i][l];
		d[i]=h;
	}
	d[1]=0.0;
	e[1]=0.0;
	/* Contents of this loop can be omitted if eigenvectors not
			wanted except for statement d[i]=a[i][i]; */
	for (i=1;i<=n;i++) {
		l=i-1;
		if (d[i]) {
			for (j=1;j<=l;j++) {
				g=0.0;
				for (k=1;k<=l;k++)
					g += a[i][k]*a[k][j];
				for (k=1;k<=l;k++)
					a[k][j] -= g*a[k][i];
			}
		}
		d[i]=a[i][i];
		a[i][i]=1.0;
		for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0;
	}
}
//
//
void CAlgebra::tqli(float d[], float e[], int n, float **z)
{
	int m,l,iter,i,k;
	float s,r,p,g,f,dd,c,b;

	for (i=2;i<=n;i++) e[i-1]=e[i];
	e[n]=0.0;
	for (l=1;l<=n;l++) {
		iter=0;
		do {
			for (m=l;m<=n-1;m++) {
				dd=(float)fabs(d[m])+(float)fabs(d[m+1]);
				if ((float)(fabs(e[m])+dd) == dd) break;
			}
			if (m != l) {
				if (iter++ == 30) TRACE("Too many iterations in tqli!\n");
				g=(d[l+1]-d[l])/(2.0f*e[l]);
				r=pythag(g,1.0);
				g=d[m]-d[l]+e[l]/(g+(float)SIGN(r,g));
				s=c=1.0;
				p=0.0;
				for (i=m-1;i>=l;i--) {
					f=s*e[i];
					b=c*e[i];
					e[i+1]=(r=pythag(f,g));
					if (r == 0.0) {
						d[i+1] -= p;
						e[m]=0.0;
						break;
					}
					s=f/r;
					c=g/r;
					g=d[i+1]-p;
					r=(d[i]-g)*s+2.0f*c*b;
					d[i+1]=g+(p=s*r);
					g=c*r-b;
					for (k=1;k<=n;k++) {
						f=z[k][i+1];
						z[k][i+1]=s*z[k][i]+c*f;
						z[k][i]=c*z[k][i]-s*f;
					}
				}
				if (r == 0.0 && i >= l) continue;
				d[l] -= p;
				e[l]=g;
				e[m]=0.0;
			}
		} while (m != l);
	}
}
//
//
#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}

void CAlgebra::elmhes(float **a, int n)
{
	// general real matrix의 eigen value를 구하기 위한 전처리 과정
	int m,j,i;
	float y,x;

	for (m=2;m<n;m++) {
		x=0.0;
		i=m;
		for (j=m;j<=n;j++) {
			if (fabs(a[j][m-1]) > fabs(x)) {
				x=a[j][m-1];
				i=j;
			}
		}
		if (i != m) {
			for (j=m-1;j<=n;j++) SWAP(a[i][j],a[m][j])
			for (j=1;j<=n;j++) SWAP(a[j][i],a[j][m])
		}
		if (x) {
			for (i=m+1;i<=n;i++) {
				if ((y=a[i][m-1]) != 0.0) {
					y /= x;
					a[i][m-1]=y;
					for (j=m;j<=n;j++)
						a[i][j] -= y*a[m][j];
					for (j=1;j<=n;j++)
						a[j][m] += y*a[j][i];
				}
			}
		}
	}
}
//
//
void CAlgebra::hqr(float **a, int n, float wr[], float wi[])
{
	// general real matrix의 eigen value를 구함
	// recipe 그대로
	// 전처리로 elmhes를 필요로 함
	int nn,m,l,k,j,its,i,mmin;
	float z,y,x,w,v,u,t,s,r,q,p,anorm;

	anorm=(float)fabs(a[1][1]);
	for (i=2;i<=n;i++)
		for (j=(i-1);j<=n;j++)
			anorm += (float)fabs(a[i][j]);
	nn=n;
	t=0.0;
	while (nn >= 1) {
		its=0;
		do {
			for (l=nn;l>=2;l--) {
				s=(float)fabs(a[l-1][l-1])+(float)fabs(a[l][l]);
				if (s == 0.0) s=anorm;
				if ((float)(fabs(a[l][l-1]) + s) == s) break;
			}
			x=a[nn][nn];
			if (l == nn) {
				wr[nn]=x+t;
				wi[nn--]=0.0;
			} else {
				y=a[nn-1][nn-1];
				w=a[nn][nn-1]*a[nn-1][nn];
				if (l == (nn-1)) {
					p=0.5f*(y-x);
					q=p*p+w;
					z=(float)sqrt((float)fabs(q));
					x += t;
					if (q >= 0.0) {
						z=p+(float)SIGN(z,p);
						wr[nn-1]=wr[nn]=x+z;
						if (z) wr[nn]=x-w/z;
						wi[nn-1]=wi[nn]=0.0;
					} else {
						wr[nn-1]=wr[nn]=x+p;
						wi[nn-1]= -(wi[nn]=z);
					}
					nn -= 2;
				} else {
					if (its == 30) TRACE("Too many iterations in hqr\n");
					if (its == 10 || its == 20) {
						t += x;
						for (i=1;i<=nn;i++) a[i][i] -= x;
						s=(float)fabs(a[nn][nn-1])+(float)fabs(a[nn-1][nn-2]);
						y=x=0.75f*s;
						w = -0.4375f*s*s;
					}
					++its;
					for (m=(nn-2);m>=l;m--) {
						z=a[m][m];
						r=x-z;
						s=y-z;
						p=(r*s-w)/a[m+1][m]+a[m][m+1];
						q=a[m+1][m+1]-z-r-s;
						r=a[m+2][m+1];
						s=(float)fabs(p)+(float)fabs(q)+(float)fabs(r);
						p /= s;
						q /= s;
						r /= s;
						if (m == l) break;
						u=(float)fabs(a[m][m-1])*((float)fabs(q)+(float)fabs(r));
						v=(float)fabs(p)*((float)fabs(a[m-1][m-1])+(float)fabs(z)+(float)fabs(a[m+1][m+1]));
						if ((float)(u+v) == v) break;
					}
					for (i=m+2;i<=nn;i++) {
						a[i][i-2]=0.0;
						if (i != (m+2)) a[i][i-3]=0.0;
					}
					for (k=m;k<=nn-1;k++) {
						if (k != m) {
							p=a[k][k-1];
							q=a[k+1][k-1];
							r=0.0;
							if (k != (nn-1)) r=a[k+2][k-1];
							if ((x=(float)fabs(p)+(float)fabs(q)+(float)fabs(r)) != 0.0) {
								p /= x;
								q /= x;
								r /= x;
							}
						}
						if ((s=(float)SIGN(sqrt(p*p+q*q+r*r),p)) != 0.0f) {
							if (k == m) {
								if (l != m)
								a[k][k-1] = -a[k][k-1];
							} else
								a[k][k-1] = -s*x;
							p += s;
							x=p/s;
							y=q/s;
							z=r/s;
							q /= p;
							r /= p;
							for (j=k;j<=nn;j++) {
								p=a[k][j]+q*a[k+1][j];
								if (k != (nn-1)) {
									p += r*a[k+2][j];
									a[k+2][j] -= p*z;
								}
								a[k+1][j] -= p*y;
								a[k][j] -= p*x;
							}
							mmin = nn<k+3 ? nn : k+3;
							for (i=l;i<=mmin;i++) {
								p=x*a[i][k]+y*a[i][k+1];
								if (k != (nn-1)) {
									p += z*a[i][k+2];
									a[i][k+2] -= p*r;
								}
								a[i][k+1] -= p*q;
								a[i][k] -= p;
							}
						}
					}
				}
			}
		} while (l < nn-1);
	}
}

void  CAlgebra::Myhqr(float **a, int n, float wr[], float **wv)
{
	// general real matrix의 eigen value를 구함
	// recipe 그대로
	// 전처리로 elmhes를 필요로 함
	int nn,m,l,k,j,its,i,mmin;
	float z,y,x,w,v,u,t,s,r,q,p,anorm;

	float *wr2 = vec_new_forRecipe(n);
	float **a2 = mat_new_forRecipe(n,n);

	for(i=1;i<=n;i++)
		for(j=1;j<=n;j++)
			a2[i][j] = a[i][j];


	anorm=(float)fabs(a[1][1]);
	for (i=2;i<=n;i++)
		for (j=(i-1);j<=n;j++)
			anorm += (float)fabs(a[i][j]);
	nn=n;
	t=0.0;
	while (nn >= 1) {
		its=0;
		do {
			for (l=nn;l>=2;l--) {
				s=(float)fabs(a[l-1][l-1])+(float)fabs(a[l][l]);
				if (s == 0.0) s=anorm;
				if ((float)(fabs(a[l][l-1]) + s) == s) break;
			}
			x=a[nn][nn];
			if (l == nn) {
				wr[nn--]=x+t;
//				wi[nn--]=0.0;
			} else {
				y=a[nn-1][nn-1];
				w=a[nn][nn-1]*a[nn-1][nn];
				if (l == (nn-1)) {
					p=0.5f*(y-x);
					q=p*p+w;
					z=(float)sqrt((float)fabs(q));
					x += t;
					if (q >= 0.0) {
						z=p+(float)SIGN(z,p);
						wr[nn-1]=wr[nn]=x+z;
						if (z) wr[nn]=x-w/z;
//						wi[nn-1]=wi[nn]=0.0;
					} else {
						wr[nn-1]=wr[nn]=x+p;
//						wi[nn-1]= -(wi[nn]=z);
					}
					nn -= 2;
				} else {
					if (its == 30) TRACE("Too many iterations in hqr\n");
					if (its == 10 || its == 20) {
						t += x;
						for (i=1;i<=nn;i++) a[i][i] -= x;
						s=(float)fabs(a[nn][nn-1])+(float)fabs(a[nn-1][nn-2]);
						y=x=0.75f*s;
						w = -0.4375f*s*s;
					}
					++its;
					for (m=(nn-2);m>=l;m--) {
						z=a[m][m];
						r=x-z;
						s=y-z;
						p=(r*s-w)/a[m+1][m]+a[m][m+1];
						q=a[m+1][m+1]-z-r-s;
						r=a[m+2][m+1];
						s=(float)fabs(p)+(float)fabs(q)+(float)fabs(r);
						p /= s;
						q /= s;
						r /= s;
						if (m == l) break;
						u=(float)fabs(a[m][m-1])*((float)fabs(q)+(float)fabs(r));
						v=(float)fabs(p)*((float)fabs(a[m-1][m-1])+(float)fabs(z)+(float)fabs(a[m+1][m+1]));
						if ((float)(u+v) == v) break;
					}
					for (i=m+2;i<=nn;i++) {
						a[i][i-2]=0.0;
						if (i != (m+2)) a[i][i-3]=0.0;
					}
					for (k=m;k<=nn-1;k++) {
						if (k != m) {
							p=a[k][k-1];
							q=a[k+1][k-1];
							r=0.0;
							if (k != (nn-1)) r=a[k+2][k-1];
							if ((x=(float)fabs(p)+(float)fabs(q)+(float)fabs(r)) != 0.0) {
								p /= x;
								q /= x;
								r /= x;
							}
						}
						if ((s=(float)SIGN(sqrt(p*p+q*q+r*r),p)) != 0.0f) {
							if (k == m) {
								if (l != m)
								a[k][k-1] = -a[k][k-1];
							} else
								a[k][k-1] = -s*x;
							p += s;
							x=p/s;
							y=q/s;
							z=r/s;
							q /= p;
							r /= p;
							for (j=k;j<=nn;j++) {
								p=a[k][j]+q*a[k+1][j];
								if (k != (nn-1)) {
									p += r*a[k+2][j];
									a[k+2][j] -= p*z;
								}
								a[k+1][j] -= p*y;
								a[k][j] -= p*x;
							}
							mmin = nn<k+3 ? nn : k+3;
							for (i=l;i<=mmin;i++) {
								p=x*a[i][k]+y*a[i][k+1];
								if (k != (nn-1)) {
									p += z*a[i][k+2];
									a[i][k+2] -= p*r;
								}
								a[i][k+1] -= p*q;
								a[i][k] -= p;
							}
						}
					}
				}
			}
		} while (l < nn-1);
	}

	for(i=1;i<=n;i++)
		wr2[i] = wr[i];


	// from jacobi.
	// 위에서 wr을 구한 결과를 이용 고유벡터(**wv) 계산
	
	// Original Source를 그대로 이용 (i=1부터 시작)
	// Symmetry Matrix에 대한 고유값(d[]) 및 고유벡터(**v)의 계산
	// 내림차순으로 Sorting됨
	int iq,ip;
	float tresh,theta,tau,sm,h,g,c;

	for (ip=1; ip<=n; ip++) {
		for (iq=1; iq<=n; iq++) wv[ip][iq]=0.0;
		wv[ip][ip]=1.0;
	}
	
	for (i=0; i<50; i++) {
		sm=0.0;
		for (ip=1; ip<=n-1; ip++) {
			for (iq=ip+1; iq<=n; iq++)
				sm += (float)fabs(a[ip][iq]);
		}

		if (sm == 0.0)
			break;

		if (i < 4)
			tresh=(float)(0.2*sm/(n*n));
		else
			tresh=0.0;
		
		for (ip=1; ip<=n-1; ip++) {
			for (iq=ip+1; iq<=n; iq++) {
				g=100.0f*(float)fabs(a[ip][iq]);
				if (i > 4 && (float)(fabs(wr2[ip])+g) == (float)fabs(wr2[ip])
					&& (float)(fabs(wr2[iq])+g) == (float)fabs(wr2[iq]))
					a[ip][iq]=0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h=wr2[iq]-wr2[ip];
					if ((float)(fabs(h)+g) == (float)fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5f*h/(a[ip][iq]);
						t=1.0f/((float)fabs(theta)+(float)sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0f/(float)sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0f+c);
					h=t*a[ip][iq];
					
					wr2[ip] -= h;
					wr2[iq] += h;
					
					a[ip][iq]=0.0;
					for (j=1; j<=ip-1; j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1; j<=iq-1; j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1; j<=n; j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1; j<=n; j++) {
						ROTATE(wv,j,ip,j,iq)
					}
				}
			}
		}
	}
}
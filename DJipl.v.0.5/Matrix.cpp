//--------------------------------------------------------------
// igenvalue, SVD, Linear equation
// Nonlinear optimization 관련 프로그램 
// from) Numerical Recipe in C
// 1996.10.18 Ha, Jong Eun
//--------------------------------------------------------------
#include "stdafx.h"
#include <math.h>
#include "matrix.h"

#define NR_END 1
#define FREE_ARG char*
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

/// double version of FMAX
static double  maxargd1,maxargd2;
#define FMAXD(a,b) (maxargd1=(a),maxargd2=(b),(maxargd1) > (maxargd2) ?\
        (maxargd1) : (maxargd2))

/// float version for FMAX
static float maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
#define SWAP_C(a,b) {swap=(a);(a)=(b);(b)=swap;} 
#define SWAP_G(a,b) {temp=(a);(a)=(b);(b)=temp;}

static double sqrarg;

//------------------------------------------------------------------------
// solve linear equation using SVD                                                     
// AA[0,...,N-1][0,...,n-1] 
// BB[0,...,N-1]
// sol[0,...,N-1]
//------------------------------------------------------------------------
void svd_linear_sol(double **AA,double *BB,double *sol,int num_par)                                   
{
	int i,j;
	double    wmax,thresh;
	double    **a,*b,*w,**v,*solution;                   
	
	double    TOL=1.0e-7;
	
	b=dvector(1L,(long)num_par);
	a=dmatrix(1L,(long)num_par,1L,(long)num_par);

	// 1. matrix substution 
	for(i=0;i<num_par;i++)
		for(j=0;j<num_par;j++)
		    a[i+1][j+1]=AA[i][j];
	for(i=0;i<num_par;i++) b[i+1]=BB[i];
    
	// 2. get SVD 
	w=dvector(1L,(long)num_par);
	v=dmatrix(1L,(long)num_par,1L,(long)num_par);
    svdcmp(a,(int)(num_par),(int)(num_par),w,v);//singular value decomposition
	
    // 3. adjust singular values   
	wmax=0.0;//edit the singular values,given TOL form the #define statement,between here...
	for(j=1;j<=num_par;j++)
	    if(w[j]>wmax)wmax=w[j];
	thresh=TOL*wmax;
	for(j=1;j<=num_par;j++)
	    if(w[j]<thresh)w[j]=0.0;
	solution=dvector((long)1,(int)num_par);    
	
	// 4. get solution 
	svbksb(a,w,v,(int)num_par,(int)num_par,b,solution);
    for(i=0;i<num_par;i++)
		sol[i]=solution[i+1];	 
	
	free_dvector(b,1L,(long)num_par);
	free_dmatrix(a,1L,(long)num_par,1L,(long)num_par);
	free_dvector(w,1L,(long)num_par);
	free_dmatrix(v,1L,(long)num_par,1L,(long)num_par);
	free_dvector(solution,1L,(long)num_par);    
}		                                          
//------------------------------------------------------------------------
// solve linear equation using SVD                                                      
// AA[0,...,numdata-1][0,...,numpar-1] 
// BB[0,...,numdata-1]
// sol[0,...,numpar-1]
//------------------------------------------------------------------------
void svd_linear_sol_mn(double **AA,double *BB,double *sol,int numdata,int numpar)                                   
{
	int i,j;
	double    wmax,thresh;
	double    **a,*b,*w,**v,*solution;                   
	double    TOL=1.0e-7;
	
	b=dvector(1L,(long)numdata);
	a=dmatrix(1L,(long)numdata,1L,(long)numpar);

	// 1. matrix substution 
	for(i=0;i<numdata;i++)
		for(j=0;j<numpar;j++)
		    a[i+1][j+1]=AA[i][j];
	for(i=0;i<numdata;i++) b[i+1]=BB[i];

	// 2. get SVD 
	w=dvector(1L,(long)numpar);
	v=dmatrix(1L,(long)numpar,1L,(long)numpar);
    svdcmp(a,(int)(numdata),(int)(numpar),w,v);//singular value decomposition
	
    // 3. adjust singular values   
	wmax=0.0;//edit the singular values,given TOL form the #define statement,between here...
	for(j=1;j<=numpar;j++)
	    if(w[j]>wmax)wmax=w[j];
	thresh=TOL*wmax;
	for(j=1;j<=numpar;j++)
	    if(w[j]<thresh)w[j]=0.0;
	solution=dvector((long)1,(int)numpar);    
	
	// 4. get solution 
	svbksb(a,w,v,(int)numdata,(int)numpar,b,solution);
    for(i=0;i<numpar;i++) 
		sol[i]=solution[i+1];	 
	
	// free
	free_dvector(b,1L,(long)numdata);
	free_dmatrix(a,1L,(long)numdata,1L,(long)numpar);
	free_dvector(w,1L,(long)numpar);
	free_dmatrix(v,1L,(long)numpar,1L,(long)numpar);
	free_dvector(solution,1L,(long)numpar);    
}		                                          
//------------------------------------------------------------------------
// solve linear equation using SVD 
// solve Ax=B A : mxn     B : nx1
// mxn의 matrix을 일차원 배열 AA로 변화하여 대입                                                       
// AA[0,...,(numdata-1)*(numpar-1)]
// BB[0,...,numdata-1]
// sol[0,...,numpar-1]
//------------------------------------------------------------------------
void solve_linear_eqn_using_svd(double *AA,double *BB,double *sol,int numdata,int numpar)                                   
{

	int i,j;
	double    wmax,thresh;
	double    **a,*b,*w,**v,*solution;                   
	double    TOL=1.0e-7;
	
	b=dvector(1L,(long)numdata);
	a=dmatrix(1L,(long)numdata,1L,(long)numpar);

	// 1. matrix substution 
	for(i=0;i<numdata;i++)
		for(j=0;j<numpar;j++)
		    a[i+1][j+1]=AA[i*numpar+j];
	for(i=0;i<numdata;i++) 
		b[i+1]=BB[i];

	// 2. get SVD 
	w=dvector(1L,(long)numpar);
	v=dmatrix(1L,(long)numpar,1L,(long)numpar);
    svdcmp(a,(int)(numdata),(int)(numpar),w,v);//singular value decomposition
	
    // 3. adjust singular values   
	wmax=0.0;//edit the singular values,given TOL form the #define statement,between here...
	for(j=1;j<=numpar;j++)
	    if(w[j]>wmax)
			wmax=w[j];
	thresh=TOL*wmax;
	for(j=1;j<=numpar;j++)
	    if(w[j]<thresh)
			w[j]=0.0;
	solution=dvector((long)1,(int)numpar);    
	
	// 4. get solution 
	svbksb(a,w,v,(int)numdata,(int)numpar,b,solution);
    for(i=0;i<numpar;i++) 
		sol[i]=solution[i+1];	 
	
	// free
	free_dvector(b,1L,(long)numdata);
	free_dmatrix(a,1L,(long)numdata,1L,(long)numpar);
	free_dvector(w,1L,(long)numpar);
	free_dmatrix(v,1L,(long)numpar,1L,(long)numpar);
	free_dvector(solution,1L,(long)numpar);    
}		                                          

//------------------------------------------------------------
// solve linear equation using SVD                                           
//------------------------------------------------------------
void svd_linear_6x6(double AA[6][6],double BB[],double sol[],int num_par)                                   
{
	int i,j;
	double    wmax,thresh;
	double    **a,*b,*w,**v,*solution;                   
	double    TOL=1.0e-7;
	
	b=dvector(1L,(long)num_par);
	a=dmatrix(1L,(long)num_par,1L,(long)num_par);
    /* 1. matrix substution */
	for(i=0;i<num_par;i++)
		for(j=0;j<num_par;j++)
		    a[i+1][j+1]=AA[i][j];
	for(i=0;i<num_par;i++) b[i+1]=BB[i];
    /* 2. get SVD */
	w=dvector(1L,(long)num_par);
	v=dmatrix(1L,(long)num_par,1L,(long)num_par);
    svdcmp(a,(int)(num_par),(int)(num_par),w,v);//singular value decomposition
	
    /* 3. adjust singular values */  
	wmax=0.0;//edit the singular values,given TOL form the #define statement,between here...
	for(j=1;j<=num_par;j++)
	    if(w[j]>wmax)wmax=w[j];
	thresh=TOL*wmax;
	for(j=1;j<=num_par;j++)
	    if(w[j]<thresh)w[j]=0.0;
	solution=dvector((long)1,(int)num_par);    
	
    /* 4. get solution */
	svbksb(a,w,v,(int)num_par,(int)num_par,b,solution);
    for(i=0;i<num_par;i++) sol[i]=solution[i+1];	 
	
	free_dvector(b,1L,(long)num_par);
	free_dmatrix(a,1L,(long)num_par,1L,(long)num_par);
	free_dvector(w,1L,(long)num_par);
	free_dmatrix(v,1L,(long)num_par,1L,(long)num_par);
	free_dvector(solution,1L,(long)num_par);    
}		                                          
//----------------------------------------------------------------------------
// solve linear equation using SVD					                                        
// [num_parXnum_par] 크기의 matrix의 inverse matrix를 구함 
// 일반적으로 하기 위해 2차원 배열을 1차원 배열 형태로 고쳐서 인자로 넘김 						 
// in[0,...,num_par-1], sol[0,...,num_par-1]
//----------------------------------------------------------------------------                 
void get_inverse_matrix(float in[],float sol[],int num_par)                                   
{
	int i,j,k;
	double    tmp;
	double    **a,*w,**v,**tmp_sol;                   
	double    TOL=1.0e-7;
	
	/// allocation 
	a=dmatrix(1L,(long)num_par,1L,(long)num_par);
	w=dvector(1L,(long)num_par);
	v=dmatrix(1L,(long)num_par,1L,(long)num_par);
	tmp_sol=dmatrix(1L,(long)num_par,1L,(long)num_par);

    /* 1. matrix substution */
	for(i=0;i<num_par;i++)
		for(j=0;j<num_par;j++)
		    a[i+1][j+1]=(double)in[i*num_par+j];
    /* 2. get SVD */
	
    svdcmp(a,(int)(num_par),(int)(num_par),w,v);//singular value decomposition
	
	/// inverse A = V*[diag(1/wj)]*U' 
	for(i=1;i<=num_par;i++)
		for(j=1;j<=num_par;j++)
			v[j][i] = (v[j][i]/w[i]);
	
	for(i=1;i<=num_par;i++)
		for(j=1;j<=num_par;j++){
			tmp=0.0;
			for(k=1;k<=num_par;k++)
				tmp += (v[i][k]*a[j][k]);
			tmp_sol[i][j]=tmp;
		}
	/// copy tmp_sol[][] to sol[]
	for(i=1;i<=num_par;i++)
		for(j=1;j<=num_par;j++)
			sol[(i-1)*num_par+(j-1)]=(float)tmp_sol[i][j];
	
	/// free
	free_dmatrix(a,1L,(long)num_par,1L,(long)num_par);
	free_dvector(w,1L,(long)num_par);
	free_dmatrix(v,1L,(long)num_par,1L,(long)num_par);
	free_dmatrix(tmp_sol,1L,(long)num_par,1L,(long)num_par);
}		                                          

void svdcmp(double  **a, int m, int n, double  w[], double  **v)
{
	double  pythag(double  a, double  b);
	int flag,i,its,j,jj,k,l,nm;
	double  anorm,c,f,g,h,s,scale,x,y,z,*rv1;

	rv1=dvector(1L,(long)n);
	g=scale=anorm=0.0;
	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i]=scale*g;
		g=s=scale=0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += fabs(a[k][i]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}
				f=a[i][i];
				g = -SIGN(sqrt(s),f);
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
			for (k=l;k<=n;k++) scale += fabs(a[i][k]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g = -SIGN(sqrt(s),f);
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
		anorm=FMAXD(anorm,(fabs(w[i])+fabs(rv1[i])));
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
			g=1.0/g;
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
				if ((double )(fabs(rv1[l])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((double )(fabs(w[nm])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if ((double )(fabs(f)+anorm) == anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=1.0/h;
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

			x=w[l];
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=pythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
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
					z=1.0/z;
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
	free_dvector(rv1,1L,(long)n);
} 
void svbksb(double  **u, double  w[], double  **v, int m, int n, double  b[], double  x[])
{
	int jj,j,i;
	double  s,*tmp;

	tmp=dvector(1L,(long)n);
	for (j=1;j<=n;j++) {
		s=0.0;
		if (w[j]) {
			for (i=1;i<=m;i++) s += u[i][j]*b[i];
			s /= w[j];
		}
		tmp[j]=s;
	}
	for (j=1;j<=n;j++) {
		s=0.0;
		for (jj=1;jj<=n;jj++) s += v[j][jj]*tmp[jj];
		x[j]=s;
	}
	free_dvector(tmp,1L,(long)n);
}
void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
	char string[200];
	sprintf(string,
		"Numerical Recipes run-time error...\n %s \n...now exiting to system...",error_text);
	AfxMessageBox(string,MB_OK);
	exit(1);
}

float *vector(long nl, long nh)
/* allocate a float vector with subscript range v[nl..nh] */
{
	float *v;

	v=(float *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl+NR_END;
}

int *ivector(long nl, long nh)
/* allocate an int vector with subscript range v[nl..nh] */
{
	int *v;

	v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl+NR_END;
}
void free_vector(float *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
void free_ivector(int *v,long nl,long nh)
/* free an int vector allocated with ivector() */
{
	free((FREE_ARG)(v+nl-NR_END));
}
float **matrix(long nrl, long nrh, long ncl, long nch)
/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
{
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	float **m;

	/* allocate pointers to rows */
	m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	/* allocate rows and set pointers to them */
	m[nrl]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;
}
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch)
/* free a float matrix allocated by matrix() */
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}


double *dvector(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
	double *v;

	v=(double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
	
	return v-nl+NR_END;
}
double **dmatrix(long nrl, long nrh, long ncl, long nch)
/* allocate a double matrix with subscript range m[nrl..nrh][ncl..nch] */
{
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	double **m;

	/* allocate pointers to rows */
	m=(double **) malloc((size_t)((nrow+NR_END)*sizeof(double*)));
	
	m += NR_END;
	m -= nrl;

	/* allocate rows and set pointers to them */
	m[nrl]=(double *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(double)));
	
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

	/* return pointer to array of pointers to rows */
	return m;
}
void free_dvector(double *v, long nl, long nh)
/* free a double vector allocated with dvector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch)
/* free a double matrix allocated by dmatrix() */
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}
double  pythag(double  a, double  b)
{
	double  absa,absb;
	absa=fabs(a);
	absb=fabs(b);
	if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}

//----------------------------------------------------------------------------                 
// Nonlinear Optimization 관련 프로그램 						  
// Levenberg-Marquardt Method in Numerical Recipe in C			  
//----------------------------------------------------------------------------                 
void mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ma,
	int lista[],int mfit,float **covar, float **alpha, float *chisq,						  
	void (*funcs)(float [], float [], float *, float []),float *alamda)
{
	int k,kk,j,ihit;
	static float ochisq;
	float pre_chisq;
	float *da,*atry,**oneda,*beta;

	oneda=matrix(1,mfit,1,1);
	atry=vector(1,ma);
	da=vector(1,ma);
	beta=vector(1,ma);

	kk=mfit+1;
	for(j=1;j<=ma;j++){
		ihit=0;
		for(k=1;k<=mfit;k++)
			if(lista[k]==j) ihit++;
		if(ihit==0)
			lista[kk++]=j;
		else if (ihit>1) AfxMessageBox("Bad LISTA permutation in MRQMIN-1",MB_OK);
	}
	if(kk != ma+1) AfxMessageBox("Bad LISTA permutation in MRQMIN-2",MB_OK);
	
	//------------------------------------------------------------------------
	// 초기 시작조건임 
	//------------------------------------------------------------------------
	if ((*alamda) < 0.0) { 
		(*alamda)=0.01F;
		mrqcof(x,y,sig,ndata,a,ma,lista,mfit,alpha,beta,chisq,funcs);
		ochisq=(*chisq);
	}		
	for(j=1;j<=mfit;j++){
		for(k=1;k<=mfit;k++) covar[j][k]=alpha[j][k];
		covar[j][j]=alpha[j][j]*(1.0F+(*alamda));
		oneda[j][1]=beta[j];
	}
	gaussj(covar,mfit,oneda,1);
	for(j=1;j<=mfit;j++)
		da[j]=oneda[j][1];

	// 끝나는 조건임 
	if(*alamda==0.0){
		covsrt(covar,ma,lista,mfit);
		return;
	}

	for(j=1;j<=ma;j++) atry[j]=a[j];
	for(j=1;j<=mfit;j++)
		atry[lista[j]]=a[lista[j]]+da[j];
	
	pre_chisq=(*chisq); ///추가 
	mrqcof(x,y,sig,ndata,atry,ma,lista,mfit,covar,da,chisq,funcs);
	
	if((*chisq)<ochisq){
		(*alamda) *= 0.1F;
		ochisq=(*chisq);
		for(j=1;j<=mfit;j++){
			for(k=1;k<=mfit;k++) alpha[j][k]=covar[j][k];
			beta[j]=da[j];
			a[lista[j]]=atry[lista[j]];
		}
		//AfxMessageBox("(*chisq)<ochisq occurred !!!",MB_OK);
	} else {
		(*alamda) *= 10.0F; ///변경 
		(*chisq)=ochisq;
	}
	free_vector(beta,1,ma);
	free_vector(da,1,ma);
	free_vector(atry,1,ma);
	free_matrix(oneda,1,mfit,1,1);
	return;
}

//------------------------------------------------------------------------
// new version:1997.2.11 이후
///////////////////////////// 수정전 ////////////////////////////////////////
//void mrqcof(float x[], float y[], float sig[], int ndata, float a[], int ia[],
//	int ma, float **alpha, float beta[], float *chisq,
//	void (*funcs)(float, float [], float *, float [], int))
/////////////////////////////////////////////////////////////////////
// F-matrix 구하는 과정에 맞게 수정 
// x[]: [1,...,N], y[]: 사용하지 않음 
// sig[]:[1,...,N-1]
//------------------------------------------------------------------------
void mrqcof(float x[], float y[], float sig[], int ndata, float a[],int ma,
			int lista[],int mfit,float **alpha, float beta[], float *chisq,						  
			void (*funcs)(float [], float [], float *, float []))					  
{
	int i,j,k;
	float ymod,wt,sig2i,dy,*dyda;
	float *tmp_in;

	dyda=vector(1L,(long)ma);
	tmp_in=vector(1L,4L);
	
	/// 초기화 
	for (j=1;j<=mfit;j++) {
		for (k=1;k<=j;k++) alpha[j][k]=0.0F;
		beta[j]=0.0F;
	}
	(*chisq)=0.0F;
	for (i=1;i<=ndata;i++) {
		if(sig[i]!=0){ /// 1997/2/24추가 
			tmp_in[1]=x[(i-1)*4+1];
			tmp_in[2]=x[(i-1)*4+2];
			tmp_in[3]=x[(i-1)*4+3];
			tmp_in[4]=x[(i-1)*4+4];
			(*funcs)(tmp_in,a,&ymod,dyda);
			sig2i=1.0F/(sig[i]*sig[i]);
			//------------------------------------------------------------------------
			// Fundamental matrix를 구하는 과정에 맞게 수정 
			//------------------------------------------------------------------------
			dy=-ymod;
			for(j=1;j<=mfit;j++) {
				wt=dyda[lista[j]]*sig2i;
				for(k=1;k<=j;k++)
					alpha[j][k] += (wt*dyda[lista[k]]);
  					beta[j] += (dy*wt);
			}
			(*chisq) += (dy*dy*sig2i);
		}
	}
	/// 추가 : 1997/2/21
	for(j=1;j<=mfit;j++){
		for(k=1;k<=j;k++) alpha[j][k] /= ndata;
		beta[j] /= (ndata);
	}
	(*chisq) /= ndata;
	
	/*
	char string[100];
	sprintf(string,"chisq:%f",*chisq);
	AfxMessageBox(string,MB_OK);
	*/
	/// 추가 끝 
	
	for (j=2;j<=mfit;j++)
		for (k=1;k<=j-1;k++) alpha[k][j]=alpha[j][k];
	free_vector(dyda,1L,(long)ma);
	free_vector(tmp_in,1L,4L);
}

//------------------------------------------------------------------------
void covsrt(float **covar, int ma, int ia[], int mfit)
//------------------------------------------------------------------------
{
	int i,j,k;
	float swap;

	for (i=mfit+1;i<=ma;i++)
		for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0F;
	k=mfit;
	for (j=ma;j>=1;j--) {
		if (ia[j]) {
			for (i=1;i<=ma;i++) SWAP_C(covar[i][k],covar[i][j])
			for (i=1;i<=ma;i++) SWAP_C(covar[k][i],covar[j][i])
			k--;
		}
	}
}

//----------------------------------------------------------------------------                 
void gaussj(float **a, int n, float **b, int m)
//----------------------------------------------------------------------------                 
{
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv,temp;

	indxc=ivector(1,n);
	indxr=ivector(1,n);
	ipiv=ivector(1,n);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) {
		big=0.0F;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=(float)fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv[k] > 1) nrerror("gaussj: Singular Matrix-1");
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=1;l<=n;l++) SWAP_G(a[irow][l],a[icol][l])
			for (l=1;l<=m;l++) SWAP_G(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix-2");
		pivinv=1.0F/a[icol][icol];
		a[icol][icol]=1.0F;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++)
			if (ll != icol) {
				dum=a[ll][icol];
				a[ll][icol]=0.0F;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}
	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP_G(a[k][indxr[l]],a[k][indxc[l]]);
	}
	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
}

//------------------------------------------------------------------------
// new version: 1997.2.11
// Fundamental Matrix를 대응점과 eipolar line과의 거리를 minimize하여 
// 구하는 것과 관련된 함수  
// 한 쌍의 대응점 (u,v)-(u',v')을 준 경우 dF/da1,dF/da2, ... ,dF/da9
// 와 이미지 1에서의 dis(=*y)를 돌려줌 (a1=f11,...,a9=f33)
// a[] : fitting parameter :여기서는 Fundamental Matrix(f11,f12,...,f33)
// x[1,...,4] : 한 쌍의 대응점 (u,v)-(u',v') : u,u':col v,v':row
// matrix의 index는 1부터 시작 
//------------------------------------------------------------------------
void funcs(float x[],float a[],float *y,float dyda[])
{
	int i;
	float mfm,l11,l12,l21,l22,coml1,coml2,comwi;
	float tmp1,tmp2;
	
	float dl11da[9],dl12da[9],dl21da[9],dl22da[9];
	float dwida[9],dfda[9];

	/// common variable							   
	///		 | a        b    -ax'-by'			   |
	///  F = | c	    d	 -cx'-dy'			   |
	///		 |-ax-cy -bx-dy  x(ax'+by')+y(cx'+dy') |
	///  (a,b,c,d)' : epipolar transform
	///  (x,y),(x',y') : epipole in image 1 & 2

	/// mfm= (m')'Fm
	/// (a  ,  b,   c,   d,   x,   y,   x',   y') - a[1] ~ a[8]
	/// a[1] a[2] a[3] a[4]	a[5] a[6] a[7]	a[8]
	mfm=x[1]*x[3]*a[1]+x[1]*x[4]*a[3]-x[1]*a[1]*a[5]-x[1]*a[3]*a[6];
	mfm += (x[2]*x[3]*a[2]+x[2]*x[4]*a[4]-x[2]*a[2]*a[5]-x[2]*a[4]*a[6]);
	mfm += (-x[3]*a[1]*a[7]-x[3]*a[2]*a[8]-x[4]*a[3]*a[7]-x[4]*a[4]*a[8]);
	mfm += (a[1]*a[5]*a[7]+a[5]*a[2]*a[8]+a[6]*a[3]*a[7]+a[6]*a[4]*a[8]);
	
	///	l11=au'+cv'-ax-cy       l12=bu'+dv'-bx-dy
	/// l21=au+bv-ax'-by'       l22=cu+dv-cx'-dy'
	l11=a[1]*x[3]+a[3]*x[4]-a[1]*a[5]-a[3]*a[6];
	l12=a[2]*x[3]+a[4]*x[4]-a[2]*a[5]-a[4]*a[6];
	l21=a[1]*x[1]+a[2]*x[2]-a[1]*a[7]-a[2]*a[8];
	l22=a[3]*x[1]+a[4]*x[2]-a[3]*a[7]-a[4]*a[8];

	coml1=(l11*l11)+(l12*l12)+(l21*l21)+(l22*l22);
	coml2=((l11*l11)+(l12*l12))*((l21*l21)+(l22*l22));
	comwi=coml1/coml2;
	comwi=(float)sqrt(comwi);

	for(i=0;i<=8;i++) {
		dl11da[i]=dl12da[i]=dl21da[i]=dl22da[i]=0.0F;
	}
	/// 
	dl11da[1]=x[3]-a[5]; dl11da[3]=x[4]-a[6];  dl11da[5]=-a[1];  dl11da[6]=-a[3];
	dl12da[2]=x[3]-a[5]; dl12da[4]=x[4]-a[6]; dl12da[5]=-a[2];  dl12da[6]=-a[4];
	dl21da[1]=x[1]-a[7]; dl21da[2]=x[2]-a[8];  dl21da[7]=-a[1];  dl21da[8]=-a[2];
	dl22da[3]=x[1]-a[7]; dl22da[4]=x[2]-a[8];  dl22da[7]=-a[3];  dl22da[8]=-a[4];

	/// dfda[9]: dfda(=d(m'Fm)/da)
	dfda[1]=x[1]*x[3]-x[1]*a[5]-x[3]*a[7]+a[5]*a[7];
	dfda[2]=x[2]*x[3]-x[2]*a[5]-x[3]*a[8]+a[5]*a[8];
	dfda[3]=x[1]*x[4]-x[1]*a[6]-x[4]*a[7]+a[6]*a[7];
	dfda[4]=x[2]*x[4]-x[2]*a[6]-x[4]*a[8]+a[6]*a[8];
	dfda[5]=-x[1]*a[1]-x[2]*a[2]+a[1]*a[7]+a[2]*a[8];
	dfda[6]=-x[1]*a[3]-x[2]*a[4]+a[3]*a[7]+a[4]*a[8];
	dfda[7]=-x[3]*a[1]-x[4]*a[3]+a[1]*a[5]+a[6]*a[3];
	dfda[8]=-x[3]*a[2]-x[4]*a[4]+a[5]*a[2]+a[6]*a[4];
	
	/// dwi/da
	for(i=1;i<=8;i++){
		tmp1=2*(l11*dl11da[i]+l12*dl12da[i]+l21*dl21da[i]+l22*dl22da[i]);
		tmp2=((l21*l21)+(l22*l22))*2*(l11*dl11da[i]+l12*dl12da[i])
			 +((l11*l11)+(l12*l12))*2*(l21*dl21da[i]+l22*dl22da[i]);
		dwida[i]= 0.5F*((tmp1*coml2)-(coml1*tmp2)) /(coml2*coml2*comwi);
	}	
	
	for(i=1;i<=8;i++)
		dyda[i]=dwida[i]*mfm+comwi*dfda[i];
	(*y) = (float)(comwi*mfm);
}

//------------------------------------------------------------------------
// Fundamental Matrix를 대응점과 eipolar line과의 거리를 minimize하여 
// 구하는 것과 관련된 함수  
// 한 쌍의 대응점 (u,v)-(u',v')을 준 경우 dF/da1,dF/da2, ... ,dF/da9
// 와 이미지 1에서의 dis(=*y)를 돌려줌 (a1=f11,...,a9=f33)
// a[] : fitting parameter :여기서는 Fundamental Matrix(f11,f12,...,f33)
// x[1,...,4] : 한 쌍의 대응점 (u,v)-(u',v')
// matrix의 index는 1부터 시작 
//------------------------------------------------------------------------
void fdis1(float x[],float a[],float *y,float dyda[])
{
	float tmp1,tmp1_2,tmp3,tmp;

	tmp1_2=(a[1]*x[1]+a[2]*x[2]+a[3])*(a[1]*x[1]+a[2]*x[2]+a[3])+
		 (a[4]*x[1]+a[5]*x[2]+a[6])*(a[4]*x[1]+a[5]*x[2]+a[6]);			 
	tmp1=(float)sqrt(tmp1_2);
	tmp3=a[1]*x[1]*x[3]+a[2]*x[3]*x[2]+a[3]*x[3]+a[4]*x[1]*x[4]+a[5]*x[2]*x[4]+
		 a[6]*x[4]+a[7]*x[1]+a[8]*x[2]+a[9];
    /// dF/da1
	tmp=(a[1]*x[1]+a[2]*x[2]+a[3])*x[1]/tmp1;
	dyda[1]=(x[1]*x[3]*tmp1-tmp3*tmp)/tmp1_2;
    /// dF/da2
	tmp=(a[1]*x[1]+a[2]*x[2]+a[3])*x[2]/tmp1;
	dyda[2]=(x[3]*x[2]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da3	
	tmp=(a[1]*x[1]+a[2]*x[2]+a[3])/tmp1;
	dyda[3]=(x[3]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da4	
	tmp=(a[4]*x[1]+a[5]*x[2]+a[6])*x[1]/tmp1;
	dyda[4]=(x[1]*x[4]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da5	
	tmp=(a[4]*x[1]+a[5]*x[2]+a[6])*x[2]/tmp1;
	dyda[5]=(x[2]*x[4]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da6	
	tmp=(a[4]*x[1]+a[5]*x[2]+a[6])/tmp1;
	dyda[6]=(x[4]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da7	
	dyda[7]=x[1]/tmp1;
	/// dF/da8	
	dyda[8]=x[2]/tmp1;
	/// dF/da9	
	dyda[9]=1.0F/tmp1;
 	/// 거리 계산 
	*y=(float)(tmp3/tmp1);
}

//------------------------------------------------------------------------
// 한 쌍의 대응점 (u,v)-(u',v')을 준 경우 dF/da1,dF/da2, ... ,dF/da9
// 와 이미지 2에서의 dis(=*y)를 돌려줌 (a1=f11,...,a9=f33)
// a[] : fitting parameter :여기서는 Fundamental Matrix(f11,f12,...,f33)
// x[1,...,4] : 한 쌍의 대응점 (u,v)-(u',v')
// matrix의 index는 1부터 시작 
//------------------------------------------------------------------------
void fdis2(float x[],float a[],float *y,float dyda[])
{
	float tmp1,tmp1_2,tmp3,tmp;

	tmp1_2=(a[1]*x[3]+a[4]*x[4]+a[7])*(a[1]*x[3]+a[4]*x[4]+a[7])+
		   (a[2]*x[3]+a[5]*x[4]+a[8])*(a[2]*x[3]+a[5]*x[4]+a[8]);			 
	tmp1=(float)sqrt(tmp1_2);
	
	tmp3=a[1]*x[1]*x[3]+a[2]*x[3]*x[2]+a[3]*x[3]+a[4]*x[1]*x[4]+a[5]*x[2]*x[4]+
		 a[6]*x[4]+a[7]*x[1]+a[8]*x[2]+a[9];
    /// dF/da1
	tmp=(a[1]*x[3]+a[4]*x[4]+a[7])*x[3]/tmp1;
	dyda[1]=(x[1]*x[3]*tmp1-tmp3*tmp)/tmp1_2;
    /// dF/da2
	tmp=(a[2]*x[3]+a[5]*x[4]+a[8])*x[3]/tmp1;
	dyda[2]=(x[3]*x[2]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da3	
	dyda[3]=x[3]/tmp1_2;
	/// dF/da4	
	tmp=(a[1]*x[3]+a[4]*x[4]+a[7])*x[4]/tmp1;
	dyda[4]=(x[1]*x[4]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da5	
	tmp=(a[2]*x[3]+a[5]*x[4]+a[8])*x[4]/tmp1;
	dyda[5]=(x[2]*x[4]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da6	
	dyda[6]=x[4]/tmp1_2;
	/// dF/da7	
	tmp=(a[1]*x[3]+a[4]*x[4]+a[7])/tmp1;
	dyda[7]=(x[1]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da8	
	tmp=(a[2]*x[3]+a[5]*x[4]+a[8])/tmp1;
	dyda[8]=(x[2]*tmp1-tmp3*tmp)/tmp1_2;
	/// dF/da9	
	dyda[9]=1.0F/tmp1;
 	/// 거리 계산 
	*y=(float)(tmp3/tmp1);

}

//------------------------------------------------------------------------
// Eigenvalue, Eigenvector 구하는 routine 첨가 
// !!! : Matlab 결과와 비교를 거침--> O.K!!! (1996.10.20)
//------------------------------------------------------------------------
#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
	a[k][l]=h+s*(g-h*tau);
//------------------------------------------------------------------------
//  in[1,...,NUN_DATA][1,..,NUM_PARAMETER]
//  sol[0,...,NUM_PARAMETER-1] 
//  input으로  in matrix를 넘기면 in'in의 minimum eigenvalue에 해당하는 
//  eigenvector를 sol[]에다 리턴 
//------------------------------------------------------------------------
void get_eig(float **in,float *sol,int numdata,int numpar)
{
	 float **a,**v,*d,val1,val2;
	 int nrot,i,j,k;

	 a=matrix(1L,(long)numpar,1L,(long)numpar);
	 v=matrix(1L,(long)numpar,1L,(long)numpar);
	 d=vector(1L,(long)numpar);

	 for(i=1;i<=numpar;i++)
		 for(j=1;j<=numpar;j++)
			 a[i][j]=0.0F;
	 // a=in'*in
	 for(i=1;i<=numpar;i++)
		 for(j=1;j<=numpar;j++)
			 for(k=1;k<=numdata;k++){
		         val1=in[k][i];
				 val2=in[k][j];
		         a[i][j] += (val1*val2);
			 }
	 jacobi(a,numpar,d,v,&nrot);
     eigsrt(d,v,numpar);
     
	 // temp 
	 FILE *hfile;
	 hfile=fopen("c:\\user\\jeha\\data\\mineigen.dat","a+");
	 fprintf(hfile,"\n");
	 for(i=1;i<=numpar;i++)	 
		 fprintf(hfile,"%f |",d[i]);
	 fprintf(hfile,"\n");
	 fclose(hfile);

	 for(i=1;i<=numpar;i++)
		 sol[i-1]=v[i][numpar];

	 free_matrix(a,1L,(long)numpar,1L,(long)numpar);
	 free_matrix(v,1L,(long)numpar,1L,(long)numpar);
	 free_vector(d,1L,(long)numpar);
}
//------------------------------------------------------------------------
// a[][] : input, d[] : eigenvalue, v[][]:eigenvector in coulumn
// *nrot : number of jacobi rotaion 
//------------------------------------------------------------------------
void jacobi(float **a, int n, float d[], float **v, int *nrot)
{
	int j,iq,ip,i;
	float tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

	b=vector(1,n);
	z=vector(1,n);
	for (ip=1;ip<=n;ip++) {
		for (iq=1;iq<=n;iq++) v[ip][iq]=0.0F;
		v[ip][ip]=1.0F;
	}
	for (ip=1;ip<=n;ip++) {
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0F;
	}
	(*nrot)=0;
	for (i=1;i<=50;i++) {
		sm=0.0F;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++)
				sm +=((float) fabs(a[ip][iq]));
		}
		if (sm == 0.0) {
			free_vector(z,1,n);
			free_vector(b,1,n);
			return;
		}
		if (i < 4)
			tresh=0.2F*sm/(n*n);
		else
			tresh=0.0F;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++) {
				g=100.0F* ((float)fabs(a[ip][iq]));
				if (i > 4 && (float)(fabs(d[ip])+g) == (float)fabs(d[ip])
					&& (float)(fabs(d[iq])+g) == (float)fabs(d[iq]))
					a[ip][iq]=0.0F;
				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if ((float)(fabs(h)+g) == (float)fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5F*h/(a[ip][iq]);
						t=1.0F/( (float)(fabs(theta)+sqrt(1.0+theta*theta)) );
						if (theta < 0.0) t = -t;
					}
					c=(float)( 1.0F/sqrt(1+t*t) );
					s=t*c;
					tau=s/(1.0F+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0F;
					for (j=1;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<=n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1;j<=n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++(*nrot);
				}
			}
		}
		for (ip=1;ip<=n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0.0F;
		}
	}
	nrerror("Too many iterations in routine jacobi");
}
//------------------------------------------------------------------------------------
// sorting eigenvalue and corresponding eigenvector in descending order
//------------------------------------------------------------------------------------
void eigsrt(float d[], float **v, int n)
{
	int k,j,i;
	float p;

	for (i=1;i<n;i++) {
		p=d[k=i];
		for (j=i+1;j<=n;j++)
			if (d[j] >= p) p=d[k=j];
		if (k != i) {
			d[k]=d[i];
			d[i]=p;
			for (j=1;j<=n;j++) {
				p=v[j][i];
				v[j][i]=v[j][k];
				v[j][k]=p;
			}
		}
	}
}
//------------------------------------------------------------------------------------
// in[0...num-1][0...num-1], sol[0...num-1]
// 주의 : jacobi : real symmetric matrix의 경우 적용
// real non-symmetric matrix의 경우 
// balanc(각 row와 column의 norm을 비슷한 크기로 조정해줌) -> 
// elmhes(reduce a general matrix to Hessenberg form using Gaussian elimination) ->
// hqr ( finding the eigenvalues of a Hessenberg matrix using the QR algorithm)
// in[0,...,(NXN-1)]
//------------------------------------------------------------------------------------
void get_eigenvale_nxn_nonsymmetric(float in[],float *sol,int *num_sol,int num)
{
	 float *wr,*wi,**a;
	 int i,j,count;

	 a=matrix(1L,(long)num,1L,(long)num);
	 wr=vector(1L,(long)num);
	 wi=vector(1L,(long)num);
	 
	 for(i=1;i<=num;i++)
		 for(j=1;j<=num;j++)
			 a[i][j]=in[(i-1)*num+j-1];

	 balanc(a,num);
	 elmhes(a,num);
	 hqr(a,num,wr,wi);

	 count=0;
	 for(i=1;i<=num;i++)
		 if( (float)fabs(wi[i]) < 1e-15 ){
			sol[count]=wr[i];
			count++;
		 }	
	 (*num_sol)=count;
 
	 free_matrix(a,1L,(long)num,1L,(long)num);
	 free_vector(wr,1L,(long)num);
	 free_vector(wi,1L,(long)num);
}

#define RADIX 2.0F
void balanc(float **a, int n)
{
	int last,j,i;
	float s,r,g,f,c,sqrdx;

	sqrdx=RADIX*RADIX;
	last=0;
	while (last == 0) {
		last=1;
		for (i=1;i<=n;i++) {
			r=c=0.0F;
			for (j=1;j<=n;j++)
				if (j != i) {
					c += ( (float)fabs(a[j][i]) );
					r += ( (float)fabs(a[i][j]) );
				}
			if (c && r) {
				g=r/RADIX;
				f=1.0;
				s=c+r;
				while (c<g) {
					f *= RADIX;
					c *= sqrdx;
				}
				g=r*RADIX;
				while (c>g) {
					f /= RADIX;
					c /= sqrdx;
				}
				if ((c+r)/f < 0.95*s) {
					last=0;
					g=(float)(1.0/f);
					for (j=1;j<=n;j++) a[i][j] *= g;
					for (j=1;j<=n;j++) a[j][i] *= f;
				}
			}
		}
	}
}
#undef RADIX


#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}
void elmhes(float **a, int n)
{
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
#undef SWAP


void hqr(float **a, int n, float wr[], float wi[])
{
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
					p=(float)(0.5*(y-x));
					q=p*p+w;
					z=(float)sqrt(fabs(q));
					x += t;
					if (q >= 0.0) {
						z=(float)(p+SIGN(z,p));
						wr[nn-1]=wr[nn]=x+z;
						if (z) wr[nn]=x-w/z;
						wi[nn-1]=wi[nn]=0.0;
					} else {
						wr[nn-1]=wr[nn]=x+p;
						wi[nn-1]= -(wi[nn]=z);
					}
					nn -= 2;
				} else {
					if (its == 30) nrerror("Too many iterations in hqr");
					if (its == 10 || its == 20) {
						t += x;
						for (i=1;i<=nn;i++) a[i][i] -= x;
						s=(float)(fabs(a[nn][nn-1])+fabs(a[nn-1][nn-2]));
						y=x=(float)(0.75*s);
						w = (float)(-0.4375*s*s);
					}
					++its;
					for (m=(nn-2);m>=l;m--) {
						z=a[m][m];
						r=x-z;
						s=y-z;
						p=(r*s-w)/a[m+1][m]+a[m][m+1];
						q=a[m+1][m+1]-z-r-s;
						r=a[m+2][m+1];
						s=(float)(fabs(p)+fabs(q)+fabs(r));
						p /= s;
						q /= s;
						r /= s;
						if (m == l) break;
						u=(float)(fabs(a[m][m-1])*(fabs(q)+fabs(r)));
						v=(float)(fabs(p)*(fabs(a[m-1][m-1])+fabs(z)+fabs(a[m+1][m+1])));
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
							if ((x=(float)(fabs(p)+fabs(q)+fabs(r))) != 0.0) {
								p /= x;
								q /= x;
								r /= x;
							}
						}
						if ((s=(float)SIGN(sqrt(p*p+q*q+r*r),p)) != 0.0) {
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
//--------------------------------------------
// root finding in polynominal 
//--------------------------------------------
#define ITMAX 20
#define TINY 1.0e-6
void qroot(float p[], int n, float *b, float *c, float eps)
{
	void poldiv(float u[], int n, float v[], int nv, float q[], float r[]);
	int iter;
	float sc,sb,s,rc,rb,r,dv,delc,delb;
	float *q,*qq,*rem;
	float d[3];

	q=vector(0,n);
	qq=vector(0,n);
	rem=vector(0,n);
	d[2]=1.0F;
	for (iter=1;iter<=ITMAX;iter++) {
		d[1]=(*b);
		d[0]=(*c);
		poldiv(p,n,d,2,q,rem);
		s=rem[0];
		r=rem[1];
		poldiv(q,(n-1),d,2,qq,rem);
		sb = -(*c)*(rc = -rem[1]);
		rb = -(*b)*rc+(sc = -rem[0]);
		dv=1.0F/(sb*rc-sc*rb);
		delb=(r*sc-s*rc)*dv;
		delc=(-r*sb+s*rb)*dv;
		*b += (delb=(r*sc-s*rc)*dv);
		*c += (delc=(-r*sb+s*rb)*dv);
		if ((fabs(delb) <= eps*fabs(*b) || fabs(*b) < TINY)
			&& (fabs(delc) <= eps*fabs(*c) || fabs(*c) < TINY)) {
			free_vector(rem,0,n);
			free_vector(qq,0,n);
			free_vector(q,0,n);
			return;
		}
	}
	nrerror("Too many iterations in routine qroot");
}
void poldiv(float u[], int n, float v[], int nv, float q[], float r[])
{
	int k,j;

	for (j=0;j<=n;j++) {
		r[j]=u[j];
		q[j]=0.0F;
	}
	for (k=n-nv;k>=0;k--) {
		q[k]=r[nv+k]/v[nv];
		for (j=nv+k-1;j>=k;j--) r[j] -= q[k]*v[j-k];
	}
	for (j=nv;j<=n;j++) r[j]=0.0F;
}
//----------------------------------------------------------
// Complex를 처리하기 위한 함수 :start
//----------------------------------------------------------
fcomplex Cadd(fcomplex a, fcomplex b)
{
	fcomplex c;
	c.r=a.r+b.r;
	c.i=a.i+b.i;
	return c;
}

fcomplex Csub(fcomplex a, fcomplex b)
{
	fcomplex c;
	c.r=a.r-b.r;
	c.i=a.i-b.i;
	return c;
}


fcomplex Cmul(fcomplex a, fcomplex b)
{
	fcomplex c;
	c.r=a.r*b.r-a.i*b.i;
	c.i=a.i*b.r+a.r*b.i;
	return c;
}

fcomplex Complex(float re, float im)
{
	fcomplex c;
	c.r=re;
	c.i=im;
	return c;
}

fcomplex Conjg(fcomplex z)
{
	fcomplex c;
	c.r=z.r;
	c.i = -z.i;
	return c;
}

fcomplex Cdiv(fcomplex a, fcomplex b)
{
	fcomplex c;
	float r,den;
	if (fabs(b.r) >= fabs(b.i)) {
		r=b.i/b.r;
		den=b.r+r*b.i;
		c.r=(a.r+r*a.i)/den;
		c.i=(a.i-r*a.r)/den;
	} else {
		r=b.r/b.i;
		den=b.i+r*b.r;
		c.r=(a.r*r+a.i)/den;
		c.i=(a.i*r-a.r)/den;
	}
	return c;
}

float Cabs(fcomplex z)
{
	float x,y,ans,temp;
	x=(float)fabs(z.r);
	y=(float)fabs(z.i);
	if (x == 0.0)
		ans=y;
	else if (y == 0.0)
		ans=x;
	else if (x > y) {
		temp=y/x;
		ans=(float)( x*sqrt(1.0+temp*temp) );
	} else {
		temp=x/y;
		ans=(float)( y*sqrt(1.0+temp*temp) );
	}
	return ans;
}

fcomplex Csqrt(fcomplex z)
{
	fcomplex c;
	float x,y,w,r;
	if ((z.r == 0.0) && (z.i == 0.0)) {
		c.r=0.0F;
		c.i=0.0F;
		return c;
	} else {
		x=(float)fabs(z.r);
		y=(float)fabs(z.i);
		if (x >= y) {
			r=y/x;
			w=(float)( sqrt(x)*sqrt(0.5*(1.0+sqrt(1.0+r*r))) );
		} else {
			r=x/y;
			w=(float)( sqrt(y)*sqrt(0.5*(r+sqrt(1.0+r*r))) );
		}
		if (z.r >= 0.0) {
			c.r=w;
			c.i=z.i/(2.0F*w);
		} else {
			c.i=(z.i >= 0) ? w : -w;
			c.r=z.i/(2.0F*c.i);
		}
		return c;
	}
}

fcomplex RCmul(float x, fcomplex a)
{
	fcomplex c;
	c.r=x*a.r;
	c.i=x*a.i;
	return c;
}
//----------------------------------------------------------
// Complex를 처리하기 위한 함수 :end
//----------------------------------------------------------


//----------------------------------------------------------
// polynomial의 근을 구하는 프로그램 
// laguer() in Numerical Recipe in C 
//----------------------------------------------------------
#define EPSS 1.0e-7F
#define MR 8
#define MT 10
#define MAXIT (MT*MR)

void laguer(fcomplex a[], int m, fcomplex *x, int *its)
{
	int iter,j;
	float abx,abp,abm,err;
	fcomplex dx,x1,b,d,f,g,h,sq,gp,gm,g2;
	static float frac[MR+1] = {0.0F,0.5F,0.25F,0.75F,0.13F,0.38F,0.62F,0.88f,1.0F};

	for (iter=1;iter<=MAXIT;iter++) {
		*its=iter;
		b=a[m];
		err=Cabs(b);
		d=f=Complex(0.0F,0.0F);
		abx=Cabs(*x);
		for (j=m-1;j>=0;j--) {
			f=Cadd(Cmul(*x,f),d);
			d=Cadd(Cmul(*x,d),b);
			b=Cadd(Cmul(*x,b),a[j]);
			err=Cabs(b)+abx*err;
		}
		err *= EPSS;
		if (Cabs(b) <= err) return;
		g=Cdiv(d,b);
		g2=Cmul(g,g);
		h=Csub(g2,RCmul(2.0F,Cdiv(f,b)));
		sq=Csqrt(RCmul((float) (m-1),Csub(RCmul((float) m,h),g2)));
		gp=Cadd(g,sq);
		gm=Csub(g,sq);
		abp=Cabs(gp);
		abm=Cabs(gm);
		if (abp < abm) gp=gm;
		dx=((FMAX(abp,abm) > 0.0 ? Cdiv(Complex((float)m,0.0F),gp)
			: RCmul( ((float)exp(log(1+abx)) ),Complex((float)cos(iter),(float)sin(iter)))));
		x1=Csub(*x,dx);
		if (x->r == x1.r && x->i == x1.i) return;
		if (iter % MT) *x=x1;
		else *x=Csub(*x,RCmul(frac[iter/MT],dx));
	}
	nrerror("too many iterations in laguer");
	return;
}
#undef EPSS
#undef MR
#undef MT
#undef MAXIT






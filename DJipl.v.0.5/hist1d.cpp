/*
 *
 *				Gyu-tae Park  1998
 */
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cimage.h"
#include "histogram.h"
//#include "gl.h"

#define M_PI	3.1415926535897932384626433832795

Hist1d::Hist1d(int m,int M,int n)
{
	int i, j;

    nbuckets = n;
    bucket = new int[nbuckets];
    pbucket = new float[nbuckets];
    mbucket = new float[nbuckets];
    wbucket = new float[nbuckets];
	sbucket = new float[nbuckets];
	
	m_bucket = new float **[3];
	w_bucket = new float **[3];

	for (i=0; i<3; i++)
	{
		m_bucket[i] = new float *[nbuckets];
		w_bucket[i] = new float *[nbuckets];
		s_bucket = new float *[nbuckets];
		for (j=0; j<nbuckets; j++)
		{
			m_bucket[i][j] = new float[nbuckets];
			w_bucket[i][j] = new float[nbuckets];
			s_bucket[j] = new float[nbuckets];
		}
	}

    min = m;
    max = M;
    clearhist();
	clearfeatures();
}

Hist1d::~Hist1d()
{
	delete []s_bucket;	s_bucket = NULL;
	delete []w_bucket;	w_bucket = NULL;
	delete []m_bucket;	m_bucket = NULL;

	delete []sbucket;	sbucket = NULL;
	delete []wbucket;	wbucket = NULL;
	delete []mbucket;	mbucket = NULL;
	delete []pbucket;	pbucket = NULL;
	delete []bucket;	bucket = NULL;
}

void Hist1d::set(int m, int M, int n)
{
    nbuckets = n;
    bucket = new int[nbuckets];
    min = m;
    max = M;
    clearhist();
	clearfeatures();
}

void Hist1d::clearhist()
{
    register int i;
    int *ptr = bucket;
    for (i=0; i<nbuckets; i++) *ptr++ = 0;
}
 
void Hist1d::clearfeatures()
{
	for (int i = 0; i < 6; i++)
		features[i] = -10000;
}

void Hist1d::addtohist(register unsigned short *sptr, register int n)
{
    register unsigned int index;

    while (n--) {
		index = *sptr++;
		if(index >= (unsigned int)nbuckets) 
			index = (unsigned int)(nbuckets-1);
		bucket[index]++;
    }
	for (int i = 0; i < nbuckets; i++){
		if (bucket[i] > max) max = bucket[i];
		if (bucket[i] < min) min = bucket[i];
	}
}

void Hist1d::addtohist(register unsigned char *sptr, register int n)
{
    register unsigned int index;

    while (n--) {
		index = *sptr++;
		if(index>=(unsigned int)nbuckets) 
			index = (unsigned int)(nbuckets-1);
		bucket[index]++;
    }
	for (int i = 0; i < nbuckets; i++) {
		if (bucket[i] > max) max = bucket[i];
		if (bucket[i] < min) min = bucket[i];
	}
}

void Hist1d::DJotsu(int *th0)
{
	int i;
	long tbucket = 0;
	float wsum = 0.0f;
	float msum = 0.0f;
	float max = -1.0f;

	for (i=0; i<nbuckets; i++)
	{
		if (bucket[i] != 0)
			tbucket += bucket[i];
	}

	for (i=0; i<nbuckets; i++)
	{
		pbucket[i] = (float)((float)bucket[i]/(float)tbucket);
		wsum += pbucket[i];
		wbucket[i] = wsum;
		msum += (i+1)*pbucket[i];
		mbucket[i] = msum;
	}

	for (i=0; i<nbuckets; i++)
	{
		sbucket[i] = 			
			//(float)((mbucket[nbuckets-1]*wbucket[i] - mbucket[i])*(mbucket[nbuckets-1]*wbucket[i] - mbucket[i]))/(wbucket[i]*(1 - wbucket[i]));
			(float)((msum*wbucket[i] - mbucket[i])*(msum*wbucket[i] - mbucket[i]))/(wbucket[i]*(1 - wbucket[i]));
	}

	for (i=0; i<nbuckets; i++)
	{
		if (sbucket[i] > max)
		{
			max = sbucket[i];
			*th0 = i;
		}
	}
}

void Hist1d::DJotsu(int *th0, int *th1)
{
	int i, j, k;
	long tbucket = 0;
	float wsum[3];
	float msum[3], mt;
	float max = -1.0f;

	for (i=0; i<3; i++)
		wsum[i] = msum[i] = 0.0f;

	for (i=0; i<nbuckets; i++)
	{
		tbucket += bucket[i];
	}

	for (k=0; k<3; k++)
		for (i=0; i<nbuckets; i++)
			for (j=0; j<nbuckets; j++)
			{
				w_bucket[k][i][j] = m_bucket[k][i][j] = 0.0f;
			}

	mt = 0.0f;
	for (i=0; i<nbuckets; i++)
	{
		pbucket[i] = (float)((float)bucket[i]/(float)tbucket);
		mt += (i+1)*pbucket[i];
	}

	for (j=0; j<nbuckets; j++)
	{
		for (i=0; i<nbuckets; i++)
		{
			for (k=0; k<3; k++)
				wsum[k] = msum[k] = 0.0f;

			for (k=0; k<i; k++)
			{
				wsum[0] += pbucket[k];
				w_bucket[0][i][0] = wsum[0];

				if (wsum[0] != 0.0f)
					msum[0] += (float)((k+1)*pbucket[k]/wsum[0]);
				m_bucket[0][i][0] = msum[0];
			}

			for (k=i; k<j; k++)
			{
				wsum[1] += pbucket[k];
				w_bucket[1][i][j] = wsum[1];

				if (wsum[1] != 0.0f)
					msum[1] += (float)((k+1)*pbucket[k]/wsum[1]);
				m_bucket[1][i][j] = msum[1];
			}

			for (k=j; k<nbuckets; k++)
			{
				wsum[2] += pbucket[k];
				w_bucket[2][0][j] = wsum[2];

				if (wsum[2] != 0.0f)
					msum[2] += (float)((k+1)*pbucket[k]/wsum[2]);
				m_bucket[2][0][j] = msum[2];
			}
		}
	}

	for (i=0; i<nbuckets; i++)
	{
		for (j=0; j<nbuckets; j++)
		{
			s_bucket[i][j] = 
			(float)(w_bucket[0][i][0]*(m_bucket[0][i][0]-mt)*(m_bucket[0][i][0]-mt)
			+ w_bucket[1][i][j]*(m_bucket[1][i][j]-mt)*(m_bucket[1][i][j]-mt)
			+ w_bucket[2][0][j]*(m_bucket[2][0][j]-mt)*(m_bucket[2][0][j]-mt));
		}
	}

	for (i=0; i<nbuckets; i++)
	{
		for (j=0; j<nbuckets; j++)
		{
			if (s_bucket[i][j] > max)
			{
				max = s_bucket[i][j];
				*th0 = i;
				*th1 = j;
			}
		}
	}
}

void Hist1d::DJkittler(int *th0)
{
	int i;
	long tbucket = 0;
	float wsum = 0.0f;
	float msum = 0.0f;
	float max = -1.0f;

	for (i=0; i<nbuckets; i++)
	{
		if (bucket[i] != 0)
			tbucket += bucket[i];
	}

	for (i=0; i<nbuckets; i++)
	{
		pbucket[i] = (float)((float)bucket[i]/(float)tbucket);
		wsum += pbucket[i];
		wbucket[i] = wsum;
		msum += (i+1)*pbucket[i];
		mbucket[i] = msum;
	}

	for (i=0; i<nbuckets; i++)
	{
		sbucket[i] = 			
			//(float)((mbucket[nbuckets-1]*wbucket[i] - mbucket[i])*(mbucket[nbuckets-1]*wbucket[i] - mbucket[i]))/(wbucket[i]*(1 - wbucket[i]));
			(float)((msum*wbucket[i] - mbucket[i])*(msum*wbucket[i] - mbucket[i]))/(wbucket[i]*(1 - wbucket[i]));
	}

	for (i=0; i<nbuckets; i++)
	{
		if (sbucket[i] > max)
		{
			max = sbucket[i];
			*th0 = i;
		}
	}
}

void Hist1d::DJfuzzy_entropy(int *th0, unsigned char *ptr, int n)
{
	int i;
	int g_max, g_min;
	const int number = 256;

	float sindex[number];
	float sbindex[number];
	float windex[number];
	float wbindex[number];
	float member[number][2];
	float entropy[number];

	i = 0;
	while(bucket[i++] != 0) ;
	g_min = i;

	i = 255;
	while(bucket[i--] != 0) ;
	g_max = i;

	float Csize = (float)(g_max - g_min);

	sindex[number-1] = windex[number-1] = 0.0f;
	for (i=0; i<number; i++)
	{
		sindex[number-1] += bucket[i];
		windex[number-1] += i*bucket[i];
	}
	
	sindex[g_min-1] = windex[g_min-1] = 0.0f;
	for (i=g_min; i<g_max; i++)
	{
		sindex[i] = sindex[i-1] + bucket[i]; 
		sbindex[i] = sindex[number-1] - sindex[i];
		windex[i] = windex[i-1] + i*bucket[i]; 
		wbindex[i] = windex[number-1] - windex[i]; 

		member[i][0] = (int)(windex[i]/sindex[i]);
		member[i][1] = (int)(wbindex[i]/sbindex[i]);

		//for (i=0


	}

}

/*
void grey(float f) {float vs[3]; vs[0] = vs[1] = vs[2] = f; c3f(vs);}
void rgb(float r, float g, float b) 
	{float vs[3]; vs[0] = r; vs[1] = g;  vs[2] = b; c3f(vs);}
void Hist1d::showhist()
{
    int i, bmax, j, total, *ptr;

    total = bmax = 0;
    ptr = bucket;
    for (i=0; i<nbuckets; i++) {
		total += *ptr;
		if(*ptr>bmax)
			bmax = *ptr;
		ptr++;
    }
    if(bmax==0) 
	total = bmax = 1;

    ortho2(0.0,(float)nbuckets,0.0,(float)bmax);
    grey(0.4);
    clear();

    grey(0.5);
    ptr = bucket;
    for (i=0; i<nbuckets; i++) 
	rectf(i+0.1,0.0,i+0.9,5.0*(float)(*ptr++));

    for (i=0; i<nbuckets; i+=10)  {
		if((i%100) == 0)
			grey(0.0);
		else
			grey(0.2);
		move2i(i,0);
		draw2i(i,bmax);
    }

    grey(0.8);
    ptr = bucket;
    for (i=0; i<nbuckets; i++) 
		rectf(i+0.1,0.0,i+0.9,(float)(*ptr++));

    rgb(1.0,0.0,0.0);
    move2i(min,0);
    draw2i(min,bmax/40);
    move2i(max,0);
    draw2i(max,bmax/40);

    ortho2(0.0,(float)nbuckets,0.0,(float)total);
    grey(0.0);
    move2i(0,0);
    j = 0;
    ptr = bucket;
    for (i=0; i<nbuckets; i++) {
		j += *ptr++;
		draw2((float)i,(float)j);
    }
}
*/
void Hist1d::histeqtable(short *tab)
{
    int i, sum, shade; 
    float maxshade;

    sum = 0;
    for(i=0; i<nbuckets; i++)
		sum += bucket[i];
    if(sum == 0) sum = 1;
    maxshade = 255.0;
    shade = 0;
    for(i=0; i<nbuckets; i++) {
		tab[i] = (short)((shade*255.0)/sum);
		shade += bucket[i];
    }
}

double Hist1d::calcMean()
{
	double  dsum = 0.0;

	for (int b = 0; b < nbuckets; b++)
		dsum += (double)( (b+1) * bucket[b]);
	return (features[Mean] = dsum / (double) nbuckets);
}

double Hist1d::calcSD()
{
	int L = nbuckets;
	double  dsum = 0.0;

	double mean = getMean();
	for (int b = 0; b < L; b++) 
		dsum += ((double)b - mean) * ((double)b - mean) * (double)bucket[b];
	
	return (features[SD] = sqrt (dsum / (double) L));
}

double Hist1d::calcSkewness()
{
	int L = nbuckets;
	double  dsum = 0.0, dtmp;
	double mean = getMean();
	double sd = getSD();

	for (int b = 0; b < L; b++) {
		dtmp = (double)b - mean;
		dsum += dtmp * dtmp * dtmp * (double)bucket[b];
	}
	
	return (features[Skewness] = (dsum / ( sd * sd * sd * (double) L)));
}

double Hist1d::calcKurtosis()
{
	int L = nbuckets;
	double  dsum = 0.0, dtmp;
	double mean = getMean();
	double sd = getSD();

	for (int b = 0; b < L; b++) {
		dtmp = (double)b - mean;
		dsum += dtmp * dtmp * dtmp * dtmp * (double)bucket[b];
	}
	
	return (features[Kurtosis] = (dsum / ( sd * sd * sd * sd * (double) L)) - 3.0);
}

double Hist1d::calcEnergy()
{
	int L = nbuckets;
	double  dsum = 0.0, dtmp;

	for (int b = 0; b < L; b++) {
		dtmp = (double)bucket[b];
		dsum +=  dtmp * dtmp;
	}
	
	return (features[Energy] = dsum / ((double)L * (double)L));
}

double Hist1d::calcEntropy()
{
	int L = nbuckets;
	double  dsum = 0.0, dtmp;

	for (int b = 0; b < L; b++) {
		dtmp = (double)bucket[b]/(double)L;
		dsum +=  dtmp * log (dtmp);
	}
	
	return (features[Entropy] = -dsum/log(2.0));
}

int Hist1d::save(char *filename, int mode, int verbose)
{
	FILE *fout;

	if ((fout = fopen(filename,"w")) == NULL) {
		return -1;
	}
	if (verbose) {
		printf ("save to file : %s\n", filename);
	}
	for (int i = 0; i < nbuckets; i++) 
		if (mode == 2) 	fprintf(fout,"%d %d\n",i, bucket[i]);
		else 			fprintf(fout,"%d\n",bucket[i]);
	fclose(fout);
	return 0;
}


Hist2d::Hist2d(int m, int n, int *d)
{
	M = m; N = n; bucket = d;
}

Hist2d::~Hist2d()
{
	delete bucket;
}

void Hist2d::clearhist()
{
    int *ptr = bucket;
	int mn = M*N;
	for (int b = 0; b < mn; b++)
		*ptr++ = 0;
}

double Hist2d::calcAutocorrelation()
{
	double dsum = 0.0;
	int *p = bucket;
	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++)
			dsum += (double)(a * b * (*p++));

	return (dsum / (double)(M * N));
}

double Hist2d::calcCovar()
{
	int a, b;
	int *p = bucket;
	double dsum = 0.0, asum = 0.0, bsum = 0.0;
	int mn = M *N;
	for (a = 0; a < M; a++)
		for (b = 0; b < N; b++, p++) {
			asum += (double)(a * (*p));
			bsum += (double)(b * (*p));
		}
	double amean = dsum / ((double) mn);
	double bmean = dsum / ((double) mn);

	double atmp, btmp;
	for (a = 0; a < M; a++)
		for (b = 0; b < N; b++) {
			atmp = (double)a - amean;
			btmp = (double)b - bmean;
			dsum += atmp * btmp * (double) (*p++);
		}

	return (dsum / (double)mn);
}

double Hist2d::calcInertia()
{
	int *p = bucket;
	double dsum = 0.0, dtmp;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)(a - b);
			dsum += dtmp * dtmp * (double)(*p++);
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcAbsValue()
{
	int *p = bucket;
	double dsum = 0.0;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dsum += abs(a - b) * (*p++);
		}

	return (dsum / (double)(M * N));
}

// inverse difference
double Hist2d::calcInvDif()
{
	int *p = bucket;
	double dsum = 0.0, dtmp;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)a - (double) b;
			dsum += (double) (*p++) / ((double)1.0 + dtmp * dtmp);
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcHomogeneity()
{
	int *p = bucket;
	double dsum = 0.0;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dsum += (double) (*p++) / ((double)1.0 + (double) abs(a-b));
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcEnergy()
{
	int *p = bucket;
	double dsum = 0.0, dtmp;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)(*p ++);
			dsum += dtmp * dtmp; 
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcEntropy()
{
	int *p = bucket;
	double dsum = 0.0;
	double dtmp;
	double log2 = log(2.0);
	double mn = 1.0 / (double)(M*N);
	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)(*p++) * mn;
			dsum += dtmp * log (dtmp); 
		}

	return (- dsum / log2);
}

double Hist2d::calcUniformity()
{
	return (calcEnergy() * M * N);
}

int Hist2d::calcMaximum()
{
	int *p = bucket;
	int	maxi = -10000;
	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++, p++) {
			if (maxi < (*p))
				maxi = *p;
		}
	return maxi;
}

double Hist2d::calcGenContrast(int k, int l)
{
	int *p = bucket;
	double	rst = 0.0;

	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++) {
			rst += pow((double) abs(a-b), k) * pow(double(*p++), l);
		}
	return rst;
}

double Hist2d::calcGenInvDif(int k, int l)
{
	int *p = bucket;
	double dsum = 0.0;
	double dtmp;

	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++) {
			if (a != b) {
				dtmp = (double)a - (double) b;
				dsum += pow((double) (*p++),l) / pow(dtmp, k); 
			}
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcClusterTendency(int k)
{
	int a, b;
	int *p = bucket;
	double dsum = 0.0, asum = 0.0, bsum = 0.0;
	int mn = M*N;

	for (a = 0; a < M; a++)
		for (b = 0; b < N; b++, p++) {
			asum += a * (*p);
			bsum += b * (*p);
		}
	double amean = asum / ((double) mn);
	double bmean = bsum / ((double) mn);

	p = bucket;
	for (a = 0; a < M; a++) 
		for (b = 0; b < N; b++) {
			dsum += pow((double)(a + b - 2.0 * amean), k) *(double) (*p++);
		}

	return (dsum / (double) mn);
}

void Hist2d::print()
{
	int *p = bucket;
	for (int r = 0; r < m(); r++) {
		for (int c = 0; c < n(); c++)
			printf ("%d ", *p++);
		printf("\n");
	}
}

Hist2d *makeCooccurenceMatrix(GrayImage *g, double d, double deg, int size,
 int verbose)
{
	Hist2d *co_matrix = new Hist2d(size, size, new int[size*size]);	
	co_matrix->clearhist();
	int *p = co_matrix->gets();
	int *pp;
	int x, y, xx, yy; int r, c;
	int dx = (int)floor(d * cos(deg*M_PI/180.0));
	int	dy = (int)floor(d * sin(deg*M_PI/180.0));

	if (verbose)
		printf ("%d degree: dx = %d, dy = %d\n", (int) deg, dx, dy);
	for (y = 0; y < g->h(); y++)
		for (x = 0; x < g->w(); x++) {
			xx =  x + dx; yy =  y + dy;
			if (yy < co_matrix->m() && xx < co_matrix->n() &&
				yy >= 0 && xx >= 0) {
				r = g->getPixel(x, y); c = g->getPixel(xx, yy);
				pp = p + c + r * co_matrix->m();
				(*pp)  ++;
			}
			if (debug) {
				printf ("(%d,%d) r,c = (%d,%d)\n", x, y,  r,c);
				co_matrix->print();
			}
			xx =  x - dx; yy =  y - dy;
			if (yy < co_matrix->m() && xx < co_matrix->n() &&
				yy >= 0 && xx >= 0) {
				r = g->getPixel(x, y); c = g->getPixel(xx, yy);
				pp = p + c + r * co_matrix->m();
				(*pp)  ++;
			}
			if (debug) {
				printf ("(%d,%d) r,c = (%d,%d)\n", x, y,  r,c);
				co_matrix->print();
			}
		}
	return co_matrix;
}
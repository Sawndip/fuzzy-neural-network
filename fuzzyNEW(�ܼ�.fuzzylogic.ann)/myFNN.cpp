// myFNN.cpp: implementation of the myFNN class.
//
//////////////////////////////////////////////////////////////////////

#include "myFNN.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <float.h>

#define M_PI (3.1415926535897932384626433832795)

#define	FILE_OUT	0
//#define	_NN_DEBUG_
#define ALPHA_0		(0.3f)
#define BETA_0		(15.3171f)
#define ALPHA_1		(0.0f)
#define BETA_1		(7.6585f)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

myFNN::myFNN()
{
	srand((unsigned)time(NULL));
	layer = NULL;
	errors = NULL;
	load_flag = 0;

	tin = NULL;
	lin = NULL;
	ein = NULL;
	tout = NULL;
	lout = NULL;
	eout = NULL;

	//cvvNamedWindow("MFin", 0);
	//cvvNamedWindow("MFout", 0);
	//cvvNamedWindow("FNN", 0);
	//cvvNamedWindow("Error", 0);

	cvInitFont(&font, CV_FONT_VECTOR0, 0.2f, 0.2f, 0.0f, 1);
}

myFNN::~myFNN()
{
	//cvReleaseImage(&(imgError));

	//cvvDestroyWindow("Error");
	//cvvDestroyWindow("FNN");
	//cvvDestroyWindow("MFin");
	//cvvDestroyWindow("MFout");

	if (eout)	delete [] eout;
	if (lout)	delete [] lout;
	if (tout)	delete [] tout;
	if (ein)	delete [] ein;
	if (lin)	delete [] lin;
	if (tin)	delete [] tin;
	if (layer && load_flag)
		delete [] layer;
	if (errors)
		delete [] errors;
}

// assume: data is in [0, 1]
void myFNN::kSOC(float *data, int no_data, float *mean, float *std, int no_cluster)
{
	int i, j;
	float a;
	int mc;
	float min = 1.0f;
	float r = 2.0f;

	for (i=0; i<no_cluster; i++)
	{
		mean[i] = (float)((float)i/(float)no_cluster + 0.5/(float)no_cluster);
	}	

	for (i=0; i<no_data; i++)
	{
		a = (float)(1.0*(float)(no_data - i)/(float)(no_data));

		min = 1.0f;

		for (j=0; j<no_cluster; j++)
		{
			if (min > fabs(data[i] - mean[j]))
			{
				min = (float)fabs(data[i] - mean[j]);
				mc = j;
			}
		}

		mean[mc] += a*(data[i] - mean[mc]);
	}

	for (j=0; j<no_cluster; j++)
	{
		min = 1.0f;
		for (i=0; i<no_cluster; i++)
		{
			if (i != j)
			{
				if (min > fabs(mean[j] - mean[i]))
				{
					min = (float)fabs(mean[j] - mean[i]);
					mc = i;
				}
			}
		}
		
		std[j] = (float)fabs(mean[j] - mean[mc])/r;
	}
}

void myFNN::initFNN()
{
	int i,j,k;
	int bias = 0, mbias;
	int sum = 0;
	int my_index = 0;

	out = new float *[no_layer];
	delta = new float *[no_layer];
	rout = new float [layer[3]];
	//rin = new float[layer[1]];
	in = new float *[no_layer];
	rules = new int[layer[2]];

	for (i=0; i<layer[2]; i++)
		rules[i] = 1;
	
	if (!load_flag)
	{
		weight = new float **[no_layer];
		connect = new int **[no_layer];
		mean[0] = new float[layer[1]];
		mean[1] = new float[layer[3]];
		sigma[0] = new float[layer[1]];
		sigma[1] = new float[layer[3]];
	}

	// make structure...
	for (i=0; i<no_layer; i++)
	{
		out[i] = new float [layer[i]];
		in[i] = new float[layer[i]];
		delta[i] = new float [layer[i]];

		if (!load_flag)
		{
			weight[i] = new float *[layer[i]];
			connect[i] = new int *[layer[i]];

			if (i>0)
			{
				for (j=0; j<layer[i]; j++)
				{
					weight[i][j] = new float[layer[i-1]];
					connect[i][j] = new int[layer[i-1]];
				}

				for (k=0; k<layer[i-1]; k++)
					for (j=0; j<layer[i]; j++)
					{
						weight[i][j][k] = 0.0f;
						connect[i][j][k] = 0;
					}
			}
		}
	}

	if (!load_flag)
	{
		bias = 0;

		// connectionist network
		// layer 1-2
		for (j=0; j<layer[0]; j++)
		{
			if (j>0)
				bias += itlayer[j-1];

			for (k=0; k<itlayer[j]; k++)
			{
				weight[1][bias + k][j] = 1.0f;
				connect[1][bias + k][j] = 1;
			}
		}

		// layer 2-3
		// just for 2 termsets!
		// go for more...
		for (i=0; i<layer[2]; i++)
		{
			//int t = i;
			bias = 0;
			mbias = 1;

			for (j=0; j<layer[0]; j++)
			{
				if (j>0)
				{
					bias += itlayer[j-1];
					mbias *= itlayer[j-1];
				}

				//my_index = bias+(t&0x1);
				//my_index = bias+(i/(int)pow(2, j))%itlayer[j];
				if (j>0)
					my_index = bias+(i/mbias)%itlayer[j];
				else
					my_index = bias+i%itlayer[j];

				weight[2][i][my_index] = 1.0f;
				connect[2][i][my_index] = 1;
				//t = t >> 1;
			}
		}

		// layer 3-4
		for (k=0; k<layer[2]; k++)
			for (j=0; j<layer[3]; j++)
			{
				weight[3][j][k] = (float)((float)rand()/(float)RAND_MAX - 0.5f);
				connect[3][j][k] = 1;
			}

		// layer 4-5
		bias = 0;

		for (j=0; j<layer[4]; j++)
		{
			if (j>0)
				bias += otlayer[j-1];
			
			for (k=0; k<otlayer[j]; k++)
			{
				weight[4][j][bias + k] = 1.0f;
				connect[4][j][bias + k] = 1;
			}
		}
	}
}

void myFNN::initFNN2()
{
	int i,j,k;
	int bias = 0;
	int sum = 0;

	out = new float *[no_layer];
	delta = new float *[no_layer];
	rout = new float [layer[3]];
	
	if (!load_flag)
	{
		weight = new float **[no_layer];
		mean[0] = new float[layer[1]];
		mean[1] = new float[layer[3]];
		sigma[0] = new float[layer[1]];
		sigma[1] = new float[layer[3]];
	}

	// make structure...
	for (i=0; i<no_layer; i++)
	{
		out[i] = new float [layer[i]];
		delta[i] = new float [layer[i]];

		if (!load_flag)
		{
			weight[i] = new float *[layer[i]];

			if (i>0)
			{
				for (j=0; j<layer[i]; j++)
					weight[i][j] = new float[layer[i-1]];

				for (k=0; k<layer[i-1]; k++)
					for (j=0; j<layer[i]; j++)
						weight[i][j][k] = 0.0f;
			}
		}
	}

	if (!load_flag)
	{
		// connectionist network
		// layer 1-2
		for (j=0; j<layer[0]; j++)
		{
			if (j>0)
				bias += itlayer[j-1];

			for (k=0; k<itlayer[j]; k++)
				weight[1][bias + k][j] = 1.0f;
		}

		// layer 2-3
		// just for 2 termsets!
		for (i=0; i<layer[2]; i++)
		{
			int t = i;

			for (j=0; j<layer[0]; j++)
			{
				weight[2][i][2*j+(t&0x1)] = 1;
				t = t >> 1;
			}
		}

		// layer 3-4
		for (k=0; k<layer[2]; k++)
			for (j=0; j<layer[3]; j++)
				weight[3][j][k] = (float)((float)rand()/(float)RAND_MAX - 0.5f);
	}
}

// 020510 org.
// up-down | down-up
// i, j, k, l, m for each layer!!!
void myFNN::evalFNN(float *input, float *output, int flag)
{
	int i, j, k, l, m;
	int cbias = 0;

	float min = 1e2;
	float sum = 0.0f;
	float ssum = 0.0f;
	float sssum;

	// down-up mode
	for (i=0; i<layer[0]; i++)
	{
		if (i>0)
			cbias += itlayer[i-1];

		out[0][i] = *(input + i);

		for (j=0; j<itlayer[i]; j++)
		{
			if (connect[1][cbias+j][i])
				in[1][cbias+j] = out[0][i];
			else
				in[1][cbias+j] = 0.0f;
		}
	}

	for (j=0; j<layer[1]; j++)
	{
		sssum = 1.0f/sigma[0][j];

		if (_isnan((double)sssum))
			out[1][j] = 0.0f;
		else
			out[1][j] = (float)exp(-(in[1][j] - mean[0][j])*(in[1][j] - mean[0][j])/(sigma[0][j]*sigma[0][j]));
	}

	for (j=0; j<layer[2]; j++)
	{
		min = 1e2;

		for (k=0; k<layer[1]; k++)
		{
			if (connect[2][j][k])
			{
				if (min > out[1][k])
					min = out[1][k];
			}
		}

		out[2][j] = min;
	}

	if (flag == 0)
	{
		// down-up
		for (j=0; j<layer[3]; j++)
		{
			sum = 0.0f;

			for (k=0; k<layer[2]; k++)
			{
				if (connect[3][j][k])
					sum += out[2][k];
			}

			out[3][j] = __min(1.0f, sum);
		}

		cbias = 0;

		for (j=0; j<layer[4]; j++)
		{
			if (j>0)
				cbias += otlayer[j-1];

			sum = 0.0f;
			ssum = 0.0f;

			for (k=0; k<otlayer[j]; k++)
				if (connect[4][j][cbias+k])
				{
					sum += (mean[1][cbias+k]*sigma[1][cbias+k])*out[3][cbias+k];
					ssum += (sigma[1][cbias+k])*out[3][cbias+k];
				}

			sssum = sum/ssum;

			if (_isnan((double)sssum))
				*(output + j) = out[4][j] = 0.0f;
			else
				*(output + j) = out[4][j] = sum/ssum;
		}
	}
	else
	{
		// up-down
		for (j=0; j<layer[3]; j++)
		{
			for (k=0; k<layer[4]; k++)
			{
				sssum = 1.0f/sigma[1][j];

				if (connect[4][k][j])
				{
					if (_isnan((double)sssum))
						rout[j] = 0.0f;
					else
						rout[j] = (float)exp(-(*(output+k)-mean[1][j])*(*(output+k)-mean[1][j])/(sigma[1][j]*sigma[1][j]));
				}
			}
		}
	}
}

void myFNN::evalFNN2(float *input, float *output, int flag)
{
	int j, k;

	float min = 1.0f;
	float sum = 0.0f;
	float ssum = 0.0f;
	int weight_no = 0;

	// down-up mode
	for (j=0; j<layer[0]; j++)
		out[0][j] = *(input + j);

	for (j=0; j<layer[1]; j++)
		for (k=0; k<layer[0]; k++)
			if (weight[1][j][k] != 0.0f)
				out[1][j] = (float)exp(-(out[0][k] - mean[0][j])*(out[0][k] - mean[0][j])/(sigma[0][j]*sigma[0][j]));

	for (j=0; j<layer[2]; j++)
	{
		min = 1.0f;

		for (k=0; k<layer[1]; k++)
		{
			if (weight[2][j][k] != 0.0f)
				min = __min(min, out[1][k]);
		}

		out[2][j] = min;
	}

	if (flag == 0)
	{
		// down-up
		for (j=0; j<layer[3]; j++)
		{
			sum = ssum = 0.0f;
			weight_no = 0;

			for (k=0; k<layer[2]; k++)
			{
				if (weight[3][j][k] != 0.0f)
				{
					weight_no++;
					sum += weight[3][j][k];
					ssum += weight[3][j][k]*out[2][k];
				}
			}

			out[3][j] = ssum/((float)weight_no*sum);
			*(output + j) = out[3][j];
		}
	}
	else
	{
		// up-down
		for (j=0; j<layer[3]; j++)
		{
			rout[j] = *(output+j);
		}
	}
}

// 20020723
void myFNN::compFNN(float *input, float *output, int no_sample)
{
	int i, j, k, l, m, t;
	float learn = 0.0f;
	int cbias = 0;
	float max = 0.0f;
	int max_index = 0;
	float sum_learn;
	//int iter = 0;

	for (t=0; t<no_sample; t++)
	{
		evalFNN((input+t*layer[0]), (output+t*layer[4]), 1);

		//iter = 0;

		do {
			sum_learn = 0.0;
			//iter++;

			for (j=0; j<layer[3]; j++)
				for (k=0; k<layer[2]; k++)
				{
					//if (rout[j] > 0.5f)
						learn = rout[j]*(-weight[3][j][k] + out[2][k]);
					//else
					//	learn = 0.0f;
					
					sum_learn += fabs(learn);
					weight[3][j][k] += learn;
				}
		} while (sum_learn > 1e-4f);

		//printf("%d \n", iter);
	}

	// get max one! for each node in 3rd layer (rule node)
	max_index = 0;
	for (j=0; j<layer[2]; j++)
	{
		cbias = 0;

		for (k=0; k<layer[4]; k++)
		{
			if (k>0)
				cbias += otlayer[k-1];

			max = 0.0f;
			for (i=0; i<otlayer[k]; i++)
			{
				if (max < weight[3][cbias + i][j])
				{
					max = weight[3][cbias + i][j];
					max_index = cbias + i;
				}

				weight[3][cbias + i][j] = 0.0f;
				connect[3][cbias + i][j] = 0;
			}

			// 20020723
			if (max > 1e-3f)
			{
				weight[3][max_index][j] = 1.0f;
				connect[3][max_index][j] = 1;
			}
		}
	}

	deleteLinks();
}

void myFNN::compFNN2(float *input, float *output, int no_sample)
{
	int i, j, k, t;
	float learn = 0.0f;
	int sign_flag = 0;
	int cbias = 0;
	float max = 0.0f;
	int max_index = 0;
	//int iter = 0;

	for (t=0; t<no_sample; t++)
	{
		//iter = 0;
		evalFNN2((input+t*layer[0]), (output+t*layer[3]), 1);

		for (i=0; i<10; i++)
		{
		//do {
		//	sign_flag = 0;

			for (j=0; j<layer[3]; j++)
				for (k=0; k<layer[2]; k++)
				{
					if (rout[j] > 0.5f)
						learn = rout[j]*(-weight[3][j][k] + out[2][k]);
					else
						learn = 0.0f;
					//learn = out[2][k]*(-weight[3][j][k] + rout[j]);
					weight[3][j][k] += learn;

		//			if (weight[3][j][k] > 0)	sign_flag++;
				}
		//} while (sign_flag != layer[3]*layer[2]);
		}
	}

	max_index = 0;

	for (j=0; j<layer[2]; j++)
	{
		for (k=0; k<layer[3]; k++)
		{
			max = 0.0f;
			
			if (max < weight[3][k][j])
			{
				max = weight[3][k][j];
				max_index = k;
			}

			weight[3][k][j] = 0.0f;

			if (max > 0.1f)
				weight[3][max_index][j] = 1.0f;
		}
	}
}

void myFNN::aoutFNN(int layer_)
{
	int i, j;

	FILE *fp;
	fp = fopen(".\\fireSTR.dat", "awt");

	for (i=0; i<no_layer; i++)
	{
		if ((i == layer_) || (layer_ == -1))
		{
			fprintf(fp, "\n");
			//printf("\n");

			for (j=0; j<layer[i]; j++)
			{
				fprintf(fp, "%.3f ", out[i][j]); 
				//printf("%.3f ", out[i][j]); 
			}
		}
	}

	fclose(fp);
}

void myFNN::freeFNN()
{
	delete [] sigma[1];
	delete [] sigma[0];
	delete [] mean[1];
	delete [] mean[0];
	delete [] in;
	delete [] rules;
	delete [] rout;
	delete [] delta;
	delete [] connect;
	delete [] weight;
	delete [] out;
}

void myFNN::showFNN(int layer_, int mode)
{
	int i, j, k;
	
	printf("\n");

	for (i=1; i<no_layer; i++)
	{
		if ((i == layer_) || (layer_ == -1))
		{
			for (j=0; j<layer[i]; j++)
				for (k=0; k<layer[i-1]; k++)				
				{
					if (connect[i][j][k])
						if (mode == 0)
							printf("[%d,%d]=%.2f ", j, k, weight[i][j][k]);
						else
							printf("[%d,%d]=%1d ", j, k, connect[i][j][k]);
				}
		}

		printf("\n");
	}
}

void myFNN::kSOCFNN(float *mean_, float *sigma_, int io, int index)
{
	int i;
	int bias = 0;

	// input
	if (io == 0)
	{
		if (index > 0)
			for (i=0; i<index; i++)
				bias += itlayer[i];

		for (i=0; i<itlayer[index]; i++)
		{
			mean[0][bias + i] = mean_[i];
			sigma[0][bias + i] = sigma_[i];
		}
	}
	// output
	else
	{
		if (index > 0)
			for (i=0; i<index; i++)
				bias += otlayer[i];

		for (i=0; i<otlayer[index]; i++)
		{
			mean[1][bias + i] = mean_[i];
			sigma[1][bias + i] = sigma_[i];
		}
	}
}

void myFNN::learnFNN(float *input, float *output, int no_sample)
{
	int i, j, k, l, m, t;
	int cbias = 0;
	float sum = 0.0f;
	float msum = 0.0f;
	float mssum;

	int min_index;
	int exam;
	float min = 1e2;
	float tsum = 0.0f;
	float max_error;

	float Pmean, Pstd;

	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\error.dat", "wt");
	}

	float *error = new float[layer[4]];
	float *out_ = new float[layer[4]];
	float errors;

	for (t=0; t<epoch; t++)
	{
		exam = rand()%no_sample;

		evalFNN((input + exam*layer[0]), out_, 0);
		
		errors = 0.0f;
		for (i=0; i<layer[4]; i++)
		{
			error[i] = *(output + exam*layer[4] + i) - out_[i];
			errors += (float)(0.5*(error[i]*error[i]));
		}
		
		//if (t%100 == 0)
		//	ShowError(t, errors);
		
		if (FILE_OUT)
			fprintf(fp, "%d %f\n", t, errors);
			
		if (errors < err_criteria)
		{
			printf("#%d : %.3f\n", t, errors);
			break;
		}

		// layer 5
		cbias = 0;

		for (i=0; i<layer[4]; i++)
		{
			if (i>0)
				cbias += otlayer[i-1];

			sum = 0.0f;
			msum = 0.0f;

			for (j=0; j<otlayer[i]; j++)
			{
				sum += (sigma[1][cbias + j]*out[3][cbias + j]);
				msum += (mean[1][cbias + j]*sigma[1][cbias + j]*out[3][cbias + j]);
			}

			mssum = 1.0f/sum;

			for (j=0; j<otlayer[i]; j++)
			{	
				
				if (_isnan(mssum))
				{
					delta[3][cbias + j] = 0.0f;
					Pmean = Pstd = 0.0f;
				}
				else
				{
					delta[3][cbias + j] = error[i]*(mean[1][cbias + j]*sigma[1][cbias + j]*sum - msum*sigma[1][cbias + j])/(sum*sum);
					Pmean = error[i]*(sigma[1][cbias + j]*out[3][cbias + j])/sum;
					Pstd = error[i]*(mean[1][cbias + j]*out[3][cbias + j]*sum - msum*out[3][cbias + j])/(sum*sum);
				}

				//float degree1 = fSMAB(mean[1][cbias + j], mean[1][cbias + j] + learn_rate*Pmean,
				//	sigma[1][cbias+j], sigma[1][cbias+j]+learn_rate*Pstd);

				if (((mean[1][cbias+j] + learn_rate*Pmean) >= 0.0f)
					|| ((mean[1][cbias+j] + learn_rate*Pmean) <= 1.0f))
				{
					mean[1][cbias + j] += learn_rate*Pmean;
				}
				//mean[1][cbias + j] = __max(0.0f, __min(1.0f, mean[1][cbias+j]));

				if (((sigma[1][cbias + j] + learn_rate*Pstd) > 0.0f))
					sigma[1][cbias + j] += learn_rate*Pstd;				
				//sigma[1][cbias + j] = __max(0.0f, __min(1.0f, sigma[1][cbias+j]));
				
			}

			//delta[4][i] = error[i];
		}

		// layer 4	see above.
		// layer 3
		for (i=0; i<layer[2]; i++)
		{
			delta[2][i] = 0.0f;

			for (j=0; j<layer[3]; j++)
				if (connect[3][j][i])
					delta[2][i] += delta[3][j];
		}

		// layer 2
		for (i=0; i<layer[1]; i++)
		{
			tsum = 0.0f;

			for (j=0; j<layer[2]; j++)
			{
				min = 1e2;
				min_index = 0;

				for (k=0; k<layer[1]; k++)
					if (connect[2][j][k])
					{
						if (out[1][k] < min)
						{
							min = out[1][k];
							min_index = k;
						}
					}
				
				if (i == min_index)
					if (connect[2][j][i])
						tsum += delta[2][j];
			}

			mssum = 1.0f/sigma[0][i];

			if (_isnan((double)mssum))
				Pmean = Pstd = 0.0f;
			else
			{
				Pmean = tsum*out[1][i]*2.0*(in[1][i] - mean[0][i])/(sigma[0][i]*sigma[0][i]);
				Pstd = Pmean*(in[1][i] - mean[0][i])/sigma[0][i];
			}

			//float degree2 = fSMAB(mean[0][i], mean[0][i] - learn_rate*Pmean,
			//	sigma[0][i], sigma[0][i]-learn_rate*Pstd);

			if (((mean[0][i] - learn_rate*Pmean) >= 0.0f)
				|| ((mean[0][i] - learn_rate*Pmean) <= 1.0f))
			{
				mean[0][i] += -learn_rate*Pmean;
			}
			//mean[0][i] = __max(0.0f, __min(1.0f, mean[0][i]));

			if ((sigma[0][i] - learn_rate*Pstd) > 0.0f)
				sigma[0][i] += -learn_rate*Pstd;
			//sigma[0][i] = __max(0.0f, __min(1.0f, sigma[0][i]));
		}
	}

	if (FILE_OUT)
		fclose(fp);

	delete [] out_;
	delete [] error;
}

void myFNN::learnFNN2(float *input, float *output, int no_sample)
{
	int i, j, k, t;
	int cbias = 0;
	float sqerror = 0.0f;
	float sum = 0.0f;
	float msum = 0.0f;

	int min_index;
	float min = 1.0f;
	float tsum = 0.0f;
	float max_error;
	int weight_no = 0;

	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\error.dat", "wt");
	}

	float *error = new float[layer[3]];
	float *out_ = new float[layer[3]];

	if (!errors)
		errors = new float[no_sample];

	for(i=0; i<no_sample; i++)
		errors[i] = 1.0f;

	for (t=0; t<epoch; t++)
	{
		j = rand()%no_sample;

		sqerror = 0.0f;

		//for (j=0; j<no_sample; j++)
		//{
			evalFNN2((input + j*layer[0]), out_, 0);
			
			for (i=0; i<layer[3]; i++)
			{
				error[i] = *(output + j*layer[3] + i) - out_[i];
				errors[j] = (float)(0.5*(error[i]*error[i]));
				//sqerror += 0.5*(error[i]*error[i]);
			}
		
		max_error = 0.0f;
		for (i=0; i<no_sample; i++)
		{
			if (max_error < error[j])
				max_error = error[j];

			sqerror += errors[j];
		}

		if (t%100 == 0)
			printf("#%d : %f\n", t, sqerror);
		
		if (FILE_OUT)
			fprintf(fp, "%d %f\n", t, sqerror);
		//}
			
		if ((sqerror < err_criteria) && (max_error < err_criteria))
		{
			printf("#%d : %f\n", t, sqerror);
			break;
		}

		// layer 4
		cbias = 0;

		for (i=0; i<layer[3]; i++)
		{
			//delta[3][i] = error[i];
			weight_no = 0;
			sum = msum = 0.0f;

			for (j=0; j<layer[2]; j++)
			{
				if (weight[3][i][j] != 0.0f)
				{
					weight_no++;
					sum += weight[3][i][j];
					msum += weight[3][i][j]*out[2][j];
				}
			}

			weight[3][i][j] += -(float)(learn_rate*(sum*out[2][i] - msum)/sum*sum);

			delta[3][i] = error[i]/(float)weight_no;
		}

		// layer 3
		for (i=0; i<layer[2]; i++)
		{
			delta[2][i] = 0.0f;

			for (j=0; j<layer[3]; j++)
				if (weight[3][j][i] != 0.0f)
					delta[2][i] += delta[3][j];
		}

		// layer 2
		for (i=0; i<layer[1]; i++)
		{
			tsum = 0.0f;

			for (j=0; j<layer[2]; j++)
			{
				min = 1.0f;

				for (k=0; k<layer[1]; k++)
					if (weight[2][j][k] != 0.0f)
					{
						if (out[1][k] < min)
						{
							min = out[1][k];
							min_index = k;
						}
					}
				
				if (i == min_index)
					tsum += delta[2][j];
			}

			mean[0][i] += -(float)(learn_rate*(tsum)*out[1][i]*2.0*(out[0][j] - mean[0][i])/(sigma[0][i]*sigma[0][i]));
			sigma[0][i] += -(float)(learn_rate*(tsum)*out[1][i]*2.0*(out[0][j] - mean[0][i])*(out[0][j] - mean[0][i])/(sigma[0][i]*sigma[0][i]*sigma[0][i]));
		}
	}

	if (FILE_OUT)
		fclose(fp);

	delete [] out_;
	delete [] error;
}

void myFNN::learnFNNsi(float *input, float *output, int no_sample)
{
	int i, j, k, t;
	int cbias = 0;
	float sqerror = 0.0f;
	float sum = 0.0f;
	float msum = 0.0f;

	int min_index;
	float min = 1.0f;
	float tsum = 0.0f;
	float max_error;

	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\error.dat", "wt");
	}

	float *error = new float[layer[4]];
	float *out_ = new float[layer[4]];

	float si = 0.0f;

	if (!errors)
		errors = new float[no_sample];

	for(i=0; i<no_sample; i++)
		errors[i] = 1.0f;

	for (t=0; t<epoch; t++)
	{

		/*
		if (t > 100)
		{

		do {
			
			j = rand()%no_sample;
			si = si1FNN(j);

		} while(si > 0.5);

		}
		else
		*/
		
		j = rand()%no_sample;

		sqerror = 0.0f;

		//for (j=0; j<no_sample; j++)
		//{
			evalFNN((input + j*layer[0]), out_, 0);
			
			for (i=0; i<layer[4]; i++)
			{
				error[i] = *(output + j*layer[4] + i) - out_[i];
				errors[j] = (float)(0.5*(error[i]*error[i]));
				//sqerror += 0.5*(error[i]*error[i]);
			}
		
		max_error = 0.0f;
		for (i=0; i<no_sample; i++)
		{
			if (max_error < error[j])
				max_error = error[j];

			sqerror += errors[j];
		}

		if (t%100 == 0)
			printf("#%d : %f\n", t, sqerror);
		
		if (FILE_OUT)
			fprintf(fp, "%d %f\n", t, sqerror);
		//}

		if ((sqerror < err_criteria) && (max_error < err_criteria))
		{
			printf("#%d : %f\n", t, sqerror);
			break;
		}

		// layer 5
		cbias = 0;

		for (i=0; i<layer[4]; i++)
		{
			if (i>0)
				cbias += otlayer[i-1];

			sum = 0.0f;
			msum = 0.0f;

			for (j=0; j<otlayer[i]; j++)
			{
				sum += (sigma[1][cbias + j]*out[3][cbias + j]);
				msum += (mean[1][cbias + j]*sigma[1][cbias + j]*out[3][cbias + j]);
			}

			for (j=0; j<otlayer[i]; j++)
			{			
				mean[1][cbias + j] += learn_rate*error[i]*(sigma[1][cbias + j]*out[3][cbias + j])/sum;
				sigma[1][cbias + j] += learn_rate*error[i]*(mean[1][cbias + j]*out[3][cbias + j]*sum - msum *out[3][cbias + j])/(sum*sum);
				delta[3][cbias + j] = error[i]*(mean[1][cbias + j]*sigma[1][cbias + j]*sum - msum *sigma[1][cbias + j])/(sum*sum);
			}

			delta[4][i] = error[i];
		}

		// layer 4	see above.

		// layer 3
		for (i=0; i<layer[2]; i++)
		{
			delta[2][i] = 0.0f;

			for (j=0; j<layer[3]; j++)
				if (weight[3][j][i] != 0.0f)
					delta[2][i] += delta[3][j];
		}

		// layer 2
		for (i=0; i<layer[1]; i++)
		{
			tsum = 0.0f;

			for (j=0; j<layer[2]; j++)
			{
				min = 1.0f;

				for (k=0; k<layer[1]; k++)
					if (weight[2][j][k] != 0.0f)
					{
						if (out[1][k] < min)
						{
							min = out[1][k];
							min_index = k;
						}
					}
				
				if (i == k)
					tsum += delta[2][j];
			}

			mean[0][i] += -(float)(learn_rate*(tsum)*out[1][i]*2.0*(out[0][j] - mean[0][i])/(sigma[0][i]*sigma[0][i]));
			sigma[0][i] += -(float)(learn_rate*(tsum)*out[1][i]*2.0*(out[0][j] - mean[0][i])*(out[0][j] - mean[0][i])/(sigma[0][i]*sigma[0][i]*sigma[0][i]));
		}
	}

	if (FILE_OUT)
		fclose(fp);

	delete [] out_;
	delete [] error;
}

float myFNN::si1FNN(int target)
{
	int j;
	float cre = 0.0f;
	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\aout.txt", "aw");
	}

	for (j=0; j<layer[4]; j++)
	{
		cre += (float)fabs(out[4][target] - out[4][j]);
	}

	if (FILE_OUT)
		fprintf(fp, "\t: %f", cre);

	printf("\t: %f", cre);

	if (FILE_OUT)
		fclose(fp);

	return (cre);
}

// bhattacharrya
void myFNN::si2FNN(int target)
{
	int j;
	float cre = 0.0f;
	float s = 0.5f;
	float m1 = 0.0f, m2 = 0.0f;
	float sigma = 0.0f;
	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\aout.txt", "aw");
	}

	for (j=0; j<layer[4]; j++)
	{
		if (j != target)
			m2 += (float)out[4][j];
		else
			m1 = (float)out[4][j];
	}

	m2 /= (float)(layer[4]);

	for (j=0; j<layer[4]; j++)
	{
		sigma += (float)(out[4][j] - m1)*(out[4][j] - m1);
	}

	sigma /= (float)(layer[4] - 1);

	cre = (float)((s*(1-s)*0.5)*(m2-m1)*(m2-m1)*(2.0/sigma));

	if (FILE_OUT)
		fprintf(fp, "\t: %f", cre);

	printf("\t: %f", cre);

	if (FILE_OUT)
		fclose(fp);
}

// S. C. 1)
void myFNN::si3FNN(int target)
{
	int j;
	float cre = 0.0f;
	float s = 0.5f;
	float m1 = 0.0f, m2 = 0.0f, m0 = 0.0f;
	float sigma = 0.0f;
	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\aout.txt", "aw");
	}

	for (j=0; j<layer[4]; j++)
	{
		if (j != target)
			m2 += (float)out[4][j];
		else
			m1 = (float)out[4][j];
	}

	m2 /= (float)(layer[4]);

	m0 = m1/7.0 + m2*6.0/7.0;

	for (j=0; j<layer[4]; j++)
	{
		sigma += (float)(out[4][j] - m1)*(out[4][j] - m1);
	}

	sigma /= (float)(layer[4] - 1);

	cre = (float)((m1-m0)*(m1-m0)/7.0 + 6.0*(m2-m0)*(m2-m0)/7.0)/(6.0*sigma/7.0);

	if (FILE_OUT)
		fprintf(fp, "\t: %f", cre);

	printf("\t: %f", cre);

	cre = (float)((m1-m2)*(m1-m2))/(6.0*sigma/7.0);

	if (FILE_OUT)
		fprintf(fp, "\t: %f", cre);

	printf("\t: %f", cre);

	cre = (float)((m1-m2)*(m1-m2)+sigma)/(6.0*sigma/7.0);

	if (FILE_OUT)
		fprintf(fp, "\t: %f", cre);

	printf("\t: %f", cre);

	if (FILE_OUT)
		fclose(fp);
}

// DJ3
// in the my proposal...
void myFNN::si4FNN(int target)
{
	int j;
	float mySI[3] = {0.0f, 0.0f, 0.0f};
	float mySIint = 0.0f;
	float max_val = -1.0f;
	FILE *fp;

	if (FILE_OUT)
		fp = fopen(".\\aout.txt", "aw");

	for (j=0; j<layer[4]; j++)
	{
		if (j != target)
		{
			if (out[4][j] > max_val)
			{
				max_val = out[4][j];
			}
		}
	}

	// 1) target sufficiency
	mySI[0] = (2.0f)/(1.0f + exp(-ALPHA*(out[4][target] - HI)));

	if (FILE_OUT)
		fprintf(fp, "\t: %f", mySI[0]);

	printf("\t: %.2f", mySI[0]);

   	// 2) dissimilarity between target and non-targets
	mySI[1] = (2.0f)/(1.0f + exp(-ALPHA*(out[4][target] - max_val))) - 1.0f;
	// 3) non-target sufficiency
	mySI[2] = (1.0f - (2.0f)/(1.0f + exp(-ALPHA*(max_val - HI))));
	
	/*
	for (j=0; j<layer[4]; j++)
	{
		if (j != target)
		{
			// 2) dissimilarity between target and non-targets
			if ((out[4][j] - out[4][target]) >= 0.0f)
				mySI[1] += (1.0f)/(1+exp(-BETA1*(out[4][j] - out[4][target])));
			else
				mySI[1] += (1.0f)/(1+exp(-BETA2*(out[4][j] - out[4][target])));
			// 3) non-target sufficiency
			mySI[2] += (1.0f - (2.0f)/(1.0f + exp(-ALPHA*(out[4][j] - HI))));
		}
	}

	mySI[1] /= (float)(layer[4] - 1);
	mySI[2] /= (float)(layer[4] - 1);
	*/

	if (FILE_OUT)
		fprintf(fp, "\t: %f", mySI[1]);

	printf("\t: %.2f", mySI[1]);

	if (FILE_OUT)
		fprintf(fp, "\t: %f", mySI[2]);

	printf("\t: %.2f", mySI[2]);

	//mySIint = a*mySI[0] + b*mySI[1] - c*mySI[2];
	mySIint = mySI[0]*mySI[1]*mySI[2];

	if (FILE_OUT)
		fprintf(fp, "\t: %f", mySIint);

	printf("\t: %.2f", mySIint);

	if (FILE_OUT)
		fclose(fp);
}

void myFNN::saveFNN(char *str)
{
	int i,j,k;

	FILE *fp;
	fp = fopen(str, "wt");

	fprintf(fp, "%d ", no_layer);

	fprintf(fp, "\n");

	for (i=0; i<no_layer; i++)
		fprintf(fp, "%d ", layer[i]);

	fprintf(fp, "\n");

	for (i=0; i<layer[0]; i++)
		fprintf(fp, "%d ", itlayer[i]);

	fprintf(fp, "\n");

	for (i=0; i<layer[4]; i++)
		fprintf(fp, "%d ", otlayer[i]);

	fprintf(fp, "\n");
	
	fprintf(fp, "%f %d %f", learn_rate, epoch, err_criteria);

	fprintf(fp, "\n");

	for (i=1; i<no_layer; i++)
	{
		for (j=0; j<layer[i]; j++)
			for (k=0; k<layer[i-1]; k++)
			{
				fprintf(fp, "%f ", weight[i][j][k]);
			}

		fprintf(fp, "\n");		
	}

	for (i=1; i<no_layer; i++)
	{
		for (j=0; j<layer[i]; j++)
			for (k=0; k<layer[i-1]; k++)
			{
				fprintf(fp, "%1d ", connect[i][j][k]);
			}

		fprintf(fp, "\n");		
	}

	for (i=0; i<layer[1]; i++)
		fprintf(fp, "%f ", mean[0][i]);
	
	fprintf(fp, "\n");

	for (i=0; i<layer[1]; i++)
		fprintf(fp, "%f ", sigma[0][i]);

	fprintf(fp, "\n");

	for (i=0; i<layer[3]; i++)
		fprintf(fp, "%f ", mean[1][i]);
	
	fprintf(fp, "\n");

	for (i=0; i<layer[3]; i++)
		fprintf(fp, "%f ", sigma[1][i]);

	fclose(fp);
}

void myFNN::loadFNN(char *str)
{
	load_flag = 1;

	int i,j,k;

	FILE *fp;
	fp = fopen(str, "rt");

	fscanf(fp, "%d ", &no_layer);

	if (!layer)
		layer = new int[no_layer];
	else
		return;

	for (i=0; i<no_layer; i++)
		fscanf(fp, "%d ", &(layer[i]));

	itlayer = new int[layer[0]];

	for (i=0; i<layer[0]; i++)
		fscanf(fp, "%d ", &(itlayer[i]));

	otlayer = new int[layer[4]];

	for (i=0; i<layer[4]; i++)
		fscanf(fp, "%d ", &(otlayer[i]));

	fscanf(fp, "%f %d %f", &learn_rate, &epoch, &err_criteria);

	weight = new float **[no_layer];

	for (i=1; i<no_layer; i++)
	{
		weight[i] = new float *[layer[i]];

		for (j=0; j<layer[i]; j++)
		{
			weight[i][j] = new float[layer[i-1]];

			for (k=0; k<layer[i-1]; k++)
			{
				fscanf(fp, "%f ", &(weight[i][j][k]));
			}
		}
	}

	connect = new int **[no_layer];

	for (i=1; i<no_layer; i++)
	{
		connect[i] = new int *[layer[i]];

		for (j=0; j<layer[i]; j++)
		{
			connect[i][j] = new int[layer[i-1]];

			for (k=0; k<layer[i-1]; k++)
			{
				fscanf(fp, "%d ", &(connect[i][j][k]));
			}
		}
	}

	mean[0] = new float[layer[1]];
	for (i=0; i<layer[1]; i++)
		fscanf(fp, "%f ", &(mean[0][i]));
	
	sigma[0] = new float[layer[1]];
	for (i=0; i<layer[1]; i++)
		fscanf(fp, "%f ", &(sigma[0][i]));

	mean[1] = new float[layer[3]];
	for (i=0; i<layer[3]; i++)
		fscanf(fp, "%f ", &(mean[1][i]));
	
	sigma[1] = new float[layer[3]];
	for (i=0; i<layer[3]; i++)
		fscanf(fp, "%f ", &(sigma[1][i]));

	fclose(fp);
}

void myFNN::ruleFNN()
{
	int rule_no = 0;
	int i,j,k,l,m;
	int rule_flag = 0;

	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\rule.txt", "wt");
	}

	printf("\n");

	int rule_cnt = 0;
	int flag_pre = 0, flag_con = 0;
	char str[100], str2[100];

	sprintf(str, "");

	for (k=0; k<layer[2]; k++)
	{
		flag_pre = flag_con = 0;
		sprintf(str, "");

		if (rules[k] == 0)
			continue;

		for (j=0; j<layer[1]; j++)
		{
			if (connect[2][k][j] == 1)
			{
				sprintf(str2, "%d ", j); 
				strcat(str, str2);

				flag_pre = 1;
			}
		}
		
		if (flag_pre == 1)
		{
			strcat(str, "=> ");
			for (l=0; l<layer[3]; l++)
			{
				if (connect[3][l][k] == 1)
				{
					sprintf(str2, "%d ", l);
					strcat(str, str2);

					flag_con = 1;
				}
			}

			if (flag_con == 1)
			{
				rule_cnt++;
				//printf("#%03d: %s %.3f\n", rule_cnt, str, out[2][k]);
				//printf("#%03d: %s\n", rule_cnt, str);
				printf("#%03d: %s \t\t\t %d\n", rule_cnt, str, k);

				if (FILE_OUT)
					fprintf(fp, "#%03d: %s\n", rule_cnt, str);
			}
		}
	}

	/*
	for (i=0; i<layer[2]; i++)
	{
		for (k=0; k<layer[3]; k++)
			//if (weight[3][k][i] == 1.0f)
			if (connect[3][k][i])
			{
				rule_no++;
				printf("#%03d: ", rule_no);

				if (FILE_OUT)
					fprintf(fp, "#%03d: ", rule_no);

				for (j=0; j<layer[1]; j++)
				{
					//if (weight[2][i][j] == 1.0f)
					if (connect[2][i][j])
					{
						printf("%d ", j);

						if (FILE_OUT)
							fprintf(fp, "%d ", j);
					}
				}
				printf("=> %d\n", k);

				if (FILE_OUT)
					fprintf(fp, "=> %d\n", k);
			}
	}
	*/

	printf("\n");

	if (FILE_OUT)
		fclose(fp);
}

// 020723
void myFNN::loadDataN1(char *str, int cond, int des, int t_len, int l_len, int e_len, int normalize)
{
	int i,j;
	//float *cond_dat, *des_dat;
	float fmin, fmax;

	t_no = t_len;
	l_no = l_len;
	e_no = e_len;
	in_no = cond;
	out_no = des;

	tin = new float[t_no*in_no];
	lin = new float[l_no*in_no];
	ein = new float[e_no*in_no];
	tout = new float[t_no*out_no];
	lout = new float[l_no*out_no];
	eout = new float[e_no*out_no];

	FILE *fp;
	fp = fopen(str, "rt");

	/*
	cond_dat = new float[t_len*cond];
	des_dat = new float[t_len*des];
	*/

	for (i=0; i<t_len*des; i++)
		tout[i] = LOW;

	//printf("\nData Loading... : %s \n", str);

	for (i=0; i<t_len; i++)
	{
		for (j=0; j<in_no; j++)
			fscanf(fp, "%f ", &(tin[i*cond+j]));
		
		fscanf(fp, "%d ", &j);
		tout[i*des+j] = HI;
	}

	//printf("\nData Normalize...\n");
	if (normalize)
	{
		for (i=0; i<cond; i++)
		{
			fmax = -1e5;
			fmin = 1e5;

			for (j=0; j<t_len; j++)
			{
				if (tin[j*cond+i] > fmax)
					fmax = tin[j*cond+i];
				if (tin[j*cond+i] < fmin)
					fmin = tin[j*cond+i];
			}

			for (j=0; j<t_len; j++)
			{
				tin[j*cond+i] = (tin[j*cond+i] - fmin)/(fmax - fmin);
			}
		}
	}

	// learning set
	for (i=0; i<l_len; i++)
		for (j=0; j<in_no; j++)
			lin[j+i*in_no] = tin[j+i*in_no];
	
	for (i=0; i<l_len; i++)
	{
		for (j=0; j<out_no; j++)
		{
			lout[j+i*out_no] = tout[j+i*out_no];
		}
	}

	// eval. set
	for (i=0; i<e_len; i++)
		for (j=0; j<in_no; j++)
			ein[j+i*in_no] = tin[j+(l_len+i)*in_no];
	
	for (i=0; i<e_len; i++)
	{
		for (j=0; j<out_no; j++)
		{
			eout[j+i*out_no] = tout[j+(l_len+i)*out_no];
		}
	}

	fclose(fp);

	/*
	delete [] des_dat;
	des_dat = NULL;
	delete [] cond_dat;
	cond_dat = NULL;
	*/
}

void myFNN::ioHistogram()
{
	int j,k,l;
	int sum = 0;
	int *ioHistX = new int[layer[1]];
	int *ioHist = new int[layer[1]*layer[3]];

	for (j=0; j<layer[1]*layer[3]; j++)
		ioHist[j] = 0;

	for (k=0; k<layer[2]; k++)
	{
		for (j=0; j<layer[1]; j++)
		{
			if (connect[2][k][j])
			{
				for (l=0; l<layer[3]; l++)
				{
					if (connect[3][l][k])
						ioHist[l+j*layer[3]]++;
				}
			}
		}
	}

	printf("\n");
	for (j=0; j<layer[1]; j++)
	{
		sum = 0;
		for (l=0; l<layer[3]; l++)
		{
			printf("%d ", ioHist[l+j*layer[3]]);
			
			if (l%2 == 0)
				sum -= ioHist[l+j*layer[3]];
			else
				sum += ioHist[l+j*layer[3]];
		}

		ioHistX[j] = sum;
		printf("\t %d", ioHistX[j]);
		
		printf("\n");

		//ioHistX[j] = sum;
		//printf("%d ", ioHistX[j]);
	}
	//printf("\n");


	/*
	printf("\n");
	for (j=0; j<layer[1]; j++)
	{
		sum = 0;
		for (l=0; l<layer[3]; l++)
			sum += ioHist[l+j*layer[3]];

		ioHistX[j] = sum;
		printf("%d ", ioHistX[j]);
	}
	printf("\n");
	*/

	delete [] ioHistX;
	ioHistX = NULL;
	delete [] ioHist;
	ioHist = NULL;
}

void myFNN::setMembership()
{
	float *imean = new float[layer[1]];
	float *istd = new float[layer[1]];
	float *omean = new float[layer[3]];
	float *ostd = new float[layer[3]];

	float *pre_dat = new float[t_no];
	float *mymean, *mystd;

	int bias = 0;
	int i,j;

	for (i=0; i<in_no; i++)
	{
		for (j=0; j<t_no; j++)
			pre_dat[j] = tin[j*in_no+i];

		mymean = new float[itlayer[i]];
		mystd = new float[itlayer[i]];
		kSOC(pre_dat, t_no, mymean, mystd, itlayer[i]);

		if (i>0)
			bias += itlayer[i-1];

		for (j=0; j<itlayer[i]; j++)
		{
			imean[bias + j] = mymean[j];
			istd[bias + j] = mystd[j];
		}

		kSOCFNN((imean + bias), (istd + bias), 0, i);

		delete [] mystd;
		delete [] mymean;
	}

	bias = 0;
	for (i=0; i<out_no; i++)
	{
		for (j=0; j<t_no; j++)
			pre_dat[j] = tout[j*out_no+i];

		mymean = new float[otlayer[i]];
		mystd = new float[otlayer[i]];
		kSOC(pre_dat, t_no, mymean, mystd, otlayer[i]);

		if (i>0)
			bias += otlayer[i-1];

		for (j=0; j<otlayer[i]; j++)
		{
			omean[bias + j] = mymean[j];
			ostd[bias + j] = mystd[j];
		}

		kSOCFNN((omean + bias), (ostd + bias), 1, i);

		delete [] mystd;
		delete [] mymean;
	}

	delete [] pre_dat;

	delete [] ostd;
	ostd = NULL;
	delete [] omean;
	omean = NULL;

	delete [] istd;
	istd = NULL;
	delete [] imean;
	imean = NULL;
}

void myFNN::recTest()
{
	int i, j;
	float ratio = 0.0f, max;
	int index, maxindex;
	float *yout = new float[layer[4]];

	int **rmat = new int *[out_no];
	for (i=0; i<out_no; i++)
	{
		rmat[i] = new int[out_no];
		for (j=0; j<out_no; j++)
			rmat[i][j] = 0;
	}

	for (i=0; i<e_no; i++)		
	{
		max = -1.0f;
		evalFNN((ein + i*in_no), yout, 0);

		//aoutFNN(4);
		for (j=0; j<out_no; j++)
		{
			if (eout[i*out_no+j] == HI)
				index = j;
		}
		//si4FNN(index);

		for (j=0; j<out_no; j++)
		{
			if (yout[j] > max)
			{
				max = yout[j];
				maxindex = j;
			}
		}

		rmat[index][maxindex] += 1;

		if (maxindex == index)
			ratio += 1.0f;
	}

	printf("\n");

	for (i=0; i<out_no; i++)
	{
		for (j=0; j<out_no; j++)
		{
			printf("%d ", rmat[i][j]);
		}
		printf("\n");
	}
	
	printf("\nRecog. Rate (Te) = %.2f\n", ratio/(float)e_no);

	delete [] rmat;
	delete [] yout;
	yout = NULL;
}

void myFNN::recTrain()
{
	FILE *fp;
	//fp = fopen(".\\FNN021111outsi.dat", "awt");
	//fp = fopen(".\\fireSTR.dat", "awt");
	//fp = fopen(".\\FNN021128SI.dat", "awt");
	fp = fopen(".\\FNN021129.dat", "awt");
	
	float mySI[3] = {0.0f, 0.0f, 0.0f};
	float mySIint = 0.0f;
	float max_val;

	int i, j, k;
	float ratio = 0.0f, max;
	int index, maxindex;
	
	int **rmat = new int *[out_no];
	for (i=0; i<out_no; i++)
	{
		rmat[i] = new int[out_no];
		for (j=0; j<out_no; j++)
			rmat[i][j] = 0;
	}

	float *yout = new float[layer[4]];

	int emotion_count = 0;

	float SIavg = 0.0f;

	for (i=0; i<l_no; i++)		
	{
		max = -1.0f;
		evalFNN((lin + i*in_no), yout, 0);

		//aoutFNN(4);
		for (j=0; j<out_no; j++)
		{
			if (lout[i*out_no+j] == HI)
				index = j;

			//fprintf(fp, "%.3f ", yout[j]);
		}

		/*
		if (emotion_count == index)
		{
			for (k=0; k<layer[2]; k++)
				fprintf(fp, "%.3f ", out[2][k]);

			fprintf(fp, "\n");

			emotion_count++;
		}
		*/

		//si4FNN(index);

//////////////////////////////////////////////////////

		max_val = -1.0f;

		for (j=0; j<layer[4]; j++)
		{
			if (j != index)
			{
				if (out[4][j] > max_val)
				{
					max_val = out[4][j];
				}
			}
		}

		// 02.11.11
		// 1) target sufficiency
		mySI[0] = (1.0f)/(1.0f + exp(-BETA_0*(out[4][index] - HI + ALPHA_0)));
   		// 2) dissimilarity between target and non-targets
		mySI[1] = (1.0f)/(1.0f + exp(-BETA_1*(out[4][index] - max_val + ALPHA_1)));
		// 3) non-target sufficiency
		mySI[2] = (1.0f)/(1.0f + exp(BETA_0*(max_val - HI + ALPHA_0)));

		mySIint = mySI[0]*mySI[1]*mySI[2];

		SIavg += mySI[1];

		//fprintf(fp, "%1d %.3f %.3f %.3f %.3f \n", index, mySI[0], mySI[1], mySI[2], mySIint);

//////////////////////////////////////////////////////

		for (j=0; j<out_no; j++)
		{
			if (yout[j] > max)
			{
				max = yout[j];
				maxindex = j;
			}
		}

		rmat[index][maxindex] += 1;

		if (maxindex == index)
			ratio += 1.0f;

	}

	SIavg /= (float)l_no;
	//fprintf(fp, "%.3f ", SIavg);

	for (i=0; i<out_no; i++)
		fprintf(fp, "%d ", rmat[i][i]);
	
	fprintf(fp, "\n");


	/*
	printf("\n");
	/*
	for (i=0; i<out_no; i++)
	{
		for (j=0; j<out_no; j++)
		{
			printf("%d ", rmat[i][j]);
		}
		printf("\n");
	}
	*/

	//aoutFNN(2);
	printf("\nRecog. Rate (Tr) = %.2f\n", ratio/(float)l_no);

	delete [] rmat;

	delete [] yout;
	yout = NULL;

	fclose(fp);
}

void myFNN::ruleCombination()
{
	int i, j, k, l, m;
	int same_cnt = 0;
	int max_index, max_pre;

	int *same_con = new int[layer[2]];
	int *over_pre = new int[layer[1]];
	int *cover = new int[layer[0]];

	// rule combination
	for (l=0; l<layer[3]; l++)
	{
		//max_pre = -1;
		same_cnt = 0;
		for (i=0; i<layer[0]; i++)	cover[i] = 0;
		for (j=0; j<layer[1]; j++)	over_pre[j] = 0;
		for (k=0; k<layer[2]; k++)	same_con[k] = 0;

		// check connection between rule node (k) and consequent node (l)
		for (k=0; k<layer[2]; k++)
		{
			if (connect[3][l][k])
			{
				same_con[k] = 1;
				same_cnt++;

				// check connection between rule node (k) and antecedant node (j)
				for (j=0; j<layer[1]; j++)
				{
					if (connect[2][k][j])
					{
						over_pre[j]++;
					}
				}
			}
		}

		int cont_flag = 0;
		for (j=0; j<layer[1]; j++)
			if (over_pre[j] == same_cnt)
				cont_flag = 1;

		if (cont_flag == 1)
		{
			// check full of connection for each membership set of each variable (input, feature)
			int cbias = 0;
			int no_cover = 0;
			int no_input_cnt = 0;
			for (i=0; i<layer[0]; i++)
			{
				if (i>0)
					cbias += itlayer[i-1];

				for (j=0; j<itlayer[i]; j++)
				{
					if (over_pre[cbias + j] > 0)
						cover[i]++;
				}

				if (cover[i] == itlayer[i])
					no_cover++;

				if (cover[i]>0)
					no_input_cnt++;
			}

			int cnt_1;
			// check if there is a chance for combining rules.
			//if (no_cover < layer[0])
			if (no_cover < no_input_cnt)
			{
				cbias = 0;
				for (i=0; i<layer[0]; i++)
				{
					if (i>0)
						cbias += itlayer[i-1];

					if (cover[i] == itlayer[i])
					{
						for (j=0; j<itlayer[i]; j++)
						{
							for (k=0; k<layer[2]; k++)
							{
								if (connect[2][k][j+cbias] && same_con[k] && connect[3][l][k])
									connect[2][k][j+cbias] = 0; //= connect[3][l][k] = 0;
							}
						}
					}
					//else if (cover[i] == 1)
					else
					{
						for (j=0; j<itlayer[i]; j++)
						{
							if (over_pre[cbias+j]==same_cnt)
							{
								cnt_1 = over_pre[cbias+j]-1;

								for (k=layer[2]-1; k>=0; k--)
								{
									if (connect[2][k][j+cbias] && same_con[k] && connect[3][l][k] && (cnt_1>0))
									{
										connect[2][k][j+cbias] = 0;//= connect[3][l][k] = 0;
										cnt_1--;
									}
								}
							}
						}
					}
				}
			}
		}

		int no_branch = 0;
		for (k=0; k<layer[2]; k++)
		{
			for (j=0; j<layer[1]; j++)
			{
				if (connect[2][k][j])
					no_branch++;
			}

			if (no_branch == 0)
				connect[3][l][k] = 0;
		}
	}

	delete [] cover;
	delete [] over_pre;
	delete [] same_con;

	deleteLinks();
}

// inout: input(0), output(1)
void myFNN::ShowMF(int inout)
{
	int x, y, z;
	int yp;
	int xbin = 200;
	int ybin = 100;
	float xf, yf, zf;
	char pixel;

	IPLIMAGE imgIn = cvCreateImage(cvSize(xbin, ybin*layer[0]), IPL_DEPTH_8U, 1);
	IPLIMAGE imgOut = cvCreateImage(cvSize(xbin, ybin*layer[4]), IPL_DEPTH_8U, 1);

	iplSet(imgIn, 255);
	iplSet(imgOut, 255);

	int bias = 0;

	switch(inout)
	{
		case 0:
			for (y=0; y<layer[0]; y++)
			{
				if (y>0)
					bias += itlayer[y-1];

				cvLine(imgIn, cvPoint(0, (y+1)*100), cvPoint(xbin-1, (y+1)*100), 0x00, 1);
				for (x=0; x<xbin; x++)
				{
					xf = (float)x/(float)(xbin - 1);

					for (z=0; z<itlayer[y]; z++)
					{
						zf = (float)exp(-(xf - mean[0][bias + z])*(xf - mean[0][bias + z])/(sigma[0][bias + z]*sigma[0][bias + z]));
						yp = (int)((float)(y+1)*100.0 - 100.0*zf);

						yp = __max(__min(yp, ybin*layer[0]-1), 0);

						if (z%2 == 0)
							pixel = 32;
						else
							pixel = 64;
						
						iplPutPixel(imgIn, x, yp, &pixel);
					}
				}
			}
			cvvResizeWindow("MFin", xbin, imgIn->height);
			cvvShowImage("MFin", imgIn);
			break;
		case 1:
			for (y=0; y<layer[4]; y++)
			{
				if (y>0)
					bias += otlayer[y-1];

				cvLine(imgOut, cvPoint(0, (y+1)*100), cvPoint(xbin-1, (y+1)*100), 0x00, 1);
				for (x=0; x<xbin; x++)
				{
					xf = (float)x/(float)(xbin - 1);

					for (z=0; z<otlayer[y]; z++)
					{
						zf = (float)exp(-(xf - mean[1][bias + z])*(xf - mean[1][bias + z])/(sigma[1][bias + z]*sigma[1][bias + z]));
						yp = (int)((float)(y+1)*100.0 - 100.0*zf);

						yp = __max(__min(yp, ybin*layer[4]-1), 0);

						if (z%2 == 0)
							pixel = 32;
						else
							pixel = 64;

						iplPutPixel(imgOut, x, yp, &pixel);
					}
				}
			}
			cvvResizeWindow("MFout", xbin, imgOut->height);
			cvvShowImage("MFout", imgOut);
			break;
	}

	cvvWaitKey(0);

	cvReleaseImage(&(imgOut));
	cvReleaseImage(&(imgIn));
}

void myFNN::ShowError(int time, float error)
{
	int xbin = 200;
	int ybin = 100;
	char pixel = 128;
	float xf, yf;

	cvvResizeWindow("Error", xbin, ybin);

	if (time == 0)
	{
		imgError = cvCreateImage(cvSize(xbin, ybin), IPL_DEPTH_8U, 1);
		iplSet(imgError, 255);
	}

	xf = (float)(time*xbin)/(float)epoch;
	yf = (float)(log10((double)error) - log10((double)err_criteria))/(float)(4.0 - log10((double)err_criteria));

	iplPutPixel(imgError, (int)xf, ybin - (int)(yf*(float)ybin) - 1, &pixel);
	
	cvvShowImage("Error", imgError);
}

float myFNN::fSMAB(float m1, float m2, float s1, float s2)
{
	return (fSMcross(m1, m2, s1, s2)/fSMunion(m1, m2, s1, s2));
}

float myFNN::fSMcross(float m1, float m2, float s1, float s2)
{
	float value;
	float value2, value3, value4;

	if (m1 >= m2)
	{
		value2 = __max(0, m2-m1+sqrt(M_PI)*(s1+s2));
		value3 = __max(0, m2-m1+sqrt(M_PI)*(s1-s2));
		value4 = __max(0, m2-m1-sqrt(M_PI)*(s1-s2));
		value = 0.5*((value2*value2)/(sqrt(M_PI)*(s1+s2)) + (value3*value3)/(sqrt(M_PI)*(s2-s1)) + (value4*value4)/(sqrt(M_PI)*(s1-s2)));
	}
	else
	{
		value2 = __max(0, m1-m2+sqrt(M_PI)*(s2+s1));
		value3 = __max(0, m1-m2+sqrt(M_PI)*(s2-s1));
		value4 = __max(0, m1-m2-sqrt(M_PI)*(s2-s1));
		value = 0.5*((value2*value2)/(sqrt(M_PI)*(s2+s1)) + (value3*value3)/(sqrt(M_PI)*(s1-s2)) + (value4*value4)/(sqrt(M_PI)*(s2-s1)));
	}

	return value;
}

float myFNN::fSMunion(float m1, float m2, float s1, float s2)
{
	return (s1*sqrt(M_PI) + s2*sqrt(M_PI) - fSMcross(m1, m2, s1, s2));
}

void myFNN::ShowFNND(int nodes)
{
	int i, j, k;
	int xbin = 300;
	int ybin = 400;

	static int image_num = 0;

	CvPoint **FNNpt;
	char str[100];
	FNNpt = new CvPoint *[no_layer];

	//cvvResizeWindow("FNN", xbin, ybin);
	
	IPLIMAGE imgFNNc = cvCreateImage(cvSize(xbin, ybin), IPL_DEPTH_8U, 3);
	iplSet(imgFNNc, 0xFFFFFF);

	for (i=0; i<no_layer; i++)
	{
		FNNpt[i] = new CvPoint[layer[i]];

		for (j=0; j<layer[i]; j++)
		{
			FNNpt[i][j].x = (int)((float)xbin/(float)(layer[i]+1)*(float)(j+1));
			FNNpt[i][j].y = ybin - 1 - (i+1)*ybin/(no_layer+1);

			cvCircle(imgFNNc, FNNpt[i][j], 5, 0x0000FF, -1);
		}
	}

	int count_connect;

	for (i=3; i>0; i--)
	{
		for (k=0; k<layer[i]; k++)
		{
			count_connect = 0;

			for (j=0; j<layer[i+1]; j++)
			{
				if (connect[i+1][j][k] == 1)
					count_connect++;			
			}

			if (count_connect == 0)
			{
				cvCircle(imgFNNc, FNNpt[i][k], 5, 0xFFFFFF, -1);
				FNNpt[i][k] = cvPoint(0, 0);
			}
		}
	}

	for (i=1; i<no_layer; i++)
	{
		count_connect = 0;

		for (j=0; j<layer[i]; j++)
			for (k=0; k<layer[i-1]; k++)
			{
				if ((connect[i][j][k] == 1) && (FNNpt[i][j].x != 0) && (FNNpt[i-1][k].x != 0))
				{
					count_connect++;
					cvLine(imgFNNc, FNNpt[i][j], FNNpt[i-1][k], 0xFF0000, 1);
				}
			}

		sprintf(str, "%d", count_connect);
		cvPutText(imgFNNc, str, cvPoint(xbin - 10, FNNpt[i][j-1].y + 30), &font, (int)(0x000000));
	}

	sprintf(str, ".\\REC#%d.jpg", image_num++);
	cvvSaveImage(str, imgFNNc);

	//cvvShowImage("FNN", imgFNN);
	//cvvWaitKey(0);
	cvReleaseImage(&(imgFNNc));

	delete [] FNNpt;
}

void myFNN::deleteLinks()
{
	int i, j, k, l, m;
	int count_connect;

	// input side.
	for (j=0; j<layer[1]; j++)
	{
		count_connect = 0;

		for (i=0; i<layer[0]; i++)
		{
			if (connect[1][j][i] == 1)
			{
				count_connect++;
			}
		}

		if (count_connect == 0)
		{
			for (k=0; k<layer[2]; k++)
			{
				if (connect[2][k][j] == 1)
					connect[2][k][j] = 0;
			}
		}
	}

	// rule side.
	for (k=0; k<layer[2]; k++)
	{
		count_connect = 0;

		for (j=0; j<layer[1]; j++)
		{
			if (connect[2][k][j] == 1)
				count_connect++;
		}

		if (count_connect == 0)
		{
			rules[k] = 0;

			for (l=0; l<layer[3]; l++)
			{
				if (connect[3][l][k] == 1)
					connect[3][l][k] = 0;
			}
		}
	}

	// out side.
	for (l=0; l<layer[3]; l++)
	{
		count_connect = 0;

		for (k=0; k<layer[2]; k++)
		{
			if (connect[3][l][k] == 1)
				count_connect++;
		}

		if (count_connect == 0)
		{
			for (m=0; m<layer[4]; m++)
			{
				if (connect[4][m][l] == 1)
					connect[4][m][l] = 0;
			}
		}
	}

	/*
	for (i=2; i>0; i--)
	{
		for (k=0; k<layer[i]; k++)
		{
			count_connect = 0;

			for (l=0; l<layer[i+1]; l++)
			{
				if (connect[i+1][l][k])
					count_connect++;
			}

			if (count_connect == 0)
			{
				for (j=0; j<layer[i-1]; j++)
				{
					if (connect[i][k][j] == 1)
						connect[i][k][j] = 0;
				}
			}
		}
	}
	*/
}

void myFNN::deleteInputNodes(int *nodes, int no_nodes)
{
	int i, j;

	for (j=0; j<no_nodes; j++)
		for (i=0; i<layer[2]; i++)
			connect[2][i][nodes[j]] = 0;

	deleteLinks();
}
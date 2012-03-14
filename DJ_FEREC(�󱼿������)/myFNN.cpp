// myFNN.cpp: implementation of the myFNN class.
//
//////////////////////////////////////////////////////////////////////

#pragma comment(lib,"rsdll.lib")

#include "stdafx.h"
#include "myFNN.h"

#define DLLEXPORT __declspec( dllexport )
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <float.h>
#include "rough.h"

#include "MainFrm.h"

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

	io_histogram_flag = FALSE;
	rule_combination_flag = FALSE;
	save_si = FALSE;
	save_rfr = FALSE;
	kill_nodes = FALSE;

	abs_sum_weight = 1e-3;
	max_weight = 1e-2;

	tin = NULL;
	lin = NULL;
	ein = NULL;
	tout = NULL;
	lout = NULL;
	eout = NULL;

	cvInitFont(&font, CV_FONT_VECTOR0, 0.2f, 0.2f, 0.0f, 1);

	out = NULL;
	weight = NULL;
	connect = NULL;
	delta = NULL;
	rout = NULL;
	rules = NULL;
	in = NULL;
	mean[0] = NULL;
	mean[1] = NULL;
	sigma[0] = NULL;
	sigma[1] = NULL;
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
// 20030402
void myFNN::kSOC(float *data, int no_data, float *mean, float *std, int no_cluster)
{
	int i, j;
	float a;
	int mc;
	float min = 1e3;
	float r = 2.0f;

	float datarange[2] = {1e3, -1e3};

	for (i=0; i<no_data; i++)
	{
		datarange[0] = __min(datarange[0], data[i]);
		datarange[1] = __max(datarange[1], data[i]);
	}

	for (i=0; i<no_cluster; i++)
	{
		//mean[i] = (float)((float)i/(float)no_cluster + 0.5/(float)no_cluster);
		mean[i] = (float)(((float)i)*(datarange[1]-datarange[0])/((float)no_cluster)
			+(0.5)*(datarange[1]-datarange[0])/((float)no_cluster)) + datarange[0];
	}

	for (i=0; i<no_data; i++)
	{
		a = (float)(1.0*(float)(no_data - i)/(float)(no_data));

		min = 1e3;

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
		min = 1e3;
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

// 020510 org.
// up-down | down-up
// i, j, k, l, m for each layer!!!
// 021217
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
			if (connect[1][cbias+j][i] == 1)
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
			out[1][j] = gaussMF(in[1][j], mean[0][j], sigma[0][j]);

	}

	for (k=0; k<layer[2]; k++)
	{
		min = 1e2;

		for (j=0; j<layer[1]; j++)
		{
			if (connect[2][k][j] == 1)
			{
				if (out[1][j] < min)
					min = out[1][j];
			}
		}

		out[2][k] = min;
	}

	if (flag == 0)
	{
		// down-up
		for (l=0; l<layer[3]; l++)
		{
			sum = 0.0f;

			for (k=0; k<layer[2]; k++)
			{
				if (connect[3][l][k])
					sum += out[2][k];
			}

			out[3][l] = __min(1.0f, sum);
		}

		cbias = 0;

		for (m=0; m<layer[4]; m++)
		{
			if (m>0)
				cbias += otlayer[m-1];

			sum = 0.0f;
			ssum = 0.0f;

			for (l=0; l<otlayer[m]; l++)
				if (connect[4][m][cbias+l] == 1)
				{
					sum += (mean[1][cbias+l]*sigma[1][cbias+l])*out[3][cbias+l];
					ssum += (sigma[1][cbias+l])*out[3][cbias+l];
				}

			sssum = sum/ssum;

			if (_isnan((double)sssum))
				*(output + m) = out[4][m] = 0.0f;
			else
				*(output + m) = out[4][m] = sssum;
		}
	}
	else
	{
		// up-down
		for (l=0; l<layer[3]; l++)
		{
			for (m=0; m<layer[4]; m++)
			{
				sssum = 1.0f/sigma[1][l];

				if (connect[4][m][l] == 1)
				{
					if (_isnan((double)sssum))
						rout[l] = 0.0f;
					else
						rout[l] = gaussMF(*(output+m), mean[1][l], sigma[1][l]);
				}
			}
		}
	}
}

// 20020723
// 20021204	-> rule node pruning by firing rate for no_sample (X)
// 20021211
void myFNN::compFNN(float *input, float *output, int no_sample)
{
	// define variables
	CTime t = CTime::GetCurrentTime();
	register __int64 freq, start, end; 
	register float Time1;//, Time2;
	register bool err;

	int i, j, k, s;//, l, m, s;
	float learn = 0.0f;
	int cbias = 0;
	float max = 0.0f;
	int max_index = 0;
	float sum_learn;
	long* rule_fire_rate = new long[layer[2]];

	//for (i=0; i<layer[2]; i++)	
	//	rule_fire_rate[i] = 0L;
	//int iter = 0;

	CHECK_TIME_START;
	for (s=0; s<no_sample; s++)
	{
		evalFNN((input+s*layer[0]), (output+s*layer[4]), 1);

		//iter = 0;

		do {
			sum_learn = 0.0f;
			//iter++;

			for (j=0; j<layer[3]; j++)
				for (k=0; k<layer[2]; k++)
				{
					if (rout[j] > 0.5f)
						learn = rout[j]*(-weight[3][j][k] + out[2][k]);
					else
						learn = 0.0f;

					//if (out[2][k] > 0.0f)
					//	rule_fire_rate[k]++;
					
					sum_learn += fabs(learn);
					weight[3][j][k] += learn;
				}
		//} while (sum_learn > 1e-3f);
		} while (sum_learn > abs_sum_weight);

		//for (k=0; k<layer[2]; k++)
		//	if (out[2][k] > 0.0f)
		//		rule_fire_rate[k]++;
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
			//if (max > 1e-2f)
			if (max > max_weight)
			{
				weight[3][max_index][j] = 1.0f;
				connect[3][max_index][j] = 1;
			}
		}
	}

	deleteLinks();

	CHECK_TIME_END(Time1, err);
	message.Format("\r\nStructure Learning [%.2f ms]", Time1);
	logview->AddLine(message);
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
	if (sigma[1])
		delete [] sigma[1];
	if (sigma[0])
		delete [] sigma[0];
	if (mean[1])
		delete [] mean[1];
	if (mean[0])
		delete [] mean[0];
	if (in)
		delete [] in;
	if (rules)
		delete [] rules;
	if (rout)
		delete [] rout;
	if (delta)
		delete [] delta;
	if (connect)
		delete [] connect;
	if (weight)
		delete [] weight;
	if (out)
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
					if (connect[i][j][k] == 1)
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

// 20030402 ???
void myFNN::learnFNN(float *input, float *output, int no_sample)
{
	int i, j, k, t;
	int cbias = 0;
	float sum = 0.0f;
	float msum = 0.0f;
	float mssum = 0.0f;

	int min_index;
	int exam;
	float min = 1e2;
	float tsum = 0.0f;
	float Pmean, Pstd;
	float *error = new float[layer[4]];
	float *out_ = new float[layer[4]];
	float errors;
	int** nodes = new int*[5];

	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\error.dat", "wt");
	}

	for (int dummy=0; dummy<5; dummy++)
	{
		nodes[dummy] = new int[layer[dummy]];

		for (int dummy2=0; dummy2<layer[dummy]; dummy2++)
			nodes[dummy][dummy2] = 0;
	}

	error_profile = new float[epoch];

	message.Format("\r\nLearning for Epoch [%d] and Error Criteria [%.0e]", epoch, err_criteria);
	logview->AddLine(message);

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

		error_profile[t] = errors;
		
		if (FILE_OUT)	fprintf(fp, "%d %f\n", t, errors);
			
		if (errors < err_criteria)
		{
			message.Format("Learning Stopped at %dth Trial [MSE %.0e]", t, errors);
			logview->AddLine(message);
			break;
		}
		
		// layer 5
		cbias = 0;

		for (i=0; i<layer[4]; i++)
		{
			if (i>0)	cbias += otlayer[i-1];

			sum = 0.0f;	msum = 0.0f;

			for (j=0; j<otlayer[i]; j++) 
			{
				sum += (sigma[1][cbias + j]*out[3][cbias + j]);
				msum += (mean[1][cbias + j]*sigma[1][cbias + j]*out[3][cbias + j]);
			}

			for (j=0; j<otlayer[i]; j++)
			{	
				delta[3][cbias + j] = error[i]*(mean[1][cbias + j]*sigma[1][cbias + j]*sum - msum*sigma[1][cbias + j])/(sum*sum);
				Pmean = error[i]*(sigma[1][cbias + j]*out[3][cbias + j])/sum;
				Pstd = error[i]*(mean[1][cbias + j]*out[3][cbias + j]*sum - msum*out[3][cbias + j])/(sum*sum);

				mean[1][cbias + j] += learn_rate*Pmean;
				sigma[1][cbias + j] += learn_rate*Pstd;				
			}
		}

		// layer 4	see above.
		// layer 3
		for (i=0; i<layer[2]; i++)
		{
			delta[2][i] = 0.0f;

			for (j=0; j<layer[3]; j++)
				if (connect[3][j][i] == 1)// && nodes[3][j])
				{
					delta[2][i] += delta[3][j];
					nodes[2][i] = 1;
				}
		}

		// layer 2
		for (i=0; i<layer[1]; i++)
		{
			tsum = 0.0f;

			for (j=0; j<layer[2]; j++)
			{
				if (connect[2][j][i] == 1)
				{
					min = 1e2;
					min_index = -1;

					for (k=0; k<layer[1]; k++)
						if ((connect[2][j][k] == 1) && nodes[2][j])
						{
							nodes[1][k] = 1;

							if (out[1][k] < min)
							{
								min = out[1][k];
								min_index = k;
							}
						}
					
					if (i == min_index)
						tsum += delta[2][j];
				}
			}

			Pmean = tsum*out[1][i]*2.0*(in[1][i] - mean[0][i])/(sigma[0][i]*sigma[0][i]);
			Pstd = Pmean*(in[1][i] - mean[0][i])/sigma[0][i];

			if (nodes[1][i])
			{
				mean[0][i] += -learn_rate*Pmean;
				sigma[0][i] += -learn_rate*Pstd;
			}
		}

		for (i=0; i<layer[1]; i++)	nodes[1][i] = 0;
		for (i=0; i<layer[2]; i++)	nodes[2][i] = 0;
		//for (i=0; i<layer[3]; i++)	nodes[3][i] = 0;
	}

	ShowErrorProfile(error_profile, 0, t);

	if (t == epoch)
	{
		message.Format("Learning Failed [MSE %.0e]", errors);
		logview->AddLine(message);
	}

	if (FILE_OUT)
		fclose(fp);

//	delete [] nodes;			nodes = NULL;
	delete [] error_profile;	error_profile = NULL;
	delete [] out_;				out_ = NULL;
	delete [] error;			error = NULL;
}

void myFNN::learnFNN2(float *input, float *output, int no_sample, int *jnodes, int no_nodes)
{
	// define variables
	int i, j, k, t;//l, m, t;
//	int l1, l2, l3, l4, l5;
	int cbias = 0;
	float sum = 0.0f;
	float msum = 0.0f;
	float mssum = 0.0f;

	int min_index;
	int exam;
	float min = 1e2;
	float tsum = 0.0f;
//	float max_error;
	float Pmean, Pstd;
	float *error = new float[layer[4]];
	float *out_ = new float[layer[4]];
	float errors;
	int** nodes = new int*[5];

	FILE *fp;

	if (FILE_OUT)
	{
		fp = fopen(".\\error.dat", "wt");
	}

	for (int dummy=0; dummy<5; dummy++)
	{
		nodes[dummy] = new int[layer[dummy]];

		for (int dummy2=0; dummy2<layer[dummy]; dummy2++)
			nodes[dummy][dummy2] = 0;
	}

	error_profile = new float[epoch];

	message.Format("\r\nLearning for Epoch [%d] and Error Criteria [%.0e]", epoch, err_criteria);
	logview->AddLine(message);

	int check_inputs = 0;

	for (t=0; t<epoch; t++)
	{
		exam = rand()%no_sample;

		if (no_nodes != 0)
		{
			for (i=0; i<no_nodes; i++)
				if (*(output + exam*layer[4] + jnodes[i]) == HI)
				{
					check_inputs = 1;
					break;
				}

			if (check_inputs == 0)
				continue;
		}

		evalFNN((input + exam*layer[0]), out_, 0);
		
		errors = 0.0f;
		for (i=0; i<layer[4]; i++)
		{
			error[i] = *(output + exam*layer[4] + i) - out_[i];
			errors += (float)(0.5*(error[i]*error[i]));
		}

		error_profile[t] = errors;
		
		//if (t%100 == 0)
		//	ShowError(t, errors);
		
		if (FILE_OUT)
			fprintf(fp, "%d %f\n", t, errors);
			
		if (errors < err_criteria)
		{
			//printf("#%d : %.3f\n", t, errors);
			//break;

			/*
			if (exam < layer[4])
			{
				// 20030402
				// temporally blocked
				message.Format("Example #%d (%dth iteration)", exam, t);
				logview->AddLine(message);

				message.Format("");
				for (i=0; i<layer[4]; i++)
				{
					message2.Format("%.0e ", error[i]);
					message += message2;
				}
				logview->AddLine(message);
				exam++;
			}
			
			if (exam == layer[4])*/
			{
				message.Format("Learning Stopped at %dth Trial [MSE %.0e]", t, errors);
				logview->AddLine(message);
				break;
			}
		}
/*		else
		{
			if (exam == layer[4])
			{
				exam = 0;
				continue;
			}
		}	*/
		
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
				
				//if (_isnan(mssum))
				//{
				//	delta[3][cbias + j] = 0.0f;
				//	Pmean = Pstd = 0.0f;
				//}
				//else
				{
					delta[3][cbias + j] = error[i]*(mean[1][cbias + j]*sigma[1][cbias + j]*sum - msum*sigma[1][cbias + j])/(sum*sum);
					Pmean = error[i]*(sigma[1][cbias + j]*out[3][cbias + j])/sum;
					Pstd = error[i]*(mean[1][cbias + j]*out[3][cbias + j]*sum - msum*out[3][cbias + j])/(sum*sum);
				}

				//float degree1 = fSMAB(mean[1][cbias + j], mean[1][cbias + j] + learn_rate*Pmean,
				//	sigma[1][cbias+j], sigma[1][cbias+j]+learn_rate*Pstd);

				//if (((mean[1][cbias+j] + learn_rate*Pmean) >= 0.0f) || ((mean[1][cbias+j] + learn_rate*Pmean) <= 1.0f))
				{
					mean[1][cbias + j] += learn_rate*Pmean;
				}
				//mean[1][cbias + j] = __max(0.0f, __min(1.0f, mean[1][cbias+j]));

				//if (((sigma[1][cbias + j] + learn_rate*Pstd) > 0.0f))
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
				if (connect[3][j][i] && nodes[3][j])
				{
					delta[2][i] += delta[3][j];
					nodes[2][i] = 1;
				}
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
					if (connect[2][j][k] && nodes[2][j])
					{
						nodes[1][k] = 1;

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

			//if (_isnan((double)mssum))
			//	Pmean = Pstd = 0.0f;
			//else
			{
				Pmean = tsum*out[1][i]*2.0*(in[1][i] - mean[0][i])/(sigma[0][i]*sigma[0][i]);
				Pstd = Pmean*(in[1][i] - mean[0][i])/sigma[0][i];
			}

			//float degree2 = fSMAB(mean[0][i], mean[0][i] - learn_rate*Pmean,
			//	sigma[0][i], sigma[0][i]-learn_rate*Pstd);

			//if (nodes[1][i])
			{
				//if (((mean[0][i] - learn_rate*Pmean) >= 0.0f) || ((mean[0][i] - learn_rate*Pmean) <= 1.0f))
				{
					mean[0][i] += -(layer[4])*learn_rate*Pmean;
				}
				//mean[0][i] = __max(0.0f, __min(1.0f, mean[0][i]));

				//if ((sigma[0][i] - learn_rate*Pstd) > 0.0f)
					sigma[0][i] += -(layer[4])*learn_rate*Pstd;
				//sigma[0][i] = __max(0.0f, __min(1.0f, sigma[0][i]));
			}
		}
	}

	ShowErrorProfile(error_profile, 0, t);

	if (t == epoch)
	{
		message.Format("Learning Failed [MSE %.0e]", errors);
		logview->AddLine(message);
	}

	if (FILE_OUT)
		fclose(fp);

	delete [] nodes;			nodes = NULL;
	delete [] error_profile;	error_profile = NULL;
	delete [] out_;				out_ = NULL;
	delete [] error;			error = NULL;
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

void myFNN::saveFNN(const char* str)
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
				fprintf(fp, "%.3f ", weight[i][j][k]);
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
		fprintf(fp, "%.3f ", mean[0][i]);
	
	fprintf(fp, "\n");

	for (i=0; i<layer[1]; i++)
		fprintf(fp, "%.3f ", sigma[0][i]);

	fprintf(fp, "\n");

	for (i=0; i<layer[3]; i++)
		fprintf(fp, "%.3f ", mean[1][i]);
	
	fprintf(fp, "\n");

	for (i=0; i<layer[3]; i++)
		fprintf(fp, "%.3f ", sigma[1][i]);

	fclose(fp);
}

void myFNN::loadFNN(const char* str)
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

// 021007, Germany
void myFNN::loadFN(const char* str)
{
	int i;//,j,k;

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

	fclose(fp);
}

// 2002.12.16
// modify for rough set usage!
void myFNN::ruleFNN()
{
	int rule_no = 0;
	int i,j,k,r;//l,m, r;
	int rule_flag = 0;
	int rule_cnt = 0;
	int flag_pre = 0, flag_con = 0;
	char str[100], str2[100], pre_str[100];
	int rule_for_r = 0;	// the number of branches(rules) of the rth node in the 4th layer

	FILE *fp;

	for (r=0; r<layer[3]; r++)
	{
		sprintf(str, ".\\rule_for_%d.sys", r);
		fp = fopen(str, "wt");

		fprintf(fp, "NAME: RULES \n");
		fprintf(fp, "ATTRIBUTES: %d \n", layer[0]+1);

		rule_for_r = 0;

		for (k=0; k<layer[2]; k++)
			if (connect[3][r][k])	rule_for_r++;

		fprintf(fp, "OBJECTS: %d \n\n", rule_for_r+1);

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
					sprintf(str2, "%d ", j%2); 
					strcat(str, str2);

					flag_pre = 1;

					strcpy(pre_str, str);
				}
			}
			
			if (flag_pre == 1)
			{
				if (connect[3][r][k] == 1)
				{
					strcpy(str, pre_str);
					sprintf(str2, "%d ", r);
					strcat(str, str2);

					fprintf(fp, "%s \n", str);
					flag_con = 1;
				}

				if (flag_con == 1)
				{
					rule_cnt++;
					//printf("#%03d: %s %.3f\n", rule_cnt, str, out[2][k]);
					//printf("#%03d: %s\n", rule_cnt, str);
					//intf("#%03d: %s \t\t\t %d\n", rule_cnt, str, k);

					//if (FILE_OUT)
						//fprintf(fp, "#%03d: %s\n", rule_cnt, str);
				}
			}
		}

		sprintf(str, "");
		for (i=0; i<layer[0]; i++)
			strcat(str, "9 ");

		sprintf(str2, "%d", r+1);
		strcat(str, str2);
		
		fprintf(fp, "%s \n", str);

		fclose(fp);
	}
}

// 021007, Germany
// 021226, Korea
void myFNN::loadDataN1(const char* str, bool normalize)
{
	int i,j;
	int offset = 0;
	float fmin, fmax;

	FILE *fp;
	fp = fopen(str, "rt");

	fscanf(fp, "%d %d %d %d %d %d", &in_no, &out_no, &t_no, &l_no, &e_no, &offset);

	tin = new float[t_no*in_no];
	lin = new float[l_no*in_no];
	ein = new float[e_no*in_no];
	tout = new float[t_no*out_no];
	lout = new float[l_no*out_no];
	eout = new float[e_no*out_no];

	for (i=0; i<t_no*out_no; i++)
		tout[i] = LOW;

	//printf("\nData Loading... : %s \n", str);

	for (i=0; i<t_no; i++)
	{
		for (j=0; j<in_no; j++)
			fscanf(fp, "%f ", &(tin[i*in_no+j]));
		
		fscanf(fp, "%d ", &j);
		tout[i*out_no+j-offset] = HI;
	}

	/*
	//printf("\nData Normalize...\n");
	//if (normalize == TRUE)
	{
		for (i=0; i<in_no; i++)
		{
			fmax = -1e5;
			fmin = 1e5;

			for (j=0; j<t_no; j++)
			{
				if (tin[j*in_no+i] > fmax)
					fmax = tin[j*in_no+i];
				if (tin[j*in_no+i] < fmin)
					fmin = tin[j*in_no+i];
			}

			if ((fmax > 1.0f) || (fmin < 0.0f))
			{
				for (j=0; j<t_no; j++)
				{
					tin[j*in_no+i] = (tin[j*in_no+i] - fmin)/(fmax - fmin);
				}
			}
		}
	}
	*/

	float fracNum = (float)t_no/(float)out_no;
	int l_e_flag = 0;
	int l_cnt = 0;
	int e_cnt = 0;

	for (i=0; i<t_no; i++)
	{
		//if ((float)(i%(int)fracNum) < (float)(fracNum/2.0f))
		if ((float)(i%100) < 50.0f)
			l_e_flag = 0;
		else
			l_e_flag = 1;

		for (j=0; j<in_no; j++)
		{
			if (!l_e_flag)
				lin[j+l_cnt*in_no] = tin[j+i*in_no];
			else
				ein[j+e_cnt*in_no] = tin[j+i*in_no];
		}

		for (j=0; j<out_no; j++)
		{
			if (!l_e_flag)
				lout[j+l_cnt*out_no] = tout[j+i*out_no];
			else
				eout[j+e_cnt*out_no] = tout[j+i*out_no];
		}

		if (!l_e_flag)
			l_cnt++;
		else
			e_cnt++;
	}

	/*
	// learning set
	for (i=0; i<l_no; i++)
		for (j=0; j<in_no; j++)
			lin[j+i*in_no] = tin[j+i*in_no];
	
	for (i=0; i<l_no; i++)
	{
		for (j=0; j<out_no; j++)
		{
			lout[j+i*out_no] = tout[j+i*out_no];
		}
	}

	// eval. set
	for (i=0; i<e_no; i++)
		for (j=0; j<in_no; j++)
			ein[j+i*in_no] = tin[j+(l_no+i)*in_no];
	
	for (i=0; i<e_no; i++)
	{
		for (j=0; j<out_no; j++)
		{
			eout[j+i*out_no] = tout[j+(l_no+i)*out_no];
		}
	}
	
	fclose(fp);
	*/

	FILE *fp2;

	fp2 = fopen(".\\wholeN2l.dat", "wt");

	for (i=0; i<l_no; i++)
	{
		for (j=0; j<in_no; j++)
			fprintf(fp2, "%f ", lin[j+i*in_no]);

		for (j=0; j<out_no; j++)
			if (lout[j+i*out_no] == HI)
			{
				fprintf(fp2, "%d ", j);
				break;
			}
		
		fprintf(fp2, "\n");
	}

	fclose(fp2);

	FILE *fp3;

	fp3 = fopen(".\\wholeN2e.dat", "wt");

	for (i=0; i<e_no; i++)
	{
		for (j=0; j<in_no; j++)
			fprintf(fp3, "%f ", ein[j+i*in_no]);

		for (j=0; j<out_no; j++)
			if (eout[j+i*out_no] == HI)
			{
				fprintf(fp3, "%d ", j);
				break;
			}
		
		fprintf(fp3, "\n");
	}

	fclose(fp3);
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

// 20021229
// 20030205
void myFNN::ioHistogram()
{
	if (!io_histogram_flag)
		return;

	// define variables
	/*
	CTime t = CTime::GetCurrentTime();
	register __int64 freq, start, end; 
	register float Time1;//, Time2;
	register bool err;
	*/

	FILE *fp;
	fp = fopen(".\\DJhistoC3.dat", "awt");

	int j,k,l;
	float sum;
	float *ioHistX = new float[layer[1]*layer[4]];
	float *ioHist = new float[layer[1]*layer[3]];
	int xbin = 5*layer[4]*layer[1];
	int ybin = 400;
	float fSFvalue[2];

	IPLIMAGE imgIO = cvCreateImage(cvSize(xbin, ybin), IPL_DEPTH_8U, 3);
	iplSet(imgIO, 255);

	// main contents
	//CHECK_TIME_START;

	for (j=0; j<layer[1]*layer[3]; j++)		ioHist[j] = 0;
	for (j=0; j<layer[1]*layer[4]; j++)		ioHistX[j] = 0;

	// 1. make 2D-relation matrix
	for (k=0; k<layer[2]; k++)
	{
		for (j=0; j<layer[1]; j++)
		{
			if (j%2 == 0)
				fSFdist(mean[0][j], mean[0][j+1], sigma[0][j], sigma[0][j+1], fSFvalue[0], fSFvalue[1]);

			if (connect[2][k][j] == 1)
			{
				for (l=0; l<layer[3]; l++)
				{
					if (connect[3][l][k] == 1)
						ioHist[l+j*layer[3]] += fSFvalue[j%2];
				}
			}
		}
	}

	// 20030205
	for (j=0; j<layer[1]; j++)
	{
		for (l=0; l<layer[3]; l++)
		{
			if (l%2 == 0)
				ioHistX[l/2+j*layer[4]] -= ioHist[l+j*layer[3]];
			else
				ioHistX[l/2+j*layer[4]] += ioHist[l+j*layer[3]];
		}
	}

	for (int i=0; i<layer[1]; i++)
	{
		for (j=0; j<layer[4]; j++)
			fprintf(fp, "%.3f ", ioHistX[j+i*layer[4]]);
		fprintf(fp, "\n");
	}

	fprintf(fp, "\n");

	float maxHist = -1, minHist = 1;
	int maxPt;

	for (k=0; k<layer[4]; k++)
	{
		maxHist = -1;
		minHist = 1;

		for (j=0; j<layer[1]; j++)
		{
			if (ioHistX[k+j*layer[4]] >= 0)
				maxHist = __max(maxHist, ioHistX[k+j*layer[4]]);
			else
				minHist = __min(minHist, ioHistX[k+j*layer[4]]);
		}

		maxHist = __max(fabs(maxHist), fabs(minHist));

		for (j=0; j<layer[1]; j++)
		{
			if (ioHistX[k+j*layer[4]] >= 0)
			{
				maxPt = (int)((float)((float)ioHistX[k+j*layer[4]]/(float)maxHist)*(float)200);
				cvRectangle(imgIO, cvPoint(j*layer[4]*5 + k*5 + 2, 200 - maxPt), cvPoint(j*layer[4]*5 + k*5 - 2, 200), CV_RGB(255*(layer[4]-k)/layer[4], 0, 255*(k)/layer[4]), -1);
			}
			else
			{
				maxPt = (int)((float)((float)ioHistX[k+j*layer[4]]/(float)maxHist)*(float)200);
				cvRectangle(imgIO, cvPoint(j*layer[4]*5 + k*5 + 2, 200), cvPoint(j*layer[4]*5 + k*5 - 2, 200 - maxPt), CV_RGB(255*(layer[4]-k)/layer[4], 0, 255*(k)/layer[4]), -1);
			}
		}
	}

//	CHECK_TIME_END(Time1, err);
//	message.Format("I/O Histogram Analysis [%.2f ms]", Time1);
//	logview->AddLine(message);

	imgview->AddImage(4,0,imgIO);
	imgview->ShowImage();

	cvReleaseImage(&(imgIO));

	delete [] ioHistX;
	ioHistX = NULL;
	delete [] ioHist;
	ioHist = NULL;

	fclose(fp);
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
	if (!e_no)
		return;

	int i, j, emotion_count = 0;
	float ratio = 0.0f, max;
	int index, maxindex;
	float *yout = new float[layer[4]];

	int *rmat = new int[out_no*out_no];
	for (i=0; i<out_no; i++)
		for (j=0; j<out_no; j++)
			rmat[i*out_no+j] = 0;

	for (i=0; i<e_no; i++)		
	{
		max = -1.0f;
		evalFNN((ein + i*in_no), yout, 0);

		for (j=0; j<out_no; j++)
		{
			if (eout[i*out_no+j] == HI)
				index = j;
		}

		if (emotion_count%out_no == index)
		{
			if (save_rfr)
				SaveRuleFiringRate(".\\myRFR.dat");
			if (save_si)
				SaveSIndex(".\\mySI.dat", yout, index);
			emotion_count++;
		}

		for (j=0; j<out_no; j++)
		{
			if (yout[j] > max)
			{
				max = yout[j];
				maxindex = j;
			}
		}

		rmat[index*out_no+maxindex] += 1;

		if (maxindex == index)
			ratio += 1.0f;
	}

	message.Format("\r\nRecognition Rate for Test Set [%.2f]", ratio/(float)e_no*100.0f);
	logview->AddLine(message);

	message.Format("");
	for (i=0; i<out_no; i++)
	{
		message2.Format("%d ", rmat[i*out_no+i]);
		message += message2;
	}
	logview->AddLine(message);

	delete [] rmat;
	rmat = NULL;
	delete [] yout;
	yout = NULL;
}

void myFNN::recTrain()
{
	if (!l_no)
		return;

	int i, j;//, k;
	float ratio = 0.0f, max;
	int index, maxindex;
	
	int *rmat = new int[out_no*out_no];
	for (i=0; i<out_no; i++)
		for (j=0; j<out_no; j++)
			rmat[i*out_no+j] = 0;

	float *yout = new float[layer[4]];

	int emotion_count = 0;

	for (i=0; i<l_no; i++)		
	{
		max = -1.0f;
		evalFNN((lin + i*in_no), yout, 0);

		for (j=0; j<out_no; j++)
		{
			if (lout[i*out_no+j] == HI)
				index = j;
		}

		if (emotion_count%out_no == index)
		{
			if (save_rfr)
				SaveRuleFiringRate(".\\myRFR.dat");
			if (save_si)
				SaveSIndex(".\\mySI.dat", yout, index);
			emotion_count++;
		}

		maxindex = out_no-1;
		for (j=0; j<out_no; j++)
		{
			if (yout[j] > max)
			{
				max = yout[j];
				maxindex = j;
			}
		}

		rmat[index*out_no+maxindex] += 1;

		if (maxindex == index)
			ratio += 1.0f;

	}

	message.Format("\r\nRecognition Rate for Training Set [%.2f]", ratio/(float)l_no*100.0f);
	logview->AddLine(message);

	message.Format("");
	for (i=0; i<out_no; i++)
	{
		message2.Format("%d ", rmat[i*out_no+i]);
		message += message2;
	}
	logview->AddLine(message);

	delete [] rmat;
	rmat = NULL;
	delete [] yout;
	yout = NULL;

//	fclose(fp);
}



// 2002.12.3
// check this yo!
// 2002.12.5
// rough set!!! (1) converting structure to file (*.sys) format
// 2002.12.16
// rough set -> problem solving by decomposition
void myFNN::ruleCombination()
{
	if (!rule_combination_flag)
		return;

	// define variables
	CTime t = CTime::GetCurrentTime();
	register __int64 freq, start, end; 
	register float Time1;//, Time2;
	register bool err;

	SYSTEM *sys1;
	value_type *rules;
	char name[100];
	int n,work=1,r,rr;
	int *opr;
	setA beg,P,Q,full,core;
	FILE *file2;
	int nn,jj;
	SYSTEM* _mainsys;// = GetMainSys();
	int premise = 0;
	int l2, l3, l4;//, l5;

	CHECK_TIME_START;

	ruleFNN();	// make N (N = layer[3]) rule_for_r.sys (0 <= r <= N) files for usage of rough set functions!

	for (l4=0; l4<layer[3]; l4++)
		for (l3=0; l3<layer[2]; l3++)
				connect[3][l4][l3] = 0;

	for (l3=0; l3<layer[2]; l3++)
		for (l2=0; l2<layer[1]; l2++)
				connect[2][l3][l2] = 0;

	for (rr=0; rr<layer[3]; rr++)
	{
		sys1=InitEmptySys();
		sprintf(name, ".\\rule_for_%d.sys", rr);
		FileToSys(sys1, name);

		sprintf(name, ".\\rule.sys.out", rr);
		if (!(file2=fopen(name,"a")))	return;

		UseSys(sys1);
		full=InitFullSetA();
		InitD(sys1);

		P=InitEmptySetA();
		Q=InitEmptySetA();

		AddSetA(P, 0); AddSetA(P, 1);
		AddSetA(P, 2); AddSetA(P, 3);
		AddSetA(P, 4); AddSetA(Q, 5);

		InitX(sys1,P,Q,MATD);
		core=InitEmptySetA();
		Core(core,MATX);	
		RedOptim( core, MATX );
		CloseSetA(core);  
		n=RedFew(&beg,MATX);
		CloseMat(sys1,MATX);

		//if ( n>0 ) free( beg );

		r = ApprRules( &rules, P, Q, NORMAL, MATD );

		if (r>0)
		{
			opr = StrengthOfRules( rules, r );
			
			_mainsys = GetMainSys();
			
			for (nn=0;nn<r;nn++) 
			{  
				for (jj=0;jj<_mainsys->attributes_num;jj++)
					if ( ContSetA( P,jj ) )
						if ((rules[nn*_mainsys->attributes_num+jj]!=MINUS)
							&& (rules[nn*_mainsys->attributes_num+jj]!=9))
						{
							premise = 1;
							fprintf(file2,"%d ", jj*2 + rules[nn*_mainsys->attributes_num+jj]);
							connect[2][nn][jj*2 + rules[nn*_mainsys->attributes_num+jj]] = 1;
						}

				for (jj=0;jj<_mainsys->attributes_num;jj++)
					if ( ContSetA( Q, jj ) && premise == 1)
						if ((rules+nn*_mainsys->attributes_num)[jj]!=MINUS)
						{
							fprintf(file2,"%d ", (rules+nn*_mainsys->attributes_num)[jj]);
							connect[3][(rules+nn*_mainsys->attributes_num)[jj]][nn] = 1;
						}

				if (premise == 1)
					fprintf(file2,"\n");

				premise = 0;
			}

		//free( rules );
		//free( opr );
		}

	}

 	CloseSetA(P);
	CloseSetA(Q);
	CloseSetA(full);
	CloseSys(sys1);

	deleteLinks();

	CHECK_TIME_END(Time1, err);
	message.Format("Rule Combination [%.2f ms]", Time1);
	logview->AddLine(message);
}

/*
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

	//fclose(fp);

	delete [] cover;
	delete [] over_pre;
	delete [] same_con;

	deleteLinks();
}
*/

// inout: input(0), output(1)
void myFNN::ShowMF(int inout)
{
	int x, y, z;
	int yp;
	int xbin = 200;
	int ybin = 100;
	float xf, zf;//yf, zf;
	char pixel[3] = {0xFF, 0x00, 0xFF};	// BGR

	IPLIMAGE imgIn = cvCreateImage(cvSize(xbin, ybin*layer[0]), IPL_DEPTH_8U, 3);
	IPLIMAGE imgOut = cvCreateImage(cvSize(xbin, ybin*layer[4]), IPL_DEPTH_8U, 3);

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
					xf = (float)x/(float)(xbin-1)*(mean[0][bias + itlayer[y]-1] + sigma[0][bias + itlayer[y]-1]*sqrt(M_PI) 
						- (mean[0][bias] - sigma[0][bias]*sqrt(M_PI)))
						+ (mean[0][bias] - sigma[0][bias]*sqrt(M_PI));

					for (z=0; z<itlayer[y]; z++)
					{
						zf = gaussMF(xf, mean[0][bias+z], sigma[0][bias+z]);
						yp = (int)((float)(y+1)*100.0 - 100.0*zf);

						yp = __max(__min(yp, ybin*layer[0]-1), 0);

						pixel[0] = z*(255/(itlayer[y]-1));
						pixel[2] = (itlayer[y]-1 - z)*(255/(itlayer[y]-1));
						
						iplPutPixel(imgIn, x, yp, pixel);
					}
				}
			}
			imgview->AddImage(2,0,imgIn);
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

						pixel[0] = z*(255/(otlayer[y]-1));
						pixel[2] = (otlayer[y]-1 - z)*(255/(otlayer[y]-1));

						iplPutPixel(imgOut, x, yp, pixel);
					}
				}
			}
			imgview->AddImage(3,0,imgOut);
			break;
	}

	imgview->ShowImage();

	cvReleaseImage(&(imgOut));
	cvReleaseImage(&(imgIn));
}

/*
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
*/

// 021008, Germany
void myFNN::ShowErrorProfile(float* errorprof, int startindex, int endindex)
{
	int xbin = 200;
	int ybin = 100;
	int time;
	float xf, yf;
	float emax, emin;
	CvPoint pt = cvPoint(0, 0);
	CvPoint ppt = cvPoint(0, 0);

	imgError = cvCreateImage(cvSize(xbin, ybin), IPL_DEPTH_8U, 3);
	iplSet(imgError, 0xFFFFFF);

	emax = -1e10;
	emin = 1e10;

	for (time = startindex; time<endindex; time++)
	{
		if (errorprof[time] > emax)
			emax = errorprof[time];
		if (errorprof[time] < emin)
			emin = errorprof[time];
	}

	emin = __min(emin, err_criteria);

	for (time = startindex; time<endindex; time++)
	{
		xf = (float)(time*xbin)/(float)(endindex - startindex + 1);
		yf = (float)(log10((double)errorprof[time]) - log10((double)emin))/(float)(log10((double)emax) - log10((double)emin));
		//yf = (float)(errorprof[time] - emin)/(emax - emin);

		pt.x = (int)xf;
		pt.y = (int)(__min(__max(ybin - (int)(yf*(float)ybin) - 1, 0), ybin-1));

		cvLine(imgError, ppt, pt, 0xFF0000, 1);
		
		ppt.x = pt.x;	ppt.y = pt.y;
	}

	yf = (float)(log10((double)err_criteria)- log10((double)emin))/(float)(log10((double)emax) - log10((double)emin));

	pt.y = (int)(__min(__max(ybin - (int)(yf*(float)ybin) - 1, 0), ybin-1));

	ppt.x = 0;	ppt.y = pt.y;

	cvLine(imgError, ppt, pt, 0x00FF00, 1);
	
	imgview->AddImage(1,0,imgError);
	imgview->ShowImage();

	cvReleaseImage(&(imgError));
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
	// define variables
	int i, j, k;
	int xbin = 300;
	int ybin = 400;
	static int image_num = 0;
	CvPoint **FNNpt;
	FNNpt = new CvPoint *[no_layer];
	int count_connect;
	int branch_count[4];
	int node_count[5];

	IPLIMAGE imgFNNc = cvCreateImage(cvSize(xbin, ybin), IPL_DEPTH_8U, 3);
	iplSet(imgFNNc, 0xFFFFFF);

	for (i=0; i<no_layer; i++)
	{
		node_count[i] = layer[i];

		FNNpt[i] = new CvPoint[layer[i]];

		for (j=0; j<layer[i]; j++)
		{
			FNNpt[i][j].x = (int)((float)xbin/(float)(layer[i]+1)*(float)(j+1));
			FNNpt[i][j].y = ybin - 1 - (i+1)*ybin/(no_layer+1);

			cvCircle(imgFNNc, FNNpt[i][j], 5, 0x0000FF, -1);
		}
	}

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
				node_count[i]--;
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

		branch_count[i-1] = count_connect;
	}

	imgview->AddImage(0, 0, imgFNNc);
	imgview->ShowImage();

	// modification
	message.Format("\r\nNode Structure: ");
	for (i=0; i<no_layer; i++)
	{
		message2.Format("%d ", node_count[i]);
		message += message2;
	}
	logview->AddLine(message);
	//

	message.Format("Link Structure: ");
	for (i=0; i<no_layer-1; i++)
	{
		message2.Format("%d ", branch_count[i]);
		message += message2;
	}
	logview->AddLine(message);

	cvReleaseImage(&(imgFNNc));

	delete [] FNNpt;
	FNNpt = NULL;
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
	if (!kill_nodes)
		return;

	int i, j;

	for (j=0; j<no_nodes; j++)
		for (i=0; i<layer[2]; i++)
			connect[2][i][nodes[j]] = 0;

	deleteLinks();
}

float myFNN::gaussMF(float x, float m, float s)
{
	float mf;

	mf = (float)exp(-(x - m)*(x - m)/(s*s));

	return (mf);
}

void myFNN::FNNinfo()
{
	int i;

	message.Format("Node Structure: ");

	for (i=0; i<no_layer; i++)
	{
		message2.Format("%d ", final_layer[i]);
		message += message2;
	}
	logview->AddLine(message);

	message.Format("Branch Structure: ");

	for (i=0; i<no_layer-1; i++)
	{
		message2.Format("%d ", final_branch[i]);
		message += message2;
	}
	logview->AddLine(message);
}

void myFNN::SaveSIndex(CString file, float* out, int index)
{
	FILE *fp;
	fp = fopen(file, "awt");

	int j;
	float mySIint;
	float mySI[3] = {0, 0, 0};
	float max_val = -1.0f;

	for (j=0; j<layer[4]; j++)
	{
		if (j != index)
		{
			if (out[j] > max_val)
			{
				max_val = out[j];
			}
		}
	}

	// 02.11.11
	// 1) target sufficiency
	mySI[0] = (1.0f)/(1.0f + exp(-BETA_0*(out[index] - HI + ALPHA_0)));
   	// 2) dissimilarity between target and non-targets
	mySI[1] = (1.0f)/(1.0f + exp(-BETA_1*(out[index] - max_val + ALPHA_1)));
	// 3) non-target sufficiency
	mySI[2] = (1.0f)/(1.0f + exp(BETA_0*(max_val - HI + ALPHA_0)));

	mySIint = mySI[0]*mySI[1]*mySI[2];

	fprintf(fp, "%1d %.3f %.3f %.3f %.3f \n", index, mySI[0], mySI[1], mySI[2], mySIint);

	fclose(fp);
}

void myFNN::SaveRuleFiringRate(CString file)
{
	FILE *fp;
	fp = fopen(file, "awt");

	for (int i=0; i<layer[2]; i++)
		fprintf(fp, "%.3f ", out[2][i]);

	fprintf(fp, "\n");

	fclose(fp);
}

void myFNN::SaveIOHistogram(CString file)
{
	FILE *fp;
	fp = fopen(file, "wt");

	int j,k,l;
	int sum, sum2;
	int *ioHist = new int[layer[1]*layer[3]];

	// main contents
	for (int i=0; i<layer[1]*layer[3]; i++)	ioHist[i] = 0;

	for (k=0; k<layer[2]; k++)
	{
		for (j=0; j<layer[1]; j++)
		{
			if (connect[2][k][j] == 1)
			{
				for (l=0; l<layer[3]; l++)
				{
					if (connect[3][l][k] == 1)
						ioHist[l+j*layer[3]]++;
				}
			}
		}
	}

	for (j=0; j<layer[1]; j++)
	{
		sum = sum2 = 0;
		for (l=0; l<layer[3]; l++)
		{
			fprintf(fp, "%d ", ioHist[l+j*layer[3]]);

			sum2 += ioHist[l+j*layer[3]];

			if (l%2 == 0)
				sum -= ioHist[l+j*layer[3]];
			else
				sum += ioHist[l+j*layer[3]];
		}

		fprintf(fp, "\t%d %d \n", sum, sum2);
	}

	delete [] ioHist;
	ioHist = NULL;

	fclose(fp);
}

// input MF X 2D-relation matrix
void myFNN::SaveIOHistogram1(CString file)
{
	FILE *fp;
	fp = fopen(file, "wt");

	int j,k,l;
	float* sum = new float[layer[4]];
	float *ioHist = new float[layer[1]*layer[3]];
	float fSFvalue[2];

	// main contents
	for (int i=0; i<layer[1]*layer[3]; i++)	ioHist[i] = 0;

	for (k=0; k<layer[2]; k++)
	{
		for (j=0; j<layer[1]; j++)
		{
			if (j%2 == 0)
				fSFdist(mean[0][j], mean[0][j+1], sigma[0][j], sigma[0][j+1], fSFvalue[0], fSFvalue[1]);

			if (connect[2][k][j] == 1)
			{
				for (l=0; l<layer[3]; l++)
				{
					if (connect[3][l][k] == 1)
						ioHist[l+j*layer[3]] += (1 - fSFvalue[j%2]);//*ioHist[l+j*layer[3]];
						//ioHist[l+j*layer[3]] += out[1][j]*ioHist[l+j*layer[3]];
						
				}
			}
		}
	}

	for (j=0; j<layer[1]; j++)
	{
		for (l=0; l<layer[4]; l++)	sum[l] = 0;

		for (l=0; l<layer[3]; l++)
		{
			fprintf(fp, "%.3f ", ioHist[l+j*layer[3]]);

			if (l%2 == 0)
				sum[l/2] -= ioHist[l+j*layer[3]];
			else
				sum[l/2] += ioHist[l+j*layer[3]];
		}

		for (l=0; l<layer[4]; l++)
			fprintf(fp, " %.3f ", sum[l]);

		fprintf(fp, "\n");
	}

	delete [] sum;
	sum = NULL;

	delete [] ioHist;
	ioHist = NULL;

	fclose(fp);
}

// output MF X 2D-relation matrix
void myFNN::SaveIOHistogram2(CString file)
{
	FILE *fp;
	fp = fopen(file, "wt");

	int j,k,l;
	float sum, sum2;
	float *ioHist = new float[layer[1]*layer[3]];

	// main contents
	for (int i=0; i<layer[1]*layer[3]; i++)	ioHist[i] = 0;

	for (k=0; k<layer[2]; k++)
	{
		for (j=0; j<layer[1]; j++)
		{
			if (connect[2][k][j] == 1)
			{
				for (l=0; l<layer[3]; l++)
				{
					if (connect[3][l][k] == 1)
						ioHist[l+j*layer[3]] += out[3][l];//*ioHist[l+j*layer[3]];
				}
			}
		}
	}

	for (j=0; j<layer[1]; j++)
	{
		sum = 0.0f;
		sum2 = 0.0f;

		for (l=0; l<layer[3]; l++)
		{
			fprintf(fp, "%.3f ", ioHist[l+j*layer[3]]);

			sum2 += ioHist[l+j*layer[3]];

			if (l%2 == 0)
				sum -= ioHist[l+j*layer[3]];
			else
				sum += ioHist[l+j*layer[3]];
		}

		fprintf(fp, "\t%.3f %.3f \n", sum, sum2);
	}

	delete [] ioHist;
	ioHist = NULL;

	fclose(fp);
}

// input MF X output MF X 2D-relation matrix
void myFNN::SaveIOHistogram3(CString file)
{
	FILE *fp;
	fp = fopen(file, "wt");

	int j,k,l;
	float sum, sum2;
	float *ioHist = new float[layer[1]*layer[3]];
	float fSFvalue[2];

	// main contents
	for (int i=0; i<layer[1]*layer[3]; i++)	ioHist[i] = 0;

	for (k=0; k<layer[2]; k++)
	{
		for (j=0; j<layer[1]; j++)
		{
			if (j%2 == 0)
				fSFdist(mean[0][j], mean[0][j+1], sigma[0][j], sigma[0][j+1], fSFvalue[0], fSFvalue[1]);

			if (connect[2][k][j] == 1)
			{
				for (l=0; l<layer[3]; l++)
				{
					if (connect[3][l][k] == 1)
						ioHist[l+j*layer[3]] += out[3][l]*fSFvalue[j%2];//*ioHist[l+j*layer[3]];
				}
			}
		}
	}

	for (j=0; j<layer[1]; j++)
	{
		sum = 0.0f;
		sum2 = 0.0f;

		for (l=0; l<layer[3]; l++)
		{
			fprintf(fp, "%.3f ", ioHist[l+j*layer[3]]);

			sum2 += ioHist[l+j*layer[3]];

			if (l%2 == 0)
				sum -= ioHist[l+j*layer[3]];
			else
				sum += ioHist[l+j*layer[3]];
		}

		fprintf(fp, "\t%.3f %.3f \n", sum, sum2);
	}

	delete [] ioHist;
	ioHist = NULL;

	fclose(fp);
}

void myFNN::fSFdist(float m1, float m2, float s1, float s2, float &v1, float &v2)
{
	if ((m1 >= m2) || (s1 == 0))
	{
		v1 = v2 = -1;
		return;
	}

	float ux;
	float ratio = (s2*s2)/(s1*s1);

	if (s1 == s2)
	{
		ux = (m2 + m1)/2;
	}
	else
	{
		ux = (2*(m2-ratio*m1) + sqrt(4*(m2 - ratio*m1)*(m2 - ratio*m1) - 4*(1-ratio)*(m2*m2 - ratio*m1*m1)))/(2*(1-ratio));

		if ((ux > m2) || (ux < m1))
			ux = (2*(m2-ratio*m1) - sqrt(4*(m2 - ratio*m1)*(m2 - ratio*m1) - 4*(1-ratio)*(m2*m2 - ratio*m1*m1)))/(2*(1-ratio));
	}

	v1 = (ux-m1)/(m2-m1);
	v2 = (m2-ux)/(m2-m1);
}
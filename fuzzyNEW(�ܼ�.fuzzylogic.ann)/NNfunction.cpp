//#include <stdafx.h>
#include "NNfunction.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
//#include "VisMatrix.h"

#define MIN1U  0
#define MAX1U  1
#define MIN8S  -128
#define MAX8S  127
#define MIN8U  0
#define MAX8U  255
#define MIN16S -32768
#define MAX16S 32767
#define MIN16U 0
#define MAX16U 65535
#define MIN32S (-2147483647-1)
#define MAX32S  2147483647
#define MIN1U_D  0.5
#define MAX1U_D  0.5
#define MIN8S_D  -127.0
#define MAX8S_D  128.0
#define MIN8U_D  0.0
#define MAX8U_D  255.0
#define MIN16S_D -32768.0
#define MAX16S_D 32767.0
#define MIN16U_D 0.0
#define MAX16U_D 65535.0
#define MIN32S_D -2147483648.0
#define MAX32S_D 2147483647.0

#define MAXLAYERS      40
#define NUMBEROFLAYERS 3
#define NUMBER_OF_INPUTS 2
#define NUMBER_OF_OUTPUTS 2
#define EPOCHLEN       40  /* number of vectors in the training set */
#define INIT_WEIGHTS_INTERVAL       0.2  /* the interval of weigts initial values */
#define FIRST_HIDDEN_LAYER_LEN 2
/* these vectors must be linear separable ?*/
#define X_class_C1 0.5
#define Y_class_C1 0.5
#define X_class_C2 0.5
#define Y_class_C2 0.05

#define X_left_Margine  0
#define Y_low_Margine_c1   1
#define X_right_Margine 2
#define Y_top_Margine_c1   2
#define Y_low_Margine_c2   -2
#define Y_top_Margine_c2   -1

#define LERNING_RATE 0.05
#define MOMENTUM     0.5
#define TEST_ARRAY_LEN 30
#define MAX_TRAINING_STEPS 35000
#define ERR_THRESHOLD 1.0e-06

static int initRand = FALSE;

void trplInitRand(int init)
{
    //srand(init);
	srand( (unsigned)time( NULL ) );
    initRand = TRUE;
}

double trplRand(int min, int max)
{
    double r;
    if (initRand == FALSE) trplInitRand(123);

    r = ((double)max-(double)min+0.99)*(double)rand()/(double)RAND_MAX +
        (double)min;
    if(r>max){
        r-=1;
    }
    if(r<min){
        r+=1;
    }

	/*
    r = ((double)rand()/(double)RAND_MAX - 0.5);
    if(r>max){
        r-=1;
    }
    if(r<min){
        r+=1;
    }
	*/

    return r;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNNfunction::CNNfunction()
{
	neuron_count = NULL;
	weights = NULL;

	/*
	int neuron_count[] = {2, 3, 2};
	int no_layer = 3;
	int i, j, k;

	weights = new float **[no_layer - 1];

	for (j=0; j<no_layer-1; j++)
	{
		weights[j] = new float *[neuron_count[j+1]];

		for (i=0; i<neuron_count[j+1]; i++)
		{
			weights[j][i] = new float[neuron_count[j]];

			for (k=0; k<neuron_count[j]; k++)
				weights[j][i][k] = (float)(((float)trplRand(-(int)(INIT_WEIGHTS_INTERVAL * MAX16U),
            (int)(INIT_WEIGHTS_INTERVAL * MAX16U)))/(float)MAX16U);
		}
	}

	psNN = RLsInitMLPServer(weights, 0, 0, neuron_count, NULL, NULL, no_layer, RL_MLP_SIGMOID, 0);

	float **input;
	int no_vector = 4;
	int no_input = neuron_count[0];
	float **output;
	int no_output = neuron_count[no_layer-1];
	float eta = 0.1;
	int step = 3e4;
	float avg_err = 1e-6;
	float mo = 0.99;
	int epoch = 0;

	input = new float *[no_vector];
	for (i=0; i<no_vector; i++)
		input[i] = new float[no_input];

	input[0][0] = 0.81884765625;
	input[0][1] = .343;
	input[1][0] = 0.0263671875;
	input[1][1] = .261;
	input[2][0] = 0.029052734375;
	input[2][1] = .158;
	input[3][0] = 0.069091796875;
	input[3][1] = .151;

	output = new float *[no_vector];
	for (i=0; i<no_vector; i++)
		output[i] = new float[no_output];

	output[0][0] = .1;
	output[0][1] = .1;
	output[1][0] = .3;
	output[1][1] = .1;
	output[2][0] = .7;
	output[2][1] = .1;
	output[3][0] = .9;
	output[3][1] = .1;

	float *result = new float[no_output];

	for (j=0; j<no_vector; j++)
	{
		RLsEvalMLPFeedForward(psNN, input[j], no_input, result, no_output);
		
		for (i=0; i<no_input; i++)
			printf("%f ", input[j][i]);

		printf(" -> ");

		for (i=0; i<no_output; i++)
			printf("%f ", result[i]);

		printf("\n");
	}

	RLsTrainMLPBackPropagation(psNN, input, no_vector, no_input, output, no_output,
		eta, step, avg_err, mo, &epoch);

	printf("epoch = %d\n", epoch);

	for (j=0; j<no_vector; j++)
	{
		RLsEvalMLPFeedForward(psNN, input[j], no_input, result, no_output);
		
		for (i=0; i<no_input; i++)
			printf("%f ", input[j][i]);

		printf(" -> ");

		for (i=0; i<no_output; i++)
			printf("%f ", result[i]);

		printf("\n");
	}

	FILE *fp = RLOpenFile(".\\test.dat", "wb");

	//FILE *fp2 = RLOpenFile(".\\test.dat", "rb");

	RLsWriteMLPServer(fp, psNN);

	//wMLPServer *pwNN2 = RLwReadMLPServer(fp2);
	//printf("", pwNN2->

	//RLCloseFile(fp2);
	RLCloseFile(fp);

	delete [] result;
	delete [] output;
	delete [] input;
	delete [] weights;
	*/
}

CNNfunction::~CNNfunction()
{
	RLsFreeMLPServer(psNN);
}

//
// 	Ouput layer with Linearity
//	sigmoid : tanh
// 
double CNNfunction::sigmoid(double x)
{
	/*
	register double y;

	if (x >= 1)
		y = 1;
	else if (-1 < x && x < 1)
		y = 0.5 + x * (1 - abs(x) / 2);
	else if (x <= -1)
		y = 0;
	
	return (y);
	*/

	return (1.0 /(1.0 + exp(-x)));
}

double CNNfunction::tanh2(double x)
{
	/*
	register double y;

	if (x > 1.92033)
		y = 0.96016;
	else if (0 < x && x <= 1.92033)
		y = 0.96016 - 0.26037 * (x - 1.92033) * (x - 1.92033);
	else if (-1.92033 < x && x < 0)
		y = 0.26037 * (x + 1.92033) * (x + 1.92033) - 0.96016;
	else if (x <= -1.92033)
		y = -0.96016;
	
	return (y);
	*/

	return (2.0 / (1.0 + exp(-2.0 * x)) - 1.0);
	//return (tanh(x));
}

double CNNfunction::calcTri_dJdm(double m, double b, double m_d, double b_d)
{
	return (IntegralTri(m-b, m, m_d, b_d) - IntegralTri(m, m+b, m_d, b_d))/b;
}

//double CNNfunction::xtri(double x, double m, double b, double m_d, double b_d)
double CNNfunction::xtri(double x, double m, double b, double m_d, double b_d)
{
	if (x < m - fabs(b)) return 0.0;
	if (x > m + fabs(b)) return 0.0;
	else return fabs(x-m)/(b*b) * tri(x, m_d, b_d);
}

double CNNfunction::calcTri_dJdb(double m, double b, double m_d, double b_d)
{
/*
	double	v = IntegrateFunc(m-b, m+b, xtri, 50, m, b, m_d, b_d);
	cerr << m << " " << b << " " << m_d << " " << b_d << " " << v << "\n";
	return (1.0/3.0 - v);
*/
	return (1.0/3.0 - IntegrateFunc_xtri(m-b, m+b, 50, m, b, m_d, b_d));
}

double CNNfunction::Jf(double x, double m, double b, double m_d, double b_d)
{
	double v = tri(x, m, b) - tri(x, m_d, b_d);
	return (v * v);
}

double CNNfunction::calcJ(double m, double b, double m_d, double b_d, double alpha1, double alpha2)
{
	//return (alpha1 *IntegrateFunc(0.0, 1.0, Jf, 50, m, b, m_d, b_d) + alpha2* (m - m_d) * (m-m_d));
	return (alpha1 *IntegrateFunc_Jf(0.0, 1.0, 50, m, b, m_d, b_d) + alpha2* (m - m_d) * (m-m_d));
}

double CNNfunction::calcJ1(double m, double b, double m_d, double b_d, double alpha1, double alpha2)
{
	return (IntegrateFunc_Jf(0.0, 1.0, 50, m, b, m_d, b_d));
}
double CNNfunction::calcJ2(double m, double b, double m_d, double b_d, double alpha1, double alpha2)
{
	return ((m - m_d) * (m-m_d));
}

double CNNfunction::fclips (double x) 
{
	return (x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 :x));
}

double CNNfunction::tri(double x, double a, double b, double c, double d)
{
	double  v = (fabs(b) < DBL_MIN)	?  1.0 : (1.0 - fabs((a - x)/b));
	
	return fclips(v);
}

double CNNfunction::IntegralTri(double from, double to, double a, double b)
{
	double sgn = 1.0;
	double v = 0.0;
	if (from > to) {double tmp = from; from=to; to=tmp; sgn = -1.0;}
	if (from < 0.0) from = 0;
	if (to > 1.0) to = 1.0;
//	if (a < 0) a = -a;
	if (b < 0) b = -b;

	double S1 =	0.5 *b;
	if (to < a - b)	 return 0.0;
	if (from > a + b)	 return 0.0;
	if (from < a - b) {
		if (to < a) v = ((to-a+b)*(1.0+(to-a)/b)*0.5);
		else if (to < a + b) v = (S1 + 0.5*(to-a)*(2.0-(to-a)/b));
		else v = b;
	} else if (from < a) {
		if (to < a) v = (0.5*(to-from)*(2.0+(to+from-2.0*a)/b));
		else if (to < a + b) v = (0.5*(a-from)*(2.0+(from-a)/b) 
									+ 0.5*(to-a)*(2.0-(to-a)/b));
		else v = (0.5*(a-from)*(2.0+(from-a)/b) + S1); 
	} else if (from < a + b) {
		if (to < a + b) v = (0.5*(to-from)*(2.0-(to+from-2.0*a)/b));
		else v = (0.5*(a+b-from)*(2.0-(b+from-a)/b));
	} else v = 0.0;

	return _copysign(v, sgn);
}

/*
double CNNfunction::IntegrateFunc(double from, double to, double (*f)(double, double, double, double, double), int N, double p1, double p2, double p3, double p4)
{
	if (from < 0.0) from = 0.0;
	if (to > 1.0) to = 1.0;
	double dx = (to-from)/N;
	double xi = from;
	double s = 0.0;
	for (int i = 0; i < N; i++, xi+= dx) 
		s += (*f)(xi, p1, p2, p3, p4);

	return (s * dx);
}*/

double CNNfunction::IntegrateFunc_xtri(double from, double to, int N, double p1, double p2, double p3, double p4)
{
	if (from < 0.0) from = 0.0;
	if (to > 1.0) to = 1.0;
	double dx = (to-from)/N;
	double xi = from;
	double s = 0.0;
	for (int i = 0; i < N; i++, xi+= dx) 
		//s += (*f)(xi, p1, p2, p3, p4);
		s += xtri(xi, p1, p2, p3, p4);

	return (s * dx);
}

double CNNfunction::IntegrateFunc_Jf(double from, double to, int N, double p1, double p2, double p3, double p4)
{
	if (from < 0.0) from = 0.0;
	if (to > 1.0) to = 1.0;
	double dx = (to-from)/N;
	double xi = from;
	double s = 0.0;
	for (int i = 0; i < N; i++, xi+= dx) 
		//s += (*f)(xi, p1, p2, p3, p4);
		s += Jf(xi, p1, p2, p3, p4);

	return (s * dx);
}

//DEL void CNNfunction::backProp(int layer_no, int *layer_str, int examples, double *in, double *out)
//DEL {
//DEL 	int 	sigmoid_type = 1;		// tanh
//DEL 	int		outputlayer_linear = 1;
//DEL 	int 	error_watch = TRUE;
//DEL 	int watch = TRUE;				// toggle watch training mode
//DEL 	int patient = TRUE;				// signal graceful termination
//DEL 	int debug = TRUE;				// signal graceful termination
//DEL 
//DEL     int		trials	 = 10000;		// number of trials
//DEL     int		verbose  = TRUE;		// verbose reporting off
//DEL     long	seedval	 = 43;			// random number seed value
//DEL     double	eta	 = 0.5;		// learning rate
//DEL 	double psi = 1.0;			// momentum for faster learning
//DEL     double	sigma	 = 0.0;			// noise variance
//DEL 	double 	alpha1 = 1.0;
//DEL 	double 	alpha2 = 1.0;
//DEL 
//DEL 	int layers = layer_no-1;
//DEL 
//DEL 	/*
//DEL     matrix	*b = new matrix[layers];			// threshold  biases
//DEL     matrix	*W = new matrix[layers];			// connection weights
//DEL 	matrix	*Wp = new matrix[layers];			// connection weights for momentum.
//DEL     matrix	*x = new matrix[layers+1];			// inputs/outputs
//DEL     matrix	*d = new matrix[layers];			// "equivalent error"
//DEL 
//DEL 	/*
//DEL     int layer;
//DEL     for (layer = 0; layer < layers; layer++) {
//DEL 		// nodes for each layer
//DEL 		x[layer].resize(layer_str[layer]);
//DEL 		// errors for each layer (except input layer) 
//DEL 		d[layer].resize(layer_str[layer+1]);
//DEL 		// biases for each layer (except input layer)
//DEL 		b[layer].resize(layer_str[layer+1]); b[layer].rndWgt(0.5);
//DEL 		// weights for two layers (output vs. input)
//DEL 		W[layer].resize(layer_str[layer+1], layer_str[layer]); W[layer].rndWgt(0.5);
//DEL 		// pre_update weights for two layers (output vs. input)
//DEL 		Wp[layer].resize(layer_str[layer+1], layer_str[layer]); Wp[layer].rndWgt(0.0);
//DEL     }
//DEL     x[layers].resize(layer_str[layers]);
//DEL 
//DEL 	int inputs = layer_str[0];
//DEL 	int outputs = layer_str[layers];
//DEL 
//DEL     matrix	X(examples,  inputs);		//  inputs
//DEL     matrix	Y(examples, outputs);		// outputs
//DEL     for (int example = 0; example < examples; example++)
//DEL 	{
//DEL 		for(int input = 0; input < inputs; input++)
//DEL 			X[example][input] = in[input+(inputs)*example];
//DEL 		for(int output = 0; output < outputs; output++)
//DEL 			Y[example][output] = out[output+(outputs)*example];
//DEL 	}
//DEL 
//DEL 
//DEL 	double error_sum = 0.0;
//DEL 
//DEL 	doubleMatrix test;
//DEL 
//DEL 	FILE *fout;
//DEL 	if ((fout = fopen("error.dat","w")) == NULL) {
//DEL         fout = stdout;
//DEL     }
//DEL 	FILE *fdebug;
//DEL 	if ((fdebug = fopen("debug.dat","w")) == NULL) {
//DEL         fdebug = stderr;
//DEL     }
//DEL 
//DEL 	fprintf(fdebug, "eta = %10.7f alpha1 = %10.7f alpha2 = %10.7f \n",eta, alpha1, alpha2);
//DEL 
//DEL     for (int trial = 0; trial < trials && patient; trial++) {
//DEL 		example	 = rand()%examples;
//DEL 		x[0]	 = X.s(example);
//DEL 
//DEL 		for (layer = 0; layer < layers-1; layer++)	// Feed Forward
//DEL 		  x[layer+1] = test.tanh2(b[layer] + x[layer]%W[layer]);
//DEL 		x[layers] = b[layers-1] + x[layers-1]%W[layers-1];
//DEL 
//DEL 		double center = x[layers][0][0];
//DEL 		double var = fabs(x[layers][0][1]);
//DEL 		double center_d = Y[example][0];
//DEL 		double var_d = fabs(Y[example][1]);
//DEL 		
//DEL 		d[layers-1][0][0] = eta*(Y[example][0] - x[layers][0][0]);
//DEL 		d[layers-1][0][1] = eta*(Y[example][1] - x[layers][0][1]);
//DEL 		//d[layers-1][0][0] = - eta*(alpha1 * NNf.calcTri_dJdm(center, var, center_d, var_d)+ alpha2 * (x[layers][0][0] - Y[example][0]));  				
//DEL 		//d[layers-1][0][1] = - eta* (alpha1 * NNf.calcTri_dJdb(center, var, center_d, var_d)+ alpha2 * (x[layers][0][1] - Y[example][1]));
//DEL 
//DEL 		// debug
//DEL 		if (debug)
//DEL 		{
//DEL 			double m = x[layers][0][0];
//DEL 			double b = x[layers][0][1];
//DEL 			double m_d = Y[example][0];
//DEL 			double b_d = Y[example][1];
//DEL 			fprintf(fdebug, "%4d %2d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n",
//DEL 					trial, example, x[layers][0][0], x[layers][0][1], 
//DEL 					Y[example][0], Y[example][1],	
//DEL 					calcTri_dJdm(x[layers][0][0], fabs(x[layers][0][1]), 
//DEL 										Y[example][0], fabs(Y[example][1])),
//DEL 					calcTri_dJdb(x[layers][0][0], fabs(x[layers][0][1]), 
//DEL 										Y[example][0], fabs(Y[example][1])),
//DEL 					IntegralTri(m-b, m, m_d, b_d), 
//DEL 					IntegralTri(m, m+b, m_d, b_d)
//DEL 					);
//DEL 		}
//DEL 
//DEL 		for (layer = layers-1; layer > 0; layer--)
//DEL 			d[layer-1] = d[layer]%W[layer].t()*(1.0 - x[layer]*x[layer]);
//DEL 
//DEL 		for (layer = 0; layer < layers; layer++) {	// Update
//DEL 			b[layer] += d[layer];
//DEL 			// org. method in p.84 eq.4.12 from QPark's Thesis
//DEL 			//W[layer] += d[layer]&x[layer];
//DEL 			// mod. method in p.85 eq.4.21 from QPark's Thesis
//DEL 			W[layer] += d[layer]&x[layer];
//DEL 			//W[layer] += Wp[layer];
//DEL 			//Wp[layer] = d[layer]&x[layer];
//DEL 		};
//DEL 
//DEL 		// error_watch
//DEL 		if (error_watch) 
//DEL 		{
//DEL 			double error = calcJ(x[layers][0][0], fabs(x[layers][0][1]), 
//DEL 								Y[example][0], Y[example][1], alpha1, alpha2) ;
//DEL 			double error1 = calcJ1(x[layers][0][0], fabs(x[layers][0][1]), 
//DEL 								Y[example][0], Y[example][1], alpha1, alpha2) ;
//DEL 			double error2 = calcJ2(x[layers][0][0], fabs(x[layers][0][1]), 
//DEL 								Y[example][0], Y[example][1], alpha1, alpha2) ;
//DEL 			double merror = Y[example][0] - x[layers][0][0];	
//DEL 			double berror = Y[example][1] - x[layers][0][1];	
//DEL 			merror = merror * merror;
//DEL 			berror = berror * berror;
//DEL 			error_sum += error;
//DEL 			fprintf(fout, "%4d %10.7f %10.7f %10.7f %10.7f %10.7f %10.7f %10.7f\n",
//DEL 					trial, error_sum/(double)(trial+1), 
//DEL 					error, error1, error2,  error_sum, 
//DEL 					merror, berror);
//DEL 		}
//DEL 		
//DEL 		// watch
//DEL 		if (watch) {
//DEL 		  cerr <<   "trial = "		<< trial
//DEL 			   << "\texample = "	<< example	
//DEL 				<< "\n";
//DEL 		}
//DEL     }
//DEL 
//DEL 	if (fout != stdout) fclose(fout);
//DEL 	if (fdebug != stderr) fclose(fdebug);
//DEL 	*/
//DEL 
//DEL //	delete []b, []W, []x, []d;
//DEL }

/*
CVisDMatrix CNNfunction::mat_sigmoid(CVisDMatrix in_mat)
{
	int col, row;
	int i, j;
	
	col = in_mat.NCols();
	row = in_mat.NRows();

	CVisDMatrix out_mat(row, col);

	for (j=0; j<col; j++)
		for (i=0; i<row; i++)
			out_mat[i][j] = sigmoid(in_mat[i][j]);

	return out_mat;
}

CVisDVector CNNfunction::vec_sigmoid(CVisDVector in_vec)
{
	int len;
	int i;
	
	len = in_vec.Length();

	CVisDVector out_vec(len);

	for (i=0; i<len; i++)
		out_vec[i] = sigmoid(in_vec[i]);

	return out_vec;
}

CVisDMatrix CNNfunction::mat_mat(CVisDMatrix mat1, CVisDMatrix mat2)
{
	int col, row;
	int i, j;
	
	col = mat1.NCols();
	row = mat1.NRows();

	CVisDMatrix out_mat(row, col);

	for (j=0; j<col; j++)
		for (i=0; i<row; i++)
			out_mat[i][j] = (mat1[i][j])*(mat2[i][j]);

	return out_mat;
}

CVisDVector CNNfunction::vec_vec(CVisDVector vec1, CVisDVector vec2)
{
	int len;
	int i;
	
	len = vec1.Length();

	CVisDVector out_vec(len);

	for (i=0; i<len; i++)
		out_vec[i] = (vec1[i])*(vec2[i]);

	return out_vec;
}

CVisDMatrix CNNfunction::mat_tanh(CVisDMatrix in_mat)
{
	int col, row;
	int i, j;
	
	col = in_mat.NCols();
	row = in_mat.NRows();

	CVisDMatrix out_mat(row, col);

	for (j=0; j<col; j++)
		for (i=0; i<row; i++)
			out_mat[i][j] = tanh2(in_mat[i][j]);

	return out_mat;
}

CVisDVector CNNfunction::vec_tanh(CVisDVector in_vec)
{
	int len;
	int i;
	
	len = in_vec.Length();

	CVisDVector out_vec(len);

	for (i=0; i<len; i++)
		out_vec[i] = tanh2(in_vec[i]);

	return out_vec;
}

CVisDMatrix CNNfunction::rand_mat(CVisDMatrix mat, float rnd)
{
	int col, row;
	int i, j;

	float rnd_wgt;
	
	col = mat.NCols();
	row = mat.NRows();

	CVisDMatrix rst_mat(row, col);

	for (j=0; j<col; j++)
		for (i=0; i<row; i++)
		{
			rnd_wgt = (float)((float)rand()/(float)RAND_MAX);
			rst_mat[i][j] = rnd * (rnd_wgt - 0.5);
		}

	return rst_mat;
}

CVisDVector CNNfunction::rand_vec(CVisDVector vec, float rnd)
{
	int len;
	int i;

	float rnd_wgt;
	
	len = vec.Length();

	CVisDVector rst_vec(len);

	for (i=0; i<len; i++)
	{
			rnd_wgt = (float)((float)rand()/(float)RAND_MAX);
			rst_vec[i] = rnd * (rnd_wgt - 0.5);
	}

	return rst_vec;
}

  /*
// 20010714@9:35a@DJ
// unified learning
void CNNfunction::NN_learning(int layer_no, int *layer_str, int examples, double *in, double *out)
{
	double 	alpha1 = 1.0;
	double 	alpha2 = 1.0;
	int layers = layer_no-1;

	if (layer_str[layers] > 2 && m_Learn == 1)
	{
		printf("Too many output...\n");
		return;
	}

	// construct
	CVisDMatrix	*b = new CVisDMatrix[layers];			// threshold  biases
    CVisDMatrix	*x = new CVisDMatrix[layers+1];			// inputs/outputs
	CVisDMatrix	*eye = new CVisDMatrix[layers+1];		// connection weights for momentum.
    CVisDMatrix	*d = new CVisDMatrix[layers];			// "equivalent error"
    CVisDMatrix	*W = new CVisDMatrix[layers];			// connection weights
    CVisDMatrix	*Wp = new CVisDMatrix[layers];			// for momentum weights
	
    int layer;
    for (layer = 0; layer < layers; layer++) 
	{
		x[layer].Resize(layer_str[layer], 1);
		eye[layer].Resize(layer_str[layer], 1);
		eye[layer] = 1.0;

		d[layer].Resize(layer_str[layer+1], 1);

		b[layer].Resize(layer_str[layer+1], 1);
		rand_mat(b[layer], m_fRnd);

		W[layer].Resize(layer_str[layer+1], layer_str[layer]);
		rand_mat(W[layer], m_fRnd);

		if (m_fMomentum != 0.0)
		{
			Wp[layer].Resize(layer_str[layer+1], layer_str[layer]);
		}
    }
    
	x[layers].Resize(layer_str[layers], 1);

	int inputs = layer_str[0];
	int outputs = layer_str[layers];

    CVisDMatrix	X(examples,  inputs);
    CVisDMatrix	Y(examples, outputs);
	
	int example, input, output;
    for ( example = 0; example < examples; example++)
	{
		for( input = 0; input < inputs; input++)
			X[example][input] = in[input+(inputs)*example];
		for( output = 0; output < outputs; output++)
			Y[example][output] = out[output+(outputs)*example];
	}

	int i;

	double error_sum;

	FILE *fp2;
	fp2 = fopen("c:\\matlabr11\\work\\feers\\gabor\\error.dat", "wt");

    for (int trial = 0; trial < m_nTrials; trial++) 
	{
		example	 = rand()%examples;
		
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[example][i];

		for (layer = 0; layer < layers-1; layer++)
		{
			switch(m_Activation)
			{
				// Sigmoid
				case 0:
					x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
					break;
				// Tanh
				case 1:
					x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
					break;
				default:
					break;
			}
		}
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		error_sum = 0.0;

		double center = x[layers][0][0];
		double var = fabs(x[layers][1][0]);
		double center_d = Y[example][0];
		double var_d = fabs(Y[example][1]);

		double error;

		for (i=0; i<outputs; i++)
		{
			switch(m_Learn)
			{
				// EBP
				case 0:
					error = (Y[example][i] - x[layers][i][0]);
					d[layers-1][i][0] = error;//*(x[layers][i][0]*(1.0 - x[layers][i][0]));
					break;
				// Fuzzy Observer (for 2 output cases!)
				case 1:
					error = (alpha1 * calcTri_dJdm(center, var, center_d, var_d)+ alpha2 * (x[layers][i][0] - Y[example][i]));
					d[layers-1][i][0] = -error;
					//d[layers-1][1][0] = - m_fLearningRate*(alpha1 * calcTri_dJdb(center, var, center_d, var_d)+ alpha2 * (x[layers][1][0] - Y[example][1]));
					break;
				default:
					break;
			}
			
			error_sum += error*error;
		}

		error_sum /= 2.0;
		fprintf(fp2, "%f \n", error_sum);

		if (trial%50 == 0)
 			printf("#%05d, ERR: %f \n", trial, error_sum);


		// learning process
		for (layer = layers-1; layer > 0; layer--)
		{
			d[layer-1] = (W[layer].Transposed()*d[layer]);

			if (layer <= layers-1)
			{
				switch(m_Activation)
				{
					// Sigmoid
					case 0:
						d[layer-1] = mat_mat(d[layer-1], mat_mat(x[layer], eye[layer] - x[layer]));
						//d[layer-1] += d[layer-1]*(0.1);
						break;
					// Tanh
					case 1:
						d[layer-1] = mat_mat(d[layer-1], eye[layer] - mat_mat(x[layer], x[layer]));
						break;
					default:
						break;
				}
			}
		}

		for (layer = 0; layer < layers; layer++) 
		{
			b[layer] += d[layer];

			switch(m_Learn)
			{
				// EBP w/ Momentum
				case 0:
					if (trial > 0 && m_fMomentum != 0.0)
						W[layer] += (W[layer] - Wp[layer]) * m_fMomentum + (d[layer]*x[layer].Transposed()) * m_fLearningRate;
					else
						W[layer] += (d[layer]*x[layer].Transposed()) * m_fLearningRate;

					Wp[layer] = W[layer];
					break;
				// Fuzzy Observer
				case 1:				
					W[layer] += (d[layer]*x[layer].Transposed()) * m_fLearningRate;				
					break;
				default:
					break;
			}
		};
    }

	fclose(fp2);

	// evaluation
	for (int j=0; j<examples; j++)
	{
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[j][i];

		for (layer = 0; layer < layers-1; layer++)
		{
			switch(m_Activation)
			{
				// Sigmoid
				case 0:
					x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
					break;
				// Tanh
				case 1:
					x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
					break;
				default:
					break;
			}
		}
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

 		printf("Example #%03d ", j);
 
 		for (int k=0; k<outputs; k++)
 			printf("out%02d: %f ", k, x[layers][k][0]);
 
 		printf("\n");
	}

	// destruct
	delete []Wp;
	delete []W;
	delete []d;
	delete []eye;
	delete []x;
	delete []b;
}
*/

void CNNfunction::NNalloc(int *layer, int layers, float v_eta, int v_step, float v_avg_err, float v_mo)
{
	int i, j, k;

	neuron_count = layer;
	no_layer = layers;
	eta = v_eta;
	step = v_step;
	avg_err = v_avg_err;
	mo = v_mo;
	no_input = neuron_count[0];
	no_output = neuron_count[no_layer - 1];

	weights = new float **[no_layer - 1];

	for (j=0; j<no_layer-1; j++)
	{
		weights[j] = new float *[neuron_count[j+1]];

		for (i=0; i<neuron_count[j+1]; i++)
		{
			weights[j][i] = new float[neuron_count[j]];

			for (k=0; k<neuron_count[j]; k++)
				weights[j][i][k] = (float)(((float)trplRand(-(int)(INIT_WEIGHTS_INTERVAL * MAX16U),
            (int)(INIT_WEIGHTS_INTERVAL * MAX16U)))/(float)MAX16U);
		}
	}

	psNN = RLsInitMLPServer(weights, 0, 0, neuron_count, NULL, NULL, no_layer, RL_MLP_SIGMOID, 0);
}

void CNNfunction::NNfree()
{
	if (weights)
		delete [] weights;	
}

void CNNfunction::NNeval(float *inputs, float* outputs, int no_sample)
{
	FILE *fp;

	fp = fopen("NNresult.dat", "awt");

	int i, j;
	float *result = new float[no_output];
	float maxval = -1.0f;
	int maxid, maxid2;

	int cmat[7][7];
	for (i=0; i<7; i++)
		for (j=0; j<7; j++)
			cmat[i][j] = 0;

	float rr = 0.0f;

	for (j=0; j<no_sample; j++)
	{
		RLsEvalMLPFeedForward(psNN, (inputs +j*no_input), no_input, result, no_output);

		maxval = -1.0f;
		for (i=0; i<no_output; i++)
		{
			if (result[i] > maxval)
			{
				maxval = result[i];
				maxid = i;
			}
		}

		for (i=0; i<no_output; i++)
		{
			if (outputs[j*no_output + i] == 0.8f)
			{
				maxid2 = i;
				break;
			}
		}

		cmat[maxid2][maxid]++;

		/*
		// print results
		for (i=0; i<no_input; i++)
			printf("%.3f ", *(inputs + i + j*no_input));

		printf(" -> ");

		for (i=0; i<no_output; i++)
			printf("%.3f ", result[i]);

		printf("\n");
		//
		*/
	}

	int sum = 0;
	for (i=0; i<7; i++)
	{
		sum += cmat[i][i];
		fprintf(fp, "%d ", cmat[i][i]);
	}

	rr = (float)sum/(float)no_sample*100.0f;

	fprintf(fp, ": %.2f \n", rr);

	delete [] result;

	fclose(fp);
}

void CNNfunction::NNtrain(float *inputs, float *outputs, int no_sample)
{
	int i, j;
	int epoch;
	float **input, **output;

	input = new float *[no_sample];
	for (i=0; i<no_sample; i++)
	{
		input[i] = new float[no_input];
		
		for (j=0; j<no_input; j++)
			input[i][j] = *(inputs + j + i*no_input);
	}

	output = new float *[no_sample];
	for (i=0; i<no_sample; i++)
	{
		output[i] = new float[no_output];
		
		for (j=0; j<no_output; j++)
			output[i][j] = *(outputs + j + i*no_output);
	}

	RLsTrainMLPBackPropagation(psNN, input, no_sample, no_input, output, no_output,
		eta, step, avg_err, mo, &epoch);

	if (RLGetStatus() != RL_StsOk)
		exit(0);

	// print epoch
	printf("last epoch = %d\n", epoch);
	//
}

void CNNfunction::NNsave(char *str)
{
	FILE *fp = RLOpenFile(str, "wb");

	RLsWriteMLPServer(fp, psNN);
	
	RLCloseFile(fp);
}

void CNNfunction::NNload(char *str)
{
	FILE *fp = RLOpenFile(str, "rb");

	psNN = RLsReadMLPServer(fp);
	
	RLCloseFile(fp);
}
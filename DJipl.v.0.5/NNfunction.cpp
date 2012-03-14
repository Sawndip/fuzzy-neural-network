#include <stdafx.h>
#include "NNfunction.h"
#include <stdio.h>
#include <float.h>
#include <math.h>
//#include "VisMatrix.h"

/*
#include "rl.h"
#include "rl_perc.h"
#include "rl_gauss.h"
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNNfunction::CNNfunction()
{
	//pwNN = RLwInitMLPServer(NULL, NULL, NULL, NULL, NULL, NULL, 0, RL_MLP_FICTIVE, 0, NULL);
	//RLwFreeMLPServer(pwNN);
	//RLFreeGaussTbls();
}

CNNfunction::~CNNfunction()
{

}

//
// 	Ouput layer with Linearity
//	sigmoid : tanh
// 
double CNNfunction::sigmoid(double x)
{
	register double y;

	if (x >= 1)
		y = 1;
	else if (-1 < x && x < 1)
		y = 0.5 + x * (1 - abs(x) / 2);
	else if (x <= -1)
		y = 0;

	//return (1.0 /(1 + exp(-x)));
	return (y);
}

double CNNfunction::tanh2(double x)
{
	register double y;

	if (x > 1.92033)
		y = 0.96016;
	else if (0 < x && x <= 1.92033)
		y = 0.96016 - 0.26037 * (x - 1.92033) * (x - 1.92033);
	else if (-1.92033 < x && x < 0)
		y = 0.26037 * (x + 1.92033) * (x + 1.92033) - 0.96016;
	else if (x <= -1.92033)
		y = -0.96016;

	//return (2 / (1 + exp(-2 * x)) - 1);
	return (y);
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

//DEL void CNNfunction::fuzzyObserver(int layer_no, int *layer_str, int examples, double *in, double *out)
//DEL {
//DEL 	double 	alpha1 = 1.0;
//DEL 	double 	alpha2 = 1.0;
//DEL 
//DEL 	int layers = layer_no-1;
//DEL 
//DEL 	// construct
//DEL 	CVisDMatrix	*b = new CVisDMatrix[layers];			// threshold  biases
//DEL     CVisDMatrix	*x = new CVisDMatrix[layers+1];			// inputs/outputs
//DEL 	CVisDMatrix	*eye = new CVisDMatrix[layers+1];			// connection weights for momentum.
//DEL     CVisDMatrix	*d = new CVisDMatrix[layers];			// "equivalent error"
//DEL     CVisDMatrix	*W = new CVisDMatrix[layers];			// connection weights
//DEL 	
//DEL     int layer;
//DEL     for (layer = 0; layer < layers; layer++) 
//DEL 	{
//DEL 		// nodes for each layer
//DEL 		x[layer].Resize(layer_str[layer], 1);
//DEL 		eye[layer].Resize(layer_str[layer], 1);
//DEL 		eye[layer] = 1.0;
//DEL 		// errors for each layer (except input layer) 
//DEL 		d[layer].Resize(layer_str[layer+1], 1);
//DEL 		// biases for each layer (except input layer)
//DEL 		b[layer].Resize(layer_str[layer+1], 1); //b[layer].rndWgt(0.5);
//DEL 		b[layer] = 0.5;
//DEL 		// weights for two layers (output vs. input)
//DEL 		W[layer].Resize(layer_str[layer+1], layer_str[layer]); //W[layer].rndWgt(0.5);
//DEL 		W[layer] = 0.5;
//DEL     }
//DEL     
//DEL 	x[layers].Resize(layer_str[layers], 1);
//DEL 
//DEL 	int inputs = layer_str[0];
//DEL 	int outputs = layer_str[layers];
//DEL 
//DEL     CVisDMatrix	X(examples,  inputs);		//  inputs
//DEL     CVisDMatrix	Y(examples, outputs);		// outputs
//DEL 	
//DEL 	int example, input, output;
//DEL     for ( example = 0; example < examples; example++)
//DEL 	{
//DEL 		for( input = 0; input < inputs; input++)
//DEL 			X[example][input] = in[input+(inputs)*example];
//DEL 		for( output = 0; output < outputs; output++)
//DEL 			Y[example][output] = out[output+(outputs)*example];
//DEL 	}
//DEL 
//DEL 	double error_sum = 0.0;
//DEL 
//DEL 	int i;
//DEL 
//DEL     for (int trial = 0; trial < m_nTrials; trial++) {
//DEL 		example	 = rand()%examples;
//DEL 		
//DEL 		for (i=0; i<inputs; i++)
//DEL 			x[0][i][0] = X[example][i];
//DEL 
//DEL 		for (layer = 0; layer < layers-1; layer++)
//DEL 			//x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
//DEL 			x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
//DEL 		
//DEL 		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];
//DEL 
//DEL 		/*
//DEL 		double center = x[layers][0][0];
//DEL 		double var = fabs(x[layers][1][0]);
//DEL 		double center_d = Y[example][0];
//DEL 		double var_d = fabs(Y[example][1]);
//DEL 		d[layers-1][0][0] = - eta*(alpha1 * calcTri_dJdm(center, var, center_d, var_d)+ alpha2 * (x[layers][0][0] - Y[example][0]));
//DEL 		d[layers-1][1][0] = - eta* (alpha1 * calcTri_dJdb(center, var, center_d, var_d)+ alpha2 * (x[layers][0][1] - Y[example][1]));
//DEL 		*/
//DEL 
//DEL 	for (i=0; i<outputs; i++)
//DEL 		// backprop.
//DEL 		d[layers-1][i][0] = m_fLearningRate*(Y[example][i] - x[layers][i][0]);
//DEL 
//DEL //		d[layers-1][0][0] = - eta*(alpha1 * calcTri_dJdm(center, var, center_d, var_d)+ alpha2 * (x[layers][0][0] - Y[example][0]));
//DEL //		d[layers-1][1][0] = - eta* (alpha1 * calcTri_dJdb(center, var, center_d, var_d)+ alpha2 * (x[layers][0][1] - Y[example][1]));
//DEL 
//DEL 		//printf("trial: %d, d1: %f, d2: %f, x1: %f, x2: %f \n", trial, d[layers-1][0][0], d[layers-1][1][0], x[layers][0][0], x[layers][1][0]);
//DEL 	printf("trial: %d, d1: %f, x1: %f, \n", trial, d[layers-1][0][0], x[layers][0][0]);
//DEL 
//DEL 		for (layer = layers-1; layer > 0; layer--)
//DEL 		{
//DEL 			//CVisDMatrix temp(1, 1);
//DEL 			//temp = (x[layer].Transposed() * x[layer]);
//DEL 			//d[layer-1] = (W[layer].Transposed()*d[layer])*(1.0 - (double)(temp[0][0]));
//DEL 			d[layer-1] = (W[layer].Transposed()*d[layer]);
//DEL 
//DEL 			if (layer <= layers-1)
//DEL 			{
//DEL 				//d[layer-1] = mat_mat(d[layer-1], x[layer]);
//DEL 				d[layer-1] = mat_mat(d[layer-1], eye[layer] - mat_mat(x[layer], x[layer]));
//DEL 			}
//DEL 		}
//DEL 
//DEL 		for (layer = 0; layer < layers; layer++) 
//DEL 		{
//DEL 			b[layer] += d[layer];
//DEL 			W[layer] += (d[layer]*x[layer].Transposed());
//DEL 		};
//DEL     }
//DEL 
//DEL 	// test it!
//DEL 	for (int j=0; j<examples; j++)
//DEL 	{
//DEL 		for (i=0; i<inputs; i++)
//DEL 			x[0][i][0] = X[j][i];
//DEL 
//DEL 		for (layer = 0; layer < layers-1; layer++)
//DEL 			x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
//DEL 		
//DEL 		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];
//DEL 
//DEL 		//printf("ex %d, y0: %f, y1: %f \n", j, x[layers][0][0], x[layers][1][0]); 
//DEL 		printf("ex %d, y0: %f \n", j, x[layers][0][0]); 
//DEL 	}
//DEL 
//DEL 
//DEL 	// destruct
//DEL 	delete []W;
//DEL 	delete []d;
//DEL 	delete []x;
//DEL 	delete []b;
//DEL }

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

void CNNfunction::ebp_tanh(int layer_no, int *layer_str, int examples, double *in, double *out)
{
	int layers = layer_no-1;

	// construct
	CVisDMatrix	*b = new CVisDMatrix[layers];			// threshold  biases
    CVisDMatrix	*x = new CVisDMatrix[layers+1];			// inputs/outputs
	CVisDMatrix	*eye = new CVisDMatrix[layers+1];		// connection weights for momentum.
    CVisDMatrix	*d = new CVisDMatrix[layers];			// "equivalent error"
    CVisDMatrix	*W = new CVisDMatrix[layers];			// connection weights
	
    int layer;
    for (layer = 0; layer < layers; layer++) 
	{
		// nodes for each layer
		x[layer].Resize(layer_str[layer], 1);
		eye[layer].Resize(layer_str[layer], 1);
		eye[layer] = 1.0;
		// errors for each layer (except input layer) 
		d[layer].Resize(layer_str[layer+1], 1);
		// biases for each layer (except input layer)
		b[layer].Resize(layer_str[layer+1], 1); //b[layer].rndWgt(0.5);
		rand_mat(b[layer]);
		// weights for two layers (output vs. input)
		W[layer].Resize(layer_str[layer+1], layer_str[layer]); //W[layer].rndWgt(0.5);
		rand_mat(W[layer]);
    }
    
	x[layers].Resize(layer_str[layers], 1);

	int inputs = layer_str[0];
	int outputs = layer_str[layers];

    CVisDMatrix	X(examples,  inputs);		//  inputs
    CVisDMatrix	Y(examples, outputs);		// outputs
	
	int example, input, output;
    for ( example = 0; example < examples; example++)
	{
		for( input = 0; input < inputs; input++)
			X[example][input] = in[input+(inputs)*example];
		for( output = 0; output < outputs; output++)
			Y[example][output] = out[output+(outputs)*example];
	}

	int i;

    for (int trial = 0; trial < m_nTrials; trial++) {
		example	 = rand()%examples;
		
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[example][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];


		for (i=0; i<outputs; i++)
			d[layers-1][i][0] = m_fLearningRate*(Y[example][i] - x[layers][i][0]);

		// verbose
		//printf("trial: %d, d1: %f, x1: %f, \n", trial, d[layers-1][0][0], x[layers][0][0]);

		// EBP process
		for (layer = layers-1; layer > 0; layer--)
		{
			d[layer-1] = (W[layer].Transposed()*d[layer]);

			if (layer < layers-1)
			{
				d[layer-1] = mat_mat(d[layer-1], eye[layer] - mat_mat(x[layer], x[layer]));
			}
		}

		for (layer = 0; layer < layers; layer++) 
		{
			b[layer] += d[layer];
			W[layer] += (d[layer]*x[layer].Transposed());
		};
    }

	// evaluation
	for (int j=0; j<examples; j++)
	{
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[j][i];

		for (layer=0; layer < layers-1; layer++)
			x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		// if verbose
		// if verbose
		printf("Example #%03d ", j);

		for (int k=0; k<outputs; k++)
			printf("out%02d: %f ", k, x[layers][k][0]);

		printf("\n");
	}

	// destruct
	delete []W;
	delete []d;
	delete []eye;
	delete []x;
	delete []b;
}

void CNNfunction::ebp_sig(int layer_no, int *layer_str, int examples, double *in, double *out)
{
	int layers = layer_no-1;

	// construct
	CVisDMatrix	*b = new CVisDMatrix[layers];			// threshold  biases
    CVisDMatrix	*x = new CVisDMatrix[layers+1];			// inputs/outputs
	CVisDMatrix	*eye = new CVisDMatrix[layers+1];		// connection weights for momentum.
    CVisDMatrix	*d = new CVisDMatrix[layers];			// "equivalent error"
    CVisDMatrix	*W = new CVisDMatrix[layers];			// connection weights
	
    int layer;
    for (layer = 0; layer < layers; layer++) 
	{
		// nodes for each layer
		x[layer].Resize(layer_str[layer], 1);
		eye[layer].Resize(layer_str[layer], 1);
		eye[layer] = 1.0;
		// errors for each layer (except input layer) 
		d[layer].Resize(layer_str[layer+1], 1);
		// biases for each layer (except input layer)
		b[layer].Resize(layer_str[layer+1], 1); //b[layer].rndWgt(0.5);
		rand_mat(b[layer]);
		// weights for two layers (output vs. input)
		W[layer].Resize(layer_str[layer+1], layer_str[layer]); //W[layer].rndWgt(0.5);
		rand_mat(W[layer]);
    }
    
	x[layers].Resize(layer_str[layers], 1);

	int inputs = layer_str[0];
	int outputs = layer_str[layers];

    CVisDMatrix	X(examples,  inputs);		//  inputs
    CVisDMatrix	Y(examples, outputs);		// outputs
	
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

//	FILE *fp2;

//	fp2 = fopen(".\\error_profile.dat", "wt");

    for (int trial = 0; trial < m_nTrials; trial++) 
	{
		example	 = rand()%examples;
		
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[example][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

//		error_sum = 0.0;
		for (i=0; i<outputs; i++)
		{
			d[layers-1][i][0] = m_fLearningRate*(Y[example][i] - x[layers][i][0]);
//			error_sum += (Y[example][i] - x[layers][i][0])*(Y[example][i] - x[layers][i][0]);
		}

//		fprintf(fp2, "%f \n", error_sum/2.0);

		// verbose
		//printf("trial: %d, d1: %f, x1: %f, \n", trial, d[layers-1][0][0], x[layers][0][0]);

		// EBP process
		for (layer = layers-1; layer > 0; layer--)
		{
			d[layer-1] = (W[layer].Transposed()*d[layer]);

			if (layer < layers-1)
			{
				d[layer-1] = mat_mat(d[layer-1], x[layer]);
				d[layer-1] = mat_mat(d[layer-1], eye[layer] - x[layer]);
				//d[layer-1] += d[layer-1]*(0.1);
			}
		}

		for (layer = 0; layer < layers; layer++) 
		{
			b[layer] += d[layer];
			W[layer] += (d[layer]*x[layer].Transposed());
		};
    }

//	fclose(fp2);

	// evaluation
	for (int j=0; j<examples; j++)
	{
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[j][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		// if verbose
		printf("Example #%03d ", j);

		for (int k=0; k<outputs; k++)
			printf("out%02d: %f ", k, x[layers][k][0]);

		printf("\n");
	}

	// destruct
	delete []W;
	delete []d;
	delete []eye;
	delete []x;
	delete []b;
}

void CNNfunction::fuzzy_observer_tanh(int layer_no, int *layer_str, int examples, double *in, double *out)
{
	// weights for each objectives
	double 	alpha1 = 1.0;
	double 	alpha2 = 1.0;

	int layers = layer_no-1;

	// construct
	CVisDMatrix	*b = new CVisDMatrix[layers];			// threshold  biases
    CVisDMatrix	*x = new CVisDMatrix[layers+1];			// inputs/outputs
	CVisDMatrix	*eye = new CVisDMatrix[layers+1];			// connection weights for momentum.
    CVisDMatrix	*d = new CVisDMatrix[layers];			// "equivalent error"
    CVisDMatrix	*W = new CVisDMatrix[layers];			// connection weights
	
    int layer;
    for (layer = 0; layer < layers; layer++) 
	{
		// nodes for each layer
		x[layer].Resize(layer_str[layer], 1);
		eye[layer].Resize(layer_str[layer], 1);
		eye[layer] = 1.0;
		// errors for each layer (except input layer) 
		d[layer].Resize(layer_str[layer+1], 1);
		// biases for each layer (except input layer)
		b[layer].Resize(layer_str[layer+1], 1); //b[layer].rndWgt(0.5);
		rand_mat(b[layer]);
		// weights for two layers (output vs. input)
		W[layer].Resize(layer_str[layer+1], layer_str[layer]); //W[layer].rndWgt(0.5);
		rand_mat(W[layer]);
    }
    
	x[layers].Resize(layer_str[layers], 1);

	int inputs = layer_str[0];
	int outputs = layer_str[layers];

    CVisDMatrix	X(examples,  inputs);		//  inputs
    CVisDMatrix	Y(examples, outputs);		// outputs
	
	int example, input, output;
    for ( example = 0; example < examples; example++)
	{
		for( input = 0; input < inputs; input++)
			X[example][input] = in[input+(inputs)*example];
		for( output = 0; output < outputs; output++)
			Y[example][output] = out[output+(outputs)*example];
	}

	int i;

    for (int trial = 0; trial < m_nTrials; trial++) 
	{
		
		example	 = rand()%examples;
		
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[example][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		double center = x[layers][0][0];
		double var = fabs(x[layers][1][0]);
		double center_d = Y[example][0];
		double var_d = fabs(Y[example][1]);

		for (i=0; i<outputs; i++)
		{
			d[layers-1][0][0] = - m_fLearningRate*(alpha1 * calcTri_dJdm(center, var, center_d, var_d)+ alpha2 * (x[layers][0][0] - Y[example][0]));
			d[layers-1][1][0] = - m_fLearningRate*(alpha1 * calcTri_dJdb(center, var, center_d, var_d)+ alpha2 * (x[layers][1][0] - Y[example][1]));
		}

		//printf("trial: %d, d1: %f, d2: %f, x1: %f, x2: %f \n", trial, d[layers-1][0][0], d[layers-1][1][0], x[layers][0][0], x[layers][1][0]);

		for (layer = layers-1; layer > 0; layer--)
		{
			d[layer-1] = (W[layer].Transposed()*d[layer]);

			if (layer < layers-1)
			{
				d[layer-1] = mat_mat(d[layer-1], eye[layer] - mat_mat(x[layer], x[layer]));
			}
		}

		for (layer = 0; layer < layers; layer++) 
		{
			b[layer] += d[layer];
			W[layer] += (d[layer]*x[layer].Transposed());
		};
    }

	// evaluation
	for (int j=0; j<examples; j++)
	{
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[j][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_tanh(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		// if verbose
		printf("Example #%03d ", j);

		for (int k=0; k<outputs; k++)
			printf("out%02d: %f ", k, x[layers][k][0]);

		printf("\n");
	}

	// destruct
	delete []W;
	delete []d;
	delete []eye;
	delete []x;
	delete []b;
}

void CNNfunction::fuzzy_observer_sig(int layer_no, int *layer_str, int examples, double *in, double *out)
{
	// weights for each objectives
	double 	alpha1 = 1.0;
	double 	alpha2 = 1.0;

	int layers = layer_no-1;

	// construct
	CVisDMatrix	*b = new CVisDMatrix[layers];			// threshold  biases
    CVisDMatrix	*x = new CVisDMatrix[layers+1];			// inputs/outputs
	CVisDMatrix	*eye = new CVisDMatrix[layers+1];			// connection weights for momentum.
    CVisDMatrix	*d = new CVisDMatrix[layers];			// "equivalent error"
    CVisDMatrix	*W = new CVisDMatrix[layers];			// connection weights
	
    int layer;
    for (layer = 0; layer < layers; layer++) 
	{
		// nodes for each layer
		x[layer].Resize(layer_str[layer], 1);
		eye[layer].Resize(layer_str[layer], 1);
		eye[layer] = 1.0;
		// errors for each layer (except input layer) 
		d[layer].Resize(layer_str[layer+1], 1);
		// biases for each layer (except input layer)
		b[layer].Resize(layer_str[layer+1], 1); //b[layer].rndWgt(0.5);
		rand_mat(b[layer]);
		// weights for two layers (output vs. input)
		W[layer].Resize(layer_str[layer+1], layer_str[layer]); //W[layer].rndWgt(0.5);
		rand_mat(W[layer]);
    }
    
	x[layers].Resize(layer_str[layers], 1);

	int inputs = layer_str[0];
	int outputs = layer_str[layers];

    CVisDMatrix	X(examples,  inputs);		//  inputs
    CVisDMatrix	Y(examples, outputs);		// outputs
	
	int example, input, output;
    for ( example = 0; example < examples; example++)
	{
		for( input = 0; input < inputs; input++)
			X[example][input] = in[input+(inputs)*example];
		for( output = 0; output < outputs; output++)
			Y[example][output] = out[output+(outputs)*example];
	}

	int i;

    for (int trial = 0; trial < m_nTrials; trial++) 
	{
		
		example	 = rand()%examples;
		
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[example][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		double center = x[layers][0][0];
		double var = fabs(x[layers][1][0]);
		double center_d = Y[example][0];
		double var_d = fabs(Y[example][1]);

		for (i=0; i<outputs; i++)
		{
			d[layers-1][0][0] = - m_fLearningRate*(alpha1 * calcTri_dJdm(center, var, center_d, var_d)+ alpha2 * (x[layers][0][0] - Y[example][0]));
			d[layers-1][1][0] = - m_fLearningRate*(alpha1 * calcTri_dJdb(center, var, center_d, var_d)+ alpha2 * (x[layers][1][0] - Y[example][1]));
		}

		//printf("trial: %d, d1: %f, d2: %f, x1: %f, x2: %f \n", trial, d[layers-1][0][0], d[layers-1][1][0], x[layers][0][0], x[layers][1][0]);

		for (layer = layers-1; layer > 0; layer--)
		{
			d[layer-1] = (W[layer].Transposed()*d[layer]);

			if (layer < layers-1)
			{
				d[layer-1] = mat_mat(d[layer-1], x[layer]);
				d[layer-1] = mat_mat(d[layer-1], eye[layer] - x[layer]);
			}
		}

		for (layer = 0; layer < layers; layer++) 
		{
			b[layer] += d[layer];
			W[layer] += (d[layer]*x[layer].Transposed());
		};
    }

	// evaluation
	for (int j=0; j<examples; j++)
	{
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[j][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		// if verbose
		printf("Example #%03d ", j);

		for (int k=0; k<outputs; k++)
			printf("out%02d: %f ", k, x[layers][k][0]);

		printf("\n");
	}

	// destruct
	delete []W;
	delete []d;
	delete []eye;
	delete []x;
	delete []b;
}

/*
CVisDVector eval_nn(CVisDVector X, CVisDMatrix W, CVisDMatrix B)
{
	int i, j, k;
	int layer;

	// construct
    CVisDMatrix	*x = new CVisDMatrix[W.NCols()+1];			// inputs/outputs
	
    int layer;
    for (layer = 0; layer < W.NCols(); layer++) 
	{
		x[layer].Resize(layer_str[layer], 1);
    }
    
	x[layers].Resize(layer_str[layers], 1);

	int inputs = layer_str[0];
	int outputs = layer_str[layers];

    CVisDMatrix	X(examples,  inputs);		//  inputs
    CVisDMatrix	Y(examples, outputs);		// outputs
	
	int example, input, output;
    for ( example = 0; example < examples; example++)
	{
		for( input = 0; input < inputs; input++)
			X[example][input] = in[input+(inputs)*example];
		for( output = 0; output < outputs; output++)
			Y[example][output] = out[output+(outputs)*example];
	}

	// evaluation
	for (j=0; j<examples; j++)
	{
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[j][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

		// if verbose
		printf("Example #%03d ", j);

		for (k=0; k<outputs; k++)
			printf("out%02d: %f ", k, x[layers][k][0]);

		printf("\n");
	}

	delete []x;
}
*/

CVisDMatrix CNNfunction::rand_mat(CVisDMatrix mat, float rnd)
{
	int col, row;
	int i, j;
	
	col = mat.NCols();
	row = mat.NRows();

	CVisDMatrix rst_mat(row, col);

	for (j=0; j<col; j++)
		for (i=0; i<row; i++)
			rst_mat[i][j] = rnd * (rand()/RAND_MAX - 0.5);

	return rst_mat;
}

CVisDVector CNNfunction::rand_vec(CVisDVector vec, float rnd)
{
	int len;
	int i;
	
	len = vec.Length();

	CVisDVector rst_vec(len);

	for (i=0; i<len; i++)
			rst_vec[i] = rnd * (rand()/RAND_MAX - 0.5);

	return rst_vec;
}

// 20010714@9:35a@DJ
// unified learning
void CNNfunction::NN_learning(int layer_no, int *layer_str, int examples, double *in, double *out)
{
	double 	alpha1 = 1.0;
	double 	alpha2 = 1.0;

	int layers = layer_no-1;

	// construct
	CVisDMatrix	*b = new CVisDMatrix[layers];			// threshold  biases
    CVisDMatrix	*x = new CVisDMatrix[layers+1];			// inputs/outputs
	CVisDMatrix	*eye = new CVisDMatrix[layers+1];		// connection weights for momentum.
    CVisDMatrix	*d = new CVisDMatrix[layers];			// "equivalent error"
    CVisDMatrix	*W = new CVisDMatrix[layers];			// connection weights
	
    int layer;
    for (layer = 0; layer < layers; layer++) 
	{
		x[layer].Resize(layer_str[layer], 1);
		eye[layer].Resize(layer_str[layer], 1);
		eye[layer] = 1.0;

		d[layer].Resize(layer_str[layer+1], 1);

		b[layer].Resize(layer_str[layer+1], 1);
		rand_mat(b[layer]);

		W[layer].Resize(layer_str[layer+1], layer_str[layer]);
		rand_mat(W[layer]);
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

	/*
	double error_sum;
	FILE *fp2;
	fp2 = fopen(".\\error_profile.dat", "wt");
	*/

    for (int trial = 0; trial < m_nTrials; trial++) 
	{
		example	 = rand()%examples;
		
		for (i=0; i<inputs; i++)
			x[0][i][0] = X[example][i];

		for (layer = 0; layer < layers-1; layer++)
			x[layer+1] = mat_sigmoid(W[layer]*x[layer] + b[layer]);
		
		x[layers] = W[layers-1]*x[layers-1] + b[layers-1];

//		error_sum = 0.0;

		double center = x[layers][0][0];
		double var = fabs(x[layers][1][0]);
		double center_d = Y[example][0];
		double var_d = fabs(Y[example][1]);

		for (i=0; i<outputs; i++)
		{
			switch(m_Learn)
			{
				// EBP
				case 0:
					d[layers-1][i][0] = m_fLearningRate*(Y[example][i] - x[layers][i][0]);
					break;
				// Fuzzy Observer (for 2 output cases!)
				case 1:
					d[layers-1][0][0] = - m_fLearningRate*(alpha1 * calcTri_dJdm(center, var, center_d, var_d)+ alpha2 * (x[layers][0][0] - Y[example][0]));
					d[layers-1][1][0] = - m_fLearningRate*(alpha1 * calcTri_dJdb(center, var, center_d, var_d)+ alpha2 * (x[layers][1][0] - Y[example][1]));
					break;
				default:
					break;
			}
			
//			error_sum += (Y[example][i] - x[layers][i][0])*(Y[example][i] - x[layers][i][0]);
		}

//		fprintf(fp2, "%f \n", error_sum/2.0);

		// learning process
		for (layer = layers-1; layer > 0; layer--)
		{
			d[layer-1] = (W[layer].Transposed()*d[layer]);

			if (layer < layers-1)
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
			W[layer] += (d[layer]*x[layer].Transposed());
		};
    }

//	fclose(fp2);

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

		// if verbose
		CString str, str2;
		str.Format("Example #%03d: ", j);

		for (int k=0; k<outputs; k++)
		{
			str2.Format("%f ", x[layers][k][0]);
			str += str2;
		}

		AfxMessageBox(str);
	}

	// destruct
	delete []W;
	delete []d;
	delete []eye;
	delete []x;
	delete []b;
}
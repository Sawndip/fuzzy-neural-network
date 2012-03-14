#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "double.Matrix.h"
#define  matrix doubleMatrix
#define  array  doubleArray

#define FALSE	0
#define TRUE	1
//
// 	Ouput layer with Linearity
//	sigmoid : tanh
// 
double sigmoid(double x)
{
	return (1.0 /(1 + exp(-x)));
}

int 	sigmoid_type = 1;		// tanh
int		outputlayer_linear = 1;

int 	error_watch = FALSE;
int watch = FALSE;				// toggle watch training mode

int patient = TRUE;				// signal graceful termination

double IntegralTri(double from, double to, double a, double b);
double calcTri_dJdb(double m, double b, double m_d, double b_d);
double calcTri_dJdm(double m, double b, double m_d, double b_d);
double tri(double ,double ,double , double =0.0, double =0.0);
double IntegrateFunc(double from, double to, double (*f)(double, double, double, double, double), int N, double p1, double p2, double p3, double p4);
double calcJ(double m, double b, double m_d, double b_d, double a1, double a2);
double calcJ1(double m, double b, double m_d, double b_d, double a1, double a2);
double calcJ2(double m, double b, double m_d, double b_d, double a1, double a2);

void
main2 (int argc, char **argv)
{
    int		trials	 = 65536;		// number of trials
    int		verbose  = FALSE;		// verbose reporting off
    long	seedval	 = 43;			// random number seed value
    double	learn	 = 64.0;		// 1.0/(learning rate)
    double	eta	 = 1.0/learn;		// learning rate
    double	sigma	 = 0.0;			// noise variance
	double 	alpha1 = 1.0;
	double 	alpha2 = 1.0;

    if (verbose)
      cerr	<< " seedval = "	<< seedval
		<< " trials = "		<< trials
		<< " sigma = "		<< sigma
		<< " learn = "		<< learn << "\n";
 
    int		layers; cin >> layers;		// number of layers
    matrix	*b = new matrix[layers];			// threshold  biases
    matrix	*W = new matrix[layers];			// connection weights
    matrix	*x = new matrix[layers+1];			// inputs/outputs
    matrix	*d = new matrix[layers];			// "equivalent error"

    int		outputs; cin >> outputs;	// number of outputs
    int		inputs = outputs;		// number of inputs
    if (verbose)
      cerr << "N(" << inputs;

    int layer;
    for (layer = 0; layer < layers; layer++) {
		int inputs = outputs;
		cin >> outputs;
		if (verbose)
			cerr << ", " << outputs;
		x[layer].resize(inputs);
		d[layer].resize(outputs);
		b[layer].resize(outputs);
		W[layer].resize(outputs, inputs);
		for (int output = 0; output < outputs; output++)
			cin >> b[layer][0][output] >> W[layer].s(output);
    }
    x[layers].resize(outputs);

    int		examples; cin >> examples;	// number of examples
    if (verbose)
      cerr << ")\t" << examples << " examples\n";
    matrix	X(examples,  inputs);		//  inputs
    matrix	Y(examples, outputs);		// outputs
    for (int example = 0; example < examples; example++)
      cin >> X.s(example) >> Y.s(example);
 
    if (learn > 0.0)
      eta	 = 1.0/learn;

	double error_sum = 0.0;

	FILE *fout;
	if ((fout = fopen("error.dat","w")) == NULL) {
        fout = stdout;
    }
	FILE *fdebug;
	if ((fdebug = fopen("debug.dat","w")) == NULL) {
        fdebug = stderr;
    }

	fprintf(fdebug, "eta = %10.7f alpha1 = %10.7f alpha2 = %10.7f \n",
			eta, alpha1, alpha2);
    for (int trial = 0; trial < trials && patient; trial++) {
		//int example	 = lrand48()%examples;
		int example	 = rand()%examples;
		x[0]	 = X.s(example);
#define	y	   Y.s(example)

		int layer;
		  //for (layer = 0; layer < layers; layer++)	// Feed Forward
		  //  x[layer+1] = tanh(b[layer] + x[layer]%W[layer]);
		  //d[layers-1] = eta*(y - x[layers])*(1.0 - x[layers]*x[layers]);

		for (layer = 0; layer < layers-1; layer++)	// Feed Forward
		  x[layer+1] = tanh(b[layer] + x[layer]%W[layer]);
		x[layers] = b[layers-1] + x[layers-1]%W[layers-1];

		double center = x[layers][0][0];
		double var = fabs(x[layers][0][1]);
		double center_d = Y[example][0];
		double var_d = fabs(Y[example][1]);
		d[layers-1][0][0] = - eta*(
			  alpha1 * calcTri_dJdm(center, var, center_d, var_d)
			+ alpha2 * (x[layers][0][0] - Y[example][0])
			) ;  				
		d[layers-1][0][1] = - eta* (
			alpha1 * calcTri_dJdb(center, var, center_d, var_d)
			+ alpha2 * (x[layers][0][1] - Y[example][1])
			) ;
/*
		d[layers-1][0][0] = - eta*(
			  alpha1 * calcTri_dJdm(x[layers][0][0], x[layers][0][1], 
									Y[example][0], Y[example][1]) 
			+ alpha2 * (x[layers][0][0] - Y[example][0])
			) ;  									// Back Propagate
		d[layers-1][0][1] = - eta* (
			alpha1 * calcTri_dJdb(	x[layers][0][0], x[layers][0][1], 
									Y[example][0], Y[example][1])  
//			+ alpha2 * (x[layers][0][1] - Y[example][1])
			) ;
*/
// debug
		{
		double m = x[layers][0][0];
		double b = x[layers][0][1];
		double m_d = Y[example][0];
		double b_d = Y[example][1];
		fprintf(fdebug, "%4d %2d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n",
				trial, example, x[layers][0][0], x[layers][0][1], 
				Y[example][0], Y[example][1],	
				calcTri_dJdm(x[layers][0][0], fabs(x[layers][0][1]), 
									Y[example][0], fabs(Y[example][1])),
				calcTri_dJdb(x[layers][0][0], fabs(x[layers][0][1]), 
									Y[example][0], fabs(Y[example][1])),
				IntegralTri(m-b, m, m_d, b_d), 
				IntegralTri(m, m+b, m_d, b_d)
				);
		}

		for (layer = layers-1; layer > 0; layer--)
			d[layer-1] = d[layer]%W[layer].t()*(1.0 - x[layer]*x[layer]);

		for (layer = 0; layer < layers; layer++) {	// Update
			b[layer] += d[layer];
			W[layer] += d[layer]&x[layer];
		};

		if (error_watch) {
			double error = calcJ(x[layers][0][0], fabs(x[layers][0][1]), 
								Y[example][0], Y[example][1], alpha1, alpha2) ;
			double error1 = calcJ1(x[layers][0][0], fabs(x[layers][0][1]), 
								Y[example][0], Y[example][1], alpha1, alpha2) ;
			double error2 = calcJ2(x[layers][0][0], fabs(x[layers][0][1]), 
								Y[example][0], Y[example][1], alpha1, alpha2) ;
			double merror = Y[example][0] - x[layers][0][0];	
			double berror = Y[example][1] - x[layers][0][1];	
			merror = merror * merror;
			berror = berror * berror;
			error_sum += error;
			fprintf(fout, "%4d %10.7f %10.7f %10.7f %10.7f %10.7f %10.7f %10.7f\n",
					trial, error_sum/(double)(trial+1), 
					error, error1, error2,  error_sum, 
					merror, berror);
		}
		if (watch) {
		  cerr <<   "trial = "		<< trial
			   << "\texample = "	<< example	
				<< "\n";
		}
    }

    format("%19.12e", 4);

    cout << layers << "\n";
    cout << inputs << "\n";
    for (layer = 0; layer < layers; layer++) {
		int outputs = b[layer].n();
		cout << outputs << "\n";
		for (int output = 0; output < outputs; output++)
			  cout << format("%19.12e\n", b[layer][0][output])
			   << W[layer].s(output);
  	}
    cout.flush();

    for (int output = 0; output < outputs; output++) {
		for (int example = 0; example < examples; example++) {
			x[0]	 = X.s(example);

			for (int layer = 0; layer < layers-1; layer++)
			  x[layer+1] = tanh(b[layer] + x[layer]%W[layer]);
			x[layers] = b[layers-1] + x[layers-1]%W[layers-1];

			cerr << format("%10.3e\t", Y[example][output])
			 << format("%10.3e\n", x[layers][0][output]);
		}
		cerr << "\n";
    }
    cerr.flush();

	delete b, W, x, d;
	if (fout != stdout) fclose(fout);
	if (fdebug != stderr) fclose(fdebug);
}

double calcTri_dJdm(double m, double b, double m_d, double b_d)
{
	return (IntegralTri(m-b, m, m_d, b_d) - IntegralTri(m, m+b, m_d, b_d))/b;
}

double xtri(double x, double m, double b, double m_d, double b_d)
{
	if (x < m - fabs(b)) return 0.0;
	if (x > m + fabs(b)) return 0.0;
	else return fabs(x-m)/(b*b) * tri(x, m_d, b_d);
}

double calcTri_dJdb(double m, double b, double m_d, double b_d)
{
/*
	double	v = IntegrateFunc(m-b, m+b, xtri, 50, m, b, m_d, b_d);
	cerr << m << " " << b << " " << m_d << " " << b_d << " " << v << "\n";
	return (1.0/3.0 - v);
*/
	return (1.0/3.0 - IntegrateFunc(m-b, m+b, xtri, 50, m, b, m_d, b_d));
}

double Jf(double x, double m, double b, double m_d, double b_d)
{
	double v = tri(x, m, b) - tri(x, m_d, b_d);
	return (v * v);
}

double calcJ(double m, double b, double m_d, double b_d, double alpha1, double alpha2)
{
	return (alpha1 *IntegrateFunc(0.0, 1.0, Jf, 50, m, b, m_d, b_d) + alpha2
* (m - m_d) * (m-m_d));
}

double calcJ1(double m, double b, double m_d, double b_d, double alpha1, double alpha2)
{
	return (IntegrateFunc(0.0, 1.0, Jf, 50, m, b, m_d, b_d));
}
double calcJ2(double m, double b, double m_d, double b_d, double alpha1, double alpha2)
{
	return ((m - m_d) * (m-m_d));
}

double fclips (double x) 
{
	return (x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 :x));
}

double tri(double x, double a, double b, double c, double d)
{
	double  v = (fabs(b) < DBL_MIN)	?  1.0 : (1.0 - fabs((a - x)/b));
	
	return fclips(v);
}

double IntegralTri(double from, double to, double a, double b)
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

double IntegrateFunc(double from, double to, double (*f)(double, double, double, double, double), int N, double p1, double p2, double p3, double p4)
{
	if (from < 0.0) from = 0.0;
	if (to > 1.0) to = 1.0;
	double dx = (to-from)/N;
	double xi = from;
	double s = 0.0;
	for (int i = 0; i < N; i++, xi+= dx) 
		s += (*f)(xi, p1, p2, p3, p4);

	return (s * dx);
}
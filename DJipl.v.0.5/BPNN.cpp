// BPNN.cpp: implementation of the BPNN class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "BPNN.h"
#include <math.h>

#define ABS(x)          (((x) > 0.0) ? (x) : (-(x)))

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define rst DEBUG_rst
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BPNN::BPNN()
{

}

BPNN::~BPNN()
{

}

/*** Return random number between 0.0 and 1.0 ***/
double BPNN::drnd()
{
  return ((double) rand() / (double) RAND_MAX);
}

/*** Return random number between -1.0 and 1.0 ***/
double BPNN::dpn1()
{
  return ((drnd() * 2.0) - 1.0);
}

/*** The squashing function.  Currently, it's a sigmoid. ***/

double BPNN::squash(double x)
{
  return (1.0 / (1.0 + exp(-x)));
}


/*** Allocate 1d array of doubles ***/

double *BPNN::alloc_1d_dbl(int n)
{
  double *rst = new double[n];

  if (rst == NULL) {
    printf("ALLOC_1D_DBL: Couldn't allocate array of doubles\n");
    return (NULL);
  }
  return (rst);
}


/*** Allocate 2d array of doubles ***/

double **BPNN::alloc_2d_dbl(int m, int n)
{
  int i;
  double **rst;
  
  rst = new double *[m];

  if (rst == NULL) {
    printf("ALLOC_2D_DBL: Couldn't allocate array of dbl ptrs\n");
    return (NULL);
  }

  for (i = 0; i < m; i++) {
    rst[i] = alloc_1d_dbl(n);
  }

  return (rst);
}


void BPNN::bpnn_randomize_weights(double **w,int m, int n)
{
  int i, j;

  for (i = 0; i <= m; i++) {
    for (j = 0; j <= n; j++) {
      w[i][j] = dpn1();
    }
  }
}


void BPNN::bpnn_zero_weights(double **w, int m, int n)
{
  int i, j;

  for (i = 0; i <= m; i++) {
    for (j = 0; j <= n; j++) {
      w[i][j] = 0.0;
    }
  }
}


void BPNN::bpnn_initialize()
{
  srand( (unsigned)time( NULL ) );
}


BPNN *BPNN::bpnn_internal_create(int n_in, int n_hidden, int n_out)
{
  BPNN *rstnet = new BPNN;

  if (rstnet == NULL) {
    printf("BPNN_CREATE: Couldn't allocate neural network\n");
    return (NULL);
  }

  rstnet->input_n = n_in;
  rstnet->hidden_n = n_hidden;
  rstnet->output_n = n_out;
  rstnet->input_units = alloc_1d_dbl(n_in + 1);
  rstnet->hidden_units = alloc_1d_dbl(n_hidden + 1);
  rstnet->output_units = alloc_1d_dbl(n_out + 1);

  rstnet->hidden_delta = alloc_1d_dbl(n_hidden + 1);
  rstnet->output_delta = alloc_1d_dbl(n_out + 1);
  rstnet->target = alloc_1d_dbl(n_out + 1);

  rstnet->input_weights = alloc_2d_dbl(n_in + 1, n_hidden + 1);
  rstnet->hidden_weights = alloc_2d_dbl(n_hidden + 1, n_out + 1);

  rstnet->input_prev_weights = alloc_2d_dbl(n_in + 1, n_hidden + 1);
  rstnet->hidden_prev_weights = alloc_2d_dbl(n_hidden + 1, n_out + 1);

  return (rstnet);
}


void BPNN::bpnn_free(BPNN *net)
{
  int n1, n2, i;

  n1 = net->input_n;
  n2 = net->hidden_n;

  delete []net->input_units;
  delete []net->hidden_units;
  delete []net->output_units;

  delete []net->hidden_delta;
  delete []net->output_delta;
  delete []net->target;

  for (i = 0; i <= n1; i++) {
    delete []net->input_weights[i];
    delete []net->input_prev_weights[i];
  }
  delete []net->input_weights;
  delete []net->input_prev_weights;

  for (i = 0; i <= n2; i++) {
    delete []net->hidden_weights[i];
    delete []net->hidden_prev_weights[i];
  }
  delete []net->hidden_weights;
  delete []net->hidden_prev_weights;

  delete net;
}


/*** Creates a rst fully-connected network from scratch,
     with the given numbers of input, hidden, and output units.
     Threshold units are automatically included.  All weights are
     randomly initialized.

     Space is also allocated for temporary storage (momentum weights,
     error computations, etc).
***/

BPNN *BPNN::bpnn_create(int n_in, int n_hidden, int n_out)
{

  BPNN *rstnet;

  rstnet = bpnn_internal_create(n_in, n_hidden, n_out);

#ifdef INITZERO
  bpnn_zero_weights(rstnet->input_weights, n_in, n_hidden);
#else
  bpnn_randomize_weights(rstnet->input_weights, n_in, n_hidden);
#endif
  bpnn_randomize_weights(rstnet->hidden_weights, n_hidden, n_out);
  bpnn_zero_weights(rstnet->input_prev_weights, n_in, n_hidden);
  bpnn_zero_weights(rstnet->hidden_prev_weights, n_hidden, n_out);

  return (rstnet);
}



void BPNN::bpnn_layerforward(double *l1, double *l2, double **conn, int n1, int n2)
{
  double sum;
  int j, k;

  /*** Set up thresholding unit ***/
  l1[0] = 1.0;

  /*** For each unit in second layer ***/
  for (j = 1; j <= n2; j++) {

    /*** Compute weighted sum of its inputs ***/
    sum = 0.0;
    for (k = 0; k <= n1; k++) {
      sum += conn[k][j] * l1[k];
    }
    l2[j] = squash(sum);
  }

}


void BPNN::bpnn_output_error(double *delta, double *target, double *output, double *err, int nj)
{
  int j;
  double o, t, errsum;

  errsum = 0.0;
  for (j = 1; j <= nj; j++) {
    o = output[j];
    t = target[j];
    delta[j] = o * (1.0 - o) * (t - o);
    errsum += ABS(delta[j]);
  }
  *err = errsum;
}


void BPNN::bpnn_hidden_error(double *delta_h, double *delta_o, double *hidden, double **who, double *err, int nh, int no)
{
  int j, k;
  double h, sum, errsum;

  errsum = 0.0;
  for (j = 1; j <= nh; j++) {
    h = hidden[j];
    sum = 0.0;
    for (k = 1; k <= no; k++) {
      sum += delta_o[k] * who[j][k];
    }
    delta_h[j] = h * (1.0 - h) * sum;
    errsum += ABS(delta_h[j]);
  }
  *err = errsum;
}


void BPNN::bpnn_adjust_weights(double *delta, int ndelta, double *ly, int nly, double **w, double **oldw, double eta, double momentum)
{
  double rst_dw;
  int k, j;

  ly[0] = 1.0;
  for (j = 1; j <= ndelta; j++) {
    for (k = 0; k <= nly; k++) {
      rst_dw = ((eta * delta[j] * ly[k]) + (momentum * oldw[k][j]));
      w[k][j] += rst_dw;
      oldw[k][j] = rst_dw;
    }
  }
}


void BPNN::bpnn_feedforward(BPNN *net)
{
  int in, hid, out;

  in = net->input_n;
  hid = net->hidden_n;
  out = net->output_n;

  /*** Feed forward input activations. ***/
  bpnn_layerforward(net->input_units, net->hidden_units,
      net->input_weights, in, hid);
  bpnn_layerforward(net->hidden_units, net->output_units,
      net->hidden_weights, hid, out);

}


void BPNN::bpnn_train(BPNN *net,double eta, double momentum, double *eo, double *eh)
{
  int in, hid, out;
  double out_err, hid_err;

  in = net->input_n;
  hid = net->hidden_n;
  out = net->output_n;

  /*** Feed forward input activations. ***/
  bpnn_layerforward(net->input_units, net->hidden_units,
      net->input_weights, in, hid);
  bpnn_layerforward(net->hidden_units, net->output_units,
      net->hidden_weights, hid, out);

  /*** Compute error on output and hidden units. ***/
  bpnn_output_error(net->output_delta, net->target, net->output_units,&out_err,out);
  bpnn_hidden_error(net->hidden_delta, net->output_delta, net->hidden_units, net->hidden_weights, &hid_err, hid, out);
  *eo = out_err;
  *eh = hid_err;

  /*** Adjust input and hidden weights. ***/
  bpnn_adjust_weights(net->output_delta, out, net->hidden_units, hid,net->hidden_weights, net->hidden_prev_weights, eta, momentum);
  bpnn_adjust_weights(net->hidden_delta, hid, net->input_units, in,net->input_weights, net->input_prev_weights, eta, momentum);

}

void BPNN::bpnn_save(BPNN *net,char *filename)
{
  int n1, n2, n3, i, j;
  double dvalue, **w;
  double *mem;

  FILE *fd;
  fd = fopen(filename, "w");

  n1 = net->input_n;  n2 = net->hidden_n;  n3 = net->output_n;
  printf("Saving %dx%dx%d network to '%s'\n", n1, n2, n3, filename);
  fflush(stdout);

  fprintf(fd, "%d %d %d \n", n1,n2,n3);

  w = net->input_weights;
  mem = new double[(n1+1)*(n2+1)];
  for (i = 0; i <= n1; i++) {
    for (j = 0; j <= n2; j++) {
      dvalue = w[i][j];
	  fprintf(fd, "%f ", dvalue);
    }
	fprintf(fd, "\n");
  }
  delete []mem;

  fprintf(fd, "\n");

  w = net->hidden_weights;
  mem = new double[(n2+1)*(n3+1)];

  for (i = 0; i <= n2; i++) {
    for (j = 0; j <= n3; j++) {
      dvalue = w[i][j];
	  fprintf(fd, "%f ", dvalue);
    }
	fprintf(fd, "\n");
  }
  delete []mem;

  fclose(fd);
  return;
}


BPNN *BPNN::bpnn_read(char *filename)
{
  BPNN *rst;
  int n1, n2, n3, i, j;
  FILE *fd;
  fd = fopen(filename, "r");

  printf("Reading '%s'\n", filename);  fflush(stdout);

  fscanf(fd, "%d %d %d", &n1, &n2,&n3);
  rst = bpnn_internal_create(n1, n2, n3);

  printf("'%s' contains a %dx%dx%d network\n", filename, n1, n2, n3);
  printf("Reading input weights...");  fflush(stdout);

  for (i = 0; i <= n1; i++) {
    for (j = 0; j <= n2; j++) {
		fscanf(fd, "%f", &rst->input_weights[i][j]);
    }
  }

  printf("Done\nReading hidden weights...");  fflush(stdout);

  for (i = 0; i <= n2; i++) {
    for (j = 0; j <= n3; j++) {
		fscanf(fd, "%f", &rst->hidden_weights[i][j]);
    }
  }
  fclose(fd);

  printf("Done\n");  fflush(stdout);

  bpnn_zero_weights(rst->input_prev_weights, n1, n2);
  bpnn_zero_weights(rst->hidden_prev_weights, n2, n3);

  return (rst);
}

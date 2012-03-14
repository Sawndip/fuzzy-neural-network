// BPNN.h: interface for the BPNN class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPNN_H__1B9ED8A6_79BA_42E2_823C_A8D3E09AD117__INCLUDED_)
#define AFX_BPNN_H__1B9ED8A6_79BA_42E2_823C_A8D3E09AD117__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BIGRND 0x7fffffff

class BPNN  
{
public:
	BPNN();
	virtual ~BPNN();

	int input_n;                  /* number of input units */
	int hidden_n;                 /* number of hidden units */
	int output_n;                 /* number of output units */

	double *input_units;          /* the input units */
	double *hidden_units;         /* the hidden units */
	double *output_units;         /* the output units */

	double *hidden_delta;         /* storage for hidden unit error */
	double *output_delta;         /* storage for output unit error */

	double *target;               /* storage for target vector */

	double **input_weights;       /* weights from input to hidden layer */
	double **hidden_weights;      /* weights from hidden to output layer */

							/*** The next two are for momentum ***/
	double **input_prev_weights;  /* previous change on input to hidden wgt */
	double **hidden_prev_weights; /* previous change on hidden to output wgt */

	
	void bpnn_zero_weights(double **w, int m, int n);
	void bpnn_randomize_weights(double **w,int m, int n);
	double **alloc_2d_dbl(int m, int n);
	double *alloc_1d_dbl(int n);
	
	double drnd();
	double dpn1();
	double squash(double x);

	/*** User-level functions ***/
	// srand.
	void bpnn_initialize();

	// create & free bpnn.
	BPNN *bpnn_create(int n_in, int n_hidden, int n_out);
	void bpnn_free(BPNN *net);

	BPNN *bpnn_internal_create(int n_in, int n_hidden, int n_out);
	void bpnn_layerforward(double *l1, double *l2, double **conn, int n1, int n2);
	void bpnn_output_error(double *delta, double *target, double *output, double *err, int nj);
	void bpnn_hidden_error(double *delta_h, double *delta_o, double *hidden, double **who, double *err, int nh, int no);
	void bpnn_adjust_weights(double *delta, int ndelta, double *ly, int nly, double **w, double **oldw, double eta, double momentum);
	void bpnn_train(BPNN *net,double eta, double momentum, double *eo, double *eh);
	void bpnn_feedforward(BPNN *net);

	// save & read weights.
	void bpnn_save(BPNN *net,char *filename);
	BPNN *bpnn_read(char *filename);
};

#endif // !defined(AFX_BPNN_H__1B9ED8A6_79BA_42E2_823C_A8D3E09AD117__INCLUDED_)

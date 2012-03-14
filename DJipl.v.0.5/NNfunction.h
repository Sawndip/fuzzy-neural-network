#ifndef _NNFUNCTION_H_
#define _NNFUNCTION_H_

/*
#include "rl.h"
#include "rl_perc.h"
*/

class CVisDMatrix;
class CVisDVector;

class CNNfunction  
{
public:
	// DJ functions
	CVisDMatrix mat_sigmoid(CVisDMatrix in_mat);
	CVisDVector vec_sigmoid(CVisDVector in_vec);
	CVisDMatrix mat_tanh(CVisDMatrix in_mat);
	CVisDVector vec_tanh(CVisDVector in_vec);
	CVisDMatrix mat_mat(CVisDMatrix mat1, CVisDMatrix mat2);
	CVisDVector vec_vec(CVisDVector vec1, CVisDVector vec2);
	CVisDMatrix rand_mat(CVisDMatrix mat, float rnd = 0.05);
	CVisDVector rand_vec(CVisDVector vec, float rnd = 0.05);
//	CVisDVector eval_nn(CVisDVector X, CVisDMatrix W, CVisDMatrix B);

	inline void SetVar(int learn = 0, int activation = 0, int trial = 10000, float lrate = 0.6, float rnd = 0.05) 
	{ m_Learn = learn; m_Activation = activation; m_nTrials = trial; m_fLearningRate = lrate; m_fRnd = rnd;}

	// DJ variables
	int m_nTrials;
	float m_fLearningRate;
	float m_fRnd;
	int m_Learn;
	int m_Activation;

	//wMLPServer *pwNN;

	// fuzzy observer에 의한 신경망 학습 알고리즘.
	// evaluation function의 차이에 유의할 것.

	// error back-propagation with tanh function
	void ebp_tanh(int layer_no, int *layer_str, int examples, double *in, double *out);
	// error back-propagation with sigmoid function
	void ebp_sig(int layer_no, int *layer_str, int examples, double *in, double *out);
	// fuzzy observer with tanh function
	void fuzzy_observer_tanh(int layer_no, int *layer_str, int examples, double *in, double *out);
	// fuzzy observer with sigmoid function
	void fuzzy_observer_sig(int layer_no, int *layer_str, int examples, double *in, double *out);
	// unified learning function
	void NN_learning(int layer_no, int *layer_str, int examples, double *in, double *out);
	
	CNNfunction();
	virtual ~CNNfunction();

	// sigmoid function
	double sigmoid(double x);
	// tanh function
	double tanh2(double x);
	
	// 각종 performance 함수들.
	// 박규태 박사님 논문을 참조할 것.
	double IntegralTri(double from, double to, double a, double b);
	double calcTri_dJdb(double m, double b, double m_d, double b_d);
	double calcTri_dJdm(double m, double b, double m_d, double b_d);
	double tri(double ,double ,double , double =0.0, double =0.0);
	double xtri(double x, double m, double b, double m_d, double b_d);
	
	//double IntegrateFunc(double from, double to, double (*f)(double, double, double, double, double), int N, double p1, double p2, double p3, double p4);
	double IntegrateFunc_xtri(double from, double to, int N, double p1, double p2, double p3, double p4);
	double IntegrateFunc_Jf(double from, double to, int N, double p1, double p2, double p3, double p4);
	double calcJ(double m, double b, double m_d, double b_d, double a1, double a2);
	double calcJ1(double m, double b, double m_d, double b_d, double a1, double a2);
	double calcJ2(double m, double b, double m_d, double b_d, double a1, double a2);
	double Jf(double x, double m, double b, double m_d, double b_d);
	
	// fuzzy clips function.
	double fclips (double x);
};

#endif
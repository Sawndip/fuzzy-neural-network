// myHMM.h: interface for the myHMM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYHMM_H__129677B1_70B8_4CAE_B08A_5A10A6573C13__INCLUDED_)
#define AFX_MYHMM_H__129677B1_70B8_4CAE_B08A_5A10A6573C13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define rl_UsesAll
#include "rl.h"

class myHMMsequence
{
public:
	int sample;
	int *sequenceNo;
	int **sequence;
};

class myHMM  
{
public:
	myHMM();
	virtual ~myHMM();

	// DJ variables
	int *HMMId;
	int *classId;
	RL_Codebook *myCodebook;
	int nStates;
	int nSymbols;
	int nHMMs;
	int nClasses;

	// DJ functions
	void HMMinit(int no_state, int no_symbol, int no_hmm, int no_class);
	void HMMfree();
	//void HMMeval(float *vector, int nObs);
	void HMMcodebook(float *data, int size, int dim, int *cluster);
	void HMMsetSequence(int seq_len, int *seq_no, int *seq_dat, myHMMsequence *mySeq);
	void HMMtrain(int id, myHMMsequence mySeq, int iter, float eps);
	void HMMtest(int no_sample, myHMMsequence mySeq, int *sol, float *correct);
	void HMMsave(char *str);
	void HMMload(char *str, int no_hmm);
};

#endif // !defined(AFX_MYHMM_H__129677B1_70B8_4CAE_B08A_5A10A6573C13__INCLUDED_)

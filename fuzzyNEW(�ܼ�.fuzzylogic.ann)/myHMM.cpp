// myHMM.cpp: implementation of the myHMM class.
//
//////////////////////////////////////////////////////////////////////

#define rl_UsesAll
#include "rl.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "myHMM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

myHMM::myHMM()
{

}

myHMM::~myHMM()
{

}

void myHMM::HMMinit(int no_state, int no_symbol, int no_hmm, int no_class)
{
	int i, j, k;
	int flags = RL_HMM_EXPONENTIAL;
	float **tprob;
	float **symbprob;
	float *iprob;
	
	nStates = no_state;
	nSymbols = no_symbol;
	nHMMs = no_hmm;
	nClasses = no_class;
	
	HMMId = new int[nHMMs];
	classId = new int[nClasses];
	iprob = new float[nStates];

	classId[0] = RLCreateHMMClass();

	for (i=0; i<nStates; i++)
	{
		tprob = new float *[nStates];
		symbprob = new float *[nStates];

		for (j=0; j<nStates; j++)
		{
			tprob[j] = new float[nStates];
			symbprob[j] = new float[nSymbols];
		}
	}

	srand( (unsigned)time( NULL ) );

	for (k=0; k<nHMMs; k++)
	{
		for (i=0; i<nStates; i++)
		{
			for (j=0; j<nStates; j++)	
			{
				if (i <= j)
					tprob[i][j] = 0.5*(float)rand()/(float)RAND_MAX;
				else
					tprob[i][j] = 0.0f;
			}

			tprob[0][nStates-1] = 0.0f;

			for (j=0; j<nSymbols; j++)	
				symbprob[i][j] = 0.5*(float)rand()/(float)RAND_MAX;
		}

		for (j=0; j<nStates; j++)
			iprob[j] = 0.5*(float)rand()/(float)RAND_MAX;

		HMMId[k] = RLsInitHMMServer(tprob, symbprob, iprob, nStates, nSymbols, flags);

		RLAddHMMToClass(classId[0], HMMId[k]);
	}	

	delete [] iprob;
	delete [] symbprob;
	delete [] tprob;
}

void myHMM::HMMcodebook(float *data, int size, int dim, int *cluster)
{
	int i, j;
	float **trainData;
	
	trainData = new float *[size];

	for (i=0; i<size; i++)
	{
		trainData[i] = new float[dim];
		for (j=0; j<dim; j++)
			trainData[i][j] = *(data + j + i*dim);
	}		

	myCodebook = RLsMakeCodebook(trainData, cluster, size, dim, size, 0x04); 
}

void myHMM::HMMfree()
{
	delete [] classId;
	delete [] HMMId;
	RLHMMFreeAll();
	//RL_FreeCodebook(myCodebook);
}

void myHMM::HMMsetSequence(int seq_len, int *seq_no, int *seq_dat, myHMMsequence *mySeq)
{
	int i, k;
	int cbias;
	
	mySeq->sample = seq_len;
	mySeq->sequenceNo = new int[seq_len];
	mySeq->sequence = new int *[seq_len];

	cbias = 0;
	for (i=0; i<mySeq->sample; i++)
	{
		mySeq->sequenceNo[i] = seq_no[i];
		mySeq->sequence[i] = new int[mySeq->sequenceNo[i]];

		if (i>0)
			cbias += mySeq->sequenceNo[i-1];

		for (k=0; k<mySeq->sequenceNo[i]; k++)
			mySeq->sequence[i][k] = seq_dat[k + cbias];
	}
}

/*
void myHMM::HMMeval(float *vector, int nObs)
{


	printf("Recognition Ratio: %.2f \n", 100.0f*rrT/(float)400.0f);
	for (k=0; k<4; k++)
		printf("Sample No. %d: %.2f \n", k, 100.0f*rrS[k]/(float)100.0f);

	delete [] testSeq.sequenceNo;
	delete [] testSeq.sequence;

	for (i=0; i<2; i++)
	{
		delete [] learnSeq[i].sequenceNo;
		delete [] learnSeq[i].sequence;
	}
}
*/

void myHMM::HMMtrain(int id, myHMMsequence mySeq, int iter, float eps)
{
	RLsTrainHMMBaumWelch(HMMId[id], mySeq.sequence, mySeq.sequenceNo, mySeq.sample, iter, &eps);
}

void myHMM::HMMtest(int no_sample, myHMMsequence mySeq, int *sol, float *correct)
{
	float HMMpdf;
	int bestId;
	int i;

	for (i=0; i<no_sample; i++)
	{
		HMMpdf = RLsEvalHMMForward(-classId[0], mySeq.sequence[i], mySeq.sequenceNo[i], &bestId);
		//HMMpdf = RLsEvalHMMViterbi(-classId[0], mySeq.sequence[i], mySeq.sequenceNo[i], &bestId);

		if (sol[i] == (bestId-1))
			correct[i] = 1.0f;
		else
			correct[i] = 0.0f;
	}
}

void myHMM::HMMsave(char *str)
{
	char str2[10];
	FILE *fp = RLOpenFile(str, "wb");
	
	for (int i=0; i<nHMMs; i++)
	{
		sprintf(str2, "myHMM%d", i);
		RLWriteHMMServer(fp, HMMId[i], str2);
	}
	
	RLCloseFile(fp);
}

void myHMM::HMMload(char *str, int no_hmm)
{
	nHMMs = no_hmm;
	char str2[10];
	FILE *fp = RLOpenFile(str, "rb");
	
	for (int i=0; i<nHMMs; i++)
	{
		sprintf(str2, "myHMM%d", i);
		HMMId[i] = RLReadHMMServer(fp, str2);
	}
	
	RLCloseFile(fp);
}
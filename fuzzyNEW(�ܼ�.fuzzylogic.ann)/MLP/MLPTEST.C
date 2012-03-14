/*M*
//                INTEL CORPORATION PROPRIETARY INFORMATION
//    This software is supplied under the terms of a license agreement or
//    nondisclosure agreement with Intel Corporation and may not be copied
//    or disclosed except in accordance with the terms of that agreement.
//        Copyright  (c) 1994 Intel Corporation.  All Rights Reserved.
//
//                          Abreviations used in function names:
//                                w    Word, 16 bit integer
//                                i    Integer, 32 bit integer
//                                s    Single Precision Floating point
//                                d    Double Precision Floating point
//
//  PVCS:
//    $Workfile:   mlptest.c  $
//    $Revision:   1.7  $
//    $Modtime:   Aug 15 1997 16:35:08  $
//
//    Filename: MLPtest.c
//  Purpose: Computes a feed-forward Multi-Layer Perceptron (MLP) Neural Network, capable of
//           any linear or non-linear mapping from one vector space to another. Weights
//           for the network are typically derived from training examples using the
//           Error Back-Propagation (EBP) learning algorithm. MLPs are typically used
//           for feature extraction or pattern classification. EBP is not implemented in
//           this library. Weights used in testing are determined by a random number
//           g enerator in MatLab.
//
//  Contents:
//
//    wMLPtest          Test RLwEvalMLPFeedForward for correctness
//    sMLPtest          Test RLsEvalMLPFeedForward for correctness
//    wMoreMLPperf      Evaluate RLwEvalMLPFeedForward performance
//    sMoreMLPperf      Evaluate RLsEvalMLPFeedForward performance
//    sMLPtestTrain     Test RLsTrainMLPBackPropagation for the classification abylyty 
//                      of feed forward neural network
//
//    RLwEvalMLPFeedForward      Performs multi-layer feed forward neural network processing on
//                        an input vector to produce a vector of neuron activations given
//                        a set of weights.
//
//    RLsEvalMLPFeedForward    Same as RLwMLPerceptron but uses floats for I/O instead 
//                          of integers.
//    sMLPTrainPerf     Evaluate RLsTrainMLPBackPropagation performance
//
//  Author:      M.Holler,I.Ryzhachkin
*M*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#ifdef RPL_WITH_SPL
#include "own_spl.h"
#endif

#include <windows.h>
#include "dataAPI.h"

#include "tstapi.h"
#include "rl.h"
#include "own_vect.h"
#include "testfunc.h"
#include "rl_perc.h"
#include "MLPtest.h"
#include "aj_api.h"
#include "rlutil.h"
#include "bmt_csv.h"
#include "bmt_err.h"
#include <float.h>

  float *finput;
  short *sinput;
  int      epochNum,EpochLen , nStep, numberOfOutputs;
  float  **pinput,**poutput,eta,errThreshold,momentum;

  static char number[128], number1[128], number2[128], number3[128], number4[128], number5[128];

char *str_outofmem="\n Out of memory ";
extern int TestDebugMode;

#define FullConnected    16
#define PartialConnected 32
//#define OLD_API

//#define GEN_TEST

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

#if (MAXLAYERS < NUMBEROFLAYERS)
    #error MAXLAYERS < NUMBEROFLAYERS
#endif

wMLPServer wMLPServerStruct;
sMLPServer sMLPServerStruct;
wMLPServer*  pwMLPServer;
sMLPServer*  psMLPServer ;
static BOOL InitwTestServer(wMLPServer* pMLPserver,int* layerNeuronCounts,
                               int NLayers,short *weightPtr,int flags,int inputNumber,int Rl_flags){
  int          t,*listPtr,***iPtr,*pneuronInputCounts,i,numberOfInputIndexes,neuronIndex,layerNumber,
               numberOfNeurons,numberOfWeights,numberOfLayers=NLayers+1;
  short int ***wPtr; /* pointer to weights */
   /* count the whole number Of Neurons exclude input layer
   Note: numberOfLayers -  is the size of layerNeuronCounts[] exclude input layer.
   From Version 4 layerNeuronCounts[] would contain the first element that is the input layer length.
  */
    /*copy layerNeuronCounts to pMLPserver-> playerNeuronCounts*/
    pMLPserver-> playerNeuronCounts = (int*)tstmAlloc(numberOfLayers* sizeof(int));
    if(pMLPserver-> playerNeuronCounts==NULL)
        return((BOOL)FALSE);
    pMLPserver-> playerNeuronCounts[0] = inputNumber;
    for(i=1; i < numberOfLayers;++i){
        pMLPserver-> playerNeuronCounts[i] = layerNeuronCounts[i-1];
    }

    /* count the whole number Of Neurons exclude input layer*/
    for(numberOfNeurons =0, layerNumber = 1; layerNumber < numberOfLayers ;
        layerNumber++){
        numberOfNeurons = numberOfNeurons + pMLPserver-> playerNeuronCounts[layerNumber];
    }

    if(flags & PartialConnected){
        /* Allocate memory for neuronInputCounts */
        pneuronInputCounts = (int*)tstmAlloc(numberOfNeurons*sizeof(int));
        if(pneuronInputCounts==NULL){
            free(pMLPserver-> playerNeuronCounts);
            return(FALSE);
        }
        /* Fill neuronInputCounts as full connected */
        for(layerNumber = 1; layerNumber < numberOfLayers ;
            ++layerNumber){
            for(i = 0; i < pMLPserver-> playerNeuronCounts[layerNumber] ;i++){
                pneuronInputCounts[i] = pMLPserver-> playerNeuronCounts[layerNumber-1];
            }
        }
        /* Fill neuronInputCounts in growing order for each layer
        for(layerNumber = 1; layerNumber < numberOfLayers ;
            layerNumber++){
            for(i = 1; i <= pMLPserver-> playerNeuronCounts[layerNumber-1] ;i++){
                pneuronInputCounts[i-1] = i;
            }
        }*/
        /* Allocate memory for pointers to neuronInputCounts */
        pMLPserver-> pneuronInputCounts = (int**)tstmAlloc((numberOfLayers-1)*sizeof(int*));
        if(pMLPserver-> pneuronInputCounts==NULL){
            free(pMLPserver-> playerNeuronCounts);
            free(pneuronInputCounts);
            return(FALSE);
        }
        /* Initialize pMLPserver-> pneuronInputCounts */
        for(t = i = 0;
            i < numberOfLayers-1;
            ++i,t = t + pMLPserver-> playerNeuronCounts[i+1]){
            pMLPserver-> pneuronInputCounts[i] = &pneuronInputCounts[t];
        }

        /* count the whole size of input lists */
        for(numberOfInputIndexes = 0,layerNumber = 1;
            layerNumber < numberOfLayers ;
            layerNumber++){
            for(i=0;i < pMLPserver-> playerNeuronCounts[layerNumber];++i){
                numberOfInputIndexes = numberOfInputIndexes +
                    (pMLPserver-> pneuronInputCounts[layerNumber-1])[i];
            }
        }
        /* Allocate memory for pointers of InputLists */
        pMLPserver-> pneuronInputLists = (int***)tstmAlloc((numberOfNeurons+numberOfLayers-1)*
                                                     sizeof(int*));
        if(pMLPserver-> pneuronInputLists==NULL){
            free(pMLPserver-> playerNeuronCounts);
            free(pneuronInputCounts);
            free(pMLPserver-> pneuronInputCounts);
            return(FALSE);
        }
        /* Allocate memory for indexes */
        listPtr = (int*)tstmAlloc(numberOfInputIndexes*sizeof(int));
        if(listPtr ==NULL){
            free(pMLPserver-> playerNeuronCounts);
            free(pneuronInputCounts);
            free(pMLPserver-> pneuronInputCounts);
            free(pMLPserver-> pneuronInputLists);
            return(FALSE);
        }
        /* Set first order index pointers */
        for(iPtr = pMLPserver-> pneuronInputLists+numberOfLayers-1,/*to the list of pointers for one layer */
            i=1; i < numberOfLayers;
            iPtr = iPtr+pMLPserver-> playerNeuronCounts[i],++i){
            pMLPserver-> pneuronInputLists[i-1] = (int**)iPtr;
            /* Set index pointers */
            for(neuronIndex = 0; neuronIndex < pMLPserver-> playerNeuronCounts[i];
                neuronIndex++){
                iPtr[neuronIndex] = (int**)listPtr;
                /*Set input indexes in growing order from 0 to pMLPserver-> playerNeuronCounts[i-1]*/
                for(t=0;t<(pMLPserver-> pneuronInputCounts[i-1])[neuronIndex];++t){
                    listPtr[t] = t;
                }
                listPtr = listPtr+t;
            }
        }
    }
    else{
        pMLPserver-> pneuronInputCounts = 0;
    }

    /* count the whole size of weight matrices */
    if(flags & FullConnected){
        for(numberOfWeights = 0,layerNumber = 1; layerNumber < numberOfLayers ;
            layerNumber++){
            numberOfWeights = numberOfWeights + pMLPserver-> playerNeuronCounts[layerNumber]*
               pMLPserver-> playerNeuronCounts[layerNumber-1];
        }
    }
    else{
        numberOfWeights = numberOfInputIndexes;
    }
    /* Allocate memory for pointers of weights */
    pMLPserver-> pweights = (short***)tstmAlloc((numberOfNeurons+numberOfLayers-1)*sizeof(short*));
    if(pMLPserver-> pweights==NULL){
        free(pMLPserver-> playerNeuronCounts);
        if(flags & PartialConnected){
            free(pneuronInputCounts);
            free(listPtr);
            free(pMLPserver-> pneuronInputCounts);
            free(pMLPserver-> pneuronInputLists);
        }
        return(FALSE);
    }
    /* Set first order weight pointers */
    for(wPtr  = pMLPserver-> pweights+numberOfLayers-1,/*to the list of pointers for one layer */
        i=1; i < numberOfLayers;
        wPtr = wPtr + pMLPserver-> playerNeuronCounts[i],++i){
        pMLPserver-> pweights[i-1] = (short**)wPtr;
        /* Set weight pointers */
        if(pMLPserver-> pneuronInputCounts == NULL){/*MLP with full connections*/
            for(neuronIndex = 0; neuronIndex < pMLPserver-> playerNeuronCounts[i];
                neuronIndex++){
                wPtr[neuronIndex] = (short**)weightPtr;
                weightPtr = weightPtr + pMLPserver-> playerNeuronCounts[i-1];
            }
        }
        else{
            for(neuronIndex = 0; neuronIndex < pMLPserver-> playerNeuronCounts[i];
                neuronIndex++){
                wPtr[neuronIndex] = (short**)weightPtr;
                weightPtr = weightPtr+( pMLPserver-> pneuronInputCounts[i-1])[neuronIndex];
            }
        }
    }
    pMLPserver-> numberOfLayers = numberOfLayers;
    pMLPserver-> flags = flags | Rl_flags;
    return(TRUE);
}
void FreewTestServer(wMLPServer* server){
    if(server-> flags & PartialConnected ){
  /*      free((server-> pweights[0])[0]);*/
            if(server-> pneuronInputCounts){
                free((server-> pneuronInputLists[0])[0]);
                free(server-> pneuronInputLists);
                free(server-> pneuronInputCounts);
            }
    }
    free(server-> pweights);
    free(server-> playerNeuronCounts);
}
 /**********/
static BOOL InitsTestServer(sMLPServer* pMLPserver,int* layerNeuronCounts,
                            int NLayers,float *weightPtr,int flags,int inputNumber,int Rl_flags){
  int          t,*listPtr,***iPtr,*pneuronInputCounts,i,numberOfInputIndexes,neuronIndex,layerNumber,
               numberOfNeurons,numberOfWeights,numberOfLayers=NLayers+1;
  float     ***wPtr; /* pointer to weights */
   /* count the whole number Of Neurons exclude input layer
   Note: numberOfLayers -  is the size of layerNeuronCounts[] exclude input layer.
   From Version 4 layerNeuronCounts[] would contain the first element that is the input layer length.
   */
    /*copy layerNeuronCounts to pMLPserver-> playerNeuronCounts*/
    pMLPserver-> playerNeuronCounts = (int*)tstmAlloc(numberOfLayers* sizeof(int));
    if(pMLPserver-> playerNeuronCounts==NULL)
        return(FALSE);
    pMLPserver-> playerNeuronCounts[0] = inputNumber;
    for(i=1; i < numberOfLayers;++i){
        pMLPserver-> playerNeuronCounts[i] = layerNeuronCounts[i-1];
    }

    /* count the whole number Of Neurons exclude input layer*/
    for(numberOfNeurons =0, layerNumber = 1; layerNumber < numberOfLayers ;
        layerNumber++){
        numberOfNeurons = numberOfNeurons + pMLPserver-> playerNeuronCounts[layerNumber];
    }

    if(flags & PartialConnected){
        /* Allocate memory for neuronInputCounts */
        pneuronInputCounts = (int*)tstmAlloc(numberOfNeurons*sizeof(int));
        if(pneuronInputCounts==NULL){
            free(pMLPserver-> playerNeuronCounts);
            return(FALSE);
        }
        /* Fill neuronInputCounts as full connected */
        for(layerNumber = 1; layerNumber < numberOfLayers; ++layerNumber){
            for(i = 0; i < pMLPserver-> playerNeuronCounts[layerNumber] ;i++){
                pneuronInputCounts[i] = pMLPserver-> playerNeuronCounts[layerNumber-1];
            }
        }
        /* Fill neuronInputCounts in growing order for each layer
        for(layerNumber = 1; layerNumber < numberOfLayers ;
            layerNumber++){
            for(i = 1; i <= pMLPserver-> playerNeuronCounts[layerNumber-1] ;i++){
                pneuronInputCounts[i-1] = i;
            }
        }*/
        /* Allocate memory for pointers to neuronInputCounts */
        pMLPserver-> pneuronInputCounts = (int**)tstmAlloc((numberOfLayers-1)*sizeof(int*));
        if(pMLPserver-> pneuronInputCounts==NULL){
            free(pMLPserver-> playerNeuronCounts);
            free(pneuronInputCounts);
            return(FALSE);
        }
        /* Initialize pMLPserver-> pneuronInputCounts */
        for(t = i = 0;
            i < numberOfLayers-1;
            ++i,t = t + pMLPserver-> playerNeuronCounts[i+1]){
            pMLPserver-> pneuronInputCounts[i] = &pneuronInputCounts[t];
        }

        /* count the whole size of input lists */
        for(numberOfInputIndexes = 0,layerNumber = 1;
            layerNumber < numberOfLayers ;
            layerNumber++){
            for(i=0;i < pMLPserver-> playerNeuronCounts[layerNumber];++i){
                numberOfInputIndexes = numberOfInputIndexes +
                    (pMLPserver-> pneuronInputCounts[layerNumber-1])[i];
            }
        }
        /* Allocate memory for pointers of InputLists */
        pMLPserver-> pneuronInputLists = (int***)tstmAlloc((numberOfNeurons+numberOfLayers-1)*
                                                     sizeof(int*));
        if(pMLPserver-> pneuronInputLists==NULL){
            free(pMLPserver-> playerNeuronCounts);
            free(pneuronInputCounts);
            free(pMLPserver-> pneuronInputCounts);
            return(FALSE);
        }
        /* Allocate memory for indexes */
        listPtr = (int*)tstmAlloc(numberOfInputIndexes*sizeof(int));
        if(listPtr ==NULL){
            free(pMLPserver-> playerNeuronCounts);
            free(pneuronInputCounts);
            free(pMLPserver-> pneuronInputCounts);
            free(pMLPserver-> pneuronInputLists);
            return(FALSE);
        }
        /* Set first order index pointers */
        for(iPtr = pMLPserver-> pneuronInputLists+numberOfLayers-1,/*to the list of pointers for one layer */
            i=1; i < numberOfLayers;
            iPtr = iPtr+pMLPserver-> playerNeuronCounts[i],++i){
            pMLPserver-> pneuronInputLists[i-1] = (int**)iPtr;
            /* Set index pointers */
            for(neuronIndex = 0; neuronIndex < pMLPserver-> playerNeuronCounts[i];
                neuronIndex++){
                iPtr[neuronIndex] = (int**)listPtr;
                /*Set input indexes in growing order from 0 to pMLPserver-> playerNeuronCounts[i-1]*/
                for(t=0;t<(pMLPserver-> pneuronInputCounts[i-1])[neuronIndex];++t){
                    listPtr[t] = t;
                }
                listPtr = listPtr+t;
            }
        }
    }
    else{
        pMLPserver-> pneuronInputCounts = 0;
    }

    /* count the whole size of weight matrices */
    if(flags & FullConnected){
        for(numberOfWeights = 0,layerNumber = 1; layerNumber < numberOfLayers ;
            layerNumber++){
            numberOfWeights = numberOfWeights + pMLPserver-> playerNeuronCounts[layerNumber]*
               pMLPserver-> playerNeuronCounts[layerNumber-1];
        }
    }
    else{
        numberOfWeights = numberOfInputIndexes;
    }
    /* Allocate memory for pointers of weights */
    pMLPserver-> pweights = (float***)tstmAlloc((numberOfNeurons+numberOfLayers-1)*sizeof(float*));
    if(pMLPserver-> pweights==NULL){
        free(pMLPserver-> playerNeuronCounts);
        if(flags & PartialConnected){
            free(pneuronInputCounts);
            free(listPtr);
            free(pMLPserver-> pneuronInputCounts);
            free(pMLPserver-> pneuronInputLists);
        }
        return(FALSE);
    }
    /* Set first order weight pointers */
    for(wPtr  = pMLPserver-> pweights+numberOfLayers-1,/*to the list of pointers for one layer */
        i=1; i < numberOfLayers;
        wPtr = wPtr + pMLPserver-> playerNeuronCounts[i],++i){
        pMLPserver-> pweights[i-1] = (float**)wPtr;
        /* Set weight pointers */
        if(pMLPserver-> pneuronInputCounts == NULL){/*MLP with full connections*/
            for(neuronIndex = 0; neuronIndex < pMLPserver-> playerNeuronCounts[i];
                neuronIndex++){
                wPtr[neuronIndex] = (float**)weightPtr;
                weightPtr = weightPtr + pMLPserver-> playerNeuronCounts[i-1];
            }
        }
        else{
            for(neuronIndex = 0; neuronIndex < pMLPserver-> playerNeuronCounts[i];
                neuronIndex++){
                wPtr[neuronIndex] = (float**)weightPtr;
                weightPtr = weightPtr+( pMLPserver-> pneuronInputCounts[i-1])[neuronIndex];
            }
        }
    }
    pMLPserver-> numberOfLayers = numberOfLayers;
    pMLPserver-> flags = flags | Rl_flags;
    return(TRUE);
}
void FreesTestServer(sMLPServer* server){
    if(server-> flags & PartialConnected ){
  /*      free((server-> pweights[0])[0]);*/
            if(server-> pneuronInputCounts){
                free((server-> pneuronInputLists[0])[0]);
                free(server-> pneuronInputLists);
                free(server-> pneuronInputCounts);
            }
    }
    free(server-> pweights);
    free(server-> playerNeuronCounts);
}
static int initRand = FALSE;

void trplInitRand(int init)
{
    srand(init);
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
    return r;
}

void sMLPtestTrain(FILE* logStream,int flags){
  /* function prototypes for reference */
    int     sPrintVectMismatch(FILE *stream, char *message, float *actual, float *expected, int vectLen,
                float max_percent_error,  float  max_error);
    int     wPrintVectMismatch(FILE *stream, char *message, short int *actual, short int *expected, int vectLen,
                float max_percent_error, int max_error);
    int     iPrintScalarMismatch(FILE *stream, char *message, int actual, int expected,
                double max_percent_error, int max_error);
    int     numberOfInputs = NUMBER_OF_INPUTS,
            numberOfWeights,numberOfLayers = NUMBEROFLAYERS, layerNumber,
            i, error = 0, err1 = 0, err2 = 0, totalErrors = 0,errNum, 
            layerNeuronCounts[MAXLAYERS]; /* an array of ints that stores the number of neurons in each layer */
    float   /*x,y,x1,y1,randVal,*/*weights,*output,maxPercentError = (float).5,
            smaxError = (float).05;
    float   C1_class[2] ={(float) X_class_C1,(float)Y_class_C1},
            C2_class[2] ={(float) X_class_C2,(float)Y_class_C2};

  RPL_FILE* trainStream;
  char *new_datfile = "nmlpstrn.dat";
  //char  new_datPath[256];

  char    message[100]= {"RLsTrainMLPBackPropagation "};
  char    message1[200];
  int      xnumberOfGeometry=2;      /* length of *pnumberOfGeometryList */
  int      xnumberOfGeometryList[2]={8,8};
  int     *pxnumberOfGeometryList=xnumberOfGeometryList;
  int      *pGeometryList;
  //int    **pxcoord;                /* geometrical figure coordinates */
  int      xGeometryList[16]={X_left_Margine,Y_low_Margine_c1,
                             X_left_Margine,Y_top_Margine_c1,
                             X_right_Margine,Y_top_Margine_c1,
                             X_right_Margine,Y_low_Margine_c1,
                             X_left_Margine,Y_low_Margine_c2,
                             X_left_Margine,Y_top_Margine_c2,
                             X_right_Margine,Y_top_Margine_c2,
                             X_right_Margine,Y_low_Margine_c2};

    sMLPCallBack MLPActivation[MAXLAYERS-1],MLPDerivative[MAXLAYERS-1];
    EpochLen = EPOCHLEN;
    nStep = MAX_TRAINING_STEPS;
    errThreshold = (float)ERR_THRESHOLD;
    momentum = (float)MOMENTUM;
    eta = (float)LERNING_RATE;
    numberOfOutputs = NUMBER_OF_OUTPUTS;
    RLSetStatus(RL_StsOk);

    for(i=0;i<numberOfLayers-1;++i){
        MLPActivation[i] = RLsSigmoidActivation ;
        MLPDerivative[i] = RLsSigmoidDerivative ;
    }

    layerNeuronCounts[0] = NUMBER_OF_INPUTS;
    layerNeuronCounts[numberOfLayers-1] = NUMBER_OF_OUTPUTS;
    for(i=1;i<(numberOfLayers-1);++i){
        layerNeuronCounts[i] = FIRST_HIDDEN_LAYER_LEN*i+50;
    }
#ifdef GEN_TEST
    testRLWriteBeginBody("title of header", "08/08/98", new_datfile);
    testRLWriteBeginTest("sMLPtestTrain", "sMLPtestTrain", "08/08/98", new_datfile);
    if(testRLWriteBeginData("sMLPtestTrain",new_datfile))
        return;
    if(testRLiWriteScalar("xnumberOfGeometry", 2, new_datfile, NULL, NULL))
        return;
    if(testRLiWriteVector("xnumberOfGeometryList", xnumberOfGeometryList, xnumberOfGeometry,
        new_datfile, NULL, NULL,NOT_USE_INDEX))
        return;
    if(testRLiWriteVector("xGeometryList", xGeometryList, 16, new_datfile, NULL, NULL,NOT_USE_INDEX))
        return;
    if(testRLsWriteScalar("LERNING_RATE", LERNING_RATE, new_datfile, NULL, NULL))
        return;
    if(testRLsWriteScalar("MOMENTUM", MOMENTUM, new_datfile, NULL, NULL))
        return;
    if(testRLsWriteScalar("ERR_THRESHOLD", ERR_THRESHOLD, new_datfile, "%g", NULL))
        return;
    if(testRLiWriteScalar("MAX_TRAINING_STEPS", MAX_TRAINING_STEPS, new_datfile, NULL, NULL))
        return;
    if(testRLiWriteScalar("EpochLen", EpochLen, new_datfile, NULL, NULL))
        return;
    if(testRLiWriteScalar("numberOfLayers", numberOfLayers, new_datfile, NULL, NULL))
        return;
    if(testRLiWriteScalar("numberOfInputs", numberOfInputs, new_datfile, NULL, NULL))
        return;
    if(testRLiWriteVector("layerNeuronCounts", &layerNeuronCounts[0], numberOfLayers,
        new_datfile, NULL, NULL,NOT_USE_INDEX))
        return;
    for(numberOfWeights = 0,layerNumber = 1; layerNumber < numberOfLayers;
        layerNumber++){
        numberOfWeights = numberOfWeights + layerNeuronCounts[layerNumber] * 
            layerNeuronCounts[layerNumber-1];
    }
    weights = (float*)tstmAlloc(numberOfWeights*sizeof(float));
    if(weights==NULL){
        fprintf(stdout, str_outofmem);
        fprintf(logStream, str_outofmem);
        sprintf(number4, str_outofmem);
        err2=1;
        return;
    }
    for(i=0;i<numberOfWeights;++i){
        weights[i] = (float)(((float)trplRand(-(int)(INIT_WEIGHTS_INTERVAL * MAX16U),
            (int)(INIT_WEIGHTS_INTERVAL * MAX16U)))/(float)MAX16U);
    }
    if(testRLsWriteVector("weights", weights, numberOfWeights, new_datfile, " %g", NULL,
        NOT_USE_INDEX)){
        free(weights);
        return;
    }
    free(weights);
    /* generate empty pinput vector for allocation */
    if(testRLiWriteVector("pinput", NULL, EPOCHLEN, new_datfile, NULL, NULL,
        NOT_USE_INDEX)){
        return;
    }
    /* generate lerning vectors */
    weights = (float*)tstmAlloc(EPOCHLEN*2*sizeof(float));
    if(weights==NULL){
        fprintf(stdout, str_outofmem);
        fprintf(logStream, str_outofmem);
        sprintf(number4, str_outofmem);
        err2=1;
        return;
    }
    for(i=0; i< EPOCHLEN/2; ++i){
        weights[i*4]   = (float)trplRand(X_left_Margine,X_right_Margine); /* class c1 - square higher the x-coordinate line*/
        weights[i*4+1] = (float)trplRand(Y_low_Margine_c1,Y_top_Margine_c1);   /* class c1*/
        weights[i*4+2] = (float)trplRand(X_left_Margine,X_right_Margine); /* class c2*/
        weights[i*4+3] = (float)trplRand(Y_low_Margine_c2,Y_top_Margine_c2); /* class c2 - square lower the x-coordinate line*/
    }
    if(testRLsWriteVector("lerningVector", weights, EPOCHLEN*2, new_datfile, " %g", NULL,
        NOT_USE_INDEX)){
        free(weights);
        return;
    }
    free(weights);
    /* generate empty poutput vector for allocation */
    if(testRLiWriteVector("poutput", NULL, EPOCHLEN, new_datfile, NULL, NULL,NOT_USE_INDEX)){
        return;
    }
    /* generate target vectors for classes c1 & c2 */
    weights = (float*)tstmAlloc(EPOCHLEN*2*sizeof(float));
    if(weights==NULL){
        fprintf(stdout, str_outofmem);
        fprintf(logStream, str_outofmem);
        sprintf(number4, str_outofmem);
        err2=1;
         return;
    }
    for(i=0; i< EPOCHLEN/2; ++i){
        weights[i*4]   = C1_class[0]; /* class c1 - square higher the x-coordinate line*/
        weights[i*4+1] = C1_class[1];   /* class c1*/
        weights[i*4+2] = C2_class[0]; /* class c2*/
        weights[i*4+3] = C2_class[1]; /* class c2 - square lower the x-coordinate line*/
    }
    if(testRLsWriteVector("targetVector", weights, EPOCHLEN*2, new_datfile, " %g", NULL,
        NOT_USE_INDEX)){
        free(weights);
        return;
    }
    free(weights);
    /* generate test vectors for c1*/
    weights = (float*)tstmAlloc(EPOCHLEN*2*sizeof(float));
    if(weights==NULL){
        fprintf(stdout, str_outofmem);
        fprintf(logStream, str_outofmem);
        sprintf(number4, str_outofmem);
        err2=1;
        return   ;
    }
    for(i=0; i< (TEST_ARRAY_LEN/2); ++i){
        weights[i*2]   = (float)trplRand(X_left_Margine,X_right_Margine);
        weights[i*2+1] = (float)trplRand(Y_low_Margine_c1,Y_top_Margine_c1);
    }
    if(testRLsWriteVector("Class1TestVector", weights, TEST_ARRAY_LEN, new_datfile, " %g", NULL,
        NOT_USE_INDEX)){
        free(weights);
        return;
    }
    for(i=0; i< (TEST_ARRAY_LEN/2); ++i){
        weights[i*2]= (float)trplRand(X_left_Margine,X_right_Margine); /* class c2*/
        weights[i*2+1]= (float)trplRand(Y_low_Margine_c2,Y_top_Margine_c2); /* class c2 - square lower the x-coordinate line*/
    }
    if(testRLsWriteVector("Class2TestVector", weights, TEST_ARRAY_LEN, new_datfile, " %g", NULL,
        NOT_USE_INDEX)){
        free(weights);
        return;
    }
    free(weights);
    if(testRLWriteFinishData(new_datfile))
        return;
    if(testRLWriteFinishTest(new_datfile))
        return;
    if(testRLWriteFinishBody(new_datfile))
        return;
#endif  /*generate*/
    trainStream=testRLOpenFile(new_datfile, "sMLPtestTrain", "08/08/98", TEST_RPL);
	if(trainStream==NULL){
        sprintf(message1, "*** Could not open MLPerceptron s data file for training %s\\%s\n", dataPath,new_datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
    }
    fprintf(stdout,    "\n\n--- Testing  RLsTrainMLPBackPropagation - float inputs and outputs\n");
    fprintf(logStream, "\n\n--- Testing  RLsTrainMLPBackPropagation - float inputs and outputs\n");
    fprintf(stdout,    "--- Data File %s\\%s\n", dataPath,new_datfile);
    fprintf(logStream, "--- Data File %s\\%s\n", dataPath,new_datfile);
    sprintf (number, "\n\n--- Testing  RLsTrainMLPBackPropagation - float inputs and outputs\n");
    sprintf (number1,"--- Data File %s\\%s\n", dataPath,new_datfile);
    sprintf (number2,"\n\n*** Errors in RLsTrainMLPBackPropagation.\n ");

	testRLFindTagData(trainStream);
    if(testRLiReadScalar(trainStream,"xnumberOfGeometry", &xnumberOfGeometry))
        return;
    if(testRLiGetVector(trainStream,"xnumberOfGeometryList",&pxnumberOfGeometryList))
        goto close_all;
    if(testRLiGetVector(trainStream,"xGeometryList", &pGeometryList))
        goto close_all;
    if(testRLsReadScalar(trainStream,"LERNING_RATE", &eta))
        return;
    if(testRLsReadScalar(trainStream,"MOMENTUM", &momentum))
        return;
    if(testRLsReadScalar(trainStream,"ERR_THRESHOLD", &errThreshold))
        return;
    if(testRLiReadScalar(trainStream,"MAX_TRAINING_STEPS", &nStep))
        return;

    testRLiReadScalar(trainStream,"EpochLen", &EpochLen);
    testRLiReadScalar(trainStream,"numberOfLayers", &numberOfLayers );
    testRLiReadScalar(trainStream,"numberOfInputs", &numberOfInputs);

    fprintf(stdout, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
            numberOfInputs, new_datfile,numberOfLayers);
    fprintf(logStream, "\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
            numberOfInputs,new_datfile, numberOfLayers);
    sprintf (number3, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs,new_datfile, numberOfLayers);

    /* read in the number of neurons in each layer into the array layerNeuronCounts */
    testRLiReadVector(trainStream,"layerNeuronCounts", layerNeuronCounts, numberOfLayers);
    /* calculate total number of weights and read in weights */
    for(numberOfWeights = 0,layerNumber = 1; layerNumber < numberOfLayers;
        layerNumber++){
        numberOfWeights = numberOfWeights + layerNeuronCounts[layerNumber] * 
            layerNeuronCounts[layerNumber-1];
    }
    if(testRLsGetVector(trainStream,"weights", &weights))
        goto close_all;
    /*allocate memory for pinput*/
    testRLiGetVector(trainStream,"pinput", &(int*)pinput);
    /* read in training  epoch vectors */
    if(testRLsGetVector(trainStream,"lerningVector", &finput))
        goto close_all;
    for(i = 0; i < EpochLen; i++){
        pinput[i] = &finput[i*numberOfInputs];
    }
    /*allocate memory for poutput*/
    testRLiGetVector(trainStream,"poutput", &(int*)poutput);
    /* read in targets c1 & c2*/
    /* read in training  epoch vectors */
    if(testRLsGetVector(trainStream,"targetVector", &output))
        goto close_all;
    for(i = 0; i < EPOCHLEN; i++){
        poutput[i] = &output[i*numberOfInputs];
    }
    /*  Main test Segment   */
    if(InitsTestServer(&sMLPServerStruct,&layerNeuronCounts[1],numberOfLayers-1,weights,
        FullConnected,numberOfInputs,flags)==FALSE){
        sprintf(number4, "\n*** FALSE code from InitsTestServer\n");
        err2 =1;
        goto close_all;
    }
    if((psMLPServer = RLsInitMLPServer(sMLPServerStruct.pweights,
        sMLPServerStruct.pneuronInputLists,
        sMLPServerStruct.pneuronInputCounts,
        sMLPServerStruct.playerNeuronCounts, MLPActivation, MLPDerivative,
        sMLPServerStruct.numberOfLayers,
        sMLPServerStruct.flags,0))==NULL){
        fprintf(logStream, "\n*** Null pointer from RLsInitMLPServer\n");
        sprintf(number4, "\n*** Null pointer from RLsInitMLPServer\n");
        err2 = 1;
        goto free_tsts;
    }
        /*  Main test Segment   */

    RLsTrainMLPBackPropagation(psMLPServer, pinput, EpochLen, numberOfInputs,
        poutput, numberOfOutputs, eta, nStep, errThreshold, momentum, &epochNum);
    if(RLGetStatus() != RL_StsOk){
        sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
        err2 = 1;
        goto free_serv;
    }
    fprintf(logStream, "\n*** errThreshold = %g epochN = %i nStep = %i \n",errThreshold,
        epochNum,nStep);
    fprintf(stdout,    "\n***  errThreshold = %g epochN = %i nStep = %i \n",errThreshold,
        epochNum,nStep);
    sprintf (number4, "\n***  errThreshold = %g epochN = %i nStep = %i \n",errThreshold,
                        epochNum,nStep);
/* check classification ability of the network for c1 class */
    for(errNum = i=0;i<(TEST_ARRAY_LEN/2);++i){
            /* read in testing vectors */
        testRLsReadVector(trainStream,"Class1TestVector", finput, numberOfInputs);
        RLsEvalMLPFeedForward(psMLPServer,finput,numberOfInputs,output,numberOfOutputs);
        error = sPrintVectMismatch(logStream, message, output, C1_class, numberOfOutputs,
                             maxPercentError, smaxError);
        if(error){
            errNum++;
            if (!err1) {
                tstErrPrintStr (number);
                tstErrPrintStr (number1);
                tstErrPrintStr (number2);
                tstErrPrintStr (number3);
                tstErrPrintStr (number4);
                err1 = 1;
            }
            tstErrsPrintVectMismatch(FALSE, message, message, output, C1_class, numberOfOutputs,
                    maxPercentError, smaxError);
        }
    }
    fprintf(logStream,"\nVerifying classification outputs");
    fprintf(logStream, "\n*** Classified  %i percents for c1 class\n",((TEST_ARRAY_LEN/2-errNum)*100)/(TEST_ARRAY_LEN/2));
    fprintf(stdout,    "\n*** Classified  %i percents for c1 class\n",((TEST_ARRAY_LEN/2-errNum)*100)/(TEST_ARRAY_LEN/2));
    if (err1) {
        tstErrPrintStr ("\nVerifying classification outputs\n");
        sprintf(number5,"\n*** Classified  %i percents for c1 class\n",((TEST_ARRAY_LEN/2-errNum)*100)/(TEST_ARRAY_LEN/2));
        tstErrPrintStr (number5);
    }
    /* check classification ability of the network for c2 class */
    for(errNum = i=0;i<(TEST_ARRAY_LEN/2);++i){
        /* read in testing vectors */
        testRLsReadVector(trainStream,"Class2TestVector", finput, numberOfInputs);
        RLsEvalMLPFeedForward(psMLPServer,finput,numberOfInputs,output,numberOfOutputs);
        error = sPrintVectMismatch(logStream, message, output, C2_class, numberOfOutputs,
                           maxPercentError, smaxError);
       if(error){
            errNum++;
            if (!err1) {
               tstErrPrintStr (number);
               tstErrPrintStr (number1);
               tstErrPrintStr (number2);
               tstErrPrintStr (number3);
               tstErrPrintStr (number4);
               err1 = 1;
            }
            tstErrsPrintVectMismatch(FALSE, message, message, output, C2_class, numberOfOutputs,
                     maxPercentError, smaxError);
        }
    }
    fprintf(logStream, "\n*** Classified  %i percents for c2 class\n",((TEST_ARRAY_LEN/2-errNum)*100)/(TEST_ARRAY_LEN/2));
    fprintf(stdout,    "\n*** Classified  %i percents for c2 class\n",((TEST_ARRAY_LEN/2-errNum)*100)/(TEST_ARRAY_LEN/2));
    if(err1) {
       tstErrPrintStr ("\nVerifying classification outputs\n");
       sprintf(number5,"\n*** Classified  %i percents for c2 class\n",((TEST_ARRAY_LEN/2-errNum)*100)/(TEST_ARRAY_LEN/2));
       tstErrPrintStr (number5);
    }

free_serv:
        RLsFreeMLPServer(psMLPServer);
free_tsts:
        FreesTestServer(&sMLPServerStruct);
close_all:
    if(err2){
       if(!err1) {
           tstErrPrintStr (number);
           tstErrPrintStr (number1);
           tstErrPrintStr (number2);
           err1 = 1;
       }
    }
    totalErrors += error;
    if(err1) {
        m_bTestAccuracyError = TRUE;
        sprintf(number5,"\n*************** Total Errors, RLsTrainMLPBackPropagation %i     ******************\n\n",totalErrors);
        tstErrPrintStr (number5);
    }
    fprintf(logStream, "\n*************** Total Errors, RLsTrainMLPBackPropagation   %i     ******************",totalErrors);
    fprintf(stdout,    "\n*************** Total Errors, RLsTrainMLPBackPropagation   %i     ******************",totalErrors);
    testRLCloseFile(trainStream);
} /* sMLPtestTrain */
void wMLPtest(FILE* logStream,char *datfile,int flags) {
  /* function prototypes for reference */
    int  sPrintVectMismatch(FILE *stream, char *message, float *actual, float *expected, int vectLen,
            float max_percent_error,  float  max_error);
    int  wPrintVectMismatch(FILE *stream, char *message, short int *actual, short int *expected, int vectLen,
            float max_percent_error, int max_error);
    int  iPrintScalarMismatch(FILE *stream, char *message, int actual, int expected,
            double max_percent_error, int max_error);
  /* variables to store data from MATLAB generated file of test vectors */
    int         /*numTestVects,*/ vectNo, numberOfInputs, inputsExponent, numberOfLayerInputs,
                weightsExponent, numberOfWeights, numberOfLayers;
    int         doScaleOutput,scaleF;
    int*        layerNeuronCounts;
    short*      MLoutput;
    short*      weights;
  /* variables to store results from recognition library function */
    short*      output;
  /* miscellaneous variables  */
#ifdef GEN_TEST
    FILE*       stream;
    int         numTestVects;
#endif
    int         layerNumber;
    int         i, error = 0, err1 = 0, err2 = 0,totalErrors = 0, imaxError = 8;
    float       maxPercentError = (float)1.0;
    char        message[100]= {"RLwEvalMLPFeedForward"};
    char        message1[200];
    char       *new_w_datfile="nmlpwtest.dat";
    wMLPCallBack MLPActivation[MAXLAYERS-1],MLPDerivative[MAXLAYERS-1];
    RPL_FILE   *nStream;
    int*        scaleFactor=&scaleF;
    RLSetStatus(RL_StsOk);

#ifdef GEN_TEST
   if ((stream = ownOpenData(datfile, "r")) == NULL) {
        sprintf(message1, "***  Could not open MLPerceptron w data file %s\\%s\n", dataPath,datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
   }
   fprintf(stdout,    "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
        fprintf(logStream, "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
        fprintf(stdout,    "--- Data File %s\\%s\n", dataPath,datfile);
        fprintf(logStream, "--- Data File %s\\%s\n", dataPath,datfile);
        sprintf (number, "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
        sprintf (number1, "--- Data File %s\\%s\n", dataPath,datfile);
        sprintf (number2, "\n\n*** Errors in RLwEvalMLPFeedForward.\n Printing out mismatching Actual and Expected Values\n");

      fscanf(stream, "%i", &numTestVects);    /* read in the number of test vectors */
      testRLWriteBeginBody("title of header", "08/08/98", new_w_datfile);
      testRLWriteBeginTest("wMLPtest", "wMLPtest", "08/08/98", new_w_datfile);
      if(testRLiWriteScalar("numTestVects", numTestVects, new_w_datfile, NULL, NULL))
          return;
      for  (vectNo  = 1; vectNo  <= numTestVects;  vectNo++){
          fprintf(stdout,    "\n Test Vector Number %i ",vectNo);
          fprintf(logStream, "\n Test Vector Number %i ",vectNo);
          sprintf(number3, "\n Test Vector Number %i \n ",vectNo);
          fscanf(stream, "%i %i",&numberOfLayers, &numberOfInputs);  /* read in the number of layers and inputs */
          fscanf(stream, "%i", &inputsExponent);
          /* read in input vector */
          sinput = (short*)calloc(numberOfInputs, sizeof(short));  /* Allocate memory for input vector */
          for (i = 0;  i  <  numberOfInputs;  i++)
              fscanf(stream,  "%hi",  &sinput[i]);
          fprintf(stdout,    "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);
          fprintf(logStream, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);
          sprintf (number4, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);

          /* read in the number of neurons in each layer into the array layerNeuronCounts */
          layerNeuronCounts = (int*)calloc(numberOfLayers, sizeof(int)); /* memory for layer neuron counts */
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++)
              fscanf(stream, "%i", &layerNeuronCounts[layerNumber]);

          /* read in the weights exponent */
          fscanf(stream, "%i", &weightsExponent);
          /* calculate total number of weights allocate memory, and read in weights */
          numberOfLayerInputs = numberOfInputs;
          numberOfWeights = 0;
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
          } /* for layerNumber */
          weights = (short*)calloc(numberOfWeights, sizeof(short));
          for (i = 0;  i  <  numberOfWeights;  i++) {
              fscanf(stream,  "%hi", &weights[i]);
          } /* for i */

          fscanf(stream, "%i", &doScaleOutput);  /* read in output scaling option */
          fscanf(stream, "%i", scaleFactor);   /* read in output scale factor */
          /* Allocate memory and read in MatLab output vector */
          numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
          MLoutput = (short*)calloc(numberOfOutputs, sizeof(short));
          output = (short*)calloc(numberOfOutputs, sizeof(short));
          for (i = 0;  i  <  numberOfOutputs;  i++) {
              fscanf(stream,  "%hi",  &MLoutput[i]);
          } /* for */
    if(testRLWriteBeginData("wMLPtest",new_w_datfile))
        return;
    if(testRLiWriteScalar("numberOfLayers", numberOfLayers, new_w_datfile, NULL, NULL))
        return;
    if(testRLiWriteScalar("numberOfInputs", numberOfInputs, new_w_datfile, NULL, NULL))
        return;
    if(testRLiWriteScalar("inputsExponent", inputsExponent, new_w_datfile, NULL, NULL))
        return;
    if(testRLwWriteVector("sinput", sinput, numberOfInputs,new_w_datfile, NULL, NULL,
        NOT_USE_INDEX))
        return;
    if(testRLiWriteVector("layerNeuronCounts", layerNeuronCounts, numberOfLayers,
            new_w_datfile, NULL, NULL, NOT_USE_INDEX))
        return;
    if(testRLiWriteScalar("weightsExponent", weightsExponent, new_w_datfile, NULL, NULL))
        return;
    if(testRLwWriteVector("weights", weights, numberOfWeights, new_w_datfile, "%hi", NULL,
        NOT_USE_INDEX))
        return;
    if(testRLiWriteScalar("doScaleOutput", doScaleOutput, new_w_datfile, NULL, NULL))
        return;
    if(testRLiWriteScalar("scaleFactor", *scaleFactor, new_w_datfile, NULL, NULL))
        return;
    /* generate empty output vector for allocation */
    if(testRLwWriteVector("output", NULL, numberOfOutputs, new_w_datfile, "%hi", NULL,NOT_USE_INDEX)){
        return;
    }
    if(testRLwWriteVector("MLoutput", MLoutput, numberOfOutputs, new_w_datfile, "%hi", NULL,
        NOT_USE_INDEX))
        return;
    if(testRLWriteFinishData(new_w_datfile))
        return;
    }/* for all test vectors*/
    if(testRLWriteFinishTest(new_w_datfile))
        return;
    if(testRLWriteFinishBody(new_w_datfile))
        return;
       free(sinput);
       free(output);
       free(MLoutput);
       free(weights);
       free(layerNeuronCounts);
       fclose(stream);
#endif  /*generate*/

    nStream=testRLOpenFile(new_w_datfile, "wMLPtest", "08/08/98", TEST_RPL);
    if(nStream==NULL){
        sprintf(message1, "***  Could not open MLPerceptron w data file %s\\%s\n",
            dataPath,new_w_datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
    }
    fprintf(stdout,    "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
    fprintf(logStream, "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
    fprintf(stdout,    "--- Data File %s\\%s\n", dataPath,datfile);
    fprintf(logStream, "--- Data File %s\\%s\n", dataPath,datfile);
    sprintf (number, "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
    sprintf (number1, "--- Data File %s\\%s\n", dataPath,datfile);
    sprintf (number2, "\n\n*** Errors in RLwEvalMLPFeedForward.\n Printing out mismatching Actual and Expected Values\n");

    for(vectNo = 1; testRLFindTagData(nStream)==RET_FOUND_TAG; vectNo++){
        fprintf(stdout,    "\n Test Vector Number   %i  ",vectNo);
        fprintf(logStream, "\n Test Vector Number   %i  ",vectNo);
        sprintf(number3, "\n Test Vector Number   %i \n ",vectNo);
        if(testRLiReadScalar(nStream,"numberOfLayers", &numberOfLayers))
            return;
        if(testRLiReadScalar(nStream,"numberOfInputs", &numberOfInputs))
            return;
        if(testRLiReadScalar(nStream,"inputsExponent", &inputsExponent))
            return;

          /* read in input vector */
        if(testRLwGetVector(nStream,"sinput", &sinput))
            return;
        fprintf(stdout,    "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);
        fprintf(logStream, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);
        sprintf (number4, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);

        /* read in the number of neurons in each layer into the array layerNeuronCounts */
        if(testRLiGetVector(nStream,"layerNeuronCounts", &layerNeuronCounts))
            return;

          /* read in the weights exponent */
        if(testRLiReadScalar(nStream,"weightsExponent", &weightsExponent))
            return;
          /* calculate total number of weights allocate memory, and read in weights */
        numberOfLayerInputs = numberOfInputs;
        numberOfWeights = 0;
        for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
        } /* for layerNumber */
        if(testRLwGetVector(nStream,"weights", &weights))
            goto close_all;
        if(testRLiReadScalar(nStream,"doScaleOutput", &doScaleOutput))
            return;
        if(testRLiReadScalar(nStream,"scaleFactor", scaleFactor))
            return;
          /* Allocate memory and read in MatLab output vector */
        numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
        testRLwGetVector(nStream,"output", &output);
        if(testRLwGetVector(nStream,"MLoutput", &MLoutput))
            goto close_all;

        if(InitwTestServer(&wMLPServerStruct,layerNeuronCounts,numberOfLayers,weights,
              FullConnected,numberOfInputs,flags)==FALSE){
              sprintf(number5, "\n*** FALSE code from InitwTestServer\n");
              err2=1;
              goto close_all;
          }

        if((pwMLPServer = RLwInitMLPServer(wMLPServerStruct.pweights,
              wMLPServerStruct.pneuronInputLists,
              wMLPServerStruct.pneuronInputCounts,
              wMLPServerStruct.playerNeuronCounts, MLPActivation, MLPDerivative,
              wMLPServerStruct.numberOfLayers,
              wMLPServerStruct.flags,
              weightsExponent,0))==NULL){
              fprintf(logStream, "\n*** Null pointer from RLwInitMLPServer\n");
              sprintf(number5, "\n*** Null pointer from RLwInitMLPServer\n");
              err2=1;
              goto free_tsts;
          }
          for(i=0;i<(pwMLPServer-> numberOfLayers-1);++i){
            MLPActivation[i] = RLwSigmoidActivation ;
            MLPDerivative[i] = RLwSigmoidDerivative ;
          }
          /*  Main test Segment   */
          RLwEvalMLPFeedForward(pwMLPServer,sinput,numberOfInputs,output,
              numberOfOutputs, doScaleOutput, scaleFactor,inputsExponent);
          fprintf(stdout,"\nVerifying outputs ");
          fprintf(logStream,"\nVerifying outputs ");
          error = wPrintVectMismatch(logStream, message, output, MLoutput, numberOfOutputs,
                     maxPercentError, imaxError);
          /* if error print */
          if(error){
              fprintf(logStream, "\n*** %i Errors detected for this test vector. \n",error);
              fprintf(stdout,    "\n*** %i Errors detected for this test vector.\n",error);
                if (!err1) {
                    tstErrPrintStr (number);
                    tstErrPrintStr (number1);
                    tstErrPrintStr (number2);
                    err1 = 1;
                }
                tstErrwPrintVectMismatch(TRUE, number3, number4, output, MLoutput, numberOfOutputs,
                              maxPercentError, imaxError);
                sprintf(number,"\n*** %i Errors detected for this test vector. \n\n",error);
                tstErrPrintStr (number);

          }else{
              fprintf(logStream, "\n---RLwEvalMLPFeedForward tests Okay\n");
              fprintf(stdout,    "\n---RLwEvalMLPFeedForward tests Okay\n");
          }/* else */
        RLwFreeMLPServer(pwMLPServer);
free_tsts:
        FreewTestServer(&wMLPServerStruct);
close_all:
        totalErrors += error;
        if(err2){
             if (!err1) {
                 tstErrPrintStr (number);
                 tstErrPrintStr (number1);
                 tstErrPrintStr (number2);
                 err1 = 1;
             }
             tstErrPrintStr (number5);
             err2 =0;
          }
     }/* for all test vectors*/
     if (err1) {
            m_bTestAccuracyError = TRUE;
            sprintf(number,"\n*************** Total Errors, RLwEvalMLPFeedForward %i     ******************\n\n",totalErrors);
            tstErrPrintStr (number);
     }
     fprintf(logStream, "\n*************** Total Errors, RLwEvalMLPFeedForward %i     ******************",totalErrors);
     fprintf(stdout,    "\n*************** Total Errors, RLwEvalMLPFeedForward %i     ******************",totalErrors);
     testRLCloseFile(nStream);
} /* wMLPtest */

/**********************************************************************************/
void sMLPtest(FILE* logStream,char *datfile,int flags){
  /* function prototypes for reference */
    int  sPrintVectMismatch(FILE *stream, char *message, float *actual, float *expected, int vectLen,
            float max_percent_error,  float  max_error);
    int  wPrintVectMismatch(FILE *stream, char *message, short int *actual, short int *expected, int vectLen,
            float max_percent_error, int max_error);
    int  iPrintScalarMismatch(FILE *stream, char *message, int actual, int expected,
            double max_percent_error, int max_error);
  /* variables to store data from MATLAB generated file of test vectors */
    int         /*numTestVects,*/ vectNo, numberOfInputs, numberOfLayerInputs,
                numberOfWeights, numberOfLayers;
    int         *layerNeuronCounts; /* an array of ints that stores the number of neurons in each layer */
    float       *MLoutput;
    float       *weights;
  /* variables to store results from recognition library function */
    float       *output;
  /* miscellaneous variables  */
    //FILE        *stream;
    int         layerNumber;
    int         i, error = 0, err1 = 0, err2 = 0, totalErrors = 0;
    float       maxPercentError = (float).00005, smaxError = (float).000005;
    char        message[100] = {"RLsEvalMLPFeedForward"};
    char        message1[200];
    sMLPCallBack MLPActivation[MAXLAYERS-1],MLPDerivative[MAXLAYERS-1];
    char       *new_s_datfile="nmlpstst.dat";
    RPL_FILE* nStream;
#ifdef GEN_TEST
    FILE*       stream;
    int         numTestVects;
#endif
    RLSetStatus(RL_StsOk);

#ifdef GEN_TEST
    if ((stream = ownOpenData(datfile, "r")) == NULL){
        sprintf(message1, "***  Could not open MLPerceptron s data file %s\\%s\n", dataPath,datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
    }
    fprintf(stdout,    "\n\n--- Testing  RLsEvalMLPFeedForward - float int inputs and float int outputs\n");
        fprintf(logStream, "\n\n--- Testing  RLsEvalMLPFeedForward - float int inputs and float int outputs\n");
        fprintf(stdout,    "--- Data File %s\\%s\n", dataPath,datfile);
        fprintf(logStream, "--- Data File %s\\%s\n", dataPath,datfile);
        sprintf (number, "\n\n--- Testing  RLsEvalMLPFeedForward - float int inputs and float int outputs\n");
        sprintf (number1, "--- Data File %s\\%s\n", dataPath,datfile);
        sprintf (number2, "\n\n*** Errors in RLsEvalMLPFeedForward.\n Printing out mismatching Actual and Expected Values\n");

      fscanf(stream, "%i", &numTestVects);    /* read in the number of test vectors */
      testRLWriteBeginBody("title of header", "08/08/98", new_s_datfile);
      testRLWriteBeginTest("sMLPtest", "sMLPtest", "08/08/98", new_s_datfile);
      if(testRLiWriteScalar("numTestVects", numTestVects, new_s_datfile, NULL, NULL))
          return;
    for  (vectNo  = 1; vectNo  <= numTestVects;  vectNo++){
          fprintf(stdout,    "\n Test Vector Number %i ",vectNo);
          fprintf(logStream, "\n Test Vector Number %i ",vectNo);
          sprintf(number3, "\n Test Vector Number %i \n ",vectNo);
          fscanf(stream, "%i %i",&numberOfLayers, &numberOfInputs);  /* read in the number of layers and inputs */
          /* read in input vector */
          finput = (float*)calloc(numberOfInputs, sizeof(float));  /* Allocate memory for input vector */
          for (i = 0;  i  <  numberOfInputs;  i++)
              fscanf(stream,  "%g",  &finput[i]);
          fprintf(stdout,    "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);
          fprintf(logStream, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);
          sprintf (number4, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, datfile,numberOfLayers);

          /* read in the number of neurons in each layer into the array layerNeuronCounts */
          layerNeuronCounts = (int*)calloc(numberOfLayers, sizeof(int)); /* memory for layer neuron counts */
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++)
              fscanf(stream, "%i", &layerNeuronCounts[layerNumber]);

          /* read in the weights exponent */
          /* calculate total number of weights allocate memory, and read in weights */
          numberOfLayerInputs = numberOfInputs;
          numberOfWeights = 0;
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
          } /* for layerNumber */
          weights = (float*)calloc(numberOfWeights, sizeof(float));
          for (i = 0;  i  <  numberOfWeights;  i++) {
              fscanf(stream,  "%g", &weights[i]);
          } /* for i */

          /* Allocate memory and read in MatLab output vector */
          numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
          MLoutput = (float*)calloc(numberOfOutputs, sizeof(float));
          output = (float*)calloc(numberOfOutputs, sizeof(float));
          for (i = 0;  i  <  numberOfOutputs;  i++) {
              fscanf(stream,  "%g",  &MLoutput[i]);
          } /* for */
    if(testRLWriteBeginData("sMLPtest",new_s_datfile))
        return;
    if(testRLiWriteScalar("numberOfLayers", numberOfLayers, new_s_datfile, NULL, NULL))
        return;
    if(testRLiWriteScalar("numberOfInputs", numberOfInputs, new_s_datfile, NULL, NULL))
        return;
    if(testRLiWriteVector("layerNeuronCounts", layerNeuronCounts, numberOfLayers,
            new_s_datfile, NULL, NULL, NOT_USE_INDEX))
        return;
    if(testRLsWriteVector("finput", finput, numberOfInputs,new_s_datfile, NULL, NULL,
        NOT_USE_INDEX))
        return;
    if(testRLsWriteVector("weights", weights, numberOfWeights, new_s_datfile, "%g", NULL,
        NOT_USE_INDEX))
        return;
    /* generate empty output vector for allocation */
    if(testRLsWriteVector("output", NULL, numberOfOutputs, new_s_datfile, "%g", NULL,NOT_USE_INDEX)){
        return;
    }
    if(testRLsWriteVector("MLoutput", MLoutput, numberOfOutputs, new_s_datfile, "%g", NULL,
        NOT_USE_INDEX))
        return;
    if(testRLWriteFinishData(new_s_datfile))
        return;
    }/* for all test vectors*/
    if(testRLWriteFinishTest(new_s_datfile))
        return;
    if(testRLWriteFinishBody(new_s_datfile))
        return;
       free(finput);
       free(output);
       free(MLoutput);
       free(weights);
       free(layerNeuronCounts);
       fclose(stream);
#endif  /*generate*/

    nStream=testRLOpenFile(new_s_datfile, "sMLPtest", "08/08/98", TEST_RPL);
    if(nStream==NULL){
        sprintf(message1, "***  Could not open MLPerceptron s data file %s\\%s\n",
            dataPath,new_s_datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
    }
    for(vectNo = 1; testRLFindTagData(nStream)==RET_FOUND_TAG; vectNo++){
        fprintf(stdout,    "\n Test Vector Number   %i  ",vectNo);
        fprintf(logStream, "\n Test Vector Number   %i  ",vectNo);
        sprintf(number3, "\n Test Vector Number   %i \n ",vectNo);
        if(testRLiReadScalar(nStream,"numberOfLayers", &numberOfLayers ))
            return;
        if(testRLiReadScalar(nStream,"numberOfInputs", &numberOfInputs))
            return;
          /* read in input vector */
        /* read in the number of neurons in each layer into the array layerNeuronCounts */
        if(testRLiGetVector(nStream,"layerNeuronCounts", &layerNeuronCounts))
            return;
        if(testRLsGetVector(nStream,"finput", &finput))
            return;
        fprintf(stdout,    "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_s_datfile,numberOfLayers);
        fprintf(logStream, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_s_datfile,numberOfLayers);
        sprintf (number4, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_s_datfile,numberOfLayers);


          /* calculate total number of weights allocate memory, and read in weights */
        numberOfLayerInputs = numberOfInputs;
        numberOfWeights = 0;
        for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
        } /* for layerNumber */
        if(testRLsGetVector(nStream,"weights", &weights))
            goto close_all;

          /* Allocate memory and read in MatLab output vector */
        numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
        /*allocate memory for output*/
//        if(testRLsGetVector(nStream,"output", &output))
//            goto close_all;
        testRLsGetVector(nStream,"output", &output);
//        output = (float*)calloc(numberOfOutputs, sizeof(float));
        if(testRLsGetVector(nStream,"MLoutput", &MLoutput))
            goto close_all;
/*  Main test Segment   */
          if(InitsTestServer(&sMLPServerStruct,layerNeuronCounts,numberOfLayers,weights,
              FullConnected,numberOfInputs,flags)==FALSE){
              sprintf(number5, "\n*** FALSE code from InitsTestServer\n");
              err2=1;
              goto close_all;
          }
          if((psMLPServer = RLsInitMLPServer(sMLPServerStruct.pweights,
              sMLPServerStruct.pneuronInputLists,
              sMLPServerStruct.pneuronInputCounts,
              sMLPServerStruct.playerNeuronCounts, MLPActivation, MLPDerivative,
              sMLPServerStruct.numberOfLayers,
              sMLPServerStruct.flags,0))==NULL){
              fprintf(logStream, "\n*** Null pointer from RLsInitMLPServer\n");
              sprintf(number5, "\n*** Null pointer from RLsInitMLPServer\n");
              err2=1;
              goto free_tsts;
            }
            /*  Main test Segment   */
          for(i=0;i<(psMLPServer-> numberOfLayers-1);++i){
            MLPActivation[i] = RLsSigmoidActivation ;
            MLPDerivative[i] = RLsSigmoidDerivative ;
          }
          RLsEvalMLPFeedForward(psMLPServer,finput,numberOfInputs,output,numberOfOutputs);
          fprintf(logStream,"\nVerifying outputs ");
          error = sPrintVectMismatch(logStream, message, output, MLoutput, numberOfOutputs,
                             maxPercentError, smaxError);
          /* if error print */
          if(error){
              fprintf(logStream, "\n*** %i Errors detected for this test vector\n",error);
              fprintf(stdout,    "\n*** %i Errors detected for this test vector\n",error);
              if(!err1) {
                  tstErrPrintStr (number);
                  tstErrPrintStr (number1);
                  tstErrPrintStr (number2);
                  err1 = 1;
              }
              tstErrsPrintVectMismatch(TRUE, number3, number4, output, MLoutput, numberOfOutputs,
                              maxPercentError, smaxError);
              for (i = 0;  i  <  numberOfOutputs;  i++) {
                  /*A6 debug */
                  fprintf(logStream,"exceeded output[%i]= %g\n",i, MLoutput[i]);
                  fprintf(stdout,"exceeded output[%i]= %g\n",i, MLoutput[i]);
              } /* for */
              sprintf(number,"\n*** %i Errors detected for this test vector. \n\n",error);
              tstErrPrintStr (number);

          }else{
               fprintf(logStream, "\n---RLsEvalMLPFeedForward tests Okay\n");
               fprintf(stdout,    "\n---RLsEvalMLPFeedForward tests Okay\n");
          } /* else */
          RLsFreeMLPServer(psMLPServer);
free_tsts:
          FreesTestServer(&sMLPServerStruct);
close_all:
          if(err2){
              if (!err1) {
                   tstErrPrintStr (number);
                   tstErrPrintStr (number1);
                   tstErrPrintStr (number2);
                   err1 = 1;
               }
               tstErrPrintStr (number5);
               err2 =0;
          }
          totalErrors += error;
    } /* for all test vectors*/
    if(err1){
        m_bTestAccuracyError = TRUE;
        sprintf(number,"\n*************** Total Errors, RLsEvalMLPFeedForward %i     ******************\n\n",totalErrors);
        tstErrPrintStr (number);
    }
    fprintf(logStream, "\n*************** Total Errors, RLsEvalMLPFeedForward   %i     ******************",totalErrors);
    fprintf(stdout,    "\n*************** Total Errors, RLsEvalMLPFeedForward   %i     ******************",totalErrors);
    testRLCloseFile(nStream);
}  /* sMLPtest */

/*
// Performance Test
*/

static float  perfAccuracy=(float)5.0;

static double duration, prob_dur[PROB_NUMBER];
static int    num_reps = 20000, i_prob;

static void  *weights, *output;
static int    numberOfInputs;
static int    numberOfLayers;
static int    inputsExponent, weightsExponent;
static int   *layerNeuronCounts;
static int    doScaleOutput;
static int   *scaleFactor;

int ReRunRLwEvalMLPFeedForward(void){
    RLwEvalMLPFeedForward(pwMLPServer,sinput,numberOfInputs,output,
                numberOfOutputs, doScaleOutput, scaleFactor,inputsExponent);
    return 1;
}
/**********************************************************************************/
void wMoreMLPperf(FILE* logStream, int clockFreq, int flags) {
  /* function prototypes for reference */
    int  wPrintVectMismatch(FILE *stream, char *message, short int *actual, short int *expected, int vectLen,
            float max_percent_error, int max_error);
    int         numTestVects=14, vectNo;
    int         numberOfLayerInputs, numberOfWeights;
  /* miscellaneous variables  */
    int         layerNumber;
    short      *temp1Input, *temp2Input, *temp1Weights, *temp2Weights;
    int         neuronSum,i, error = 0, totalErrors = 0;
    double      timePerMultrplyAccumulate, connectionsPerSecond;
    char        wPar[]="numberOfInputs,numberOfLayers,numberOfWeights";

    int         Input_Vector_size[14] = {2,4,2,16,42,16,4,4,21,31,155, 64, 80,128};
    int         layerNumber_list[14]  = {2,2,3,3,2,2,4,4,1,2,3,2,2,2};
    int         layerNeuronCounts_list[34] = {20,26,14,34,25,40,13,44,14,31,1,
	                                         42,27,35,25,6,36,30,4,14,45,36,6,4,36, 
                                             184,165,16,128,64,140,80,84,64};
    int         *plist=layerNeuronCounts_list;

    int         Input_Exponent_List[14]   = {-14,-14,-14,-14,-14,-14,-14,-14,-14,-14,
                                             -14,-14,-14,-14};
    int         Weights_Exponent_List[14] = {-13,-13,-13,-13,-13,-13,-13,-13,-13,-13,
                                             -13,-13,-13,-13};
    int         doScaleOutput_List[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    int         scaleFactor_List[14]   = {2,2,2,2,2,2,2,2,2,2,2,2,2,2};

    wMLPCallBack MLPActivation[MAXLAYERS-1],MLPDerivative[MAXLAYERS-1];
    RLSetStatus(RL_StsOk);

        fprintf(stdout,    "\n\n--- Measuring RLwEvalMLPFeedForward performance: additional test for short ints\n");
        fprintf(logStream, "\n\n--- Measuring RLwEvalMLPFeedForward performance: additional test for short ints\n");

        tstCsvBeginData(wPar);

      for  (vectNo  = 0; vectNo  < numTestVects;  vectNo++) {
            fprintf(stdout,    "\n\n Test Vector Number   %i  ",vectNo+1);
            fprintf(logStream, "\n\n Test Vector Number   %i  ",vectNo+1);
        numberOfLayers = layerNumber_list[vectNo];
        numberOfInputs = Input_Vector_size[vectNo];
        inputsExponent =  Input_Exponent_List[vectNo];
        /* read in input vector */
        temp1Input = (short*)calloc(numberOfInputs+8, sizeof(short));  /* Allocate memory for input vector */
        temp2Input = temp1Input;
        sinput = tRL_Align8(temp1Input, 0);
        for (i = 0;  i  <  numberOfInputs;  i++)
             sinput[i] = 0;
        fprintf(stdout,    "\n\n    Input Vector size %4d, Number of Layers %d\n",
                    numberOfInputs, numberOfLayers);
        fprintf(logStream, "\n\n    Input Vector size %4d, Number of Layers %d\n",
                        numberOfInputs, numberOfLayers);

          /* read in the number of neurons in each layer into the array layerNeuronCounts */
        layerNeuronCounts = (int*)calloc(numberOfLayers, sizeof(int)); /* memory for layer neuron counts */
        for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++){
            layerNeuronCounts[layerNumber] = plist[layerNumber];
        }
        plist += layerNumber_list[vectNo];
        weightsExponent =  Weights_Exponent_List[vectNo];
        /* calculate total number of weights, allocate memory, and read in weights */
        numberOfLayerInputs = numberOfInputs;
        numberOfWeights = 0;
        for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
             numberOfWeights += numberOfLayerInputs*layerNeuronCounts[layerNumber];
             numberOfLayerInputs = layerNeuronCounts[layerNumber]; /* outputs of one layer become inputs to next */
        } /* for layerNumber */
        temp1Weights = (short*)calloc(numberOfWeights+8, sizeof(short));
        temp2Weights = temp1Weights;
        weights = tRL_Align8(temp1Weights, 0);
        for (i = 0;  i  <  numberOfWeights;  i++) {
             ((short*)weights)[i] = 0;
        } /* for i */

        doScaleOutput =  doScaleOutput_List[vectNo];
        scaleFactor = (int*)calloc(1, sizeof(int));
        *scaleFactor =  scaleFactor_List[vectNo];
        /* Allocate memory and read in MatLab output vector */
        numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
        output = (short*)calloc(numberOfOutputs, sizeof(short));
          if(InitwTestServer(&wMLPServerStruct,layerNeuronCounts,numberOfLayers,weights,
              FullConnected,numberOfInputs,flags)==FALSE)
              goto free_mem;
          if((pwMLPServer = RLwInitMLPServer(wMLPServerStruct.pweights,
              wMLPServerStruct.pneuronInputLists,
              wMLPServerStruct.pneuronInputCounts,
              wMLPServerStruct.playerNeuronCounts, MLPActivation, MLPDerivative,
              wMLPServerStruct.numberOfLayers,
              wMLPServerStruct.flags,
              weightsExponent,0))==NULL){
                  fprintf(logStream, "\n*** Null pointer from RLwInitMLPServer\n");
                  goto free_tsts;
        }
          for(i=0;i<(pwMLPServer-> numberOfLayers-1);++i){
            MLPActivation[i] = RLwSigmoidActivation ;
            MLPDerivative[i] = RLwSigmoidDerivative ;
          }
        /*  Main test Segment   */
        RLwEvalMLPFeedForward(pwMLPServer,sinput,numberOfInputs,output,
                numberOfOutputs, doScaleOutput, scaleFactor,inputsExponent);

/*  Performance test Segment   */

        /* Print the network architecture */
        fprintf(stdout,    "\nNeural Network Architecture = %3d ", numberOfInputs);
        fprintf(logStream, "\nNeural Network Architecture = %3d ", numberOfInputs);
        for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
             fprintf(stdout,   "x %3d ", *(layerNeuronCounts + layerNumber));
             fprintf(logStream,"x %3d ", *(layerNeuronCounts + layerNumber));
        } /* for */
        fprintf(stdout,    "\nTotal Weights in the network = %i\n", numberOfWeights);
        fprintf(logStream, "\nTotal Weights in the network = %i\n", numberOfWeights);

        num_reps = tstTimingLoop(perfAccuracy, &ReRunRLwEvalMLPFeedForward, 10);
        T_BEGIN
        for (i = 0; i < num_reps; i++) {
            RLwEvalMLPFeedForward(pwMLPServer,sinput,numberOfInputs,output,
                numberOfOutputs, doScaleOutput, scaleFactor,inputsExponent);
        } /* for */
        T_END
            for(neuronSum = i = 0; i < numberOfLayers; i++) {
                neuronSum += layerNeuronCounts[i];
            }
            sprintf (number4, "%6.2f", ((duration/num_reps)*1000000.0 * clockFreq)/neuronSum);
            sprintf (number, "%.3lg", duration * 1000000.0 / (double)num_reps);
            tstCsvPutData("w","EvalMLPFeedForward",
                           number, "us",
                           number4, "cp_neuron",
                           "",
                          "numberOfInputs",  itoa(numberOfInputs,number1,10),
                          "numberOfLayers",  itoa(numberOfLayers,number2,10),
                          "numberOfWeights",  itoa(numberOfWeights,number3,10),
                          NULL);
         /* Calculating and outputing Performance Metrics */
        fprintf(stdout,    "\n--- Microseconds to execute one time = %6d", (int)((duration/num_reps)*1000000.0));

        timePerMultrplyAccumulate = duration/(num_reps*numberOfWeights);
        connectionsPerSecond = 1/timePerMultrplyAccumulate;

        fprintf(stdout,   "\n--- Average Connections per second   = %g ", connectionsPerSecond);
        fprintf(logStream,"\n--- Average Connections per second   = %g ", connectionsPerSecond);
         fprintf(stdout,   "\n--- Clocks per Connection = %g Clocks\n", clockFreq, timePerMultrplyAccumulate * clockFreq * 1000000);
        fprintf(logStream,"\n--- Clocks per Connection = %g Clocks\n", clockFreq, timePerMultrplyAccumulate * clockFreq * 1000000);

        RLwFreeMLPServer(pwMLPServer);
free_tsts:
        FreewTestServer(&wMLPServerStruct);
free_mem:
        free(temp2Input);
        free(output);
        free(temp2Weights);
        free(layerNeuronCounts);
        free(scaleFactor);
        totalErrors += error;
         } /* for all test vectors*/
        fprintf(logStream, "\n*************** Total Errors, RLwEvalMLPFeedForward  %i     ******************\n",totalErrors);
        fprintf(stdout,    "\n*************** Total Errors, RLwEvalMLPFeedForward  %i     ******************\n",totalErrors);
        tstCsvEndData();
}  /* wMoreMLPperf */


int ReRunRLsEvalMLPFeedForward(void) {
    RLsEvalMLPFeedForward(psMLPServer,finput,numberOfInputs,output,numberOfOutputs);
    return 1;
}
int weightsCompare(float *src1,float *src2,int len) {
  int i;
    for (i=0;i<len;++i){
        if(src1[i] != src2[i])
            return(i);
    }
    return 0;
}
void sMoreMLPperf(FILE* logStream, int clockFreq,int flags) {
    int         numTestVects=14, vectNo, numberOfLayerInputs, numberOfWeights;
    int         layerNumber;
    int         neuronSum,i, error=0, totalErrors = 0;
    double      timePerMultrplyAccumulate, connectionsPerSecond;
    char        sPar[]="numberOfInputs,numberOfLayers,numberOfWeights";

    int         Input_Vector_size[14]={2,47,21,19,35,22,37,1,31,28,155,64,80,128};
    int         layerNumber_list[14] = {2,2,1, 3,3, 2,5, 5,4,1,3,2,2,2};
    int         layerNeuronCounts_list[37] = {47,20,21,45,28,2,44,34,39,34,18,22,5,49,
                                              19,22,35,47,33,37,23,40,1,38,7,8,7,11,184,
                                              165,16,128,64,140,80,84,64};
    int         *plist=layerNeuronCounts_list;
    sMLPCallBack MLPActivation[MAXLAYERS-1],MLPDerivative[MAXLAYERS-1];
    RLSetStatus(RL_StsOk);

        fprintf(stdout,    "\n\n--- Measuring RLsEvalMLPFeedForward performance: additional test for floats\n");
        fprintf(logStream, "\n\n--- Measuring RLsEvalMLPFeedForward performance: additional test for floats\n");
        tstCsvBeginData(sPar);
      for  (vectNo  = 0; vectNo  < numTestVects;  vectNo++) {
            fprintf(stdout,    "\n\n Test Vector Number   %i  ",vectNo+1);
            fprintf(logStream, "\n\n Test Vector Number   %i  ",vectNo+1);
            numberOfLayers = layerNumber_list[vectNo];
            numberOfInputs = Input_Vector_size[vectNo];
            finput = (float*)calloc(numberOfInputs, sizeof(float));  /* Allocate memory for input vector */

          /* read in input vector */
          for (i = 0;  i  <  numberOfInputs;  i++)
              finput[i]=(float)0.;
          fprintf(stdout,    "\n\n    Input Vector size %4d, Number of Layers %d\n",
                        numberOfInputs, numberOfLayers);
          fprintf(logStream, "\n\n    Input Vector size %4d, Number of Layers %d\n",
                        numberOfInputs, numberOfLayers);

          /* read in the number of neurons in each layer into the array numberOfNeurons */
          layerNeuronCounts = (int*)calloc(numberOfLayers, sizeof(int)); /* memory for layer neuron counts */
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++){
              layerNeuronCounts[layerNumber] = plist[layerNumber];
          }
          plist += layerNumber_list[vectNo];
          /* calculate total number of weights, allocate memory, and read in weights */
          numberOfLayerInputs = numberOfInputs;
          numberOfWeights = 0;
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
          } /* for layerNumber */
          weights = (float*)calloc(numberOfWeights, sizeof(float));
          for (i = 0;  i  <  numberOfWeights;  i++) {
              ((float*)weights)[i] = (float)0.;
          } /* for i */

          /* Allocate memory and read in MatLab output vector */
          numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
          output = (float*)calloc(numberOfOutputs, sizeof(float));
/*  Main test Segment   */
          if(InitsTestServer(&sMLPServerStruct,layerNeuronCounts,numberOfLayers,weights,
              FullConnected,numberOfInputs,flags)==FALSE)
              goto free_mem;
          if((psMLPServer = RLsInitMLPServer(sMLPServerStruct.pweights,
              sMLPServerStruct.pneuronInputLists,
              sMLPServerStruct.pneuronInputCounts,
              sMLPServerStruct.playerNeuronCounts, MLPActivation, MLPDerivative,
              sMLPServerStruct.numberOfLayers,
              sMLPServerStruct.flags,0))==NULL){
                  fprintf(logStream, "\n*** Null pointer from RLsInitMLPServer\n");
                  goto free_tsts;
            }
          for(i=0;i<(psMLPServer-> numberOfLayers-1);++i){
            MLPActivation[i] = RLsSigmoidActivation ;
            MLPDerivative[i] = RLsSigmoidDerivative ;
          }

/*  Performance test Segment   */

            /* Print the network architecture */
            fprintf(stdout,    "\nNeural Network Architecture = %3d ", numberOfInputs);
            fprintf(logStream, "\nNeural Network Architecture = %3d ", numberOfInputs);
            for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
                fprintf(stdout,   "x %3d ", *(layerNeuronCounts + layerNumber));
                fprintf(logStream,"x %3d ", *(layerNeuronCounts + layerNumber));
            } /* for */
             fprintf(stdout,    "\nTotal Weights in the network = %i\n", numberOfWeights);
             fprintf(logStream, "\nTotal Weights in the network = %i\n", numberOfWeights);

            num_reps = tstTimingLoop(perfAccuracy, &ReRunRLsEvalMLPFeedForward, 10);
            T_BEGIN
            for(i = 0; i < num_reps; i++) {
                RLsEvalMLPFeedForward(psMLPServer,finput,numberOfInputs,
                    output,numberOfOutputs);
            } /* for */
            T_END
            for(neuronSum = i = 0; i < numberOfLayers; i++) {
                neuronSum += layerNeuronCounts[i];
            }
            sprintf (number4, "%6.2f", ((duration/num_reps)*1000000.0 * clockFreq)/neuronSum);
            sprintf (number, "%.3lg", duration * 1000000.0 / (double)num_reps);
            tstCsvPutData("s","EvalMLPFeedForward",
                          number, "us",
                          number4,"cp_neuron",
                          "",
                          "numberOfInputs",  itoa(numberOfInputs,number1,10),
                          "numberOfLayers",  itoa(numberOfLayers,number2,10),
                          "numberOfWeights",  itoa(numberOfWeights,number3,10),
                          NULL);
             /* Calculating and outputing Performance Metrics */

            fprintf(stdout,    "\n--- Microseconds to execute one time = %6d", (int)((duration/num_reps)*1000000.0));

            timePerMultrplyAccumulate = duration/(num_reps*numberOfWeights);
            connectionsPerSecond = 1/timePerMultrplyAccumulate;

            fprintf(stdout,   "\n--- Average Connections per second   = %g ", connectionsPerSecond);
            fprintf(logStream,"\n--- Average Connections per second   = %g ", connectionsPerSecond);
            fprintf(stdout,  "\n--- Clocks per Connection = %g Clocks\n", clockFreq, timePerMultrplyAccumulate * clockFreq * 1000000);
            fprintf(logStream,"\n--- Clocks per Connection = %g Clocks\n", clockFreq, timePerMultrplyAccumulate * clockFreq * 1000000);

            RLsFreeMLPServer(psMLPServer);
free_tsts:
            FreesTestServer(&sMLPServerStruct);
free_mem:
            free(finput);
            free(output);
            free(weights);
            free(layerNeuronCounts);
            totalErrors += error;
         } /* for all test vectors*/
        fprintf(logStream, "\n*************** Total Errors RLsEvalMLPFeedForward  %i     ******************\n",totalErrors);
        fprintf(stdout,    "\n*************** Total Errors RLsEvalMLPFeedForward  %i     ******************\n",totalErrors);
        tstCsvEndData();

} /* sMoreMLPperf */

/***********************************************************************************/
//
//          The test of function RLsSigmoidActivation on regularity of work.
//
//          Author: Vladimir Truschin,
//          modified for new data api by Ryzhachkin I.
//
/***********************************************************************************/

#define PCSERR  (float)0.0001
#define ABSSERR (float)0.0001

void sSigmActivTest(FILE *logStream) {
//    FILE      *stream;
    int        /*numTestVects,*/ vectNo, sizeSampl, error;
    int        i;
    float     *fsample,*fdst;
    char      *new_sigma_datfile="nssigma.dat";
    RPL_FILE  *nStream;
    char       message1[200];
#ifdef GEN_TEST
    FILE*       stream;
    int         numTestVects;
    float     **sampl,**dst;
#endif
    RLSetStatus(RL_StsOk);

#ifdef GEN_TEST
    if ((stream = ownOpenData("ssigma.dat", "r")) == NULL) {
        printf("***  Could not open samples data file %s\\ssigma.dat\n", dataPath);
        fprintf(logStream, "***  Could not open samples data file %s\\ssigma.dat\n", dataPath);
        return;
    }
    fscanf(stream, "%d", &numTestVects);
    testRLWriteBeginBody("title of header", "08/08/98", new_sigma_datfile);
    testRLWriteBeginTest("sSigmActivTest", "sSigmActivTest", "08/08/98", new_sigma_datfile);
    if(testRLiWriteScalar("numTestVects", numTestVects, new_sigma_datfile, NULL, NULL))
        return;
    for  (vectNo  = 1; vectNo  <= numTestVects;  vectNo++) {
        fscanf(stream, "%d", &sizeSampl);
        sAllocateTable(&sampl, 1, sizeSampl);
        sReadTable(stream, sampl, 1, sizeSampl);
        if(testRLWriteBeginData("sSigmActivTest",new_sigma_datfile))
            return;
        if(testRLiWriteScalar("sizeSampl", sizeSampl, new_sigma_datfile, NULL, NULL))
            return;
        if(testRLsWriteVector("sampl", sampl[0], sizeSampl,new_sigma_datfile," %g", NULL,
            NOT_USE_INDEX))
            return;
        /* NULL size write for memory allocation */
        if(testRLsWriteVector("dst", NULL, sizeSampl,new_sigma_datfile, NULL, NULL,
            NOT_USE_INDEX))
            return;
        if(testRLWriteFinishData(new_sigma_datfile))
            return;

        sDeallocateTable(sampl, 1, sizeSampl);
    } /* for */
    if(testRLWriteFinishTest(new_sigma_datfile))
        return;
    if(testRLWriteFinishBody(new_sigma_datfile))
        return;

    fclose(stream);     
#endif

    nStream=testRLOpenFile(new_sigma_datfile, "sSigmActivTest", "08/08/98", TEST_RPL);
    if(nStream==NULL){
        sprintf(message1, "***  Could not open RLsSigmoidActivation data file %s\\%s\n",
            dataPath,new_sigma_datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
    }
    error = 0;
    printf("\n\n--- Testing  function RLsSigmoidActivation - float data\n");
    printf("--- Data File %s\\%s\n", dataPath,new_sigma_datfile);
    fprintf(logStream, "\n\n--- Testing function RLsSigmoidActivation - float data\n");
    fprintf(logStream, "--- Data File %s\\%s\n", dataPath,new_sigma_datfile);
    sprintf (number, "\n*** Errors in RLsSigmoidActivation().\n");
    for(vectNo = 1; testRLFindTagData(nStream)==RET_FOUND_TAG; vectNo++){
        if(testRLiReadScalar(nStream,"sizeSampl", &sizeSampl))
            return;
        fprintf(logStream, "\n--- Test #%2d, Length sample vector %4d.",
                vectNo, sizeSampl);
        sprintf(number1, "\n*** Errors in test #%2d, Length sample vector %4d.",
                vectNo, sizeSampl);
        if(testRLsGetVector(nStream,"sampl", &fsample))
            return;
        testRLsGetVector(nStream,"dst", &fdst);
        for (i = 0; i < sizeSampl; i++) {
            fdst[i] = (float)(1.0f/(1.0f + exp(-fsample[i])));
        }
        RLsSigmoidActivation(fsample, fsample, sizeSampl);
        sprintf(number2,"%s\n*** Bad value results.",number1);
        if  (sPrintVectMismatch(logStream, number2,
            fsample, fdst, sizeSampl, PCSERR, ABSSERR)) {
            tstErrsPrintVectMismatch(!error, number, number2,
                fsample, fdst, sizeSampl, PCSERR, ABSSERR);
            error = 1;
        }
        fprintf(logStream, "\n--- End of test #%2d, Length sample vector %4d.\n",
                vectNo, sizeSampl);
    } /* for */

    if (error) {
        m_bTestAccuracyError = TRUE;
        fprintf(logStream, "\n\n*** Errors found in function RLsSigmoidActivation. Reported in Log file.\n");
        printf("\n*** Errors found in function RLsSigmoidActivation. Reported in Log file.\n");
    } else {
        fprintf(logStream, "\n\n--- Test of function RLsSigmoidActivation Okay\n\n");
        printf("\n--- Test of function RLsSigmoidActivation Okay\n\n");
    } /* if */
    testRLCloseFile(nStream);
} /* End of the test of function RLsSigmoidActivation on regularity of work */
/***********************************************************************************/
//
//      The test of function RLsSigmoidActivation on performancy of work.
//
//      Author: Vladimir Truschin
//
/***********************************************************************************/

static float    sigma_perfAccuracy=(float)5.0;
static double   duration, prob_dur[PROB_NUMBER];
static int      sigma_num_reps = 20000, i_prob;
static int      sizeSampl;
#define SIGMA_MAX_LEN 448
//__declspec(align(16)) static float  sampl[SIGMA_MAX_LEN];
static float  sampl[SIGMA_MAX_LEN];

int ReRunRLsSigmoidActivation(void) {
    RLsSigmoidActivation(sampl, sampl, sizeSampl);
    return 1;
}

void sSigmActivPerf(FILE *logStream, int clockFreq) {
//    FILE   *stream;
  int     numTst=6, tstNo, perMS, i;
  char    sPar[]="sizeVector";
  int     lsizeSampl[]={4,10,40,140,420,SIGMA_MAX_LEN};
    RLSetStatus(RL_StsOk);
    printf("\n\n--- Performance Testing Function RLsSigmoidActivation - float data\n");
    fprintf(logStream, "\n\n--- Performance Testing Function RLsSigmoidActivation - float data\n");

    for(i=0;i<SIGMA_MAX_LEN;++i){
        sampl[i] = (float)(((float)trplRand(-(int)(INIT_WEIGHTS_INTERVAL * MAX16U),

            (int)(INIT_WEIGHTS_INTERVAL * MAX16U)))/(float)MAX16U);
    }
    tstCsvBeginData(sPar);
    for(tstNo  = 1; tstNo  <= numTst;  tstNo++) {
        sizeSampl = lsizeSampl[tstNo-1];
        fprintf(logStream, "\n--- Test #%2d, Length sample vector %4d.\n",
                        tstNo, sizeSampl);

        sigma_num_reps = tstTimingLoop(sigma_perfAccuracy, &ReRunRLsSigmoidActivation, 10);
        T_BEGIN
            for(i = 0; i < sigma_num_reps; i++)
                RLsSigmoidActivation(sampl, sampl, sizeSampl);
        T_END
        sprintf (number, "%.3lg", duration * 1000000.0 / (double)sigma_num_reps);
        sprintf (number3, "%6.2lf", ((duration/sigma_num_reps)*1000000.0*(double)clockFreq)/sizeSampl);
        tstCsvPutData("s","SigmoidActivation",
                           number, "us",
                           number3, "cpe", "",
                          "sizeVector", itoa(sizeSampl,number2,10),
                           NULL);
        perMS = (int)(sigma_num_reps/(duration * 1000));
        if(perMS != 0)
            fprintf(logStream, "--- Function RLsSigmoidActivation per Millsec = %6d\n",perMS);
        else{
            perMS = (int)(sigma_num_reps/duration);
            fprintf(logStream, "--- Function RLsSigmoidActivation per Sec     = %6d\n",perMS);
        } /* if */
    } /* for */
    tstCsvEndData();
} /* End of the test of function RLsSigmoidActivation on performancy of work */
int ReRunRLsTrainMLPBackPropagation(void){
    RLsTrainMLPBackPropagation(psMLPServer, pinput, EpochLen, numberOfInputs,
        poutput, numberOfOutputs, eta, nStep, errThreshold, momentum, &epochNum);
    return 1;
}
void sMLPTrainPerf(FILE* logStream, int clockFreq,int flags) {
  /* function prototypes for reference */
    int  sPrintVectMismatch(FILE *stream, char *message, float *actual, float *expected, int vectLen,
                            float max_percent_error,  float  max_zero_error);
  /* variables to store data from MATLAB generated file of test vectors */
    int         numTestVects, vectNo, numberOfLayerInputs, numberOfWeights;
    float      *weights,*MLoutput;
  /* miscellaneous variables  */
    FILE       *stream;
    int         layerNumber, neuronSum,i,t, error, totalErrors = 0,
                k, *layerNeuronCounts;
    double      timePerMultrplyAccumulate, connectionsPerSecond;
    char        sPar[]="numberOfInputs,numberOfLayers,numberOfWeights";
    float      *output;

    sMLPCallBack MLPActivation[MAXLAYERS-1],MLPDerivative[MAXLAYERS-1];
    EpochLen = 1;
    nStep = 1;
    eta = (float)LERNING_RATE;
    errThreshold = (float)1.0e-11;
    momentum = (float)MOMENTUM;
    RLSetStatus(RL_StsOk);
    if((stream = ownOpenData("mlpstest.dat", "r")) == NULL) {
        printf("***  Could not open MLPerceptron s data file %s\\mlpstest.dat\n", dataPath);
        fprintf(logStream, "***  Could not open MLPerceptron data file %s\\mlpstest.dat\n", dataPath);
     }else {     /* go ahead and test */
        fprintf(stdout,    "\n\n--- Measuring RLsTrainMLPBackPropagation performance - float inputs and outputs\n");
        fprintf(logStream, "\n\n--- Measuring RLsTrainMLPBackPropagation performance - float inputs and outputs\n");
        fprintf(logStream, "--- Data File %s\\mlpstest.dat\n", dataPath);
        fprintf(stdout,    "--- Data File %s\\mlpstest.dat\n", dataPath);

        tstCsvBeginData(sPar);

        fscanf(stream, "%i", &numTestVects);    /* read in the number of test vectors */
        for(vectNo  = 1; vectNo  <= numTestVects;  vectNo++) {
            fprintf(stdout,    "\n\n Test Vector Number   %i  ",vectNo);
            fprintf(logStream, "\n\n Test Vector Number   %i  ",vectNo);
            fscanf(stream, "%i %i",&numberOfLayers, &numberOfInputs);
            finput = (float*)tstmAlloc(numberOfInputs*EpochLen * sizeof(float));
            if(finput == NULL){
                fprintf(stdout,   "\n--- Out of memory for input");
                fprintf(logStream,   "\n--- Out of memory for input");
                goto sum_err;
            };
            /* read in input vector */
            for(i = 0;  i  <  numberOfInputs;  i++) {
                fscanf(stream,  "%g",  &finput[i]);
            }
          fprintf(stdout,    "\n\n    Input Vector size %4d, Data File mlpstest.dat, Number of Layers %d\n",
                        numberOfInputs, numberOfLayers);
          fprintf(logStream, "\n\n    Input Vector size %4d, Data File mlpstest.dat, Number of Layers %d\n",
                        numberOfInputs, numberOfLayers);

          /* read in the number of neurons in each layer into the array numberOfNeurons */
          layerNeuronCounts = (int*)calloc(numberOfLayers, sizeof(int)); /* memory for layer neuron counts */
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++)
              fscanf(stream, "%i", &layerNeuronCounts[layerNumber]);

          /* calculate total number of weights, allocate memory, and read in weights */
          numberOfLayerInputs = numberOfInputs;
          numberOfWeights = 0;
          for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
          } /* for layerNumber */
          weights = (float*)calloc(numberOfWeights, sizeof(float));
          for (i = 0;  i  <  numberOfWeights;  i++) {
              fscanf(stream,  "%g", &(((float*)weights)[i]));
          } /* for i */

          /* Allocate memory and read in MatLab output vector */
          numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
          MLoutput = (float*)calloc(numberOfOutputs, sizeof(float));
          output = (float*)calloc(numberOfOutputs, sizeof(float));
          for (i = 0;  i  <  numberOfOutputs;  i++) {
              fscanf(stream,  "%g",  &MLoutput[i]);
          } /* for */
    /* Allocate memory for input vector */
    pinput = (float**)tstmAlloc(EpochLen*sizeof(float*));
    /* Allocate memory for input vector */
    if(pinput==NULL){
        free(weights);
        fprintf(stdout, str_outofmem);
        fprintf(logStream, str_outofmem);
        sprintf(number4, str_outofmem);
        goto sum_err;
    }
    for(i = 0; i < EpochLen; i++){
        pinput[i] = &finput[i*numberOfInputs];
    }
    poutput = (float**)tstmAlloc(EpochLen * sizeof(float*));  /* Allocate memory for input vector */
    if(poutput==NULL){
        free(weights);
        free(finput);
        free(pinput);
        fprintf(stdout, str_outofmem);
        fprintf(logStream, str_outofmem);
        sprintf(number4, str_outofmem);
        goto sum_err;
    }
    /* read in training  epoch vectors */
    for(i = 0; i < EpochLen; i++){
        poutput[i] = &output[i*numberOfOutputs];
        for(k = 0;  k < numberOfOutputs;  k++){
            output[i*numberOfOutputs+k]=(float)0.1;
        }
    }
/*  Main test Segment   */
          if(InitsTestServer(&sMLPServerStruct,layerNeuronCounts,numberOfLayers,weights,
              FullConnected,numberOfInputs,flags)==FALSE)
              goto free_mem;
          if((psMLPServer = RLsInitMLPServer(sMLPServerStruct.pweights,
              sMLPServerStruct.pneuronInputLists,
              sMLPServerStruct.pneuronInputCounts,
              sMLPServerStruct.playerNeuronCounts, MLPActivation, MLPDerivative,
              sMLPServerStruct.numberOfLayers,
              sMLPServerStruct.flags,0))==NULL){
                  fprintf(logStream, "\n*** Null pointer from RLsInitMLPServer\n");
                  goto free_tsts;
            }
          for(i=0;i<(psMLPServer-> numberOfLayers-1);++i){
            MLPActivation[i] = RLsSigmoidActivation ;
            MLPDerivative[i] = RLsSigmoidDerivative ;
          }
/*  Performance test Segment   */

            fprintf(stdout,   "\nMeasuring RLsTrainMLPBackPropagation performance; floats");
            fprintf(logStream,"\nMeasuring RLsTrainMLPBackPropagation performance; floats");

            /* Print the network architecture */
            fprintf(stdout,    "\nNeural Network Architecture = %3d ", numberOfInputs);
            fprintf(logStream, "\nNeural Network Architecture = %3d ", numberOfInputs);
            for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
                fprintf(stdout,   "x %3d ", *(layerNeuronCounts + layerNumber));
                fprintf(logStream,"x %3d ", *(layerNeuronCounts + layerNumber));
            } /* for */
             fprintf(stdout,    "\nTotal Weights in the network = %i\n", numberOfWeights);
             fprintf(logStream, "\nTotal Weights in the network = %i\n", numberOfWeights);

            num_reps = tstTimingLoop(perfAccuracy, &ReRunRLsTrainMLPBackPropagation, 10);
            T_BEGIN
            for(t = 0; t < num_reps; ++t){
                RLsTrainMLPBackPropagation(psMLPServer, pinput, EpochLen, numberOfInputs,
                    poutput, numberOfOutputs, eta, nStep, errThreshold, momentum, &epochNum);
            } /* for */
            T_END
            if(RLGetStatus() != RL_StsOk){
                sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
                goto free_serv;
            }
            fprintf(logStream, "\n*** errThreshold = %g epochN = %i nStep = %i \n",errThreshold,
                epochNum,nStep);
            fprintf(stdout,    "\n***  errThreshold = %g epochN = %i nStep = %i \n",errThreshold,
                epochNum,nStep);
            sprintf (number4, "\n***  errThreshold = %g epochN = %i nStep = %i \n",errThreshold,
                epochNum,nStep);
            for(neuronSum = i = 0; i < numberOfLayers; i++){
                neuronSum += layerNeuronCounts[i];
            }
            sprintf (number4, "%6.2f", ((duration/num_reps)*1000000.0 * clockFreq)/neuronSum);
            sprintf (number, "%.3lg", duration * 1000000.0 / (double)num_reps);
            tstCsvPutData("s","RLsTrainMLPBackPropagation",
                          number, "us",
                          number4,"cp_neuron",
                          "",
                          "numberOfInputs",  itoa(numberOfInputs,number1,10),
                          "numberOfLayers",  itoa(numberOfLayers,number2,10),
                          "numberOfWeights",  itoa(numberOfWeights,number3,10),
                          NULL);
             /* Calculating and outputing Performance Metrics */
            fprintf(stdout,    "\n--- Microseconds to execute one time = %6d", (int)((duration/num_reps)*1000000.0));

            timePerMultrplyAccumulate = duration/(num_reps*numberOfWeights);
            connectionsPerSecond = 1/timePerMultrplyAccumulate;

            fprintf(stdout,   "\n--- Average Connections per second   = %g ", connectionsPerSecond);
            fprintf(logStream,"\n--- Average Connections per second   = %g ", connectionsPerSecond);
            fprintf(stdout,   "\n--- Clocks per Connection = %g Clocks\n", clockFreq, timePerMultrplyAccumulate * clockFreq * 1000000);
            fprintf(logStream,"\n--- Clocks per Connection = %g Clocks\n", clockFreq, timePerMultrplyAccumulate * clockFreq * 1000000);

            fprintf(logStream, "\n--- RLsTrainMLPBackPropagation tests Okay");
            fprintf(stdout,    "\n--- RLsTrainMLPBackPropagation tests Okay");
free_serv:
            RLsFreeMLPServer(psMLPServer);
free_tsts:
            FreesTestServer(&sMLPServerStruct);
free_mem:
            free(finput);
            free(output);
    free(pinput);
    free(poutput);
    free(MLoutput);
            free(weights);
            free(layerNeuronCounts);
sum_err:
            totalErrors += error;
         } /* for all test vectors*/
        fprintf(logStream, "\n*************** Total Errors RLsTrainMLPBackPropagation  %i     ******************\n",totalErrors);
        fprintf(stdout,    "\n*************** Total Errors RLsTrainMLPBackPropagation  %i     ******************\n",totalErrors);
        fclose(stream);
        tstCsvEndData();
    } /* else */

}  /* sMLPTrainPerf */
void wMLPReadWriteServerTest(FILE* logStream,char * new_w_datfile,int flags) {
  /* function prototypes for reference */
    int  sPrintVectMismatch(FILE *stream, char *message, float *actual, float *expected, int vectLen,
            float max_percent_error,  float  max_error);
    int  wPrintVectMismatch(FILE *stream, char *message, short int *actual, short int *expected, int vectLen,
            float max_percent_error, int max_error);
    int  iPrintScalarMismatch(FILE *stream, char *message, int actual, int expected,
            double max_percent_error, int max_error);
  /* variables to store data from MATLAB generated file of test vectors */
    int         /*numTestVects,*/ vectNo, numberOfInputs, inputsExponent, numberOfLayerInputs,
                weightsExponent, numberOfWeights, numberOfLayers;
    int         doScaleOutput,scaleF;
    int*        layerNeuronCounts;
    short*      MLoutput;
    short*      weights;
  /* variables to store results from recognition library function */
    short*      output;
  /* miscellaneous variables  */
    //FILE*       stream;
    int         layerNumber;
    int         error = 0, err1 = 0, err2 = 0,totalErrors = 0, imaxError = 8;
    float       maxPercentError = (float)1.0;
    char        message[100]= {"RLwWriteMLPServer,RLwReadMLPServer"};
    char        message1[200];
    RPL_FILE   *nStream;
    int*        scaleFactor=&scaleF;
	FILE       *out;
    wMLPServer *pwMLPServer2;
    RLSetStatus(RL_StsOk);

    nStream=testRLOpenFile(new_w_datfile, "wMLPtest", "08/08/98", TEST_RPL);
    if(nStream==NULL){
        sprintf(message1, "***  Could not open MLPerceptron w data file %s\\%s\n",
            dataPath,new_w_datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
    }
    fprintf(stdout,    "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
    fprintf(logStream, "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
    fprintf(stdout,    "--- Data File %s\\%s\n", dataPath,new_w_datfile);
    fprintf(logStream, "--- Data File %s\\%s\n", dataPath,new_w_datfile);
    sprintf (number, "\n\n--- Testing  RLwEvalMLPFeedForward - short int inputs and short int outputs\n");
    sprintf (number1, "--- Data File %s\\%s\n", dataPath,new_w_datfile);
    sprintf (number2, "\n\n*** Errors in RLwEvalMLPFeedForward.\n Printing out mismatching Actual and Expected Values\n");

    for(vectNo = 1; testRLFindTagData(nStream)==RET_FOUND_TAG; vectNo++){
        fprintf(stdout,    "\n Test Vector Number   %i  ",vectNo);
        fprintf(logStream, "\n Test Vector Number   %i  ",vectNo);
        sprintf(number3, "\n Test Vector Number   %i \n ",vectNo);
        if(testRLiReadScalar(nStream,"numberOfLayers", &numberOfLayers))
            return;
        if(testRLiReadScalar(nStream,"numberOfInputs", &numberOfInputs))
            return;
        if(testRLiReadScalar(nStream,"inputsExponent", &inputsExponent))
            return;
        /* read in input vector */
        if(testRLwGetVector(nStream,"sinput", &sinput))
            return;
        fprintf(stdout,    "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_w_datfile,numberOfLayers);
        fprintf(logStream, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_w_datfile,numberOfLayers);
        sprintf (number4, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_w_datfile,numberOfLayers);
        /* read in the number of neurons in each layer into the array layerNeuronCounts */
        if(testRLiGetVector(nStream,"layerNeuronCounts", &layerNeuronCounts))
            return;
        /* read in the weights exponent */
        if(testRLiReadScalar(nStream,"weightsExponent", &weightsExponent))
            return;
        /* calculate total number of weights allocate memory, and read in weights */
        numberOfLayerInputs = numberOfInputs;
        numberOfWeights = 0;
        for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
        } /* for layerNumber */
        if(testRLwGetVector(nStream,"weights", &weights))
            goto close_all;
        if(testRLiReadScalar(nStream,"doScaleOutput", &doScaleOutput))
            return;
        if(testRLiReadScalar(nStream,"scaleFactor", scaleFactor))
            return;
          /* Allocate memory and read in MatLab output vector */
        numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
        /*allocate memory for output*/
        testRLwGetVector(nStream,"output", &output);
        if(testRLwGetVector(nStream,"MLoutput", &MLoutput))
            goto close_all;
        if(InitwTestServer(&wMLPServerStruct,layerNeuronCounts,numberOfLayers,weights,
              FullConnected,numberOfInputs,flags)==FALSE){
              sprintf(number5, "\n*** FALSE code from InitwTestServer\n");
              err2=1;
              goto close_all;
        }
        if((pwMLPServer = RLwInitMLPServer(wMLPServerStruct.pweights,
              wMLPServerStruct.pneuronInputLists,
              wMLPServerStruct.pneuronInputCounts,
              wMLPServerStruct.playerNeuronCounts, NULL, NULL,
              wMLPServerStruct.numberOfLayers,
              wMLPServerStruct.flags,
              weightsExponent,0))==NULL){
              fprintf(logStream, "\n*** Null pointer from RLwInitMLPServer\n");
              sprintf(number5, "\n*** Null pointer from RLwInitMLPServer\n");
              err2=1;
              goto free_tsts;
        }
	    if ((out=RLOpenFile("mlp_rw_test.dat" ,"wb"))==NULL) 
            goto free_s;
    	RLwWriteMLPServer( out, pwMLPServer);
        RLCloseFile(out);
        if(RLGetStatus() != RL_StsOk){
            sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
            err2 = 1;
            goto free_s;
        }
	    if ((out=RLOpenFile("mlp_rw_test.dat" ,"rb"))==NULL) 
              goto free_tsts;
    	pwMLPServer2 = RLwReadMLPServer(out);
	    if(pwMLPServer2 == NULL)
              goto free_tsts;
        RLCloseFile(out);

        /*  Main test Segment   */
        RLwEvalMLPFeedForward(pwMLPServer,sinput,numberOfInputs,output,
              numberOfOutputs, doScaleOutput, scaleFactor,inputsExponent);
        if(RLGetStatus() != RL_StsOk){
            sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
            err2 = 1;
            goto free_a;
        }
        RLwEvalMLPFeedForward(pwMLPServer2,sinput,numberOfInputs, MLoutput,
              numberOfOutputs, doScaleOutput, scaleFactor,inputsExponent);
        if(RLGetStatus() != RL_StsOk){
            sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
            err2 = 1;
            goto free_a;
        }
        fprintf(stdout,"\nVerifying outputs ");
        fprintf(logStream,"\nVerifying outputs ");
        error = wPrintVectMismatch(logStream, message, output, MLoutput, numberOfOutputs,
                     maxPercentError, imaxError);
        /* if error print */
        if(error){
            fprintf(logStream, "\n*** %i Errors detected for this test vector. \n",error);
            fprintf(stdout,    "\n*** %i Errors detected for this test vector.\n",error);
            if (!err1) {
                  tstErrPrintStr (number);
                  tstErrPrintStr (number1);
                  tstErrPrintStr (number2);
                  err1 = 1;
            }
            tstErrwPrintVectMismatch(TRUE, number3, number4, output, MLoutput, numberOfOutputs,
                            maxPercentError, imaxError);
            sprintf(number,"\n*** %i Errors detected for this test vector. \n\n",error);
            tstErrPrintStr (number);

        }else{
            fprintf(logStream, "\n---RLwEvalMLPFeedForward tests Okay\n");
            fprintf(stdout,    "\n---RLwEvalMLPFeedForward tests Okay\n");
        }/* else */
free_a:
        RLwFreeMLPServer(pwMLPServer2);
free_s:
        RLwFreeMLPServer(pwMLPServer);
free_tsts:
        FreewTestServer(&wMLPServerStruct);
close_all:
        totalErrors += error;
        if(err2){
             if (!err1) {
                 tstErrPrintStr (number);
                 tstErrPrintStr (number1);
                 tstErrPrintStr (number2);
                 err1 = 1;
             }
             tstErrPrintStr (number5);
             err2 =0;
          }
     }/* for all test vectors*/
     if (err1) {
            m_bTestAccuracyError = TRUE;
            sprintf(number,"\n*************** Total Errors, RLwEvalMLPFeedForward %i     ******************\n\n",totalErrors);
            tstErrPrintStr (number);
     }
     fprintf(logStream, "\n*************** Total Errors, RLwEvalMLPFeedForward %i     ******************",totalErrors);
     fprintf(stdout,    "\n*************** Total Errors, RLwEvalMLPFeedForward %i     ******************",totalErrors);
     testRLCloseFile(nStream);
} /* wMLPReadWriteServerTest */

/**********************************************************************************/
void sMLPReadWriteServerTest(FILE* logStream,char * new_s_datfile,int flags){
  /* function prototypes for reference */
    int  sPrintVectMismatch(FILE *stream, char *message, float *actual, float *expected, int vectLen,
            float max_percent_error,  float  max_error);
    int  wPrintVectMismatch(FILE *stream, char *message, short int *actual, short int *expected, int vectLen,
            float max_percent_error, int max_error);
    int  iPrintScalarMismatch(FILE *stream, char *message, int actual, int expected,
            double max_percent_error, int max_error);
    int         vectNo, numberOfInputs, numberOfLayerInputs,
                numberOfWeights, numberOfLayers;
    int         *layerNeuronCounts; /* an array of ints that stores the number of neurons in each layer */
    float       *MLoutput;
    float       *weights;
    float       *output;
    int         layerNumber;
    int         i, error = 0, err1 = 0, err2 = 0, totalErrors = 0;
    float       maxPercentError = (float).00005, smaxError = (float).000005;
    char        message[100] = {"sMLPReadWriteServerTest"};
    char        message1[200];
    sMLPServer *psMLPServer2;
	FILE       *out;

    RPL_FILE* nStream;
    RLSetStatus(RL_StsOk);

    nStream=testRLOpenFile(new_s_datfile, "sMLPtest", "08/08/98", TEST_RPL);
    if(nStream==NULL){
        sprintf(message1, "***  Could not open MLPerceptron s data file %s\\%s\n",
            dataPath,new_s_datfile);
        printf(message1);
        fprintf(logStream, message1);
        m_bTestAccuracyError = TRUE;
        tstErrPrintStr(message1);
        return;
    }
    for(vectNo = 1; testRLFindTagData(nStream)==RET_FOUND_TAG; vectNo++){
        fprintf(stdout,    "\n Test Vector Number   %i  ",vectNo);
        fprintf(logStream, "\n Test Vector Number   %i  ",vectNo);
        sprintf(number3, "\n Test Vector Number   %i \n ",vectNo);
        if(testRLiReadScalar(nStream,"numberOfLayers", &numberOfLayers ))
            return;
        if(testRLiReadScalar(nStream,"numberOfInputs", &numberOfInputs))
            return;
          /* read in input vector */
        /* read in the number of neurons in each layer into the array layerNeuronCounts */
        if(testRLiGetVector(nStream,"layerNeuronCounts", &layerNeuronCounts))
            return;
        if(testRLsGetVector(nStream,"finput", &finput))
            return;
        fprintf(stdout,    "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_s_datfile,numberOfLayers);
        fprintf(logStream, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_s_datfile,numberOfLayers);
        sprintf (number4, "\n\n--- Input Vector size %4d, Data File %s, Number of Layers %d\n",
                        numberOfInputs, new_s_datfile,numberOfLayers);


          /* calculate total number of weights allocate memory, and read in weights */
        numberOfLayerInputs = numberOfInputs;
        numberOfWeights = 0;
        for (layerNumber = 0; layerNumber < numberOfLayers; layerNumber++) {
              numberOfWeights += numberOfLayerInputs**(layerNeuronCounts + layerNumber);
              numberOfLayerInputs = *(layerNeuronCounts + layerNumber); /* outputs of one layer become inputs to next */
        } /* for layerNumber */
        if(testRLsGetVector(nStream,"weights", &weights))
            goto close_all;

          /* Allocate memory and read in MatLab output vector */
        numberOfOutputs = *(layerNeuronCounts + numberOfLayers-1);
        /*allocate memory for output*/
        testRLsGetVector(nStream,"output", &output);
        if(testRLsGetVector(nStream,"MLoutput", &MLoutput))
            goto close_all;
		/*  Main test Segment   */
        if(InitsTestServer(&sMLPServerStruct,layerNeuronCounts,numberOfLayers,weights,
            FullConnected,numberOfInputs,flags)==FALSE){
            sprintf(number5, "\n*** FALSE code from InitsTestServer\n");
            err2=1;
            goto close_all;
        }
        if((psMLPServer = RLsInitMLPServer(sMLPServerStruct.pweights,
            sMLPServerStruct.pneuronInputLists,
              sMLPServerStruct.pneuronInputCounts,
              sMLPServerStruct.playerNeuronCounts, NULL, NULL,
              sMLPServerStruct.numberOfLayers,
              sMLPServerStruct.flags,0))==NULL){
              fprintf(logStream, "\n*** Null pointer from RLsInitMLPServer\n");
              sprintf(number5, "\n*** Null pointer from RLsInitMLPServer\n");
              err2=1;
              goto free_tsts;
        }
            /*  Main test Segment   */
	    //if((out=fopen("mlp_rw_test.dat" ,"wb"))==NULL) 
	    if((out=RLOpenFile("mlp_rw_test.dat" ,"wb"))==NULL) 
              goto free_s;
    	RLsWriteMLPServer( out, psMLPServer);
        //fclose(out);
        RLCloseFile(out);
        if(RLGetStatus() != RL_StsOk){
            sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
            err2 = 1;
            goto free_s;
        }
	    //if ((out=fopen("mlp_rw_test.dat" ,"rb"))==NULL) 
	    if ((out=RLOpenFile("mlp_rw_test.dat" ,"rb"))==NULL) 
              goto free_s;
    	psMLPServer2 = RLsReadMLPServer(out);
	    if(psMLPServer2 == NULL)
              goto free_s;
        //fclose(out);
        RLCloseFile(out);

        RLsEvalMLPFeedForward(psMLPServer,finput,numberOfInputs,output,numberOfOutputs);
        if(RLGetStatus() != RL_StsOk){
            sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
            err2 = 1;
            goto free_a;
        }

        RLsEvalMLPFeedForward(psMLPServer2,finput,numberOfInputs, MLoutput,numberOfOutputs);
        if(RLGetStatus() != RL_StsOk){
            sprintf(number4, "\n*** FALSE code from RLGetStatus\n");
            err2 = 1;
            goto free_a;
        }

        fprintf(logStream,"\nVerifying outputs ");
        error = sPrintVectMismatch(logStream, message, output, MLoutput, numberOfOutputs,
                             maxPercentError, smaxError);
          /* if error print */
        if(error){
              fprintf(logStream, "\n*** %i Errors detected for this test vector\n",error);
              fprintf(stdout,    "\n*** %i Errors detected for this test vector\n",error);
              if(!err1) {
                  tstErrPrintStr (number);
                  tstErrPrintStr (number1);
                  tstErrPrintStr (number2);
                  err1 = 1;
              }
              tstErrsPrintVectMismatch(TRUE, number3, number4, output, MLoutput, numberOfOutputs,
                              maxPercentError, smaxError);
              for (i = 0;  i  <  numberOfOutputs;  i++) {
                  /*A6 debug */
                  fprintf(logStream,"exceeded output[%i]= %g\n",i, MLoutput[i]);
                  fprintf(stdout,"exceeded output[%i]= %g\n",i, MLoutput[i]);
              } /* for */
              sprintf(number,"\n*** %i Errors detected for this test vector. \n\n",error);
              tstErrPrintStr (number);

        }else{
               fprintf(logStream, "\n---RLsEvalMLPFeedForward tests Okay\n");
               fprintf(stdout,    "\n---RLsEvalMLPFeedForward tests Okay\n");
        } /* else */
free_a:
        RLsFreeMLPServer(psMLPServer2);
free_s:
        RLsFreeMLPServer(psMLPServer);
free_tsts:
        FreesTestServer(&sMLPServerStruct);
close_all:
        if(err2){
              if(!err1) {
                   tstErrPrintStr (number);
                   tstErrPrintStr (number1);
                   tstErrPrintStr (number2);
                   err1 = 1;
               }
               tstErrPrintStr (number5);
               err2 =0;
        }
          totalErrors += error;
    } /* for all test vectors*/
    if(err1){
        m_bTestAccuracyError = TRUE;
        sprintf(number,"\n*************** Total Errors, RLsEvalMLPFeedForward %i     ******************\n\n",totalErrors);
        tstErrPrintStr (number);
    }
    fprintf(logStream, "\n*************** Total Errors, RLsEvalMLPFeedForward   %i     ******************",totalErrors);
    fprintf(stdout,    "\n*************** Total Errors, RLsEvalMLPFeedForward   %i     ******************",totalErrors);
    testRLCloseFile(nStream);
}  /* sMLPReadWriteServerTest */
 
void MLPtest(FILE *logStream){
    RLSetStatus(RL_StsOk);
    sMLPtest(logStream,"mlpstest.dat",RL_MLP_SIGMOID );
    sSigmActivTest(logStream);
    sMLPReadWriteServerTest(logStream,"nmlpstst.dat",RL_MLP_SIGMOID );
    wMLPtest(logStream,"mlpwtest.dat",RL_MLP_SIGMOID );
    wMLPReadWriteServerTest(logStream,"nmlpwtest.dat",RL_MLP_SIGMOID );
    sMLPtestTrain(logStream,RL_MLP_SIGMOID  );
}
void MLPperf(FILE  *logStream, int clockFreq){
    wMoreMLPperf(logStream, clockFreq,RL_MLP_SIGMOID );
    sMoreMLPperf(logStream, clockFreq,RL_MLP_SIGMOID );
    sSigmActivPerf(logStream, clockFreq);
    sMLPTrainPerf(logStream, clockFreq,RL_MLP_SIGMOID );
}

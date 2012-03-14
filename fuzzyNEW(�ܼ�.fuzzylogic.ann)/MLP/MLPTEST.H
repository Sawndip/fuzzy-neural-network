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
//    $Workfiles$   $Revisions$    $Modtime:   Nov 28 1995 04:50:48  $
//
//    Filename: MLPtest.h
//  Purpose: Computes a feed-forward Multi-Layer Perceptron (MLP) Neural Network, capable of
//           any linear or non-linear mapping from one vector space to another. Weights 
//           for the network are typically derived from training examples using the 
//           Error Back-Propagation (EBP) learning algorithm. MLPs are typically used
//           for feature extraction or pattern classification. EBP is not implemented in
//           this library. Weights used in testing are determined by a random number 
//           generator in MatLab.   
//
//  Contents: The functions included test the following functions for correctness and 
//           evaluates their performance.  
//
//    RLwMLPerceptron      Performs multi-layer feed forward neural network processing on 
//                        an input vector to produce a vector of neuron activations given 
//                        a set of weights.   
//
//    RLsMLPerceptron    Same as RLwMLPerceptron but uses floats for I/O instead of integers. 
//                        This version is somewhat faster because fewer type conversions are 
//                        required.  
//
//  Author:      M.Holler
*M*/

#ifndef _MLPtest_h
#define _MLPtest_h

void MLPtest(FILE *logStream);
void MLPperf(FILE *logStream, int clockFreq);
                                                       
#endif

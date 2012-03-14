/*******************************************************************/
/*******************************************************************/
/***                                                             ***/
/***               SOME MORE QUERIES FOR SYSTEM                  ***/
/***                   ( FINDING REDUCTS )                       ***/
/***                                                             ***/
/*** part of the RSL system written by M.Gawrys J.Sienkiewicz    ***/
/***                                                             ***/
/*******************************************************************/
/*******************************************************************/

#ifndef _REDUCT1_H_
#define _REDUCT1_H_

DLLEXPORT int Red (setA *red,int matrix);
     /* finds all reducts for information system,      */
     /* sets red to yhe allocated reducts table	       */
     /* function returns number of reducts             */


DLLEXPORT int RedRel (setA *red,setA P,setA Q,int matrix_type);
      /* like Red, but reducts are Q-relative and      */
      /* computed from set of attributes P             */

DLLEXPORT int RedLess (setA *red,int N,int matrix_type);
      /* finds all reducts which contain less than     */
      /* N attributes, other parameters and result     */
      /* like Red                                      */

DLLEXPORT int RedRelLess (setA *red,setA P,setA Q,int N,int matrix_type);
      /* like RedLess, but reducts are Q-relative of P */


DLLEXPORT int RedSetA (setA *red,setA quasicore,int matrix_type);
      /* finds only reducts including quasicore ,      */
      /* some reducts can be not minimal ( formally    */
      /* they are not reducts ) , other parameters and */
      /* result like Red                               */

DLLEXPORT int RedRelSetA (setA *red,setA quasicore,setA P,setA Q,int matrix_type);
      /* like RedSetA, but reducts are Q-relative of P  */


DLLEXPORT int RedFirst (setA *red,int N,int matrix_type);
      /* finds first N reducts ( only quasi-reduct ),  */
      /* other parameters and result like Red          */

DLLEXPORT int RedRelFirst (setA *red,setA P,setA Q,int N,int matrix_type);
      /* like RedFirst, but reducts are Q-relative of P */

#endif
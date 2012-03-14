/*******************************************************************/
/*******************************************************************/
/***                                                             ***/
/***                   BASIC QUERRIES FOR SYSTEM                 ***/
/***                                                             ***/
/*** part of the RSL system written by M.Gawrys J.Sienkiewicz    ***/
/***                                                             ***/
/*******************************************************************/
/*******************************************************************/

/* If a function from this section value a set, this set should be */
/* first initialized and then given to the function as a first     */
/* argument. Functions from this group does not work on matrix X.  */

#ifndef _RBASIC_H_
#define _RBASIC_H_

DLLEXPORT int LowAppr (setO appr,setO X,setA P,int matrix_type);
DLLEXPORT int LowApprA(setO appr,setO X,setA P);
DLLEXPORT int LowApprD(setO appr,setO X,setA P);
	/* P-lower approximation of X */

DLLEXPORT int UppAppr (setO appr,setO X,setA P,int matrix_type);
DLLEXPORT int UppApprA(setO appr,setO X,setA P);
DLLEXPORT int UppApprD(setO appr,setO X,setA P);
	/* P-upper appriximation of X */

DLLEXPORT int Bound (setO bound,setO X,setA P,int matrix_type);
DLLEXPORT int BoundA(setO bound,setO X,setA P);
DLLEXPORT int BoundD(setO bound,setO X,setA P);
	/* P-boundary of X */

DLLEXPORT float AccurCoef (setO X,setA P,int matrix_type);
DLLEXPORT float AccurCoefA(setO X,setA P);
DLLEXPORT float AccurCoefD(setO X,setA P);
	/* Accuracy coefficient of X with respect to P */

DLLEXPORT float ClassCoef (setO X,setA P,int matrix_type);
DLLEXPORT float ClassCoefA(setO X,setA P);
DLLEXPORT float ClassCoefD(setO X,setA P);
	/* quality of classification (X,not X) */
	/* with respect to P */

DLLEXPORT int Pos (setO pos,setA P,setA Q,int matrix_type);
DLLEXPORT int PosA(setO pos,setA P,setA Q);
DLLEXPORT int PosD(setO pos,setA P,setA Q);
	/* P-positive region of Q */

DLLEXPORT float DependCoef (setA P,setA Q,int matrix_type);
DLLEXPORT float DependCoefA(setA P,setA Q);
DLLEXPORT float DependCoefD(setA P,setA Q);
	/* degree of dependency Q from P */

DLLEXPORT float SignifCoef (int attr,setA P,int matrix_type);
DLLEXPORT float SignifCoefA(int attr,setA P);
DLLEXPORT float SignifCoefD(int attr,setA P);
	/* significance of attribute attr  */
	/* in the set P */

DLLEXPORT float SignifRelCoef (int attr,setA P,setA Q,int matrix_type);
DLLEXPORT float SignifRelCoefA(int attr,setA P,setA Q);
DLLEXPORT float SignifRelCoefD(int attr,setA P,setA Q);
	/* significance of attribute attr */
	/* in the set P, relatively to Q */


DLLEXPORT int CardCoef(setA P,int matrix_type);
	/* returns number of matrix fields */
	/* covered by P (MATA is treated as MATD) */
	/* but elements are evaluated "on-line" */

#endif
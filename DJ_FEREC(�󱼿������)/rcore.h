/*******************************************************************/
/*******************************************************************/
/***                                                             ***/
/***              SOME MORE QUERIES FOR SYSTEM                   ***/
/***          ( FINDING CORES AND CHECKING REDUCTS )             ***/
/***                                                             ***/
/*** part of the RSL system written by M.Gawrys J.Sienkiewicz    ***/
/***                                                             ***/
/*******************************************************************/
/*******************************************************************/

#ifndef _RCORE_H_
#define _RCORE_H_

DLLEXPORT int Core  (setA core,int matrix_type);
DLLEXPORT int CoreA (setA core);
DLLEXPORT int CoreDX(setA core,int matrix_type);
	/* finds a core of all attributes */

DLLEXPORT int CoreRel (setA core,setA P,setA Q,int matrix_type);
DLLEXPORT int CoreRelA(setA core,setA P,setA Q);
DLLEXPORT int CoreRelD(setA core,setA P,setA Q);
	/* finds a core of P relativly to Q */

DLLEXPORT int IsOrtho(setA red,setA over,int matrix_type);
	/* return 1 if red is orthogonal */
	/* otherwise returns 0 */

DLLEXPORT int IsOrthoRel(setA red,setA over,setA P,setA Q,int matrix_type);
	/* return 1 if red is Q-orthogonal in P */
	/* otherwise returns 0 */

DLLEXPORT int IsCover  (setA red,int matrix_type);
DLLEXPORT int IsCoverA (setA red);
DLLEXPORT int IsCoverDX(setA red,int matrix_type);
	/* return 1 if red is a cover */
	/* otherwise returns 0 */

DLLEXPORT int IsCoverRel (setA red,setA P,setA Q,int matrix_type);
DLLEXPORT int IsCoverRelA(setA red,setA P,setA Q);
DLLEXPORT int IsCoverRelD(setA red,setA P,setA Q);
	/* return 1 if red is Q-cover in P */
	/* otherwise returns 0 */

DLLEXPORT int IsRed(setA red,int matrix_type);
	/* returns 1 if red is a reduct */
	/* otherwise returns 0 */

DLLEXPORT int IsRedRel(setA red,setA P,setA Q,int matrix_type);
	/* returns 1 if red is a Q-relative reduct of P */
	/* otherwise returns 0 */

#endif
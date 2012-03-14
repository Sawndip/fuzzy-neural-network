/*******************************************************************/
/*******************************************************************/
/***                                                             ***/
/***       OPERATIONS ON SETS OF ATTRIBUTES AND OBJECTS          ***/
/***                                                             ***/
/*** part of the RSL system written by M.Gawrys J. Sienkiewicz   ***/
/***                                                             ***/
/*******************************************************************/
/*******************************************************************/

#ifndef _RSET_H_
#define _RSET_H_

#include <stdarg.h>

extern cluster_type _mask[];
extern int _cluster_bytes;
extern int _cluster_bits;

DLLEXPORT setO InitEmptySetO(void);
DLLEXPORT setA InitEmptySetA(void);
	/* allocates memory for set and initializes it */
        /* as empty, returns allocated set */

DLLEXPORT setO InitFullSetO(void);
DLLEXPORT setO InitFullSetA(void);
	/* allocates memory for set and initializes it */
	/* as full, returns newly allocated set */

DLLEXPORT void TabToSetO(setO set,int num,int tab[]);
DLLEXPORT void TabToSetA(setA set,int num,int tab[]);
	/* sets a set with num elements of table tab */

DLLEXPORT void ArgToSetO(setO set,int num,...);
DLLEXPORT void ArgToSetA(setA set,int num,...);
	/* sets a set with num arguments of function */

DLLEXPORT void CloseSetO(setO set);
DLLEXPORT void CloseSetA(setA set);
	/* free memory used by set */

DLLEXPORT void OrSetO(setO or,setO s1,setO s2);
DLLEXPORT void OrSetA(setA or,setA s1,setA s2);
	/* puts union of s1 and s2 to or */

DLLEXPORT void AndSetO(setO and,setO s1,setO s2);
DLLEXPORT void AndSetA(setA and,setA s1,setA s2);
	/* puts product of s1 and s2 to and */

DLLEXPORT void DifSetO(setO dif,setO s1,setO s2);
DLLEXPORT void DifSetA(setA dif,setA s1,setA s2);
	/* puts complement of s1 in s2 to dif */

DLLEXPORT void NotSetO(setO not,setO set);
DLLEXPORT void NotSetA(setO not,setA set);
	/* puts a complement of set to not */

DLLEXPORT void ClearSetO(setO set);
DLLEXPORT void ClearSetA(setA set);
	/* clears set */

DLLEXPORT void FillSetO(setO set);
DLLEXPORT void FillSetA(setA set);
	/* fills set with domain */

DLLEXPORT int AddSetO(setO set,int obj);
DLLEXPORT int AddSetA(setA set,int attr);
	/* adds element to set */

DLLEXPORT int DelSetO(setO set,int obj);
DLLEXPORT int DelSetA(setA set,int attr);
	/* deletes element from set */

DLLEXPORT int InSetO(setO bbig, setO ssmall);
DLLEXPORT int InSetA(setA bbig,setA ssmall);
	/* return 1 if set big contains set small */
	/* otherwise returns 0 */

DLLEXPORT int ContSetO(setO set,int obj);
DLLEXPORT int ContSetA(setA set,int attr);
	/* returns 1 if set contains element */
	/* otherwise returns 0 */

DLLEXPORT int InterSetO(setO s1,setO s2);
DLLEXPORT int InterSetA(setA s1,setA s2);
	/* returns 1 if s1 and s2 have nonempty product */
	/* otherwise returns 0 */

DLLEXPORT int IsEmptySetO(setO set);
DLLEXPORT int IsEmptySetA(setA set);
	/* returns 1 if set is empty */
	/* otherwise returns 0 */

DLLEXPORT int CardSetO(setO set);
DLLEXPORT int CardSetA(setA set);
	/* returns cardinality of set */

DLLEXPORT void CopySetO(setO dest,setO source);
DLLEXPORT void CopySetA(setA dest,setO source);
	/* copy source to dest */

DLLEXPORT int CompSetO(setO set1,setO set2);
DLLEXPORT int CompSetA(setA set1,setA set2);
	/* returns 1 if set1 and set2 are identical */
	/* otherwise returns 0 */

DLLEXPORT int SizeSetO(void);
DLLEXPORT int SizeSetA(void);
	/* returns number of clusters in set representation */
	/* in the active system sizeof(cluster_type)=_cluster_bytes */

DLLEXPORT void AttrValSetO(setO set,int attr,value_type val);
	/* puts into set all object that have value val */
	/* on attribute attr */

DLLEXPORT int ClassSetO( setO aclass, int obj, setA Q );
	/* fills class with all object that have the same values */
	/* on all attributes from Q as the object obj, uses MATA */

DLLEXPORT void PrintSetO(setO set);
DLLEXPORT void PrintSetA(setA set);
	/* outputs set to screen */

#endif
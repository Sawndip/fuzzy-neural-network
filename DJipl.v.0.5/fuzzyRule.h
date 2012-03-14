//
//
//
#ifndef _FUZZY_RULE_
#define _FUZZY_RULE_

#include <stdio.h>
#include <stdarg.h>
//#include <Inventor/SbPList.h>
#include <afxcoll.h>
#include "fuzzy.h"

class LinguisticVar {
	char *name;
public:
	CPtrList termSet;

	char *getName() const 	{ return name;}
	void setName(char *str)	{ if(str){if(name) delete name; name=strdup(str);}}

	void setTermSet(char *nameStr, ...);
	void setTermSet(char *nameStr, va_list args);

	void setFuzzySet(int n, FuzzySet *fset);
	FuzzySet *getFuzzySet(int which) 	{ return ((FuzzySet *)termSet.GetAt(termSet.FindIndex(which)));}
	FuzzySet *getFuzzySet(char *str);
	char 	 *getFuzzySetName(int which){ return (getFuzzySet(which)->getName());}

	int getLength() { return termSet.GetCount();}
	void graph();

	//
	//	constructor/destructor
	//
	LinguisticVar(char *nameStr = NULL, ...);
	LinguisticVar(char *nameStr, va_list args);
	~LinguisticVar();
};

class FuzzyRule {
public:
	CPtrList ifVars;
	CPtrList thenVars;
};

class FuzzyRuleBase {
	char *name;
	int numRules;
	int numIfVars;
	int numThenVars;
	CPtrList fuzzyRules;
	CPtrList ifLingVars;		// LinguisticVar *
	CPtrList thenLingVars;	// LinguisticVar *
public:
	FuzzyRuleBase(char *str = NULL, int i=0, int j=0, int r=0);
	~FuzzyRuleBase();
	char *getName() const 		{ return name;}
	void setName(char *str) 	{ if(str){if(name) delete name; name=strdup(str);}}
	int GetCount() 			{ return numRules;}
	int getIfPartLength() 		{ return numIfVars;}
	int getThenPartLength() 	{ return numThenVars;}
	int getLinguisticVarLength(){ return (numIfVars + numThenVars);}

	void setRule(int l, ...);
	void setRulev(int l, CPtrList & list);
	FuzzyRule *getRule(int l);
	int  checkRule(FuzzyRule *fuzzyRule);

	void setLinguisticVar(int n, char *nameStr, ...);
	void setLinguisticVar(int n, char *nameStr, va_list args);
	void setLinguisticVar(int n, LinguisticVar *lingVar);
	void setIfPartLinguisticVar(int l, char *nameStr, ...)
								{ va_list args; va_start(args, nameStr);
								setLinguisticVar(l, nameStr, args); va_end(args);}
	void setIfPartLinguisticVar(int l, LinguisticVar *lingVar)
												   {setLinguisticVar(l, lingVar);}
	void setThenPartLinguisticVar(int l, char *nameStr, ...)
						{ va_list args; va_start(args, nameStr);
						setLinguisticVar(l+ifLingVars.GetCount(), nameStr, args); 
						va_end(args);}
	void setThenPartLinguisticVar(int l, LinguisticVar *lingVar)
							{setLinguisticVar(l+ifLingVars.GetCount(), lingVar);}

	LinguisticVar *getLinguisticVar(int n);
	LinguisticVar *getIfPartLinguisticVar(int l)   
				{return (l < numIfVars ? (LinguisticVar *)ifLingVars.GetAt(ifLingVars.FindIndex(l)) : NULL);}

	LinguisticVar *getThenPartLinguisticVar(int l)
				{return (l < numThenVars ? (LinguisticVar *)thenLingVars.GetAt(thenLingVars.FindIndex(l)) : NULL);}

	FuzzySet *getIfPartFuzzySet(int l, int n)
				{ return ((LinguisticVar *)ifLingVars.GetAt(ifLingVars.FindIndex(n)))->getFuzzySet(
						(char *)((FuzzyRule *)fuzzyRules.GetAt(fuzzyRules.FindIndex(l)))->ifVars.GetAt(
						((FuzzyRule *)fuzzyRules.GetAt(fuzzyRules.FindIndex(l)))->ifVars.FindIndex(n)));}

	FuzzySet *getThenPartFuzzySet(int l, int n)
				{ return ((LinguisticVar *)thenLingVars.GetAt(thenLingVars.FindIndex(n)))->getFuzzySet(
						(char *)((FuzzyRule *)fuzzyRules.GetAt(fuzzyRules.FindIndex(l)))->thenVars.GetAt(
						((FuzzyRule *)fuzzyRules.GetAt(fuzzyRules.FindIndex(l)))->thenVars.FindIndex(n)));}

	// 
	// Inference
	//
	FuzzySet *inference(CPtrList& fuzzyInput);
	
	//
	// Input/Output 
	//
	void 	printFuzzyRuleBase(int lingOn = 0);
	void 	printFuzzyRule(int l);
	int		load(char *filename);
	int parseTermSet(LinguisticVar *lv, FILE *fin, int &n);

	int parseVariable(char *ifthen, FILE *fin, int &n);
	int parseRule(int l, FILE *fin, int &n);
	int parseRules(FILE *fin, int &n);
	int parseRuleBase(FILE *fin, int &n);
};
#endif
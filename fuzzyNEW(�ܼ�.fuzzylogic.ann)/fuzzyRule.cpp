//
//
//
#include <string.h>
#include "fuzzyRule.h"

#define DEBUG	0
//#define _WGT_

//extern int DEBUG;

LinguisticVar::LinguisticVar(char *nameStr, va_list args)
{
	name = NULL;
	setTermSet(nameStr, args);
}

LinguisticVar::LinguisticVar(char *nameStr, ...)
{
	if (nameStr == NULL) {
		name = NULL;
		return;
	}

	va_list args;
	if (nameStr) name = strdup(nameStr);
	else name = NULL;

	va_start(args, nameStr);

	setTermSet(nameStr, args);
	va_end(args);
}

LinguisticVar::~LinguisticVar()
{
	delete name;
//	for (int i = 0; i < termSet.getLength(); i++)
	for (int i = 0; i < termSet.GetCount(); i++)
		delete (FuzzySet *)termSet.GetAt(termSet.FindIndex(i));
}

// 
//	Input: NULL termination
//
void LinguisticVar::setTermSet(char *nameStr, ...)
{
	va_list args;
	va_start(args, nameStr);

	if (nameStr) { if(name) delete name; name = strdup(nameStr);}
	else name = NULL;

	FuzzySet *fset = NULL;
	while((fset = va_arg(args, FuzzySet *)) != NULL) 
//		termSet.append(fset);
		termSet.AddTail(fset);
	va_end(args);
}

void LinguisticVar::setTermSet(char *nameStr, va_list args)
{
	if (nameStr) { if(name) delete name; name = strdup(nameStr);}
	
	FuzzySet *fset = NULL;
	while((fset = va_arg(args, FuzzySet *)) != NULL) 
//		termSet.append(fset);
		termSet.AddTail(fset);
}

FuzzySet *LinguisticVar::getFuzzySet(char *str)
{
//	for (int i = 0; i < termSet.getLength(); i++)
	for (int i = 0; i < termSet.GetCount(); i++)
		if (!strcmp(str, getFuzzySetName(i)))
//			return (FuzzySet *)termSet[i];
			return (FuzzySet *)termSet.GetAt(termSet.FindIndex(i));
	return NULL;
}

void LinguisticVar::setFuzzySet(int n, FuzzySet *fset)
{
//	if (n == 0)
		termSet.AddTail((FuzzySet *)NULL);

	POSITION pos = termSet.FindIndex(n);
	FuzzySet *oldFset = (FuzzySet *)termSet.GetAt(pos);
	
	if (oldFset) delete oldFset;
	//	termSet.remove(n);
	termSet.RemoveAt(termSet.FindIndex(n));
	//	termSet.insert(fset, n);
	termSet.InsertAfter(termSet.FindIndex(n-1), fset);
}

static char *gnucommands[] = { 
	"set yrange [0:1]",
	"plot 'membership.dat' with lines",
	"pause 120"
};

void LinguisticVar::graph()
{
	char *dataFilename = "ling_var.dat";
	char *gnufile = "ling_var.gnuplot";

	FILE *fout;
	if ((fout = fopen(dataFilename,"w")) == NULL) return ;

	// check num
	int num = getFuzzySet(0)->getLength();
	for (int j = 0; j < getLength(); j++) 
		if (num != getFuzzySet(j)->getLength()) {
			fprintf (stderr, "not the same lenth\n");
			return ;
		}
	
	for (int i = 0; i < num; i++) {
		fprintf (fout, "%4.2f ", getFuzzySet(0)->getX(i));
		for (int j = 0; j < getLength(); j++) 
			fprintf (fout, "%4.2f ", getFuzzySet(j)->getIndexedValue(i));
		fprintf (fout, "\n");
	}
	fclose(fout);

	if ((fout = fopen(gnufile,"w")) == NULL) return ;

	for (i = 0; i < 3; i++) {
		if (i == 1) {
			fprintf (fout, "plot ");
			for (int j = 0; j < getLength(); j++) {
				fprintf (fout, "\'%s\' using 1:%d title \'%s:%s\' with lines", 
					dataFilename, j+2, getName(), getFuzzySet(j)->getName());
				if (j != getLength()-1)
					fprintf (fout, ", ");
			}
			fprintf (fout, "\n");
		} else fprintf (fout, "%s\n", gnucommands[i]);
	}
	fclose(fout);

    char command[200];
	char *errFile = "errors";
    sprintf (command,"gnuplot %s >> %s 2>> %s &", gnufile, errFile, errFile);

    int err = system(command);
    if (err) {
        fprintf (stderr,"execution error: %s\n", command);
        return;
    }
}

//
//	FuzzyRuleBase class methods
//
FuzzyRuleBase::FuzzyRuleBase(char *nameStr, int i, int j, int r)
{
	if (nameStr) name = strdup(nameStr);
	else name = NULL;
	numIfVars = i;
	numThenVars = j;
	numRules = r;
}

FuzzyRuleBase::~FuzzyRuleBase()
{
	int which;
//	while (which = fuzzyRules.getLength()) {
	while (which = fuzzyRules.GetCount()) {
		which --;
		//FuzzyRule *rule = (FuzzyRule *)fuzzyRules[which];
		FuzzyRule *rule = (FuzzyRule *)fuzzyRules.GetAt(fuzzyRules.FindIndex(which));
		//for (int i = 0; i < rule->ifVars.getLength(); i++) {
		for (int i = 0; i < rule->ifVars.GetCount(); i++) {
			//delete (char *)rule->ifVars[i];
			delete (char *)rule->ifVars.GetAt(rule->ifVars.FindIndex(i));
		}
		//for (i = 0; i < rule->thenVars.getLength(); i++)
		for (i = 0; i < rule->thenVars.GetCount(); i++)
			//delete (char *)rule->thenVars[i];
			delete (char *)rule->thenVars.GetAt(rule->thenVars.FindIndex(i));
		//fuzzyRules.remove(which);
		fuzzyRules.RemoveAt(fuzzyRules.FindIndex(which));
		delete  rule;
	}
}

//
// under coding
//

int FuzzyRuleBase::checkRule(FuzzyRule *fuzzyRule)
{
	//
	// check rules
	//
//	if (fuzzyRule->ifVars.getLength() != numIfVars) return False;
//	if (fuzzyRule->thenVars.getLength() != numThenVars) return False;
	if (fuzzyRule->ifVars.GetCount() != numIfVars) return False;
	if (fuzzyRule->thenVars.GetCount() != numThenVars) return False;
	int error = 0;
	for (int i = 0; i < numIfVars; i++) {
		LinguisticVar *lVar = (LinguisticVar *)ifLingVars.GetAt(ifLingVars.FindIndex(i));
		for (int j = 0; j < lVar->getLength() && 
		strcmp((char *)fuzzyRule->ifVars.GetAt(fuzzyRule->ifVars.FindIndex(i)), lVar->getFuzzySet(j)->getName());
		j++) ;
		if (j == lVar->getLength()) { // error
			printf ("if part : %s mismatch termset: %s\n", 
				lVar->getName(), (char *)fuzzyRule->ifVars.GetAt(fuzzyRule->ifVars.FindIndex(i)));
			error ++;
		}
	}

	for (i = 0; i < numThenVars; i++) {
		LinguisticVar *lVar = (LinguisticVar *)thenLingVars.GetAt(thenLingVars.FindIndex(i));
		for (int j = 0; j < lVar->getLength() && 
		strcmp((char *)fuzzyRule->thenVars.GetAt(fuzzyRule->thenVars.FindIndex(i)), lVar->getFuzzySet(j)->getName());
		j++);
		if (j == lVar->getLength()) { // error
			printf ("then part : %s mismatch termset: %s\n", 
				lVar->getName(), (char *)fuzzyRule->ifVars.GetAt(fuzzyRule->ifVars.FindIndex(i)));
			error ++;
		}
	}
	if (error) return False;
	return True;
}

void FuzzyRuleBase::setRule(int l, ...)
{
	va_list args;
	va_start(args, l);

	FuzzyRule	*fuzzyRule = new FuzzyRule;
	char *string;
	
	for (int i = 0; i < numIfVars; i++) {
		string = va_arg(args, char *);
		fuzzyRule->ifVars.AddTail(strdup(string));
	}

	for (i = 0; i < numThenVars; i++) {
		string = va_arg(args, char *);
		fuzzyRule->thenVars.AddTail(strdup(string));
	}

	if (fuzzyRules.GetCount() >= l) {
		delete fuzzyRules.GetAt(fuzzyRules.FindIndex(l));
		fuzzyRules.RemoveAt(fuzzyRules.FindIndex(l));
	}
	va_end(args);

	//
	// check rules
	//
	if(checkRule(fuzzyRule) == False) {
		printf ("check %d-th rule: if parts\n", l+1);
		return ;
	}
	int error = 0;
	for (i = 0; i < numIfVars; i++) {
		LinguisticVar *lVar = (LinguisticVar *)ifLingVars.GetAt(ifLingVars.FindIndex(i));
		for (int j = 0; j < lVar->getLength() && 
		strcmp((char *)fuzzyRule->ifVars.GetAt(fuzzyRule->ifVars.FindIndex(i)), lVar->getFuzzySet(j)->getName());
		j++) ;
		if (j == lVar->getLength()) { // error
			printf ("if part of %d-th rule: %s mismatch termset: %s\n", 
				l+1, lVar->getName(), (char *)fuzzyRule->ifVars.GetAt(fuzzyRule->ifVars.FindIndex(i)));
			error ++;
		}
	}

	if (fuzzyRule->thenVars.GetCount() != numThenVars){
		printf ("check %d-th rule: then parts\n", l+1);
		return ;
	}
	for (i = 0; i < numThenVars; i++) {
		LinguisticVar *lVar = (LinguisticVar *)thenLingVars.GetAt(thenLingVars.FindIndex(i));
		for (int j = 0; j < lVar->getLength() && 
		strcmp((char *)fuzzyRule->thenVars.GetAt(fuzzyRule->thenVars.FindIndex(i)), lVar->getFuzzySet(j)->getName());
		j++);
		if (j == lVar->getLength()) { // error
			printf ("if part of %d-th rule: %s mismatch termset: %s\n", 
				l+1, lVar->getName(), (char *)fuzzyRule->thenVars.GetAt(fuzzyRule->thenVars.FindIndex(i)));
			error ++;
		}
	}
//	if (error) return ;

	fuzzyRules.InsertAfter(fuzzyRules.FindIndex(l-1), fuzzyRule);
}

//void FuzzyRuleBase::setRulev(int l, SbPList& list)
void FuzzyRuleBase::setRulev(int l, CPtrList& list)
{
	/*
	if (list.GetCount() != numIfVars + numThenVars) {
		printf ("setRulev: %d != %d(if) %d(then)\n", list.GetCount(), numIfVars, numThenVars);
		return ;
	}
	*/

	FuzzyRule	*fuzzyRule = new FuzzyRule;
	int numIfCount = 0;
	for (int i = 0; i < numIfVars; i++) 
	{
		if (IfVarIndex[i])
		{
			//fuzzyRule->ifVars.AddTail(strdup((char *)list.GetAt(list.FindIndex(i))));
			fuzzyRule->ifVars.AddTail(strdup((char *)list.GetAt(list.FindIndex(numIfCount++))));
			//numIfCount++;
		}
		else
			fuzzyRule->ifVars.AddTail(strdup("Z"));
	}

	for (int j = 0; j < numThenVars; j++) 
		//fuzzyRule->thenVars.AddTail(strdup((char *)list.GetAt(list.FindIndex(i++))));
		fuzzyRule->thenVars.AddTail(strdup((char *)list.GetAt(list.FindIndex(numIfCount + j))));

	// truth value for advanced logic
	fuzzyRule->truth = (float)atof(strdup((char *)list.GetTail()));

	//
	// check rules
	//
	if(checkRule(fuzzyRule) == False) {
		printf ("check %d-th rule: if parts\n", l+1);
		return ;
	}

	if (fuzzyRules.GetCount() >= l) {
		fuzzyRules.AddTail((FuzzyRule *)NULL);
		delete fuzzyRules.GetAt(fuzzyRules.FindIndex(l));
		fuzzyRules.RemoveAt(fuzzyRules.FindIndex(l));
	}
	fuzzyRules.InsertAfter(fuzzyRules.FindIndex(l-1), fuzzyRule);
}

//
// this method does not fuction.
//
void FuzzyRuleBase::setLinguisticVar(int l, char *nameStr, ...)
{
	va_list args;
	va_start(args, nameStr);
	setLinguisticVar(l, nameStr, args);
	va_end(args);
}

void FuzzyRuleBase::setLinguisticVar(int l, char *nameStr, va_list args)
{
	LinguisticVar	*lingVar = new LinguisticVar(nameStr, args);
	setLinguisticVar(l, lingVar);
}

void FuzzyRuleBase::setLinguisticVar(int l, LinguisticVar *lingVar)
{
	if (l < numIfVars) { 
		if (l < ifLingVars.GetCount()) {
			delete ifLingVars.GetAt(ifLingVars.FindIndex(l));
			ifLingVars.RemoveAt(ifLingVars.FindIndex(l));
		} 
		ifLingVars.InsertAfter(ifLingVars.FindIndex(l-1), lingVar);
	} else if (l < numIfVars + numThenVars) {
		l -= numIfVars;
		if (l < thenLingVars.GetCount()) {
			delete thenLingVars.GetAt(thenLingVars.FindIndex(l));
			thenLingVars.RemoveAt(thenLingVars.FindIndex(l));
		}
		thenLingVars.InsertAfter(thenLingVars.FindIndex(l-1), lingVar);
	} else {
		printf ("Error in FuzzyRuleBase::setLinguisticVars\n");
	}
}

LinguisticVar *FuzzyRuleBase::getLinguisticVar(int i)
{
	if (i < ifLingVars.GetCount()) return (LinguisticVar *)ifLingVars.GetAt(ifLingVars.FindIndex(i));
	else if (i < ifLingVars.GetCount() + thenLingVars.GetCount())
		return (LinguisticVar *)thenLingVars.GetAt(thenLingVars.FindIndex(i-ifLingVars.GetCount()));
	else return (LinguisticVar *)NULL;
}

FuzzyRule *FuzzyRuleBase::getRule(int l)
{
	if (l < fuzzyRules.GetCount()) return (FuzzyRule *)fuzzyRules.GetAt(fuzzyRules.FindIndex(l));
	else return (FuzzyRule *)NULL;
}

#define MAX(x, y)	((x) > (y) ? (x): (y))
#define MIN(x, y)	((x) > (y) ? (y): (x))

FuzzySet *FuzzyRuleBase::inference(CPtrList &fuzzyInput)
{
	FuzzySet *rst = new FuzzySet(*getThenPartFuzzySet(0,0)); 
	rst->setName("Inferense Result");
	rst->setMembershipType(FuzzySet::Arbitrary);

	for (int l = 0; l < fuzzyRules.GetCount(); l++) {
		FuzzySet *tmpRst = new FuzzySet(*getThenPartFuzzySet(l,0));
		tmpRst->setName("tmp Result");
		tmpRst->setMembershipType(FuzzySet::Arbitrary);

		//
		// for l-th Rules
		//
		FuzzyRule *fuzzyRule = (FuzzyRule *)fuzzyRules.GetAt(fuzzyRules.FindIndex(l));	
		// input parts
		float mn = 1.0; float MN = 1.0;
		
		// 2002.03.06
		// modify this coding...
		if (fuzzyInput.GetCount() != fuzzyRule->ifVars.GetCount()) {
			printf ("check inputs\n");
			return NULL;
		}
		for (int n = 0; n < fuzzyRule->ifVars.GetCount(); n++) {
			// get i-th input fuzzy set membership
			FuzzySet *X = getIfPartFuzzySet(l, n);
			FuzzySet *A = (FuzzySet *)fuzzyInput.GetAt(fuzzyInput.FindIndex(n));

			/*
			if (strcmp(X->getName(), A->getName()))
				continue;
				*/

			float mx = 0;
			// max min(X, A)
			int from = X->getSupportFromi(), to = X->getSupportToi()+1;
			for (int i = from; i < to; i++) {
				mn = MIN (X->getIndexedValue(i), A->getIndexedValue(i));
				mx = MAX (mn, mx);
			}
			MN = MIN(MN, mx);
		}

		if (DEBUG) printf ("alpha = %3.1f\n", MN);
		// output
		for (n = 0; n < fuzzyRule->thenVars.GetCount(); n++) {
			FuzzySet *B = getThenPartFuzzySet(l, n);
			// get i-th input fuzzy set membership
			int from = 0, to = B->getLength();
			float f;
			for (int i = from; i < to; i++) {
#ifdef	_WGT_
				float truth = (float)fuzzyRule->truth;
				tmpRst->setIndexedValue(i, MIN (f = B->getIndexedValue(i), MN)*truth);
#endif
				tmpRst->setIndexedValue(i, MIN (f = B->getIndexedValue(i), MN));
			}
		}

		for (int i = 0; i < rst->getLength(); i++) {
			rst->setIndexedValue(i, 
					MAX (rst->getIndexedValue(i), tmpRst->getIndexedValue(i)));
		}

		delete tmpRst;
	}	// loop : l

	return rst;
}

void FuzzyRuleBase::printFuzzyRule(int l)
{
	printf ("%2d-th Rule: ", l+1);
	printf ("IF ");
	for (int i = 0; i < getIfPartLength(); i++) {
		char *varStr = getLinguisticVar(i)->getName();
		char *fuzzyStr = (char *)(getRule(l)->ifVars.GetAt(getRule(l)->ifVars.FindIndex(i)));
		printf ("\"%s\" is \"%s\" ",  varStr, fuzzyStr);
		if (i != getIfPartLength() - 1)
			printf ("and ");
	}	
	
	printf ("THEN ");
	for (i = 0; i < getThenPartLength(); i++) {
		char *varStr = getLinguisticVar(getIfPartLength()+i)->getName();
		char *fuzzyStr = (char *)getRule(l)->thenVars.GetAt(getRule(l)->thenVars.FindIndex(i));
		printf ("\"%s\" is \"%s\" ",  varStr, fuzzyStr);
		if (i != getThenPartLength() - 1)
			printf ("and ");
	}	
	printf ("\n");
}

void FuzzyRuleBase::printFuzzyRuleBase(int lingOn)
{
	int i,j,l;

	if (lingOn) 
	for (i = 0; i < getLinguisticVarLength(); i++) 
		getLinguisticVar(i)->graph();
	
	if(DEBUG)
	for (j = 0; j < getLinguisticVar(i)->getLength(); j++) {
		getLinguisticVar(i)->getFuzzySet(j)->print();
		getLinguisticVar(i)->getFuzzySet(j)->graph();
	}

	printf ("Fuzzy RuleBase:\n");
	for (l = 0; l < GetCount(); l++) 
		printFuzzyRule(l);
}

char *getNextStr(char *&s)
{
	while((*s == ' ' || *s == '\n' || *s == '\t') && *s) s++;
	char *s1 = s;
	while(*s && !(*s == ' ' || *s == '\n' || *s == '\t')) s++;
	if (s == s1) return s;
	s++;
	*(s-1) = '\0'; 
	return s1;
//	return strtok(s, " \n\t");
}

char *getNextSeg(char *&s)
{
	while(*s == ' ' || *s == '\n' || *s == '\t') 
		s++;
	char *s1 = s;
	while(!(*s1 == ' ' || *s1 == '\n' || *s1 == '\t')) 
		s1++;
	s1++;
	*(s1-1) = '\0'; 
	return s1;
}

static const int STRBUFSIZE = 512;

int FuzzyRuleBase::parseTermSet(LinguisticVar *lv, FILE *fin, int &n)
{
	char strBuf[STRBUFSIZE];

	CPtrList fuzzySets;
	FuzzySet *fset = NULL;
	int index = 0;
	float ud[2];
	int num;
	while(fgets(strBuf, STRBUFSIZE, fin) != NULL) {
		char *segStr = strBuf;
		char *str = getNextStr(segStr);
		n++;
		if (!strcmp(str, "Name:")) {
			fset = new FuzzySet(getNextStr(segStr));		
			if (fset) lv->setFuzzySet(index++, fset);	
		} else if (!strcmp(str, "UniverseOfDiscourse:")) {
			sscanf(segStr,"%f%f", &ud[0], &ud[1]);
			fset->setUnivOfDiscourse(ud[0], ud[1]);
		} else if (!strcmp(str, "NoOfSamples:")) {
			sscanf(segStr,"%d", &num);
			fset->setLength(num);
		} else if (!strcmp(str, "MembershipType:")) {
			str = getNextStr(segStr);	
			if (!strcmp(str, "Singleton")) 
				fset->setMembershipType(FuzzySet::Singleton);	
			else if (!strcmp(str, "Triangular")) 
				fset->setMembershipType(FuzzySet::Triangular);	
			else if (!strcmp(str, "Trapezoidal")) 
				fset->setMembershipType(FuzzySet::Trapezoidal);	
			else if (!strcmp(str, "Gaussian")) 
				fset->setMembershipType(FuzzySet::Gaussian);	
			else if (!strcmp(str, "Arbitrary")) 
				fset->setMembershipType(FuzzySet::Arbitrary);	
			else fprintf (stderr,"Warning: check rulebase file: %d-th line\n", n);
		} else if (!strcmp(str, "Parameters:")) {
			float d[4];
			switch(fset->getMembershipType()) {
			case FuzzySet::Singleton:
				sscanf(segStr,"%f", &d[0]);
				fset->setMembershipPara(d, 1);
				break;
			case FuzzySet::Triangular:
				sscanf(segStr,"%f%f", &d[0], &d[1]);
				fset->setMembershipPara(d, 2);
				break;
			case FuzzySet::Trapezoidal:
				sscanf(segStr,"%f%f%f%f", &d[0], &d[1], &d[2], &d[3]);
				fset->setMembershipPara(d, 4);
				break;
			case FuzzySet::Gaussian:
				sscanf(segStr,"%f%f", &d[0], &d[1]);
				fset->setMembershipPara(d, 2);
				break;
			case FuzzySet::Arbitrary:
		//		for(int i = 0; i < fset->getLength(); i++)
		//			d[i] = 
				printf ("not implemented\n");
				break;
			default:
				break;
			}
			fset->setSupport();
			// parseParameters(fset, fin, n);
		}
		else if (!strncmp(str, "!", 1))  	;					// comments
		else if (!strcmp(str, "")) 	;
		else if (!strcmp(str, "EndTermSet")) 	break;
		else fprintf (stderr,"Warning: check rulebase file: %d-th line\n", n);
	}
	// result:
	if (DEBUG) printf ("%s Termset[%d fuzzysets] stored\n", lv->getName(), index);
	return n;
}

int FuzzyRuleBase::parseVariable(char *ifthen, FILE *fin, int &n)
{
	char strBuf[STRBUFSIZE];
/*
	float ud[2];
	int N;
	float increment;
*/

	LinguisticVar *lVar =  NULL;
	int l = 0;
	while(fgets(strBuf, STRBUFSIZE, fin) != NULL) {
		char *segStr = strBuf;
		char *str = getNextStr(segStr);
		n++;
		if (!strcmp(str, "Name:")) {
			lVar =  new LinguisticVar(NULL,1,1,1);
			lVar->setName(getNextStr(segStr));
			if (!strcmp(ifthen, "if")) {
				if (numIfVars <= l) numIfVars = l + 1;
				setIfPartLinguisticVar(l, lVar);
			} else if (!strcmp(ifthen, "then")) {
				if (numThenVars <= l) numThenVars = l + 1;
				setThenPartLinguisticVar(l, lVar);
			} else {
				delete lVar;
				fprintf (stderr,"Error in parse %s Variables\n", ifthen);
				return n;
			}
			l++;
		} else if (!strncmp(str, "!", 1))  	;			// comments
/*
		} else if (!strcmp(str, "UniverseOfDiscourse:")) {
			sscanf(str, "%f%f", &ud[0], &ud[1]);
		} else if (!strcmp(str, "NoOfSamples:")) {
			sscanf(str, "%d", &N);
			increment = (ud[1] - ud[0])/(float)N;
		}
*/
		else if (!strcmp(str, "BeginTermSet"))
			parseTermSet(lVar, fin, n);	
		else if (!strcmp(str, "EndIfVariables"))	break;
		else if (!strcmp(str, "EndThenVariable")) 	break;
		else if (!strcmp(str, "")) 	;
		else fprintf (stderr,"Warning: check rulebase file: %d-th line\n", n);
	}
	// result:
	if (DEBUG)	printf ("%s variables: %d linguistic vars stored\n", ifthen, l);

	if (DEBUG) {
		for (int i = 0; i < getLinguisticVarLength(); i++){
			LinguisticVar *lv = getLinguisticVar(i);
			printf ("linguistic var: %s\n", lv->getName());
			for (int j = 0; j < lv->getLength(); j++){
				FuzzySet *fset = lv->getFuzzySet(j);	
				printf ("FuzzySet %s: %4.2f %4.2f\n", 
				fset->getName(),  fset->getUnivOfDiscourse()[0], fset->getUnivOfDiscourse()[1]);
			}
		}
	}
	
	return n;
}

//
//	Input Condition: Rule: %d
//	1 line check
//
int FuzzyRuleBase::parseRule(int l, FILE *fin, int &n)
{
	char strBuf[STRBUFSIZE];
	int i;
	CPtrList pList;

	for (i=0; i<10; i++)
		IfVarIndex[i] = 0;

	if (fgets(strBuf, STRBUFSIZE, fin) != NULL) {
		n ++;
		char *segStr = strBuf;
		char *str = getNextStr(segStr);
		// If 
		if (strcmp(str, "If")) return n;	// error
		str = getNextStr(segStr);
		for (i = 0; i < getIfPartLength(); i++) {
			// xi is
			//if (strcmp(str, getIfPartLinguisticVar(i)->getName())) return  n; // error
			if (!strcmp(str, getIfPartLinguisticVar(i)->getName())) 
			{
				IfVarIndex[i] = 1;

				str = getNextStr(segStr);	
				if (!strcmp(str, "is")) str = getNextStr(segStr);
				else return n; 	// error
				pList.AddTail(strdup(str));	
				str = getNextStr(segStr);
				if (!strcmp(str, "and")) str = getNextStr(segStr);
				else if (!strcmp(str, "Then")) { 
					str = getNextStr(segStr); break;
				}
			}
			else
				IfVarIndex[i] = 0;
		}	
		for (i = 0; i < getThenPartLength(); i++) {
			// xi is
			LinguisticVar *lv = getThenPartLinguisticVar(i);
			if (strcmp(str, getThenPartLinguisticVar(i)->getName())) return  n; // error
			str = getNextStr(segStr);	
			if (!strcmp(str, "is")) str = getNextStr(segStr);
			else return n; 	// error
			pList.AddTail(strdup(str));	
			str = getNextStr(segStr);	
			if (!strcmp(str, "and")) str = getNextStr(segStr);
			else break;
		}

		//float truth = atoi(str);
		pList.AddTail(strdup(str));
		
		setRulev(l, pList);
	}
	if(DEBUG)	printFuzzyRule(l);

	if (numRules < fuzzyRules.GetCount()) numRules = fuzzyRules.GetCount();
	
	if (DEBUG) {
		for (l = 0; l < numRules; l++) {
			printf ("%d : %d-th rule: ", n, l);
			FuzzyRule *frule = getRule(l);
			for (i = 0; i < frule->ifVars.GetCount(); i++)
				printf(" %s", (char *)frule->ifVars.GetAt(frule->ifVars.FindIndex(i)));
			for (i = 0; i < frule->thenVars.GetCount(); i++)
				printf(" %s", (char *)frule->thenVars.GetAt(frule->thenVars.FindIndex(i)));
			puts("");
		}
	}
	for (i = 0; i < pList.GetCount(); i++)
		delete (char *)pList.GetAt(pList.FindIndex(i));
	pList.RemoveAll();
	return n;
}

//
//	Input Condition: BeginRule
//	Output Condition: EndRule
//
int FuzzyRuleBase::parseRules(FILE *fin, int &n)
{
	char strBuf[STRBUFSIZE];

	int rules = 0;

	while(fgets(strBuf, STRBUFSIZE, fin) != NULL) {
		n++;
		char *segStr = strBuf;
		char *str = getNextStr(segStr);
		if (!strcmp(str, "Name:")) 		setName(getNextSeg(segStr)); 
		// default rule 에 대한 처리 부분 추가...
		else if (!strcmp(str, "Rule:")) 
		{
			rules++;
			parseRule(atoi(getNextStr(segStr))-1, fin, n);
		}
		else if (!strcmp(str, "EndRule"))
		{
			numRules = rules;
			return n;
		}
		else if (!strcmp(str, "")) 	;
		else if (!strncmp(str, "!", 1))  	;					// comments
		else fprintf (stderr,"Warning: check rulebase file: %d-th line\n", n);
	}
	return n;
}

//
//	Input Condition: BeginRuleBase
//	Output Condition: EndRuleBase
//
int FuzzyRuleBase::parseRuleBase(FILE *fin, int &n)
{
	char strBuf[STRBUFSIZE];

	while(fgets(strBuf, STRBUFSIZE, fin) != NULL) {
		n++;
		char *segStr = strBuf;
		char *str = getNextStr(segStr);
		if (!strcmp(str, "Name:")) setName(getNextStr(segStr)); 
		// not require
		else if (!strcmp(str, "DefuzzificationType:")) {
			str = getNextStr(segStr);	
			int def = 0; 
			if (!strcmp(str, "Maximum")) 				def = FuzzySet::Maximum;	
			else if (!strcmp(str, "CenterOfGravity")) 	def = FuzzySet::CenterOfGravity;
			else fprintf (stderr,"Warning: check rulebase file: %d-th line\n", n);
		}
		else if (!strcmp(str, "")) 	;
		else if (!strncmp(str, "!", 1))  	;					// comments
		else if (!strcmp(str, "BeginIfVariables"))
			parseVariable("if", fin, n);	
		else if (!strcmp(str, "BeginThenVariable")) 
			parseVariable("then", fin, n);	
		else if (!strcmp(str, "BeginRule")) 		
			parseRules(fin, n);	
		else if (!strcmp(str, "EndRuleBase")) 		
			return n;
		else fprintf (stderr,"Warning: check rulebase file: %d-th line\n", n);
	}
	return n;
}

int FuzzyRuleBase::load(char *filename)
{
	char strBuf[STRBUFSIZE];
	FILE *fin;

	if((fin = fopen(filename, "r")) == NULL) {
		fprintf (stderr,"cannot open rulebase file: %s\n", filename);
		return -1;
	}
	
	int n = 1;
	while(fgets(strBuf, STRBUFSIZE, fin) != NULL) {
		n++;
		char *segStr = strBuf;
		char *str = getNextStr(segStr);
		if (!strcmp(str, "BeginRuleBase"))
			parseRuleBase(fin, n);
		else if (!strncmp(str, "!", 1))  	;					// comments
		else if (!strcmp(str, "")) 	;
		else fprintf (stderr,"Warning: check rulebase file: %s 's %d-th line\n",
					 filename, n);
	}
	fclose(fin);

	if (DEBUG)	printf ("%s: %d lines\n", filename, n);

	//printFuzzyRuleBase(True);
	
	if (DEBUG) {
		for (int r = 0; r < getLinguisticVarLength(); r++) {
			LinguisticVar *lv = getLinguisticVar(r);
			printf ("%s: ", lv->getName());
			for (int i = 0; i < lv->getLength(); i++)
				lv->getFuzzySet(i)->print();
		}
		for (r = 0; r < numRules; r++) {
			printf ("%d-th rule: ", r);
			FuzzyRule *frule = getRule(r);
			for (int i = 0; i < frule->ifVars.GetCount(); i++)
				printf(" %s", (char *)frule->ifVars.GetAt(frule->ifVars.FindIndex(i)));
			for (i = 0; i < frule->thenVars.GetCount(); i++)
				printf(" %s", (char *)frule->thenVars.GetAt(frule->thenVars.FindIndex(i)));
			puts("");
		}
	}
	return 0;
}

void FuzzyRuleBase::LoadRules(void)
{
	// Neutral
	nor_upp = new FuzzyRuleBase;
	nor_upp->load(".\\FRB\\nor.upp.frb");
	nor_mid = new FuzzyRuleBase;
	nor_mid->load(".\\FRB\\nor.mid.frb");
	nor_low = new FuzzyRuleBase;
	nor_low->load(".\\FRB\\nor.low.frb");

	// Surprise
	sur_upp = new FuzzyRuleBase;
	sur_upp->load(".\\FRB\\sur.upp.frb");
	sur_mid = new FuzzyRuleBase;
	sur_mid->load(".\\FRB\\sur.mid.frb");
	sur_low = new FuzzyRuleBase;
	sur_low->load(".\\FRB\\sur.low.frb");

	// Fear
	fea_upp = new FuzzyRuleBase;
	fea_upp->load(".\\FRB\\fea.upp.frb");
	fea_mid = new FuzzyRuleBase;
	fea_mid->load(".\\FRB\\fea.mid.frb");
	fea_low = new FuzzyRuleBase;
	fea_low->load(".\\FRB\\fea.low.frb");

	// Disgust
	dis_upp = new FuzzyRuleBase;
	dis_upp->load(".\\FRB\\dis.upp.frb");
	dis_mid = new FuzzyRuleBase;
	dis_mid->load(".\\FRB\\dis.mid.frb");
	dis_low = new FuzzyRuleBase;
	dis_low->load(".\\FRB\\dis.low.frb");

	// Anger
	ang_upp = new FuzzyRuleBase;
	ang_upp->load(".\\FRB\\ang.upp.frb");
	ang_mid = new FuzzyRuleBase;
	ang_mid->load(".\\FRB\\ang.mid.frb");
	ang_low = new FuzzyRuleBase;
	ang_low->load(".\\FRB\\ang.low.frb");

	// Happy
	hap_mid = new FuzzyRuleBase;
	hap_mid->load(".\\FRB\\hap.mid.frb");
	hap_low = new FuzzyRuleBase;
	hap_low->load(".\\FRB\\hap.low.frb");

	// Sad
	sad_upp = new FuzzyRuleBase;
	sad_upp->load(".\\FRB\\sad.upp.frb");
	sad_mid = new FuzzyRuleBase;
	sad_mid->load(".\\FRB\\sad.mid.frb");
	sad_low = new FuzzyRuleBase;
	sad_low->load(".\\FRB\\sad.low.frb");
}

void FuzzyRuleBase::FreeRules(void)
{
	delete sad_low;
	delete sad_mid;
	delete sad_upp;
	delete hap_low;
	delete hap_mid;
	delete ang_low;
	delete ang_mid;
	delete ang_upp;
	delete dis_low;
	delete dis_mid;
	delete dis_upp;
	delete fea_low;
	delete fea_mid;
	delete fea_upp;
	delete sur_low;
	delete sur_mid;
	delete sur_upp;
	delete nor_low;
	delete nor_mid;
	delete nor_upp;
}

void FuzzyRuleBase::InferRules(CPtrList *uList, CPtrList *mList, CPtrList *lList, int write)
{
	FILE *fp;

	if (write)
	{
		fp = fopen(".\\result.txt", "at"); 
	}

	nor_res = nor_upp->inference(*uList);
	sur_res = sur_upp->inference(*uList);
	fea_res = fea_upp->inference(*uList);
	dis_res = dis_upp->inference(*uList);
	ang_res = ang_upp->inference(*uList);
	sad_res = sad_upp->inference(*uList);

	if (write)
	{
		fprintf(fp, "\nResult(U) : %5.3f %5.3f %5.3f %5.3f %5.3f 0.000 %5.3f\n", 
			nor_res->defuzzifier(FuzzySet::CenterOfGravity),
			sur_res->defuzzifier(FuzzySet::CenterOfGravity),
			fea_res->defuzzifier(FuzzySet::CenterOfGravity),
			dis_res->defuzzifier(FuzzySet::CenterOfGravity),
			ang_res->defuzzifier(FuzzySet::CenterOfGravity),
			sad_res->defuzzifier(FuzzySet::CenterOfGravity));
	}

	nor_res = nor_mid->inference(*mList);
	sur_res = sur_mid->inference(*mList);
	fea_res = fea_mid->inference(*mList);
	dis_res = dis_mid->inference(*mList);
	ang_res = ang_mid->inference(*mList);
	hap_res = hap_mid->inference(*mList);
	sad_res = sad_mid->inference(*mList);

	if (write)
	{
		fprintf(fp, "\nResult(M) : %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f\n", 
			nor_res->defuzzifier(FuzzySet::CenterOfGravity),
			sur_res->defuzzifier(FuzzySet::CenterOfGravity),
			fea_res->defuzzifier(FuzzySet::CenterOfGravity),
			dis_res->defuzzifier(FuzzySet::CenterOfGravity),
			ang_res->defuzzifier(FuzzySet::CenterOfGravity),
			hap_res->defuzzifier(FuzzySet::CenterOfGravity),
			sad_res->defuzzifier(FuzzySet::CenterOfGravity));
	}

	nor_res = nor_low->inference(*lList);
	sur_res = sur_low->inference(*lList);
	fea_res = fea_low->inference(*lList);
	dis_res = dis_low->inference(*lList);
	ang_res = ang_low->inference(*lList);
	hap_res = hap_low->inference(*lList);
	sad_res = sad_low->inference(*lList);

	if (write)
	{
		fprintf(fp, "\nResult(L) : %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f %5.3f\n", 
			nor_res->defuzzifier(FuzzySet::CenterOfGravity),
			sur_res->defuzzifier(FuzzySet::CenterOfGravity),
			fea_res->defuzzifier(FuzzySet::CenterOfGravity),
			dis_res->defuzzifier(FuzzySet::CenterOfGravity),
			ang_res->defuzzifier(FuzzySet::CenterOfGravity),
			hap_res->defuzzifier(FuzzySet::CenterOfGravity),
			sad_res->defuzzifier(FuzzySet::CenterOfGravity));
	}

	if (write)
		fclose(fp);
}

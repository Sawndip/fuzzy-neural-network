
#include <stdio.h>
#include <iostream.h>
#include "fuzzyRule.h"

void main()
{
	FuzzyRuleBase faceExpRuleBase(2, 1, 2);
	FuzzyRuleBase &rb = faceExpRuleBase;

/*
	rb.setLinguisticVar(0, new LinguisticVar("x1", "L", "M"));
	rb.setLinguisticVar(1, new LinguisticVar("x2", "L", "M"));
	rb.setLinguisticVar(2, new LinguisticVar("emotion", "Surprise", "Normal"));
	rb.setLinguisticVar(0, "x1", "L", "M");
	rb.setLinguisticVar(1, "x2", "L", "M");
	rb.setLinguisticVar(2, "emotion", "Surprise", "Normal");
*/
	float pas[2][2] = {{ 0.2, 0.2}, {0.8, 0.2}};
	FuzzySet x1L("L",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[0], 2),
			 x1M("M",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[1], 2);
	FuzzySet x2L("L",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[0], 2),
			 x2M("M",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[1], 2);
	FuzzySet surprise("Surprise",0.0, 1.0, .02, FuzzySet::Triangular, pas[0], 2),
			 normal("Normal",0.0, 1.0, .02, FuzzySet::Triangular, pas[1], 2);

	rb.setLinguisticVar(0, "x1", &x1L, &x2M, NULL);
	rb.setLinguisticVar(1, "x2", &x2L, &x2M, NULL);
	rb.setLinguisticVar(2, "emotion", &surprise, &normal, NULL);
	rb.setRule(0, "L", "M", "Surprise", NULL);
	rb.setRule(1, "M", "L", "Normal", NULL);

	// print out rules

	printf ("Fuzzy Rule:\n");
	for (int l = 0; l < rb.getLength(); l++) {
		printf ("IF ");
		for (int i = 0; i < rb.getIfPartLength(); i++) {
			LinguisticVar	*lingVar = rb.getLinguisticVar(i);
			char *varStr = rb.getLinguisticVar(i)->getName();
			char *fuzzyStr = (char *)rb.getRule(l)->ifVars[i];
			printf ("%s is %s ",  varStr, fuzzyStr);
			if (i != rb.getIfPartLength() - 1)
				printf ("and ");
		}	
		
		printf ("THEN ");
		for (i = 0; i < rb.getThenPartLength(); i++) {
			char *varStr = rb.getLinguisticVar(rb.getIfPartLength()+i)->getName();
			char *fuzzyStr = (char *)rb.getRule(l)->thenVars[i];
			printf ("%s is %s ",  varStr, fuzzyStr);
			if (i != rb.getThenPartLength() - 1)
				printf ("and ");
		}	
		printf ("\n");
	}

	x1L.print(); x1M.print();
	x2L.print(); x2M.print();
	surprise.print(); normal.print();

//	FuzzySet in1("input1",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[0], 2),
//			 in2("input2",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[1], 2);
	FuzzySet in1(x1L), in2(x2M);
	SbPList inList;
	inList.append(&in1); inList.append(&in2);
	FuzzySet *result = rb.inference(inList);
	result->print();

	FuzzySet in21(x1M), in22(x2L);
	SbPList in2List;
	in2List.append(&in21); in2List.append(&in22);
	FuzzySet *result2 = rb.inference(in2List);
	result2->print();
}

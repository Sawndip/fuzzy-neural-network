
#include <stdio.h>
#include <iostream.h>
#include "fuzzyRule.h"

// 7 rulebases
const int nIF = 2, nTHEN = 1;
FuzzyRuleBase 	normalRuleBase	(	 "normal", nIF, nTHEN, 9),
				surpriseRuleBase(  "surprise", nIF, nTHEN, 9),
				fearRuleBase	(	   "fear", nIF, nTHEN, 9),
				disgustRuleBase	(   "disgust", nIF, nTHEN, 9),
				angerRuleBase	(     "anger", nIF, nTHEN, 9),
				sadRuleBase		(   "sadness", nIF, nTHEN, 9),
				happyRuleBase	( "happiness", nIF, nTHEN, 9);

float trapPas[3][4] = { { 0.0, 0.25, 0.0,  0.25}, 
						{ 0.4, 0.6,  0.1,  0.1}, 
						{ 0.75, 1.0,  0.25, 0.0}};
float triPas[3][2] = {{0.25, 0.5}, {0.5, 0.5}, {0.75, 0.5}};

// x1 : eye openness
FuzzySet x1L("L",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, trapPas[0], 4),
		 x1M("M",0.0, 1.0, 0.02, FuzzySet::Triangular,  triPas[1], 2),
		 x1H("H",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, trapPas[2], 4);
// x2 : mout openness
FuzzySet x2L("L",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, trapPas[0], 4),
		 x2M("M",0.0, 1.0, 0.02, FuzzySet::Triangular,  triPas[1], 2),
		 x2H("H",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, trapPas[2], 4);
// y : happiness
FuzzySet y1L("L",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, trapPas[0], 4),
		 y1M("M",0.0, 1.0, 0.02, FuzzySet::Triangular,  triPas[1], 2),
		 y1H("H",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, trapPas[2], 4);

SbPList *feRulebases = NULL;
int debug = 0;
static	int gflg = 0, pflg = 0;

void initFuzzyRulebase()
{
	FuzzyRuleBase *fileRuleBase = new FuzzyRuleBase;
	fileRuleBase->load("happy.frb");
	exit(0);

	feRulebases = new SbPList;

	feRulebases->append(&normalRuleBase);
	feRulebases->append(&surpriseRuleBase);
	feRulebases->append(&fearRuleBase);
	feRulebases->append(&disgustRuleBase);
	feRulebases->append(&angerRuleBase);
	feRulebases->append(&sadRuleBase);
	feRulebases->append(&happyRuleBase);

	for (int n = 0; n < feRulebases->getLength(); n++) {
		FuzzyRuleBase *rb = (FuzzyRuleBase *)((*feRulebases)[n]);
		rb->setLinguisticVar(0, "x1", &x1L, &x1M, &x1H, NULL);
		rb->setLinguisticVar(1, "x2", &x2L, &x2M, &x2H, NULL);
		rb->setLinguisticVar(2, rb->getName(), &y1L, &y1M, &y1H, NULL);
	}

	//
	// set Rulebase
	//
	n = 0;
	normalRuleBase.setRule(n++, "H", "L", "H");
		normalRuleBase.setRule(n++, "L", "L", "L");
		normalRuleBase.setRule(n++, "L", "M", "L");
		normalRuleBase.setRule(n++, "L", "H", "L"); 
		normalRuleBase.setRule(n++, "M", "L", "L"); 
		normalRuleBase.setRule(n++, "M", "M", "L"); 
		normalRuleBase.setRule(n++, "M", "H", "L"); 
//		normalRuleBase.setRule(n++, "H", "L", "L"); 
		normalRuleBase.setRule(n++, "H", "M", "L"); 
		normalRuleBase.setRule(n++, "H", "H", "L"); 
	n = 0;
	surpriseRuleBase.setRule(n++, "H", "H", "H"); 
		surpriseRuleBase.setRule(n++, "L", "L", "L"); 
		surpriseRuleBase.setRule(n++, "L", "M", "L"); 
		surpriseRuleBase.setRule(n++, "L", "H", "L"); 
		surpriseRuleBase.setRule(n++, "M", "L", "L");
		surpriseRuleBase.setRule(n++, "M", "M", "L");
		surpriseRuleBase.setRule(n++, "M", "H", "L");
		surpriseRuleBase.setRule(n++, "H", "L", "L");
		surpriseRuleBase.setRule(n++, "H", "M", "L");
//		surpriseRuleBase.setRule(n++, "H", "H", "L");
	n = 0;
	fearRuleBase.setRule(n++, "H", "H", "H");
	fearRuleBase.setRule(n++, "H", "M", "H");
		fearRuleBase.setRule(n++, "L", "L", "L");
		fearRuleBase.setRule(n++, "L", "M", "L");
		fearRuleBase.setRule(n++, "L", "H", "L");
		fearRuleBase.setRule(n++, "M", "L", "L");
		fearRuleBase.setRule(n++, "M", "M", "L");
		fearRuleBase.setRule(n++, "M", "H", "L");
		fearRuleBase.setRule(n++, "H", "L", "L");
//		fearRuleBase.setRule(n++, "H", "M", "L");
//		fearRuleBase.setRule(n++0, "H", "H", "L");
	n = 0;
	disgustRuleBase.setRule(n++, "L", "H", "H");
	disgustRuleBase.setRule(n++, "L", "L", "H");
		disgustRuleBase.setRule(n++, "L", "M", "L");
//		disgustRuleBase.setRule(n++, "L", "H", "L");
		disgustRuleBase.setRule(n++, "M", "L", "L");
		disgustRuleBase.setRule(n++, "M", "M", "L");
		disgustRuleBase.setRule(n++, "M", "H", "L");
		disgustRuleBase.setRule(n++, "H", "L", "L");
		disgustRuleBase.setRule(n++, "H", "M", "L");
		disgustRuleBase.setRule(n++, "H", "H", "L");
	n = 0;
	angerRuleBase.setRule(n++, "L", "L", "H");
	angerRuleBase.setRule(n++, "L", "H", "H");
//		angerRuleBase.setRule(n++, "L", "L", "L");
		angerRuleBase.setRule(n++, "L", "M", "L");
//		angerRuleBase.setRule(n++, "L", "H", "L");
		angerRuleBase.setRule(n++, "M", "L", "L");
		angerRuleBase.setRule(n++, "M", "M", "L");
		angerRuleBase.setRule(n++, "M", "H", "L");
		angerRuleBase.setRule(n++, "H", "L", "L");
		angerRuleBase.setRule(n++, "H", "M", "L");
		angerRuleBase.setRule(n++, "H", "H", "L");
	n = 0;
	sadRuleBase.setRule(n++, "L", "L", "H");
//		sadRuleBase.setRule(n++, "L", "L", "L");
		sadRuleBase.setRule(n++, "L", "M", "L");
		sadRuleBase.setRule(n++, "L", "H", "L");
		sadRuleBase.setRule(n++, "M", "L", "L");
		sadRuleBase.setRule(n++, "M", "M", "L");
		sadRuleBase.setRule(n++, "M", "H", "L");
		sadRuleBase.setRule(n++, "H", "L", "L");
		sadRuleBase.setRule(n++, "H", "M", "L");
		sadRuleBase.setRule(n++, "H", "H", "L");
	n = 0;
	happyRuleBase.setRule(n++, "L", "H", "H");
		happyRuleBase.setRule(n++, "L", "L", "L");
		happyRuleBase.setRule(n++, "L", "M", "L");
//		happyRuleBase.setRule(n++, "L", "H", "L");
		happyRuleBase.setRule(n++, "M", "L", "L");
		happyRuleBase.setRule(n++, "M", "M", "L");
		happyRuleBase.setRule(n++, "M", "H", "L");
		happyRuleBase.setRule(n++, "H", "L", "L");
		happyRuleBase.setRule(n++, "H", "M", "L");
		happyRuleBase.setRule(n++, "H", "H", "L");

	// print out rules
	for (n = 0; n < feRulebases->getLength(); n++) {
		FuzzyRuleBase *rb = (FuzzyRuleBase *)((*feRulebases)[n]);
		rb->printFuzzyRuleBase(n == 0 ? 1 : 0);
	}
}

static char *gnucommands[] = { 
	"set yrange [0:1]",
	"plot 'membership.dat' with lines",
	"pause 120"
};

void graphs(SbPList &fuzzySets)
{
	char *dataFilename = "tmp";
	char *gnufile = "tmp.gnuplot";

	printf ("%d fuzzySets\n", fuzzySets.getLength());
	FILE *fout;
	char fname[200];
	for (int j = 0; j < fuzzySets.getLength(); j++) {
		sprintf (fname,"%s%d.dat", dataFilename, j);
		if ((fout = fopen(fname,"w")) == NULL) return ;

		FuzzySet *fst = (FuzzySet *)fuzzySets[j];
		for (int i = 0; i < fst->getLength(); i++) 
			fprintf (fout, "%4.2f %4.2f\n", fst->getX(i), 
						fst->getIndexedValue(i));
		fclose(fout);
	}

	if ((fout = fopen(gnufile,"w")) == NULL) return ;

	for (int i = 0; i < 3; i++) {
		if (i == 1) {
			fprintf (fout, "plot ");
			for (int j = 0; j < fuzzySets.getLength(); j++) {
				FuzzySet *fst = (FuzzySet *)fuzzySets[j];
				sprintf (fname,"%s%d.dat", dataFilename, j);
				fprintf (fout, "\'%s\' using 1:2 title \'%s\' with lines", 
					fname, fst->getName());
				if (j != fuzzySets.getLength()-1)
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

void recall()
{
//	FuzzySet in1("input1",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[0], 2),
//			 in2("input2",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[1], 2);
	FuzzySet in11(x1L), in12(x2H);
	FuzzySet in21(x1H), in22(x2L);
	SbPList in1List, in2List;
	in1List.append(&in11); in1List.append(&in12);
	in2List.append(&in21); in2List.append(&in22);
	SbPList rst;
	for (int n = 0; n < feRulebases->getLength(); n++) {
		FuzzyRuleBase *rb = (FuzzyRuleBase *)((*feRulebases)[n]);
		FuzzySet *result = rb->inference(in1List);
		result->setName(rb->getName());
		if (pflg) result->print();
		if (gflg) result->graph();
		float def = result->defuzzifier(FuzzySet::CenterOfGravity);
		printf("defuzzification by %s rulebase: %3.1f\n", rb->getName(), def);
		rst.append(result);
	}

//	graphs(rst);
/*
	for (n = 0; n < feRulebases->getLength(); n++) {
		FuzzyRuleBase *rb = (FuzzyRuleBase *)((*feRulebases)[n]);
		FuzzySet *result = rb->inference(in2List);
		result->setName(rb->getName());
		if (pflg) result->print();
		if (gflg) result->graph();
		float def = result->defuzzifier(FuzzySet::CenterOfGravity);
		printf("defuzzification: %3.1f\n", def);
	}
*/
}

void main(int argc, char *argv[])
{
	int c;
    extern char *optarg;
	extern int optind;
	int errflg = 0;
    
	while ((c = getopt(argc, argv, "gp")) != EOF)
		   switch (c) {
		   case 'g': gflg = 1; break;
		   case 'p': pflg = 1; break;
		   case '?':
				errflg++;
		   }
	if (errflg) {
	   (void)fprintf(stderr, "usage: %s [-g|-p]\n", argv[0]);
		exit (2);
	}

	initFuzzyRulebase();
	recall();
}

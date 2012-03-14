//#include <afxcoll.h>
#include <afx.h>
#include <stdio.h>
#include <stdlib.h>
#include "myresource.h"
//#include <iostream.h>

#include "fuzzyRule.h"
//#include "NNfunction.h"
#include "myFNN.h"
//#include "myHMM.h"

#define	LOW	(0.2f)
#define	HI	(0.8f)

#define	SAVE_FLAG	0

void TestFuzzy(void);
void TestNN(void);
void TestFNN(void);		// first version
//void TestFNN2(void);
void TestFNN3(void);	// feature selection test
void TestFNN4(void);	// file loading
void TestFNNmy(void);
void TestFNNX(void);
//void TestHMM(void);

void main(void)
{
	//TestFuzzy();
	//TestNN();
	//TestFNN();
	TestFNN3();	// load previous trained FNN
	//TestFNN4();	// feature-selection
	//TestFNNmy();	// facial expression
	//TestFNNX();		// Test with XOR problem.
	//TestHMM();
}

void TestFNN(void)
{
	CTime t = CTime::GetCurrentTime();

	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	int i,j;

	myFNN fnn;
	int example = 7;

	//float data[10] = {0.11, 0.91, 0.41, 0.43, 0.39, 0.09, 0.13, 0.93, 0.88, 0.86};
	//float output[10] = {0, 2, 1, 1, 1, 0, 0, 2, 2, 2};
	//float mean2[2] = {0.3f, 0.7f};
	//float mean3[2] = {0.2f, 0.5f, 0.8f};
	//float std2[2] = {0.3f, 0.3f};
	//float std3[2] = {0.2f, 0.2f, 0.2f};

	// FNN structure setting
	// layer structure
	int layer[] = {5, 10, 32, 14, 7};
	//int layer[] = {2, 4, 4, 4, 2};
	
	// in_layer
	int ilayer[] = {2, 2, 2, 2, 2};
	float imean[] = {0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f};
	float istd[] = {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f};

	// out_layer
	int olayer[] = {2, 2, 2, 2, 2, 2, 2};
	float omean[] = {0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f};
	float ostd[] = {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f};

	// overall param.
	fnn.setFNN(layer, 5, ilayer, olayer, 0.15, 1e-6, 1e4);
	fnn.initFNN();


	// mean & std init.
	int bias = 0;
	for (i=0; i<layer[0]; i++)	
	{
		if (i>0)
			bias += ilayer[i-1];

		fnn.kSOCFNN((imean + bias), (istd + bias), 0, i);
	}

	bias = 0;
	for (j=0; j<layer[4]; j++)
	{
		if (j>0)
			bias += olayer[j-1];

		fnn.kSOCFNN((omean + bias), (ostd + bias), 1, j);
	}

	fnn.ShowFNND();
	fnn.ShowMF(0);
	fnn.ShowMF(1);

	// input sample
	float tinput[] = {HI, LOW, LOW, LOW, LOW,
	HI, LOW, LOW, HI, HI,
	HI, LOW, HI, HI, HI,
	LOW, HI, HI, LOW, LOW,
	LOW, HI, LOW, LOW, LOW,
	HI, LOW, HI, HI, LOW,
	LOW, LOW, HI, LOW, LOW};

	float ttinput[] = {HI, LOW, HI, LOW, LOW,
		LOW, LOW, LOW, LOW, LOW};

	float sdinput[] = {LOW, HI, HI, HI, LOW};
	float sdoutput[] = {LOW, LOW, LOW, LOW, LOW, HI, LOW};


	float dinput[] = {LOW, LOW, HI, HI, LOW,
		HI, LOW, LOW, LOW, HI,
		HI, HI, LOW, LOW, LOW,
		LOW, HI, HI, LOW, LOW,
		LOW, HI, HI, HI, LOW};

	/*
	float tinput[] = {LOW, LOW,
		LOW, HI,
		HI, LOW,
		HI, HI};
	*/

	// output sample
	float toutput[] = {HI, LOW, LOW, LOW, LOW, LOW, LOW,
	LOW, HI, LOW, LOW, LOW, LOW, LOW,
	LOW, LOW, HI, LOW, LOW, LOW, LOW,
	LOW, LOW, LOW, HI, LOW, LOW, LOW,
	LOW, LOW, LOW, LOW, HI, LOW, LOW,
	LOW, LOW, LOW, LOW, LOW, HI, LOW,
	LOW, LOW, LOW, LOW, LOW, LOW, HI};
	/*
	float toutput[] = {LOW, LOW,
		HI, LOW,
		HI, HI,
		LOW, HI};
		*/

	// dummy output
	float youtput[] = {0, 0, 0, 0, 0, 0, 0};
	//float youtput[] = {0, 0};

	//example = 1;
	// comp. learning for consequence of fuzzy rules
	fnn.compFNN(tinput, toutput, example);
	//fnn.compFNN(sdinput, sdoutput, example);

	// backprop. learning of FNN
	fnn.learnFNN(tinput, toutput, example);
	//fnn.learnFNN(sdinput, sdoutput, example);

	fnn.ShowMF(0);
	fnn.ShowMF(1);


	//fnn.aoutFNN(-1);
	//getchar();

	// time check!
	//FILE *fp;
	//fp = fopen("c:\\matlabr11\\work\\timeFNN.txt", "at");

	// evaluation
	for (i=0; i<example; i++)		
	{
		CHECK_TIME_START;
		fnn.evalFNN((tinput + i*layer[0]), youtput, 0);
		CHECK_TIME_END(Time1, err);
		
		fnn.aoutFNN(4);
		
		CHECK_TIME_START;
		fnn.si4FNN(i);
		CHECK_TIME_END(Time2, err);

		//fprintf(fp, "%f %f\n", Time1, Time2);
	}
	
	/*
	fnn.evalFNN(ttinput, youtput, 0);
	fnn.aoutFNN(4);
	fnn.si4FNN(0);

	fnn.evalFNN(ttinput + layer[0], youtput, 0);
	fnn.aoutFNN(4);
	fnn.si4FNN(1);
	*/

	/*
	for (i=0; i<5; i++)
	{
		fnn.evalFNN(dinput + i*layer[0], youtput, 0);
		fnn.aoutFNN(4);
		fnn.si4FNN(5);
	}
	*/


	//fclose(fp);

	//fnn.kSOC(data, 10, mean, std, 3);
	fnn.ruleFNN();

	char str[100];

	if (SAVE_FLAG)
	{
		sprintf(str, "c:\\matlabr11\\work\\%d%d%d%d%d.dat", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute());
		fnn.saveFNN(str);
	}

	fnn.freeFNN();
}

// 2002/11/10 mod.
void TestFNN3(void)
{
	/*
	CTime t = CTime::GetCurrentTime();

	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;
	*/

	int i, j;
	char str[100];

	int std_array[6] = {1, 2, 5, 10, 20, 40};
	int node_no[] = {0, 1, 2, 2, 3, 4, 5};

	int nodes1[] = {5};
	int nodes2[] = {0, 5};
	int nodes3[] = {5, 6};
	int nodes4[] = {0, 5, 6};
	int nodes5[] = {0, 5, 6, 8};
	int nodes6[] = {0, 2, 5, 6, 8};

	for (i=0; i<7; i++)
	{
		myFNN fnn;

		fnn.loadFNN(".\\2002710439.dat");
		fnn.initFNN();
		fnn.ruleCombination();

		switch(i)
		{
			case 1:
				fnn.deleteInputNodes(nodes1, node_no[i]);
				break;
			case 2:
				fnn.deleteInputNodes(nodes2, node_no[i]);
				break;
			case 3:
				fnn.deleteInputNodes(nodes3, node_no[i]);
				break;
			case 4:
				fnn.deleteInputNodes(nodes4, node_no[i]);
				break;
			case 5:
				fnn.deleteInputNodes(nodes5, node_no[i]);
				break;
			case 6:
				fnn.deleteInputNodes(nodes6, node_no[i]);
				break;
			default:
				break;
		}
	
		for (j=0; j<6; j++)
		{
			sprintf(str, ".\\FNN020902std%d.dat", std_array[j]);
			fnn.loadDataN1(str, 5, 7, 700, 700, 0, 0);

			printf("REC#%d with stdv %d \n", i, std_array[j]);

			//CHECK_TIME_START;
			fnn.recTrain();
			//CHECK_TIME_END(Time1, err);
			//printf("%f \n", Time1);

			/*
			// evaluation
			for (i=0; i<example; i++)		
			{
				fnn.evalFNN((tinput + i*fnn.layer[0]), youtput, 0);
				fnn.aoutFNN(4);
				fnn.si4FNN(i);
			}
			*/
			
			//fnn.kSOC(data, 10, mean, std, 3);
			//fnn.ruleFNN();
		}

		/*
		fp = fopen(".\\FNN021128SI.dat", "awt");
		fprintf(fp, "\n");
		fclose(fp);
		*/

		fnn.freeFNN();
	}
}

// 020722
void TestFNN4(void)
{
	int i;//,j;

	myFNN fnn;
	fnn.loadDataN1(".\\HandOri002.dat", 3, 4, 4, 4, 0);
	//fnn.loadDataN1(".\\iris.dat", 4, 3, 150, 100, 50);
	//fnn.loadDataN1(".\\DF_class71.dat", 4, 3, 18, 12, 6);

	//fnn.loadFNN(".\\2002710439.dat");
	/*
	int layer[] = {4, 10, 32, 6, 3};
	int ilayer[] = {4, 2, 2, 2};
	int olayer[] = {2, 2, 2};
	*/
	/*
	int layer[] = {4, 12, 81, 6, 3};
	int ilayer[] = {3, 3, 3, 3};
	int olayer[] = {2, 2, 2};
	*/
	int layer[] = {3, 8, 16, 16, 8};
	int ilayer[] = {4, 2, 2};
	int olayer[] = {2, 2, 2, 2, 2, 2, 2, 2};


	fnn.setFNN(layer, 5, ilayer, olayer, 0.15, 1e-5, 1e5);
	fnn.initFNN();
	fnn.setMembership();

	fnn.compFNN(fnn.lin, fnn.lout, fnn.l_no);
	//fnn.ShowFNND();
	//fnn.ruleCombination();
	//fnn.ShowFNND();

	fnn.learnFNN(fnn.lin, fnn.lout, fnn.l_no);

	//cvvWaitKey(0);

	fnn.recTrain();
	//fnn.recTest();

	//fnn.saveFNN(".\\HSRecGroup4.dat");

	fnn.freeFNN();
}

// 20020830
void TestFNNmy(void)
{
	int i;//,j;

	myFNN fnn;
	//fnn.loadDataN1(".\\FNNin08221.dat", 5, 7, 7, 7, 0);
	fnn.loadDataN1(".\\FNNin020906.dat", 5, 5, 5, 5, 0, 0);
	//fnn.loadDataN1(".\\FNN020902std10.dat", 5, 7, 700, 700, 0, 0);
	
	//fnn.loadFNN(".\\2002710439.dat");
	int layer[] = {5, 10, 32, 10, 5};
	int ilayer[] = {2, 2, 2, 2, 2};
	int olayer[] = {2, 2, 2, 2, 2};

	fnn.setFNN(layer, 5, ilayer, olayer, 0.15, 1e-6, 1e4);
	fnn.initFNN();
	fnn.setMembership();
	
	fnn.compFNN(fnn.lin, fnn.lout, fnn.l_no);
	fnn.ShowFNND();

	fnn.ruleCombination();
	fnn.ShowFNND();

	//fnn.ioHistogram();
	//int node[] = {1,3,4,7,8};
	//fnn.deleteInputNodes(node, 5);

	fnn.learnFNN(fnn.lin, fnn.lout, fnn.l_no);

	fnn.recTrain();
	fnn.recTest();

	//fnn.saveFNN(".\\HSRecGroup4.dat");

	fnn.freeFNN();
}

void TestFNNX(void)
{
	int i;

	myFNN fnn;
	fnn.loadDataN1(".\\xor.dat", 2, 2, 4, 4, 0, 0);

	int layer[] = {2, 4, 4, 4, 2};
	int ilayer[] = {2, 2};
	int olayer[] = {2, 2};

	fnn.setFNN(layer, 5, ilayer, olayer, 0.15, 1e-4, 1e4);
	fnn.initFNN();
	fnn.setMembership();

	fnn.compFNN(fnn.lin, fnn.lout, fnn.l_no);
	fnn.ShowFNND();
	fnn.ruleCombination();
	fnn.ShowFNND();

	fnn.learnFNN(fnn.lin, fnn.lout, fnn.l_no);

	fnn.recTrain();
	fnn.recTest();

	fnn.freeFNN();
}

void TestFuzzy(void)
{
	FuzzyRuleBase myRule;

	// 1. Loading Rules!
	printf("Loading Rules...\n");

	myRule.LoadRules();

	// 2. Make input membership
	printf("Make input membership...\n");

	float bi[2][4] = {{0.0f, 0.3, 0.0f, 0.4}, {0.7, 1.0f, 0.4, 0.0}};
	float tri[3][4] = {{0.0f, 0.25, 0.0f, 0.25}, {0.5, 0.25, 0.0f, 0.0}, {0.75, 1.0f, 0.25, 0.0}};
	float quad[4][4] = {{0.0f, 0.2, 0.0f, 0.2}, {0.4, 0.2, 0.0f, 0.0}, {0.6, 0.2, 0.0f, 0.0}, {0.8, 1.0f, 0.2, 0.0}};

	// Upper face
	FuzzySet u1l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[0],4);
	FuzzySet u1m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, tri[1],2);
	FuzzySet u1h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[2],4);
	FuzzySet u2l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[0],4);
	FuzzySet u2m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, tri[1],2);
	FuzzySet u2h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[2],4);
	FuzzySet u3l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[0],4);
	FuzzySet u3h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[1],4);

	// Middle face
	FuzzySet m1l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[0],4);
	FuzzySet m1m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, tri[1],2);
	FuzzySet m1h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[2],4);
	FuzzySet m2m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[0],4);
	FuzzySet m2h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[1],4);
	FuzzySet m3m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[0],4);
	FuzzySet m3h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[1],4);
	FuzzySet m4l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[0],4);
	FuzzySet m4m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, tri[1],2);
	FuzzySet m4h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[2],4);

	// Lower face
	FuzzySet l1l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, quad[0], 4);
	FuzzySet l1m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, quad[1], 2);
	FuzzySet l1h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, quad[2], 2);
	FuzzySet l1vh("VH", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, quad[3], 4);
	FuzzySet l2l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[0],4);
	FuzzySet l2m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, tri[1],2);
	FuzzySet l2h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[2],4);
	FuzzySet l3l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, quad[0], 4);
	FuzzySet l3m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, quad[1], 2);
	FuzzySet l3h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, quad[2], 2);
	FuzzySet l3vh("VH", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, quad[3], 4);
	FuzzySet l4l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[0],4);
	FuzzySet l4m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, tri[1],2);
	FuzzySet l4h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, tri[2],4);
	FuzzySet l5m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[0],4);
	FuzzySet l5h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[1],4);
	FuzzySet l6l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, quad[0], 4);
	FuzzySet l6m("M", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, quad[1], 2);
	FuzzySet l6h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, quad[2], 2);
	FuzzySet l6vh("VH", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, quad[3], 4);
	FuzzySet l7l("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[0],4);
	FuzzySet l7h("H", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Trapezoidal, bi[1],4);

	// 3. Inference given ideal membership!
	printf("Inference...\n");

	// 1
	CPtrList uList;
	uList.AddTail(&u1m);
	uList.AddTail(&u2h);
	uList.AddTail(&u3l);

	CPtrList mList;
	mList.AddTail(&m1m);
	mList.AddTail(&m2m);
	mList.AddTail(&m3m);
	mList.AddTail(&m4m);

	CPtrList lList;
	lList.AddTail(&l1l);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 2
	lList.RemoveAll();
	lList.AddTail(&l1l);
	lList.AddTail(&l2l);
	lList.AddTail(&l3m);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 3
	uList.RemoveAll();
	uList.AddTail(&u1h);
	uList.AddTail(&u2h);
	uList.AddTail(&u3l);

	mList.RemoveAll();
	mList.AddTail(&m1h);
	mList.AddTail(&m2m);
	mList.AddTail(&m3m);
	mList.AddTail(&m4m);

	lList.RemoveAll();
	lList.AddTail(&l1h);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4h);
	lList.AddTail(&l5h);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 4
	lList.RemoveAll();
	lList.AddTail(&l1h);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4m);
	lList.AddTail(&l5h);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 5
	uList.RemoveAll();
	uList.AddTail(&u1h);
	uList.AddTail(&u2l);
	uList.AddTail(&u3h);

	mList.RemoveAll();
	mList.AddTail(&m1m);
	mList.AddTail(&m2m);
	mList.AddTail(&m3m);
	mList.AddTail(&m4m);

	lList.RemoveAll();
	lList.AddTail(&l1h);
	lList.AddTail(&l2m);
	lList.AddTail(&l3h);
	lList.AddTail(&l4h);
	lList.AddTail(&l5m);
	lList.AddTail(&l6h);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 6
	lList.RemoveAll();
	lList.AddTail(&l1m);
	lList.AddTail(&l2m);
	lList.AddTail(&l3h);
	lList.AddTail(&l4h);
	lList.AddTail(&l5m);
	lList.AddTail(&l6h);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 7
	uList.RemoveAll();
	uList.AddTail(&u1l);
	uList.AddTail(&u2h);
	uList.AddTail(&u3l);

	mList.RemoveAll();
	mList.AddTail(&m1l);
	mList.AddTail(&m2h);
	mList.AddTail(&m3m);
	mList.AddTail(&m4m);

	lList.RemoveAll();
	lList.AddTail(&l1h);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 8
	lList.RemoveAll();
	lList.AddTail(&l1l);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6vh);
	lList.AddTail(&l7h);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 9
	lList.RemoveAll();
	lList.AddTail(&l1l);
	lList.AddTail(&l2l);
	lList.AddTail(&l3m);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6vh);
	lList.AddTail(&l7h);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 10
	uList.RemoveAll();
	uList.AddTail(&u1l);
	uList.AddTail(&u2l);
	uList.AddTail(&u3l);

	mList.RemoveAll();
	mList.AddTail(&m1l);
	mList.AddTail(&m2m);
	mList.AddTail(&m3m);
	mList.AddTail(&m4h);

	lList.RemoveAll();
	lList.AddTail(&l1l);
	lList.AddTail(&l2l);
	lList.AddTail(&l3m);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 11
	lList.RemoveAll();
	lList.AddTail(&l1vh);
	lList.AddTail(&l2h);
	lList.AddTail(&l3m);
	lList.AddTail(&l4l);
	lList.AddTail(&l5h);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 12
	uList.RemoveAll();
	uList.AddTail(&u1l);
	uList.AddTail(&u2l);
	uList.AddTail(&u3l);

	mList.RemoveAll();
	mList.AddTail(&m1l);
	mList.AddTail(&m2h);
	mList.AddTail(&m3h);
	mList.AddTail(&m4m);

	lList.RemoveAll();
	lList.AddTail(&l1l);
	lList.AddTail(&l2m);
	lList.AddTail(&l3vh);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 13
	lList.RemoveAll();
	lList.AddTail(&l1l);
	lList.AddTail(&l2l);
	lList.AddTail(&l3vh);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 14
	lList.RemoveAll();
	lList.AddTail(&l1m);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4m);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 15
	lList.RemoveAll();
	lList.AddTail(&l1h);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4m);
	lList.AddTail(&l5h);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 16
	lList.RemoveAll();
	lList.AddTail(&l1h);
	lList.AddTail(&l2l);
	lList.AddTail(&l3m);
	lList.AddTail(&l4m);
	lList.AddTail(&l5h);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 17
	lList.RemoveAll();
	lList.AddTail(&l1vh);
	lList.AddTail(&l2m);
	lList.AddTail(&l3m);
	lList.AddTail(&l4m);
	lList.AddTail(&l5h);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 18
	lList.RemoveAll();
	lList.AddTail(&l1vh);
	lList.AddTail(&l2l);
	lList.AddTail(&l3m);
	lList.AddTail(&l4m);
	lList.AddTail(&l5h);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 19
	uList.RemoveAll();
	uList.AddTail(&u1m);
	uList.AddTail(&u2l);
	uList.AddTail(&u3h);

	mList.RemoveAll();
	mList.AddTail(&m1l);
	mList.AddTail(&m2m);
	mList.AddTail(&m3m);
	mList.AddTail(&m4l);

	lList.RemoveAll();
	lList.AddTail(&l1l);
	lList.AddTail(&l2l);
	lList.AddTail(&l3l);
	lList.AddTail(&l4l);
	lList.AddTail(&l5m);
	lList.AddTail(&l6m);
	lList.AddTail(&l7l);

	myRule.InferRules(&uList, &mList, &lList, 1);

	// 6. Dealloc memory
	myRule.FreeRules();
}

/*
void TestNN(void)
{
	int layer[] = {5, 5, 7};
	CNNfunction nn;

	nn.NNalloc(layer, 3, 0.15, 1e4, 1e-4, 0.5);
	//nn.NNload(".\\NN020727.dat");

	int no_sample = 7;
	
	float input[] = {HI, LOW, LOW, LOW, LOW,
	HI, LOW, LOW, HI, HI,
	HI, LOW, HI, HI, HI,
	LOW, HI, HI, LOW, LOW,
	LOW, HI, LOW, LOW, LOW,
	HI, LOW, HI, HI, LOW,
	LOW, LOW, HI, LOW, LOW};
	
	float output[] = {HI, LOW, LOW, LOW, LOW, LOW, LOW,
	LOW, HI, LOW, LOW, LOW, LOW, LOW,
	LOW, LOW, HI, LOW, LOW, LOW, LOW,
	LOW, LOW, LOW, HI, LOW, LOW, LOW,
	LOW, LOW, LOW, LOW, HI, LOW, LOW,
	LOW, LOW, LOW, LOW, LOW, HI, LOW,
	LOW, LOW, LOW, LOW, LOW, LOW, HI};

	nn.NNeval((float *)input, no_sample);
	nn.NNtrain(input, output, no_sample);
	nn.NNeval((float *)input, no_sample);

	//nn.NNsave(".\\NN020727.dat");

	nn.NNfree();
}
*/

/*
void main2(void)
{
/*	// fuzzy rule base
	FuzzyRuleBase *hap = new FuzzyRuleBase;
	hap->load("simple.intention.frb");
	//hap->printFuzzyRuleBase();

	// make input membership
	float pas[1][2] = {{0.3,0.3}};
	FuzzySet u1("L",0.0,1.0,(double)(1.0/256.0),FuzzySet::Triangular,pas[0],2);
	SbPList uList;
	uList.append(&u1);

	// inference using input mf. and given fuzzy rule base
	FuzzySet *result2 = hap->inference(uList);
	printf("\noutput = %5.3f\n", result2->defuzzifier(FuzzySet::CenterOfGravity));

	delete hap;*/

/*	int 	sigmoid_type = 1;		// tanh
int		outputlayer_linear = 1;

int 	error_watch = FALSE;
int watch = FALSE;				// toggle watch training mode

int patient = TRUE;				// signal graceful termination


	CNNfunction NNf;

    int		trials	 = 1000;		// number of trials
    int		verbose  = FALSE;		// verbose reporting off
    long	seedval	 = 43;			// random number seed value
    double	learn	 = 64.0;		// 1.0/(learning rate)
    double	eta	 = 1.0/learn;		// learning rate
    double	sigma	 = 0.0;			// noise variance
	double 	alpha1 = 1.0;
	double 	alpha2 = 1.0;

    if (verbose)
      cerr	<< " seedval = "	<< seedval
		<< " trials = "		<< trials
		<< " sigma = "		<< sigma
		<< " learn = "		<< learn << "\n";
 
    int		layers = 2;		// number of layers
    matrix	*b = new matrix[layers];			// threshold  biases
    matrix	*W = new matrix[layers];			// connection weights
    matrix	*x = new matrix[layers+1];			// inputs/outputs
    matrix	*d = new matrix[layers];			// "equivalent error"

    int		outputs = 1;	// number of outputs
    int		inputs = outputs;		// number of inputs
    if (verbose)
      cerr << "N(" << inputs;

    int layer;
    for (layer = 0; layer < layers; layer++) {
		//int inputs = outputs;
		//cin >> outputs;
		if (verbose)
			cerr << ", " << outputs;
		x[layer].resize(inputs);
		d[layer].resize(outputs);
		b[layer].resize(outputs);
		W[layer].resize(outputs, inputs);
		for (int output = 0; output < outputs; output++)
			cin >> b[layer][0][output] >> W[layer].s(output);
    }
    x[layers].resize(outputs);

	printf("init done\n");

    int		examples = 1;	// number of examples
    if (verbose)
      cerr << ")\t" << examples << " examples\n";
    matrix	X(examples,  inputs);		//  inputs
    matrix	Y(examples, outputs);		// outputs
    for (int example = 0; example < examples; example++)
      cin >> X.s(example) >> Y.s(example);

	printf("sample setting done\n");
 
    if (learn > 0.0)
      eta	 = 1.0/learn;

	double error_sum = 0.0;

	doubleMatrix test;

	FILE *fout;
	if ((fout = fopen("error.dat","w")) == NULL) {
        fout = stdout;
    }
	FILE *fdebug;
	if ((fdebug = fopen("debug.dat","w")) == NULL) {
        fdebug = stderr;
    }

	fprintf(fdebug, "eta = %10.7f alpha1 = %10.7f alpha2 = %10.7f \n",eta, alpha1, alpha2);
    for (int trial = 0; trial < trials && patient; trial++) {
		//int example	 = lrand48()%examples;
		example	 = rand()%examples;
		x[0]	 = X.s(example);
//#define	y	   Y.s(example)

		int layer;
		  //for (layer = 0; layer < layers; layer++)	// Feed Forward
		  //  x[layer+1] = tanh(b[layer] + x[layer]%W[layer]);
		  //d[layers-1] = eta*(y - x[layers])*(1.0 - x[layers]*x[layers]);

		for (layer = 0; layer < layers-1; layer++)	// Feed Forward
		  x[layer+1] = test.tanh2(b[layer] + x[layer]%W[layer]);
		  //x[layer+1] = b[layer] + x[layer]%W[layer];
		x[layers] = b[layers-1] + x[layers-1]%W[layers-1];

		double center = x[layers][0][0];
		double var = fabs(x[layers][0][1]);
		double center_d = Y[example][0];
		double var_d = fabs(Y[example][1]);
		d[layers-1][0][0] = - eta*(
			  alpha1 * NNf.calcTri_dJdm(center, var, center_d, var_d)
			+ alpha2 * (x[layers][0][0] - Y[example][0])
			) ;  				
		d[layers-1][0][1] = - eta* (
			alpha1 * NNf.calcTri_dJdb(center, var, center_d, var_d)
			+ alpha2 * (x[layers][0][1] - Y[example][1])
			) ;
// debug
		{
		double m = x[layers][0][0];
		double b = x[layers][0][1];
		double m_d = Y[example][0];
		double b_d = Y[example][1];
		fprintf(fdebug, "%4d %2d %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f %5.2f\n",
				trial, example, x[layers][0][0], x[layers][0][1], 
				Y[example][0], Y[example][1],	
				NNf.calcTri_dJdm(x[layers][0][0], fabs(x[layers][0][1]), 
									Y[example][0], fabs(Y[example][1])),
				NNf.calcTri_dJdb(x[layers][0][0], fabs(x[layers][0][1]), 
									Y[example][0], fabs(Y[example][1])),
				NNf.IntegralTri(m-b, m, m_d, b_d), 
				NNf.IntegralTri(m, m+b, m_d, b_d)
				);
		}

		for (layer = layers-1; layer > 0; layer--)
			d[layer-1] = d[layer]%W[layer].t()*(1.0 - x[layer]*x[layer]);

		for (layer = 0; layer < layers; layer++) {	// Update
			b[layer] += d[layer];
			W[layer] += d[layer]&x[layer];
		};

		if (error_watch) {
			double error = NNf.calcJ(x[layers][0][0], fabs(x[layers][0][1]), 
								Y[example][0], Y[example][1], alpha1, alpha2) ;
			double error1 = NNf.calcJ1(x[layers][0][0], fabs(x[layers][0][1]), 
								Y[example][0], Y[example][1], alpha1, alpha2) ;
			double error2 = NNf.calcJ2(x[layers][0][0], fabs(x[layers][0][1]), 
								Y[example][0], Y[example][1], alpha1, alpha2) ;
			double merror = Y[example][0] - x[layers][0][0];	
			double berror = Y[example][1] - x[layers][0][1];	
			merror = merror * merror;
			berror = berror * berror;
			error_sum += error;
			fprintf(fout, "%4d %10.7f %10.7f %10.7f %10.7f %10.7f %10.7f %10.7f\n",
					trial, error_sum/(double)(trial+1), 
					error, error1, error2,  error_sum, 
					merror, berror);
		}
		if (watch) {
		  cerr <<   "trial = "		<< trial
			   << "\texample = "	<< example	
				<< "\n";
		}
    }

    format("%19.12e", 4);

    cout << layers << "\n";
    cout << inputs << "\n";
    for (layer = 0; layer < layers; layer++) {
		int outputs = b[layer].n();
		cout << outputs << "\n";
		for (int output = 0; output < outputs; output++)
			  cout << format("%19.12e\n", b[layer][0][output])
			   << W[layer].s(output);
  	}
    cout.flush();

    for (int output = 0; output < outputs; output++) {
		for (int example = 0; example < examples; example++) {
			x[0]	 = X.s(example);

			for (int layer = 0; layer < layers-1; layer++)
			  x[layer+1] = test.tanh2(b[layer] + x[layer]%W[layer]);
			  //x[layer+1] = b[layer] + x[layer]%W[layer];
			x[layers] = b[layers-1] + x[layers-1]%W[layers-1];

			cerr << format("%10.3e\t", Y[example][output])
			 << format("%10.3e\n", x[layers][0][output]);
		}
		cerr << "\n";
    }
    cerr.flush();

	//delete b, W, x, d;
	if (fout != stdout) fclose(fout);
	if (fdebug != stderr) fclose(fdebug);
}
*/

/*
// DJ
// 2000.11.1
void main4(void)
{
	//extern int debug = 1;
	FuzzyRuleBase faceExpRuleBase("face", 1, 1, 2);
	FuzzyRuleBase &rb = faceExpRuleBase;

	rb.setLinguisticVar(0, new LinguisticVar("x1", "L", "M"));
	rb.setLinguisticVar(1, new LinguisticVar("x2", "L", "M"));
	rb.setLinguisticVar(2, new LinguisticVar("emotion", "Surprise", "Normal"));
	rb.setLinguisticVar(0, "x1", "L", "M");
	rb.setLinguisticVar(1, "x2", "L", "M");
	rb.setLinguisticVar(2, "emotion", "Surprise", "Normal");

	float pas[2][2] = {{ 0.3, 0.3, {0.7, 0.3}};
	float pas2[2][4] = {{0.0,0.3,0.0,0.4,{0.7,1.0,0.4,0.0}};
	// input
	FuzzySet u1L("u1L",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[0], 2),
			 u1H("u1H",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[1], 2);
	// outputs
	FuzzySet posL("PosL",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, pas2[0], 4),
			 posH("PosH",0.0, 1.0, 0.02, FuzzySet::Trapezoidal, pas2[1], 4);
//	FuzzySet negL("NegL",0.0, 1.0, .02, FuzzySet::Trapezoidal, pas2[0], 4),
//			 negH("NegH",0.0, 1.0, .02, FuzzySet::Trapezoidal, pas2[1], 4);

//	posL.print();
//	posH.print();

	// 1 input ling.
	rb.setLinguisticVar(0, "u1", &u1L, &u1H, NULL);

	// 2 output ling.
	rb.setLinguisticVar(1, "positive", &posL, &posH, NULL);
//	rb.setLinguisticVar(1, "negative", &negL, &negH, NULL);

	// 4 rules
	rb.setRule(0, "u1L", "PosL", NULL);
	rb.setRule(1, "u1H", "PosH", NULL);
//	rb.setRule(0, "u1L", "NegH", NULL);
//	rb.setRule(1, "u1H", "NegL", NULL);

	// print out rules
/*	printf ("Fuzzy Rule:\n");
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
//	x1L.print(); x1M.print();
//	x2L.print(); x2M.print();
//	surprise.print(); normal.print();

	int i;
	// positive
	//float prm[10][2] = {{0.3,0.3,{0.3,0.3,{0.6702,0.3001,{0.5768,0.3003,{0.3096,0.3007,{0.3045,0.3005,{.6915,.3001,{.7,.3,{.7,.3,{.3092,.3008}};	// combination of geo and gabor
	//float prm[10][2] = {{0.3,0.3,{0.3,0.3,{0.6946,0.3000,{0.6342,0.3000,{0.3027,0.3000,{0.3022,0.3000,{.6993,.3000,{.7,.3,{.7,.3,{.305,.3}};	// geo
	float prm[10][2] = {{0.3,0.3,{0.3,0.3,{0.6992,0.3000,{0.5422,0.3000,{0.3043,0.3000,{0.3,0.3000,{.7,.3000,{.7,.3,{.7,.3,{.3,.3}};	// gabor
	// negative
	//float prm[10][2] = {{0.7,0.3,{0.7,0.3,{0.3043,0.3,{0.3493,0.3,{0.6888,0.3,{0.6958,0.3,{.3004,.3,{.3,.3,{.3,.3,{.688,.3}};

	for(i=0;i<10;i++)
	{

		FuzzySet in1("input1",0.0, 1.0, 0.02, FuzzySet::Triangular, prm[i], 2);
	//			 in2("input2",0.0, 1.0, 0.02, FuzzySet::Triangular, pas[1], 2);
	//	FuzzySet in1(x1L), in2(x2M);
		SbPList inList;
		inList.append(&in1);
		//inList.append(&in2);
		FuzzySet *result = rb.inference(inList);
		//result->print();

		printf("\noutput = %5.3f\n", result->defuzzifier(FuzzySet::CenterOfGravity));
	}

	CNNfunction nn;
	double in[12] = {0.1333,0.0548,8.62,0.1447,0.0607,10.51,0.3348,0.1588,3.32,0.3117,0.1579,3.62};
	double out[8] = {0.3,0.3,0.3,0.3,0.7,0.3,0.7,0.3};
	int layers[4] = {3,5,5,2};
	//int layers[3] = {3,3,2};
	//nn.backProp(3,layers,4,in,out);

	// fuzzy rule base
	FuzzyRuleBase *hap = new FuzzyRuleBase;
	hap->load("simple.intention.frb");
	hap->printFuzzyRuleBase();

	// make input membership
	float pasn[1][2] = {{0.3,0.3}};
	FuzzySet u1("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, pasn[0],2);
	SbPList uList;
	uList.append(&u1);

	// inference using input mf. and given fuzzy rule base
	FuzzySet *result2 = hap->inference(uList);
	printf("\noutput = %5.3f\n", result2->defuzzifier(FuzzySet::CenterOfGravity));

	delete hap;
}
*/

//20020728 HMM class �ϼ�.
/*
void TestHMM()
{
	myHMMsequence learnSeq[2], testSeq;
	myHMM hmmTest;

	// data set!
	int sampleNo1[] = {8, 8, 7, 2, 10, 8, 7, 9, 10, 3};
	int sampleSet1[] = {0, 0, 1, 1, 2, 2, 3, 3,
		0, 1, 1, 2, 1, 1, 3, 3,
		0, 2, 1, 2, 1, 2, 3,
		0, 3,
		0, 2, 1, 2, 1, 0, 1, 2, 3, 3,
		1, 0, 1, 0, 0, 2, 2, 2,
		1, 0, 1, 2, 3, 2, 2,
		0, 1, 3, 1, 1, 2, 2, 3, 3,
		0, 1, 0, 0, 0, 2, 3, 2, 2, 3,
		0, 1, 3
	};

	int sampleNo2[] = {8, 7, 9, 5, 9, 7, 9, 10, 9, 6};
	int sampleSet2[] = {3, 3, 2, 2, 1, 1, 0, 0,
		3, 3, 0, 1, 2, 1, 0,
		2, 3, 2, 3, 2, 1, 0, 1, 0,
		3, 3, 1, 1, 0,
		3, 0, 3, 0, 2, 1, 1, 0, 0,
		2, 3, 3, 2, 2, 1, 0,
		1, 3, 3, 1, 2, 0, 0, 0, 0,
		1, 1, 0, 1, 1, 3, 3, 3, 2, 3,
		3, 3, 0, 3, 3, 1, 2, 0, 0, 
		3, 3, 2, 0, 0, 0
	};

	int sol[] = {0, 1, 1, 0};
	int testNo[] = {8, 8, 7, 7};
	int testSet[] = {0, 1, 1, 1, 2, 3, 3, 3,	// 0
		3, 0, 3, 1, 2, 1, 0, 0,	// 1
		2, 3, 2, 1, 0, 1, 0,	// 1
		0, 3, 1, 1, 1, 2, 3	//	0
	};

	int no_state = 3;
	int no_output = 4;
	int no_hmm = 2;
	int no_learn_sample = 10;
	int no_test_sample = 4;
	int no_class = 1;

	float correct[] = {0.0f, 0.0f, 0.0f, 0.0f};
	float rrT;
	float *rrS = new float[no_test_sample];

	hmmTest.HMMinit(no_state, no_output, no_hmm, no_class);	// no_state, possible_output, no_hmm
	//hmmTest.HMMload(".\\20020728.dat", no_hmm);

	hmmTest.HMMsetSequence(no_learn_sample, sampleNo1, sampleSet1, &(learnSeq[0]));
	hmmTest.HMMsetSequence(no_learn_sample, sampleNo2, sampleSet2, &(learnSeq[1]));
	hmmTest.HMMsetSequence(no_test_sample, testNo, testSet, &(testSeq));

	int bestHMMId;
	float bestPDF;
	float eps = 1e-5f;
	int i,j,k;

	rrT = 0.0f;
	for (k=0; k<4; k++)	rrS[k] = 0.0f;

	for (j=0; j<100; j++)
	{
		// training
		for (i=0; i<no_hmm; i++)
		{
			eps = 1e-5f;
			hmmTest.HMMtrain(i, learnSeq[i], 1, eps);
		}

		hmmTest.HMMtest(4, testSeq, sol, correct);

		for (k=0; k<4; k++)
		{
			rrS[k] += correct[k];
			rrT += correct[k];
		}
	}

	printf("Recognition Ratio: %.2f \n", 100.0f*rrT/(float)400.0f);
	for (k=0; k<4; k++)
		printf("Sample No. %d: %.2f \n", k, 100.0f*rrS[k]/(float)100.0f);

	//hmmTest.HMMsave(".\\20020728.dat");

	delete [] rrS;
	delete [] testSeq.sequenceNo;
	delete [] testSeq.sequence;

	for (i=0; i<2; i++)
	{
		delete [] learnSeq[i].sequenceNo;
		delete [] learnSeq[i].sequence;
	}

	hmmTest.HMMfree();
}
*/
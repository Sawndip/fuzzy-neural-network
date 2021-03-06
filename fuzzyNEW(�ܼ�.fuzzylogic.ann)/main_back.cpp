//#include <afxcoll.h>
#include <stdio.h>
#include <stdlib.h>
//#include <iostream.h>

#include "fuzzyRule.h"
#include "NNfunction.h"
#include "myFNN.h"

#define	LOW	(0.3f)
#define	HI	(0.7f)

void TestFuzzy(void);
void TestNN(void);
void TestFNN(void);

void main(void)
{
	//TestFuzzy();
	//TestNN();
	TestFNN();
}

void TestFNN(void)
{
	int i,j;

	myFNN fnn;
	int example = 4;

	//float data[10] = {0.11, 0.91, 0.41, 0.43, 0.39, 0.09, 0.13, 0.93, 0.88, 0.86};
	//float output[10] = {0, 2, 1, 1, 1, 0, 0, 2, 2, 2};
	//float mean2[2] = {0.3f, 0.7f};
	//float mean3[2] = {0.2f, 0.5f, 0.8f};
	//float std2[2] = {0.3f, 0.3f};
	//float std3[2] = {0.2f, 0.2f, 0.2f};

	// FNN structure setting
	// layer structure
	//int layer[] = {5, 10, 32, 14, 7};
	int layer[] = {2, 4, 4, 4, 2};
	
	// in_layer
	int ilayer[] = {2, 2, 2, 2, 2};
	float imean[] = {0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f};
	float istd[] = {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f};

	// out_layer
	int olayer[] = {2, 2, 2, 2, 2, 2, 2};
	float omean[] = {0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f, 0.3f, 0.7f};
	float ostd[] = {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f};

	// overall param.
	fnn.setFNN(layer, 5, ilayer, olayer, 0.05, 1e-6, 1e5);
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

	// input sample
	/*
	float tinput[] = {HI, LOW, LOW, LOW, LOW,
	HI, LOW, LOW, HI, HI,
	HI, LOW, HI, HI, HI,
	LOW, HI, HI, LOW, LOW,
	LOW, HI, LOW, LOW, LOW,
	HI, LOW, HI, HI, LOW,
	LOW, LOW, HI, LOW, LOW};
	*/
	float tinput[] = {LOW, LOW,
		LOW, HI,
		HI, LOW,
		HI, HI};

	// output sample
	/*
	float toutput[] = {1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1};
	*/
	float toutput[] = {LOW, LOW,
		HI, LOW,
		HI, HI,
		LOW, HI};

	// dummy output
	//float youtput[] = {1, 0, 0, 0, 0, 0, 0};
	float youtput[] = {0, 0};

	// comp. learning for consequence of fuzzy rules
	fnn.compFNN(tinput, toutput, example);

	// backprop. learning of FNN
	fnn.learnFNN(tinput, toutput, example);

	// evaluation
	for (i=0; i<example; i++)		
	{
		fnn.evalFNN((tinput + i*layer[0]), youtput, 0);
		fnn.aoutFNN(4);
	}	

	//fnn.kSOC(data, 10, mean, std, 3);

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

	float bi[2][4] = {{0.0, 0.3, 0.0, 0.4}, {0.7, 1.0, 0.4, 0.0}};
	float tri[3][4] = {{0.0, 0.25, 0.0, 0.25}, {0.5, 0.25, 0.0, 0.0}, {0.75, 1.0, 0.25, 0.0}};
	float quad[4][4] = {{0.0, 0.2, 0.0, 0.2}, {0.4, 0.2, 0.0, 0.0}, {0.6, 0.2, 0.0, 0.0}, {0.8, 1.0, 0.2, 0.0}};

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

void TestNN(void)
{
	// DJ NN function
	CNNfunction nn;

	/*
	short int input[] = {0, 1};
	int no_input = 2;
	short int output;
	int no_output = 1;
	int input_scale = 10;
	int scale_output[] = {RL_FIXED_SCALE, RL_FIXED_SCALE, RL_FIXED_SCALE};
	int scale_factor = 10;

	RLwEvalMLPFeedForward(nn.pwNN, input, no_input, &output, no_output, 
		input_scale, scale_output, scale_factor);

	printf("%d", output);
	*/

	/*
	double in[160*2];
	double out[2*2];
	
	int i;

	for (i=0; i<4; i++)
	{
		out[i] = 0.0;
	}

	out[0] = 1.0;
	out[3] = 1.0;
	//out[8] = 1.0;

	FILE *fp;
	
	fp = fopen("c:\\matlabr11\\work\\feers\\gabor\\002gf.dat", "rt");

	for (i=0; i<160; i++)
	{
		fscanf(fp, "%f ", &in[i]);
	}
	
	fclose(fp);

	fp = fopen("c:\\matlabr11\\work\\feers\\gabor\\003gf.dat", "rt");

	for (i=0; i<160; i++)
	{
		i += 160;
		fscanf(fp, "%f ", &in[i]);
	}
	
	fclose(fp);
/*
	fp = fopen("c:\\matlabr11\\work\\feers\\gabor\\004gf.dat", "rt");

	for (i=0; i<160; i++)
	{
		i += 320;
		fscanf(fp, "%f ", &in[i]);
	}
	
	fclose(fp);

/*
	fp = fopen(".\\fea001_gf.dat", "rt");

	for (i=0; i<320; i+=2)
	{
		i += 640;
		fscanf(fp, "%f %f ", &in[i], &in[i+1]);
	}
	
	fclose(fp);

	fp = fopen(".\\hap001_gf.dat", "rt");

	for (i=0; i<320; i+=2)
	{
		i += 960;
		fscanf(fp, "%f %f ", &in[i], &in[i+1]);
	}
	
	fclose(fp);

	fp = fopen(".\\nor001_gf.dat", "rt");

	for (i=0; i<320; i+=2)
	{
		i += 1280;
		fscanf(fp, "%f %f ", &in[i], &in[i+1]);
	}
	
	fclose(fp);

	fp = fopen(".\\sad001_gf.dat", "rt");

	for (i=0; i<320; i+=2)
	{
		i += 1600;
		fscanf(fp, "%f %f ", &in[i], &in[i+1]);
	}
	
	fclose(fp);

	fp = fopen(".\\sur001_gf.dat", "rt");

	for (i=0; i<320; i+=2)
	{
		i += 1920;
		fscanf(fp, "%f %f ", &in[i], &in[i+1]);
	}
	
	fclose(fp);
*/
	//nn.SetVar(10000, 0.6f);
	/*
	nn.SetVar(0, 0, 10000, 0.2, 0.005, 0.6);

	const int inputs = 12;
	const int outputs = 8;
	const int layer_no = 4;

	double in[inputs] = {0.1333, 0.0548, 8.62, 0.1447, 0.0607, 10.51, 0.3348, 0.1588, 3.32, 0.3117, 0.1579, 3.62};
	//double in[inputs] = {0.1333, 0.0548, 0.1447, 0.0607, 0.3348, 0.1588, 0.3117, 0.1579};
	//double in[inputs] = {0, 0, 0, 1, 1, 0, 1, 1};
	double out[outputs] = {0.3, 0.3, 0.3, 0.7, 0.7, 0.3, 0.7, 0.7};
	//double out[outputs] = {0, 0, 0, 1, 0, 1, 1, 1};
	int layers[layer_no] = {3, 5, 5, 2};
	//int layers[layer_no] = {2, 3, 2};
	//int layers[layer_no] = {160, 20, 2};

	srand( (unsigned)time( NULL ) );
	
	nn.NN_learning(4, layers, 4, in, out);
	*/
}

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

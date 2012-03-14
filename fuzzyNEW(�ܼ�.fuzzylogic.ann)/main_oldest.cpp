#include <stdio.h>
#include <stdlib.h>

#include "fuzzyRule.h"
#include "NNfunction.h"

// 20010703@16:21@DJ
void main(void)
{
	// DJ CPtrList Usage.
	/*
	CPtrList pList;
	int i;

	for (i=0; i<100; i++)
		pList.AddTail((int *)(i+10));

	pList.RemoveAt(pList.FindIndex(98));

	i = 128;
	pList.InsertAfter(pList.FindIndex(96), (int *)i);

	for (i=0; i<pList.GetCount(); i++)
		printf("%d, %d \n", i, pList.GetAt(pList.FindIndex(i)));
		*/

	/*
	// DJ Fuzzy RuleSet.
	// fuzzy rule base
	FuzzyRuleBase *hap = new FuzzyRuleBase;
	hap->load("simple.intention.frb");
	hap->printFuzzyRuleBase();

	// make input membership
	float pasn[1][2] = {{0.3f, 0.3f}};
	FuzzySet u1("L", 0.0, 1.0, (double)(1.0/256.0), FuzzySet::Triangular, pasn[0],2);
	CPtrList uList;
	uList.AddTail(&u1);

	// inference using input mf. and given fuzzy rule base
	FuzzySet *result2 = hap->inference(uList);
	printf("\noutput = %5.3f\n", result2->defuzzifier(FuzzySet::CenterOfGravity));

	delete hap;
	*/

	// DJ NN function
	CNNfunction nn;

	//nn.SetVar(10000, 0.6f);
	nn.SetVar(1000, 0.5, 0.1);

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

	 //double in[12] = {0.1333, 0.0548, 8.62, 0.1447, 0.0607, 10.51, 0.3348, 0.1588, 3.32, 0.3117, 0.1579, 3.62};
	//double in[8] = {0, 0, 0, 1, 1, 0, 1, 1};
	//double out[8] = {0.3, 0.3, 0.3, 0.7, 0.7, 0.3, 0.7, 0.7};
	//double out[8] = {0, 0, 0, 1, 0, 1, 1, 1};
	//int layers[4] = {3, 5, 5, 2};
	//int layers[3] = {2, 3, 2};
	int layers[3] = {160, 10, 2};
	
	nn.ebp_sig(3, layers, 2, in, out);
	//nn.ebp_tanh(3, layers, 3, in, out);
	//nn.fuzzy_observer_sig(3,layers, 3, in, out);
	//nn.fuzzy_observer_tanh(4, layers, 4, in, out);
}

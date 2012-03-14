//#include <afx.h>
#include <stdio.h>
#include <stdlib.h>
#include "myresource.h"
#include "NNfunction.h"

//#include "fuzzyRule.h"
//#include "myFNN.h"

#define	LOW	(0.2f)
#define	HI	(0.8f)

#define	SAVE_FLAG	0

//void TestFNN(void);		// first version
void TestNN(void);

void main(void)
{
	//TestFNN();
	TestNN();
}

void TestNN(void)
{
	int layer[] = {5, 10, 32, 14, 7};
	CNNfunction nn;

	nn.NNalloc(layer, 5, 0.15, 1e3, 1e-3, 0.8);	
	//nn.NNload(".\\NN030205.dat");

	int no_sample = 4700;
	int i, j, outputv;

	float* input;
	float* output;
	input = new float[no_sample*layer[0]];
	output = new float[no_sample*layer[4]];

	for (i=0; i<no_sample*layer[4]; i++)
		output[i] = 0.2f;

	FILE *fp;

	fp = fopen(".\\wholeN2l.dat", "rt");

	for (i=0; i<no_sample; i++)
	{
		fscanf(fp, "%f %f %f %f %f %d ", &input[i*layer[0]], &input[i*layer[0]+1], &input[i*layer[0]+2], 
			&input[i*layer[0]+3], &input[i*layer[0]+4], &outputv);

		output[i*layer[4]+outputv] = 0.8f;
	}

	fclose(fp);
	
	/*
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
	*/

	nn.NNtrain(input, output, no_sample);
	nn.NNsave(".\\NN030205.dat");

	nn.NNeval((float *)input, (float *)output, no_sample);

	fp = fopen(".\\wholeN2e.dat", "rt");

	for (i=0; i<no_sample*layer[4]; i++)
		output[i] = 0.2f;

	for (i=0; i<no_sample; i++)
	{
		fscanf(fp, "%f %f %f %f %f %d", &input[i*layer[0]], &input[i*layer[0]+1], &input[i*layer[0]+2], 
			&input[i*layer[0]+3], &input[i*layer[0]+4], &outputv);

		output[i*layer[4]+outputv] = 0.8f;
	}

	fclose(fp);

	nn.NNeval((float *)input, (float *)output, no_sample);

	nn.NNfree();

	delete [] input;
	delete [] output;
}

/*
// 2002/11/28 mod.
void TestFNN(void)
{
	CTime t = CTime::GetCurrentTime();

	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	int i, j;
	char str[100];
	FILE *fp;
	//fp = fopen(".\\FNN021128SI.dat", "awt");

	int std_array[6] = {1, 2, 5, 10, 20, 40};
	int node_no[] = {0, 1, 2, 2, 3, 4, 5};

	int nodes1[] = {5};
	int nodes2[] = {0, 5};
	int nodes3[] = {5, 6};
	int nodes4[] = {0, 5, 6};
	int nodes5[] = {0, 5, 6, 8};
	int nodes6[] = {0, 2, 5, 6, 8};

	float TimeAvg = 0.0f;

	int count = 1;

	while(count-- > 0)
	{

		printf("COUNT#%d \n", count);

	for (i=0; i<7; i++)
	{
		myFNN fnn;

		TimeAvg = 0.0f;

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

		fnn.ioHistogram();

		//fnn.ShowFNND();
	
		for (j=0; j<6; j++)
		{
			sprintf(str, ".\\FNN020902std%d.dat", std_array[j]);
			fnn.loadDataN1(str, 5, 7, 700, 700, 0, 0);

			printf("REC#%d with stdv %d \n", i, std_array[j]);

			//CHECK_TIME_START;
			fnn.recTrain();
			//CHECK_TIME_END(Time1, err);
			//printf("%f \n", Time1);

			//TimeAvg += (float)Time1;

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
//		}

		//TimeAvg /= (float)(6.0f);
		//fprintf(fp, "%.2f ", TimeAvg);

		/*
		fp = fopen(".\\FNN021128SI.dat", "awt");
		fprintf(fp, "\n");
		fclose(fp);

		fnn.freeFNN();

		fp = fopen(".\\FNN021129.dat", "awt");
		fprintf(fp, "\n");
		fclose(fp);

	}

	
	}

	//fclose(fp);
}
*/

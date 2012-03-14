#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "fuzzy.h"

FuzzySet::FuzzySet(char *namestr, float ud_from , float ud_to , 
		float inc , MembershipType t, float *parastr , int npara )
{
	int length = strlen(namestr);
	if(length){
		name = new char[length+1];
		strcpy(name,namestr);
	}
	ud[0] = ud_from;
	ud[1] = ud_to;
	increment = inc;
	type = t;
	N = (int)((ud[1] - ud[0])/increment);

	support[0] = ud_from; support[1] = ud_to;
	paras = NULL;

	if(npara){
		numParas = npara;
		paras = new float[numParas];
		if(parastr) {
			for(int i=0;i<numParas;i++)
				paras[i]=parastr[i];
		}
	} else return ;
	
	setSupport();
}

FuzzySet::FuzzySet(MembershipType t,float *para,char *namestr)
		: type(t),increment((float)1.0e-6),numParas(0),tol((float)1.0e-5) 
{
	int length = strlen(namestr);
	if(length){
		name = new char[length+1];
		strcpy(name,namestr);
	}
	if(para){
		numParas = length/sizeof(float);
		paras = new float[numParas];
		if(numParas){
			for(int i=0;i<numParas;i++)
				paras[i] = para[i];	
		}
	}
}

FuzzySet::FuzzySet(FuzzySet & A)
{
	name = strdup(A.getName());
	type = A.getMembershipType();
	ud[0] = A.getUnivOfDiscourse()[0];
	ud[1] = A.getUnivOfDiscourse()[1];
	support[0] = A.getSupportFromf();
	support[1] = A.getSupportTof();
	increment = A.getIncrement();
	N = (int)((ud[1] - ud[0])/increment);
	type = A.getMembershipType();	
	paras = NULL;
	numParas = A.getMembershipPara(paras);
	tol = A.getTolerance();
}

// constructor for given type and parameter
//		type 0: center,width
//		type 1: center1,center2,width1,width2
//		type 2: begin point, ... , end point
FuzzySet::FuzzySet(MembershipType t,float para1, float para2,
			float para3 ,float para4)
{

	type = t;
	switch(type){
		case Singleton:
				numParas = 2;
				paras = new float[numParas];
				paras[0] = para1,paras[1] = para2;
				break;
		case Triangular:
				numParas = 2;
				paras = new float[numParas];
				paras[0] = para1,paras[1] = para2;
				ud[0] = para1 - para2;
				ud[1] = para1 + para2;
				break;
		case Trapezoidal:
				numParas = 4;
				paras = new float[numParas];
				paras[0] = para1, paras[1] = para2;
				paras[2] = para3, paras[3] = para4;
				ud[0] = para1 - para3;
				ud[1] = para2 + para4;
				break;
		case Gaussian:
				numParas = 2;
				paras = new float[numParas];
				paras[0] = para1, paras[1] = para2;
				break;
		case Arbitrary:

				break;
		case LinearRPN:

				break;
	}
}

void FuzzySet::setMembershipType(MembershipType t)
{
	type = t;
	switch(t){
	case Arbitrary:
		numParas = N;
		if (paras) delete [] paras;
		paras = new float[numParas];
		for (int i = 0; i < numParas; i++) paras[i] = 0.0; //init
		break;
	}
	//setSupport();
}
void FuzzySet::setMembershipPara(float *para, int n)
{
	numParas = n;

	if(paras) delete [] paras;
	paras = new float[numParas];

	for(int i=0;i<numParas;i++)
		paras[i] = para[i];

	setSupport();
}

int FuzzySet::getMembershipPara(float *&p)
{
	if (numParas) {
		if (!p)	p = new float[numParas];
		for (int i = 0; i < numParas; i++) p[i] = paras[i];
	}
	return numParas;
}

void FuzzySet::setSupport()
{
	if (!paras) return ;
	switch(type) {
		case Singleton:
			support[1] = support[0] = paras[0];
			break;
		case Triangular:
			support[0] = paras[0] - paras[1]/2;
			support[1] = paras[0] + paras[1]/2;
			break;
		case Trapezoidal:
			support[0] = paras[0] - paras[2];
			support[1] = paras[1] + paras[3];
			break;
		case Arbitrary:
			support[0] = paras[0];
			support[1] = paras[numParas-1];
			break;
		case Gaussian: break;
		case LinearRPN:
		default:
			support[0] = ud[0]; support[1] = ud[1];
			break;
	}
}

//  return membership value of defined fuzzy set
//	type is defined previously by MembershipType

float FuzzySet::getValue(float x)
{
	if(x>support[1] || x<support[0]) return 0.0;

		float center;
		float width;
		float center1; float width1;  
		float center2; float width2;
		float mean;
		float div;
		double var;
		int index;
		int i;
		
		switch(type) {
	case Singleton:	
		return (equals(x, paras[0], (float)0.001) ? paras[1]: (float)0.0);
	case Triangular:	// y = 1 - |x-c|/b for |x-c| <= b
		center = paras[0];
		width = (float)(paras[1] * 0.50);
		if (x < center-width) 	return 0.0;
		else if(x < center) 	return((width-center+x)/width);
		else if(x < center+width) return((width+center-x)/width);
		else return 0.0;
	case Trapezoidal:
		center1 = paras[0];  width1 = paras[2];  
		center2 = paras[1];  width2 = paras[3];
		if      (x < support[0]) return 0.0;
		else if (x < center1) 	 return ((width1 + x - center1)/width1);
		else if (x < center2) 	 return 1.0;
		else if (x < support[1]) return ((width2 - x + center2)/width2);
		else return 0.0;
	case Gaussian:
		mean = paras[0];
		div  = paras[1];
		var = 2*div*div;
		return (float)(exp((x-mean)*(x-mean)/var)/sqrt(var));
	case Arbitrary:
		index=int((float)numParas * (x - ud[0]) / (ud[1] - ud[0]));
		return(paras[index]);
	case LinearRPN:
		for(i=0;i<numParas-1;i++){
				if(paras[i]<=x && paras[i*2+2]>x){
					return(((paras[i*2]-x)/ (paras[i*2+2]-paras[i*2])
						*(paras[i*2+3]-paras[i*2+1]))+paras[i*2+1]);
				}
		}
		break;
	default:
			return 0.0;
	}
	return 0;
}

float FuzzySet::getX(int index)
{
	return (ud[0] + (float)index/(float)N * (ud[1] - ud[0]));
}

float FuzzySet::getIndexedValue(int k)
{
	switch(type) {
		case Arbitrary: return(paras[k]);
		default:
		return getValue(ud[0] + (float)k/(float)N * (ud[1] - ud[0]));
	}
}

void FuzzySet::setIndexedValue(int x, float value)
{
	switch(type) {
		case Singleton:
		case Triangular:
		case Trapezoidal:
		case Gaussian:
		case LinearRPN:
		default:
			printf ("FuzzySet type mismatch\n");
			break;
		case Arbitrary:
			if (x < 0|| x > numParas)
				break;
			else
				paras[x] = value;
			break;
	}
}

float FuzzySet::defuzzifier(DefuzzifierType deType)
{

	int i;
	float value;
	float den, num;
	float mx, mxIndex;

	switch(deType){
	case Maximum:
		mx = 0.0; mxIndex = 0;
		for (i = 0; i < N; i++) {
			value = getIndexedValue(i);
			if (value > mx) { 
				mxIndex = (float)i;
				mx = value;
			}
		}
//		SbVec2f *v2f = new SbVec2f(getX(mxIndex), value);
//		return  v2f;
		return getX((int)mxIndex);
	default:
		printf ("not implemented-> cog method\n");
	case CenterOfGravity:
		den = 0.0; num = 0.0;
		for (i = 0; i < N; i++) {
			num += getX(i) * getIndexedValue(i);
			den += getIndexedValue(i);
		}
		return (iszero(den, (float)1.0e-5) ? -1:(num/den));
//	case CenterAverage:
//	case ModifiedCenterAverage:
	}
}

void FuzzySet::print()
{
	int i;

	printf ("FuzzySet \"%s\": ", name);
	printf ("\n\tUniverse of Discourse:[%4.2f, %4.2f] ", ud[0], ud[1]);
	printf ("\n\t%d samples, %f interval ", N, increment);
	printf ("\n\tsupport: %3.2f %3.2f", support[0], support[1]);
	printf ("\n\t");
	switch(type) {
		case Singleton:
			printf ("singleton: %4.2f %4.2f\n", paras[0], paras[1]);
			break;
		case Triangular:
			printf ("triangular: %4.2f %4.2f\n", paras[0], paras[1]);
			break;
		case Trapezoidal:
			printf ("trapezoidal: %4.2f %4.2f %4.2f %4.2f\n", 
						paras[0], paras[1], paras[2], paras[3]);
			break;
		case Gaussian:
			printf ("gaussian: %4.2f %4.2f\n", paras[0], paras[1]);
			break;
		case LinearRPN:
			printf ("linear repN\n");
			break;
		case Arbitrary:
			printf ("array:[");
			for (i = 0; i < numParas; i++) {
				printf (" %4.2f", paras[i]);
				if (i != numParas-1) printf (",");
			}
			printf ("]\n");
			break;
		default:
			break;
	}
}

static char *gnucommands[] = { 
	"set yrange [0:1]",
	"plot 'membership.dat' with lines",
	"pause 120"
};
void FuzzySet::graph()
{
	char *dataFilename = "membership.dat";
	char *gnufile = "membership.gnuplot";

	FILE *fout;
	if ((fout = fopen(dataFilename,"w")) == NULL) return ;

	for (int i = 0; i < N; i++) {
		fprintf (fout, "%4.2f %4.2f\n", getX(i), getIndexedValue(i));
	}
	fclose(fout);

	if ((fout = fopen(gnufile,"w")) == NULL) return ;

	for (i = 0; i < 3; i++) {
		if (i == 1) 
			fprintf (fout, "plot \'membership.dat\' using 1:2 title \'%s\' with lines\n", 
				name);
		else fprintf (fout, "%s\n", gnucommands[i]);
	}
	fclose(fout);

	/*
    char command[100];
    sprintf (command,"gnuplot %s > /dev/null 2> /dev/null &", gnufile);

    int err = system(command);
    if (err) {
        fprintf (stderr,"execution error: %s\n", command);
        return;
    }
	*/
 
}

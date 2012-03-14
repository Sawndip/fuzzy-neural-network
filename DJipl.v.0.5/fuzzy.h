#ifndef _FUZZY_LOGIC_
#define _FUZZY_LOGIC_

#include <string.h>
#include <math.h>

enum Bool { False, True};

class FuzzySet {
public:
	enum MembershipType {Singleton, Triangular, Trapezoidal, Gaussian, 
						Arbitrary,LinearRPN};
	enum DefuzzifierType {Maximum, CenterOfGravity};
private:
	char *name;
	float ud[2]; // universeOfdiscourse	 from, to
	MembershipType type;
//	DefuzzifierType deType;

	float support[2];	// from, to
	float increment;
	int	  N;	//  = (to - from)/increment	

	float *paras; 	// membership parameters
	int	  numParas;
	float tol;	// tolerence about zero
	Bool equals(float x, float y,float tolerance)
		{ return iszero( x - y, tolerance); }
	Bool iszero(float x, float tolerance) 
		{ return ((fabs (x) < tolerance) ? True: False); }
public:
	
	FuzzySet(char *name = NULL, float ud_from = 0., float ud_to = 0., 
			float inc = 1.0e-6, MembershipType t = Arbitrary, 
			float *pars = NULL, int nPara = 0);
	FuzzySet(MembershipType t,float para1, float para2,
			float para3=0 ,float para4=0);
	FuzzySet(MembershipType t,float *para,char *namestr);
	FuzzySet(FuzzySet & );
	~FuzzySet(){if (name) delete name; if (paras) delete paras;};

	char * 	getName() { return name;};
	void 	setName(char *str) 
				{if(str){if(name) delete name; name=strdup(str);}}

	void 	setUnivOfDiscourse(float ud_from, float ud_to) 
				{ ud[0] = ud_from; ud[1] = ud_to; 
				if (!iszero(increment, tol)) N = (int)((ud[1] - ud[0]) / increment);};
	float *	getUnivOfDiscourse(){return ud;}

	void 	setIncrement(float inc) {increment = inc;
				if (!iszero(increment, tol)) N = (int)((ud[1] - ud[0]) / increment);};
	float 	getIncrement() {return increment;}

	void 	setMembershipType(MembershipType t); // {type = t;}
	MembershipType getMembershipType() { return type;}

	void 	setMembershipPara(float *pars, int n);
	int 	getMembershipPara(float *&pars);

	void	setSupport(); 
	int		getSupportFromi() 
			{return (int)((support[0] - ud[0])/(ud[1] - ud[0]) * (float)N+0.5);}
	int		getSupportToi() 
			{return (int)((support[1] - ud[0])/(ud[1] - ud[0]) * (float)N+0.5);}
	float	getSupportFromf() 	{ return support[0];}
	float	getSupportTof() 	{ return support[1];}
	void	setLength(int n) 	{ N = n; increment = (ud[1] - ud[0]) / (float)N; }
	int		getLength() 		{ return N;}

	float 	getValue(float);
	float 	getIndexedValue(int);
	void 	setIndexedValue(int, float);
//	void 	setIndexedValue(float, float);

	float getX(int);
	float getTolerance() {return tol;}
	float defuzzifier(DefuzzifierType deType);
	void  print();
	void  graph();
};
#endif


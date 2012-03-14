#include <stdafx.h>
#include <stdio.h>
#include <memory.h>
#include "FMMNN.h"

#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define F(a, b) ((a)*(b) > 1 ? 1 : ((a)*(b) < 0 ? 0 : (a)*(b)))

/*-------------------------------------------------------
	기본 생성자
-------------------------------------------------------*/
CFMMNN::CFMMNN()
{
	m_nDimension = 0;
	m_nClass = 0;
	m_lfTheta = 0.0;
	m_lfGamma = 0.0;

	m_pInput = NULL;
	m_pOutput = NULL;
	
	m_pBox = NULL;
	m_nBox = NULL;
}

/*-------------------------------------------------------
	생성자

	nDimension(입력) : 입력 벤터의 차원수 = 입력 x의 dimension (논문에서 n)
	nClass(입력) : 클래스 수 = 구분하고자 하는 클래스의 수 (논문에서 p)
	lfTheta(입력) : 확장을 제한하는 한계치, theta값, 0 ~ 1, 값이 클수록 박스 수가 적다.
	lfGamma(입력) : membership function의 감소정도, sensitivity, > 0, 값이 클수록 crisp하게 된다.
-------------------------------------------------------*/
CFMMNN::CFMMNN(int nDimension, int nClass, float lfTheta, float lfGamma)
{
	m_nClass = 0;
	m_pOutput = NULL;
	m_pBox = NULL;
	m_nBox = NULL;

	Init(nDimension, nClass, lfTheta, lfGamma);
}

/*-------------------------------------------------------
	초기화 하는 함수

	nDimension(입력) : 입력벡터의 차원수 = 입력 x의 dimension (논문에서 n)
	nClass(입력) : 클래스 수 = 구분하고자 하는 클래스의 수 (논문에서 p)
	lfTheta(입력) : 확장을 제한하는 한계치, theta값, 0 ~ 1, 값이 클수록 박스 수가 적다.
	lfGamma(입력) : membership function의 감소정도, sensitivity, > 0, 값이 클수록 crisp하게 된다.
-------------------------------------------------------*/
void CFMMNN::Init(int nDimension, int nClass, float lfTheta, float lfGamma)
{
	Clear();

	m_nDimension = nDimension;
	m_nClass = nClass;
	m_lfTheta = lfTheta;
	m_lfGamma = lfGamma;
	
	m_pInput = NULL; // 입력은 외부에서 가져오는 거라서 메모리 안 잡는다.
	m_pOutput = new float[nClass]; // 출력은 클래스 수만큼 잡는다.
	
	// 여기서부터는 신경쓰지 마소. 이차원 배열 만들려고 이 짓 하는 거니까.
	// m_pBox[i][j] : i번째 클래스의 j번째 박스 --> 이렇게 쓸 수 있게 만들라꼬.
	m_pBox = new MINMAX*[nClass]; // 클래스 수만큼 박스 넣을 자리 만든다.
	for(register int i = 0; i < nClass; i ++)
		m_pBox[i] = NULL; // 초기화
	m_nBox = new int[nClass]; // 클래스 수만큼 박스 개수 기억할 자리 만든다.
	memset(m_nBox, 0, sizeof(int) * nClass); // 0으로 초기화
}

/*-------------------------------------------------------
	소멸자
-------------------------------------------------------*/
CFMMNN::~CFMMNN()
{
	Clear();
}

/*-------------------------------------------------------
	모든 박스를 제거하고 초기화하는 함수
-------------------------------------------------------*/
void CFMMNN::Clear()
{
	// 입력 벡터의 메모리는 여기서 만든 게 아니니까 제거할 필요없다. 외부에서 처리해야 한다.

	// 출력 벡터의 메모리 제거
	if(m_pOutput)
	{
		delete [] m_pOutput;
		m_pOutput = NULL;
	}
	
	// 여기서부터는 신경쓰지 마소. 이차원 배열 때문에 이러는 거니까.
	// 박스 메모리 제거.. 이거 좀 복잡한데
	for(register int i = 0; i < m_nClass; i ++)
	{
		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			// 각 박스의 min, max 제거
			MINMAX* p = &m_pBox[i][j];
			delete p->min;
			p->min = NULL;
			delete p->max;
			p->max = NULL;
		}

		// i번째 클래스의 박스 제거
		if(m_pBox[i])
		{
			delete [] m_pBox[i];
			m_pBox[i] = NULL;
		}
	}

	// 전체 박스 제거
	if(m_pBox)
	{
		delete [] m_pBox;
		m_pBox = NULL;
	}

	if(m_nBox)
	{
		delete [] m_nBox; // 각 클래스의 박스 수 제거
		m_nBox = NULL;
	}

	m_nDimension = 0;
	m_nClass = 0;
	m_lfTheta = 0.0;
	m_lfGamma = 0.0;
	m_pInput = NULL;
	m_pOutput = NULL;
}

/*-------------------------------------------------------
	nClass번째 클래스에 박스를 하나 추가하는 함수

	nClass(입력) : 클래스 번호(0부터 시작)
-------------------------------------------------------*/
void CFMMNN::AddBox(int nClass)
{
	register int nBox = m_nBox[nClass]; // nClass번째 클래서의 하이퍼 박스 개수

	if(nBox == 0) // nClass번째 클래스에 박스가 하나도 없었으면
	{
		m_pBox[nClass] = new MINMAX[1]; // 박스 하나 만든다.
	}
	else // nClass번째 클래스에 박스가 있었으면
	{
		// 배열이 잡혀 있었으니까 배열을 하나 늘이기 위해서는 
		// 새로운 크기만큼 새로 배열 잡고 원래꺼 복사한 다음 
		// 원래꺼 메모리 지우고 새로운 것에다 포인터 연결해야 한다.

		MINMAX* pNewBox = new MINMAX[nBox+1]; // 새로운 크기만큼 배열 잡는다.
		memcpy(pNewBox, m_pBox[nClass], nBox * sizeof(MINMAX)); // 원래꺼를 새로운 것에다가 복사한다.

		delete [] m_pBox[nClass]; // 원래꺼 지우고
		m_pBox[nClass] = pNewBox; // 새로운 배열에 포인터를 연결한다.
	}

	// 새로 생긴 박스를 초기화한다.
	MINMAX* p = &m_pBox[nClass][nBox];
	p->min = new float[m_nDimension];
	p->max = new float[m_nDimension];
	for(register int i = 0; i < m_nDimension; i ++)
	{
		p->min[i] = m_pInput[i];
		p->max[i] = m_pInput[i];
	}
	m_nBox[nClass] ++; // 박스 수 하나 증가
}

/*-------------------------------------------------------
	박스가 확장 가능한가? 알아보는 함수

	pBox(입력) : 박스
	return : 확장할 수 있으면 true, 없으면 false
-------------------------------------------------------*/
bool CFMMNN::IsExpandable(MINMAX* pBox)
{
	register MINMAX* p = pBox;
	for(register int i = 0; i < m_nDimension; i ++)
	{
		if(max(p->max[i], m_pInput[i]) - min(p->min[i], m_pInput[i]) > m_lfTheta)
			return false;
	}

	return true;
}

/*-------------------------------------------------------
	박스를 확장하는 함수

	pBox(입력) : 박스
-------------------------------------------------------*/
void CFMMNN::ExpandBox(MINMAX* pBox)
{
	register MINMAX* p = pBox;

	// 각 꼭지점 값을 바꾼다.
	for(register int i = 0; i < m_nDimension; i ++)
	{
		p->max[i] = max(p->max[i], m_pInput[i]); // wji = max(wji, xhi)
		p->min[i] = min(p->min[i], m_pInput[i]); // vji = min(vji, xhi)
	}
}

/*-------------------------------------------------------
	박스의 membership value 알아내는 함수

	pBox(입력) : 박스
	return : 해당 박스의 소속값
-------------------------------------------------------*/
float CFMMNN::GetMembership(MINMAX* pBox)
{
	register float min = 1.0;
	register MINMAX* p = pBox;
	for(register int i = 0; i < m_nDimension; i ++)
	{
		register float tmp = min(1 - F(m_pInput[i] - p->max[i], m_lfGamma), 1 - F(p->min[i] - m_pInput[i], m_lfGamma));
		if(tmp < min)
			min = tmp;
	}
	
	return min;
}

/*-------------------------------------------------------
	nClass 클래스의 확장 가능한 것 중 membership value가 가장 큰 박스를 찾는 함수

	nClass(입력) : 클래스 번호(0부터 시작)
	return : 찾았으면 해당 박스 번호, 못 찾았으면 -1
-------------------------------------------------------*/
int CFMMNN::FindMaxMembershipBox(int nClass)
{
	register float max = -1.0;
	register int nMax = -1;
	for(register int i = 0; i < m_nBox[nClass]; i ++)
	{
		register MINMAX* pBox = &m_pBox[nClass][i];
		register float t = GetMembership(pBox);
		if(t > max && IsExpandable(pBox))
		{
			max = t;
			nMax = i;
		}
	}

	return nMax;
}

/*-------------------------------------------------------
	두 개의 박스가 overlap되는지 테스트하는 함수

	pBox1(입력) : 첫번째 박스
	pBox2(입력) : 두번째 박스
	nDir(출력) : overlap되면 가장 겹침이 적은 방향
	nCase(출력) : overlap된 케이스(1 ~ 4)
	return : overlap되면 true, 아니면 false
-------------------------------------------------------*/
bool CFMMNN::TestOverlap(MINMAX* pBox1, MINMAX* pBox2, int& nDir, int& nCase)
{
	register float* v1 = pBox1->min;
	register float* w1 = pBox1->max;
	register float* v2 = pBox2->min;
	register float* w2 = pBox2->max;
	register int caseTmp;
	register float minNew = 1.0, minOld = 1.0;

	for(register int i = 0; i < m_nDimension; i ++)
	{
		// case 1
		// ----------
		//      -----------
		if(v1[i] < v2[i] && v2[i] < w1[i] && w1[i] < w2[i])
		{
			minNew = min(minOld, w1[i] - v2[i]);
			caseTmp = 1;
		}
		// case 2
		//      -----------
		// ----------
		else if(v2[i] < v1[i] && v1[i] < w2[i] && w2[i] < w1[i])
		{
			minNew = min(minOld, w2[i] - v1[i]);
			caseTmp = 2;
		}
		// case 3
		// -----------------
		//      ------
		else if(v1[i] <= v2[i] && v2[i] <= w2[i] && w2[i] <= w1[i])
		{
			minNew = min(minOld, min(w2[i] - v1[i], w1[i] - v2[i]));
			caseTmp = 3;
		}
		// case 4
		//      ------
		// -----------------
		else if(v2[i] <= v1[i] && v1[i] <= w1[i] && w1[i] <= w2[i])
		{
			minNew = min(minOld, min(w1[i] - v2[i], w2[i] - v1[i]));
			caseTmp = 4;
		}
		else
			return false; // overlap이 없음

		if(minNew < minOld) // 겹침이 더 적은 방향 찾았음
		{
			nDir = i;
			minOld = minNew;
			nCase = caseTmp;
		}
	}

	return true;
}

/*-------------------------------------------------------
	두 개의 박스를 축소시키는 함수

	pBox1(입력) : 첫번째 박스
	pBox2(입력) : 두번째 박스
	nDir(입력) : 축소시키는 방향(0부터 시작)
	nCase(입력) : overlap된 케이스(1 ~ 4)
	return : 축소되면 true, 아니면 false
-------------------------------------------------------*/
bool CFMMNN::Contract(MINMAX* pBox1, MINMAX* pBox2, int nDir, int nCase)
{
	register float& v1 = pBox1->min[nDir];
	register float& w1 = pBox1->max[nDir];
	register float& v2 = pBox2->min[nDir];
	register float& w2 = pBox2->max[nDir];

	switch(nCase)
	{
	case 1:
	// case 1
	// ----------
	//      -----------
		w1 = v2 = (w1 + v2) / 2.0f;
		return true;
	case 2:
	// case 2
	//      -----------
	// ----------
		v1 = w2 = (v1 + w2) / 2.0f;
		return true;
	case 3:
	// case 3
	// -----------------
	//      ------
		if(w2 - v1 < w1 - v2)
			v1 = w2;
		else
			w1 = v2;
		return true;
	case 4:
	// case 4
	//      ------
	// -----------------
		if(w1 - v2 < w2 - v1)
			v2 = w1;
		else
			w2 = v1;
		return true;
	default:
		return false;
	}
}

/*-------------------------------------------------------
	nClass번째 클래스에 속하는 박스에 대해 overlap 테스트해서 축소하는 함수

	pBox(입력) : 박스
	nClass(입력) : 클래스 번호(0부터 시작), pBox가 속한 클래스
-------------------------------------------------------*/
void CFMMNN::TestBox(MINMAX* pBox, int nClass)
{
	register MINMAX* p1 = pBox;

	for(register int i = 0; i < m_nClass; i ++)
	{
		if(i == nClass)
			continue;

		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			register MINMAX* p2 = &m_pBox[i][j];
			register int nDir, nCase;
			if(TestOverlap(p1, p2, nDir, nCase))
				Contract(p1, p2, nDir, nCase);
		}
	}
}

/*-------------------------------------------------------
	입력벡터에 대해 membership value가 가장 큰 클래스를 찾는 함수

	return : membership value가 가장 큰 클래스(0부터 시작)
-------------------------------------------------------*/
int CFMMNN::FindMaxMembershipClass()
{
	memset(m_pOutput, 0, sizeof(float) * m_nClass);

	register float max = -1.0;
	int nClass = -1;
	for(register int i = 0; i < m_nClass; i ++)
	{
		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			register float t = GetMembership(&m_pBox[i][j]);
			if(t > m_pOutput[i])
				m_pOutput[i] = t;
		}
		if(m_pOutput[i] > max)
		{
			max = m_pOutput[i];
			nClass = i;
		}
	}

	return nClass;
}

/*-------------------------------------------------------
	전체 박스 개수 얻는 함수

	return : 전체 박스 개수
-------------------------------------------------------*/
int CFMMNN::GetTotalBoxNum()
{
	int sum = 0;
	for(register int i = 0; i < m_nClass; i ++)
		sum += m_nBox[i];

	return sum;
}

/*-------------------------------------------------------
	학습하는 함수

	nClass(입력) : 학습 데이터의 클래스
	pInput(입력) : 하나의 학습 입력 벡터 x, 길이는 m_nInput(=n)과 같아야 한다.

	* 사실은 n개 이상이어도 된다. 하지만 n개보다 적으면 문제가 생긴다.
-------------------------------------------------------*/
void CFMMNN::Training(int nClass, float* pInput)
{
	if(pInput == NULL)
		return;
	m_pInput = pInput;

	int nBox = FindMaxMembershipBox(nClass);
	if(nBox < 0) // 확장할 수 있는 게 없으면
		AddBox(nClass); // 박스를 하나 새로 만든다.
	else // 확장할 수 있으면
	{
		register MINMAX* pBox = &m_pBox[nClass][nBox];
		ExpandBox(pBox); // 확장한다.
		TestBox(pBox, nClass); // overlap 테스트해서 축소한다.
	}
}

/*-------------------------------------------------------
	테스트하는 함수

	pInput(입력) : 하나의 테스트 입력 벡터 x, 길이는 m_nInput(=n)과 같아야 한다.
	* 사실은 n개 이상이어도 된다. 하지만 n개보다 적으면 문제가 생긴다.
	return : membership value가 가장 큰 클래스 번호
-------------------------------------------------------*/
int CFMMNN::Test(float* pInput)
{
	if(pInput == NULL)
		return -1;
	m_pInput = pInput;

	return FindMaxMembershipClass();
}

/*-------------------------------------------------------
	테스트하는 함수

	pInput(입력) : 하나의 테스트 입력 벡터 x, 길이는 m_nInput(=n)과 같아야 한다.
	pOutput(입력) : 입력 벡터에 대한 각 클래스의 membership value를 반환받기 위한 float* 형 변수
	* 사실은 n개 이상이어도 된다. 하지만 n개보다 적으면 문제가 생긴다.
	return : membership value가 가장 큰 클래스 번호
-------------------------------------------------------*/
int CFMMNN::Test(float* pInput, float* &pOutput)
{
	int nClass = Test(pInput);
	pOutput = GetResultMembership();
	
	return nClass;
}

/*-------------------------------------------------------
	하이퍼박스 정보 읽어오는 함수

	fp(입력) : 파일 포인터
-------------------------------------------------------*/
void CFMMNN::LoadBox(FILE* fp)
{
	for(register int i = 0; i < m_nClass; i ++)
		fscanf(fp, "%d", &m_nBox[i]);
	m_pBox = new MINMAX*[m_nClass];
	for(i = 0; i < m_nClass; i ++)
	{
		m_pBox[i] = new MINMAX[m_nBox[i]];
		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			m_pBox[i][j].min = new float[m_nDimension];
			m_pBox[i][j].max = new float[m_nDimension];
			for(register int k = 0; k < m_nDimension; k ++)
				fscanf(fp, "%f %f", &m_pBox[i][j].max[k], &m_pBox[i][j].min[k]);
		}
	}
}

void CFMMNN::LoadBox2(CString file)
{
	FILE *fp;

	fp = fopen(file, "rt");

	for(register int i = 0; i < m_nClass; i ++)
		fscanf(fp, "%d", &m_nBox[i]);
	m_pBox = new MINMAX*[m_nClass];
	for(i = 0; i < m_nClass; i ++)
	{
		m_pBox[i] = new MINMAX[m_nBox[i]];
		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			m_pBox[i][j].min = new float[m_nDimension];
			m_pBox[i][j].max = new float[m_nDimension];
			for(register int k = 0; k < m_nDimension; k ++)
				fscanf(fp, "%f %f", &m_pBox[i][j].max[k], &m_pBox[i][j].min[k]);
		}
	}

	fclose(fp);
}

/*-------------------------------------------------------
	하이퍼박스 정보 저장하는 함수

	fp(입력) : 파일 포인터
-------------------------------------------------------*/
void CFMMNN::SaveBox(FILE* fp)
{
	for(register int i = 0; i < m_nClass; i ++)
		fprintf(fp, "%d ", m_nBox[i]);
	fprintf(fp, "\n");
	for(i = 0; i < m_nClass; i ++)
	{
		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			for(register int k = 0; k < m_nDimension; k ++)
				//fprintf(fp, "%lf %lf ", m_pBox[i][j].max[k], m_pBox[i][j].min[k]);
				fprintf(fp, "%f %f ", m_pBox[i][j].max[k], m_pBox[i][j].min[k]);
			fprintf(fp, "\n");
		}
	}
}

void CFMMNN::SaveBox2(CString file)
{
	FILE *fp;
	fp = fopen(file, "wt");

	for(register int i = 0; i < m_nClass; i ++)
		fprintf(fp, "%d ", m_nBox[i]);
	fprintf(fp, "\n");
	for(i = 0; i < m_nClass; i ++)
	{
		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			for(register int k = 0; k < m_nDimension; k ++)
				//fprintf(fp, "%lf %lf ", m_pBox[i][j].max[k], m_pBox[i][j].min[k]);
				fprintf(fp, "%f %f ", m_pBox[i][j].max[k], m_pBox[i][j].min[k]);
			fprintf(fp, "\n");
		}
	}

	fclose(fp);
}
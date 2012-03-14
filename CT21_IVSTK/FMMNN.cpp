#include <stdafx.h>
#include <stdio.h>
#include <memory.h>
#include "FMMNN.h"

#define max(a, b)  (((a) > (b)) ? (a) : (b))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define F(a, b) ((a)*(b) > 1 ? 1 : ((a)*(b) < 0 ? 0 : (a)*(b)))

/*-------------------------------------------------------
	�⺻ ������
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
	������

	nDimension(�Է�) : �Է� ������ ������ = �Է� x�� dimension (������ n)
	nClass(�Է�) : Ŭ���� �� = �����ϰ��� �ϴ� Ŭ������ �� (������ p)
	lfTheta(�Է�) : Ȯ���� �����ϴ� �Ѱ�ġ, theta��, 0 ~ 1, ���� Ŭ���� �ڽ� ���� ����.
	lfGamma(�Է�) : membership function�� ��������, sensitivity, > 0, ���� Ŭ���� crisp�ϰ� �ȴ�.
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
	�ʱ�ȭ �ϴ� �Լ�

	nDimension(�Է�) : �Էº����� ������ = �Է� x�� dimension (������ n)
	nClass(�Է�) : Ŭ���� �� = �����ϰ��� �ϴ� Ŭ������ �� (������ p)
	lfTheta(�Է�) : Ȯ���� �����ϴ� �Ѱ�ġ, theta��, 0 ~ 1, ���� Ŭ���� �ڽ� ���� ����.
	lfGamma(�Է�) : membership function�� ��������, sensitivity, > 0, ���� Ŭ���� crisp�ϰ� �ȴ�.
-------------------------------------------------------*/
void CFMMNN::Init(int nDimension, int nClass, float lfTheta, float lfGamma)
{
	Clear();

	m_nDimension = nDimension;
	m_nClass = nClass;
	m_lfTheta = lfTheta;
	m_lfGamma = lfGamma;
	
	m_pInput = NULL; // �Է��� �ܺο��� �������� �Ŷ� �޸� �� ��´�.
	m_pOutput = new float[nClass]; // ����� Ŭ���� ����ŭ ��´�.
	
	// ���⼭���ʹ� �Ű澲�� ����. ������ �迭 ������� �� �� �ϴ� �Ŵϱ�.
	// m_pBox[i][j] : i��° Ŭ������ j��° �ڽ� --> �̷��� �� �� �ְ� �����.
	m_pBox = new MINMAX*[nClass]; // Ŭ���� ����ŭ �ڽ� ���� �ڸ� �����.
	for(register int i = 0; i < nClass; i ++)
		m_pBox[i] = NULL; // �ʱ�ȭ
	m_nBox = new int[nClass]; // Ŭ���� ����ŭ �ڽ� ���� ����� �ڸ� �����.
	memset(m_nBox, 0, sizeof(int) * nClass); // 0���� �ʱ�ȭ
}

/*-------------------------------------------------------
	�Ҹ���
-------------------------------------------------------*/
CFMMNN::~CFMMNN()
{
	Clear();
}

/*-------------------------------------------------------
	��� �ڽ��� �����ϰ� �ʱ�ȭ�ϴ� �Լ�
-------------------------------------------------------*/
void CFMMNN::Clear()
{
	// �Է� ������ �޸𸮴� ���⼭ ���� �� �ƴϴϱ� ������ �ʿ����. �ܺο��� ó���ؾ� �Ѵ�.

	// ��� ������ �޸� ����
	if(m_pOutput)
	{
		delete [] m_pOutput;
		m_pOutput = NULL;
	}
	
	// ���⼭���ʹ� �Ű澲�� ����. ������ �迭 ������ �̷��� �Ŵϱ�.
	// �ڽ� �޸� ����.. �̰� �� �����ѵ�
	for(register int i = 0; i < m_nClass; i ++)
	{
		for(register int j = 0; j < m_nBox[i]; j ++)
		{
			// �� �ڽ��� min, max ����
			MINMAX* p = &m_pBox[i][j];
			delete p->min;
			p->min = NULL;
			delete p->max;
			p->max = NULL;
		}

		// i��° Ŭ������ �ڽ� ����
		if(m_pBox[i])
		{
			delete [] m_pBox[i];
			m_pBox[i] = NULL;
		}
	}

	// ��ü �ڽ� ����
	if(m_pBox)
	{
		delete [] m_pBox;
		m_pBox = NULL;
	}

	if(m_nBox)
	{
		delete [] m_nBox; // �� Ŭ������ �ڽ� �� ����
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
	nClass��° Ŭ������ �ڽ��� �ϳ� �߰��ϴ� �Լ�

	nClass(�Է�) : Ŭ���� ��ȣ(0���� ����)
-------------------------------------------------------*/
void CFMMNN::AddBox(int nClass)
{
	register int nBox = m_nBox[nClass]; // nClass��° Ŭ������ ������ �ڽ� ����

	if(nBox == 0) // nClass��° Ŭ������ �ڽ��� �ϳ��� ��������
	{
		m_pBox[nClass] = new MINMAX[1]; // �ڽ� �ϳ� �����.
	}
	else // nClass��° Ŭ������ �ڽ��� �־�����
	{
		// �迭�� ���� �־����ϱ� �迭�� �ϳ� ���̱� ���ؼ��� 
		// ���ο� ũ�⸸ŭ ���� �迭 ��� ������ ������ ���� 
		// ������ �޸� ����� ���ο� �Ϳ��� ������ �����ؾ� �Ѵ�.

		MINMAX* pNewBox = new MINMAX[nBox+1]; // ���ο� ũ�⸸ŭ �迭 ��´�.
		memcpy(pNewBox, m_pBox[nClass], nBox * sizeof(MINMAX)); // �������� ���ο� �Ϳ��ٰ� �����Ѵ�.

		delete [] m_pBox[nClass]; // ������ �����
		m_pBox[nClass] = pNewBox; // ���ο� �迭�� �����͸� �����Ѵ�.
	}

	// ���� ���� �ڽ��� �ʱ�ȭ�Ѵ�.
	MINMAX* p = &m_pBox[nClass][nBox];
	p->min = new float[m_nDimension];
	p->max = new float[m_nDimension];
	for(register int i = 0; i < m_nDimension; i ++)
	{
		p->min[i] = m_pInput[i];
		p->max[i] = m_pInput[i];
	}
	m_nBox[nClass] ++; // �ڽ� �� �ϳ� ����
}

/*-------------------------------------------------------
	�ڽ��� Ȯ�� �����Ѱ�? �˾ƺ��� �Լ�

	pBox(�Է�) : �ڽ�
	return : Ȯ���� �� ������ true, ������ false
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
	�ڽ��� Ȯ���ϴ� �Լ�

	pBox(�Է�) : �ڽ�
-------------------------------------------------------*/
void CFMMNN::ExpandBox(MINMAX* pBox)
{
	register MINMAX* p = pBox;

	// �� ������ ���� �ٲ۴�.
	for(register int i = 0; i < m_nDimension; i ++)
	{
		p->max[i] = max(p->max[i], m_pInput[i]); // wji = max(wji, xhi)
		p->min[i] = min(p->min[i], m_pInput[i]); // vji = min(vji, xhi)
	}
}

/*-------------------------------------------------------
	�ڽ��� membership value �˾Ƴ��� �Լ�

	pBox(�Է�) : �ڽ�
	return : �ش� �ڽ��� �ҼӰ�
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
	nClass Ŭ������ Ȯ�� ������ �� �� membership value�� ���� ū �ڽ��� ã�� �Լ�

	nClass(�Է�) : Ŭ���� ��ȣ(0���� ����)
	return : ã������ �ش� �ڽ� ��ȣ, �� ã������ -1
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
	�� ���� �ڽ��� overlap�Ǵ��� �׽�Ʈ�ϴ� �Լ�

	pBox1(�Է�) : ù��° �ڽ�
	pBox2(�Է�) : �ι�° �ڽ�
	nDir(���) : overlap�Ǹ� ���� ��ħ�� ���� ����
	nCase(���) : overlap�� ���̽�(1 ~ 4)
	return : overlap�Ǹ� true, �ƴϸ� false
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
			return false; // overlap�� ����

		if(minNew < minOld) // ��ħ�� �� ���� ���� ã����
		{
			nDir = i;
			minOld = minNew;
			nCase = caseTmp;
		}
	}

	return true;
}

/*-------------------------------------------------------
	�� ���� �ڽ��� ��ҽ�Ű�� �Լ�

	pBox1(�Է�) : ù��° �ڽ�
	pBox2(�Է�) : �ι�° �ڽ�
	nDir(�Է�) : ��ҽ�Ű�� ����(0���� ����)
	nCase(�Է�) : overlap�� ���̽�(1 ~ 4)
	return : ��ҵǸ� true, �ƴϸ� false
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
	nClass��° Ŭ������ ���ϴ� �ڽ��� ���� overlap �׽�Ʈ�ؼ� ����ϴ� �Լ�

	pBox(�Է�) : �ڽ�
	nClass(�Է�) : Ŭ���� ��ȣ(0���� ����), pBox�� ���� Ŭ����
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
	�Էº��Ϳ� ���� membership value�� ���� ū Ŭ������ ã�� �Լ�

	return : membership value�� ���� ū Ŭ����(0���� ����)
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
	��ü �ڽ� ���� ��� �Լ�

	return : ��ü �ڽ� ����
-------------------------------------------------------*/
int CFMMNN::GetTotalBoxNum()
{
	int sum = 0;
	for(register int i = 0; i < m_nClass; i ++)
		sum += m_nBox[i];

	return sum;
}

/*-------------------------------------------------------
	�н��ϴ� �Լ�

	nClass(�Է�) : �н� �������� Ŭ����
	pInput(�Է�) : �ϳ��� �н� �Է� ���� x, ���̴� m_nInput(=n)�� ���ƾ� �Ѵ�.

	* ����� n�� �̻��̾ �ȴ�. ������ n������ ������ ������ �����.
-------------------------------------------------------*/
void CFMMNN::Training(int nClass, float* pInput)
{
	if(pInput == NULL)
		return;
	m_pInput = pInput;

	int nBox = FindMaxMembershipBox(nClass);
	if(nBox < 0) // Ȯ���� �� �ִ� �� ������
		AddBox(nClass); // �ڽ��� �ϳ� ���� �����.
	else // Ȯ���� �� ������
	{
		register MINMAX* pBox = &m_pBox[nClass][nBox];
		ExpandBox(pBox); // Ȯ���Ѵ�.
		TestBox(pBox, nClass); // overlap �׽�Ʈ�ؼ� ����Ѵ�.
	}
}

/*-------------------------------------------------------
	�׽�Ʈ�ϴ� �Լ�

	pInput(�Է�) : �ϳ��� �׽�Ʈ �Է� ���� x, ���̴� m_nInput(=n)�� ���ƾ� �Ѵ�.
	* ����� n�� �̻��̾ �ȴ�. ������ n������ ������ ������ �����.
	return : membership value�� ���� ū Ŭ���� ��ȣ
-------------------------------------------------------*/
int CFMMNN::Test(float* pInput)
{
	if(pInput == NULL)
		return -1;
	m_pInput = pInput;

	return FindMaxMembershipClass();
}

/*-------------------------------------------------------
	�׽�Ʈ�ϴ� �Լ�

	pInput(�Է�) : �ϳ��� �׽�Ʈ �Է� ���� x, ���̴� m_nInput(=n)�� ���ƾ� �Ѵ�.
	pOutput(�Է�) : �Է� ���Ϳ� ���� �� Ŭ������ membership value�� ��ȯ�ޱ� ���� float* �� ����
	* ����� n�� �̻��̾ �ȴ�. ������ n������ ������ ������ �����.
	return : membership value�� ���� ū Ŭ���� ��ȣ
-------------------------------------------------------*/
int CFMMNN::Test(float* pInput, float* &pOutput)
{
	int nClass = Test(pInput);
	pOutput = GetResultMembership();
	
	return nClass;
}

/*-------------------------------------------------------
	�����۹ڽ� ���� �о���� �Լ�

	fp(�Է�) : ���� ������
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
	�����۹ڽ� ���� �����ϴ� �Լ�

	fp(�Է�) : ���� ������
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
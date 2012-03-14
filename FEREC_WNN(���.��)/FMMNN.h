//
// FMMNN Ŭ����
// ���� �� : �ڱ��� (�Ʊ���)
// ���� �ñ� : 2002�� 12�� 7��
// ���� 1.3

// ������ : �⺻ ����� Original FMMNN�� ��������, �ڽ� ũ�� ���Ѱ� Membership Function�� GFMMNN�� ������.

#ifndef _FMMNN_H_
#define _FMMNN_H_

typedef struct _MINMAX
{
	float* min;
	float* max;
} MINMAX;

class CFMMNN
{
protected:
	int m_nDimension; // �Է� ������ Dimension
	int m_nClass; // �����ϰ��� �ϴ� Ŭ���� ��
	float m_lfTheta, m_lfGamma; // theta, gamma
	
	float* m_pInput; // �Է� ����
	float* m_pOutput; // ��� ���� = �� Ŭ������ ���� ����� ��

	int* m_nBox;
	MINMAX** m_pBox;
	
	void AddBox(int nClass);
	bool IsExpandable(MINMAX* pBox);
	void ExpandBox(MINMAX* pBox);
	float GetMembership(MINMAX* pBox);
	int FindMaxMembershipBox(int nClass);
	bool TestOverlap(MINMAX* pBox1, MINMAX* pBox2, int& nDir, int& nCase);
	bool Contract(MINMAX* pBox1, MINMAX* pBox2, int nDir, int nCase);
	void TestBox(MINMAX* pBox, int nClass);
	int FindMaxMembershipClass();

public:
	CFMMNN();
	CFMMNN(int nDimension, int nClass, float lfTheta, float lfGamma);
	~CFMMNN();

	void SetTheta(float lfTheta) { m_lfTheta = lfTheta; }
	void SetGamma(float lfGamma) { m_lfGamma = lfGamma; }
	int GetDimension() { return m_nDimension; }
	int GetClassNum() { return m_nClass; }
	float GetTheta() { return m_lfTheta; }
	float GetGamma() { return m_lfGamma; }
	int GetBoxNum(int nClass) { return m_nBox[nClass]; }
	int GetTotalBoxNum();

	void Init(int nDimension, int nClass, float lfTheta, float lfGamma);
	void Clear();
	
	MINMAX* GetBox(int nClass, int nNum) { return &m_pBox[nClass][nNum]; } // nClass : Ŭ���� ��ȣ(0���� ����), nNum : �ڽ� ��ȣ(0���� ����)

	void Training(int nClass, float* pInput);
	int Test(float* pInput);
	int Test(float* pInput, float* &pOutput);
	float* GetResultMembership() { return m_pOutput; } // �׽�Ʈ ���, �� Ŭ������ ���� membership value ��ȯ
	void LoadBox(FILE* fp);
	void SaveBox(FILE* fp);
	void LoadBox2(CString file);
	void SaveBox2(CString file);
};

#endif

/* �Ŵ���

	1. �ʱ�ȭ

	1) ��� ������ ����� ���
	�� ���� �Ķ���͸� �ٷ� �־����� ���ϱ� ������ Init �Լ��� ����ؾ� �Ѵ�.

	��)
	class CMyTest
	{
		....
		CFMMNN m_fmm;
		...
	};

	CMyTest::MyFunction()
	{
		m_fmm.Init(2, 3, 0.3, 4);
	}

	2) ���� ������ �����ϰų� new�� ��ü�� ������ ���
	�� ���� ������ �� �Ķ���͸� �־��ְų� Init���� �Ķ���͸� ���� �� �ִ�.
	��, ������ �� �Ķ���͸� ���� ��쿡�� Init�� ������� �� �� ! �޸� ���� �����.

	��)
	CMyTest::MyFunction()
	{
		CFMMNN fmm(2, 3, 0.3, 4);
	}
	Ȥ��
	CMyTest::MyFunction()
	{
		CFMMNN fmm;
		fmm.Init(2, 3, 0.3, 4);
	}
	Ȥ��
	CMyTest::MyFunction()
	{
		CFMMNN* pFMM = new CFMMNN(2, 3, 0.3, 4);
	}
	Ȥ��
	CMyTest::MyFunction()
	{
		CFMMNN* pFMM = new CFMMNN;
		pFMM->Init(2, 3, 0.3, 4);
	}

	2. �Ķ���� ����
	�Ķ���� ������ ���� Init�� ����� ���� �ִ�.
	SetTheta�� SetGamma �Լ��� Theta�� Gamma�� ���� ������ ���� �ִ�.

	3. �Ķ���� �б�
	�ʱ�ȭ �� �־��� �Է� ���� ������(dimension), Ŭ���� ���� �� �� �ִ�.
	GetDimension�� GetClassNum �Լ��� ����.

	���� Theta���� Gamma���� �� �� �ִ�.
	GetTheta�� GetGamma �Լ��� ����.
	
	4. �н�
	�н���ų �Է� ���Ϳ� �ش� Ŭ���� ��ȣ�� �Է����� �ִ´�.
	�̶� �Է� ������ dimension�� �ʱ�ȭ �� �־��� dimension�� ���ƾ� �Ѵ�.
	�Է� ������ �� ������Ʈ ���� 0 ~ 1 (normalized�� ��)�̾�� �Ѵ�.
	Training �Լ��� ����.

	��)
	CMyTest::MyFunction()
	{
		CFMMNN fmm(2, 3, 0.3, 4);
		float in[2] = { 0.1, 0.2 };
		fmm.Training(0, in);
	}

	5. �׽�Ʈ
	�׽�Ʈ�� �Է� ���͸� �Է����� �ִ´�.
	�̶� �Է� ������ dimension�� �ʱ�ȭ �� �־��� dimension�� ���ƾ� �Ѵ�.
	�Է� ������ �� ������Ʈ ���� 0 ~ 1 (normalized�� ��)�̾�� �Ѵ�.
	Test �Լ��� 2������ �ִµ� �ϳ��� ��� Ŭ���� ��ȣ�� ��ȯ�ϰ�, �ٸ� �ϳ��� �� Ŭ������ membership value�� �Բ� ��ȯ�Ѵ�.
	
	��)
	CMyTest::MyFunction()
	{
		CFMMNN fmm(2, 3, 0.3, 4);
		float in[2] = { 0.1, 0.2 };
		fmm.Training(0, in);
		int nClass = fmm.Test(in); // nClass�� in�� �׽�Ʈ�� ����� ��ȯ�� Ŭ���� ��ȣ��.
								   // ��, nClass�� 0 �̶�� in�� Ŭ���� 0���� �з��� ���δ�.
		Ȥ��
		float* out;
		int nClass = fmm.Test(in, out); // �̷��� �ϸ� nClass�� ���� ���� ����,
										// �׽�Ʈ ���� in�� �� Ŭ������ ���� �Ҽ� ���� (membership value)�� out���� ��ȯ�ȴ�.
		printf("%lf %lf %lf", out[0], out[1], out[2]);
	}

	�� Ŭ������ ���� �Ҽ� ������ GetResultMembership() �Լ��� ���� ���� ���� �ִ�.
	GetResultMembership �Լ��� ���� ���� Test �Լ��� ���� ������ �� ���̴�.
	�׷��� ������ NULL�� ��ȯ �ްų� ���� �׽�Ʈ ����� ��ȯ �ް� �ȴ�.

	��)
	CMyTest::MyFunction()
	{
		...
		int nClass = fmm.Test(in);
		float* out = fmm.GetResultMembership();
	}

  6. �ڽ� ������ ����.
	Ŭ���� ��ȣ�� nClass�� Ŭ������ �����۹ڽ� ���� ��ü �����۹ڽ� ���� �� �� �ִ�.
	GetBoxNum �Լ��� GetTotalBoxNum �Լ��� ����.

	Ŭ���� ��ȣ�� nClass�̰� �ڽ� ��ȣ�� nNum�� �����۹ڽ��� ���� �� �ִ�.
	GetBox �Լ��� ����.

	��)
	CMyTest::MyFunction()
	{
		...
		int nBox = fmm.GetBox(0);
		int nTotalBox = fmm.GetTotalBoxNum();
		MINMAX* pBox = fmm.GetBox(0, 0);
	}

	7. ������
	��� ������ ������ �Ǿ��ų� ���� ������ ������ �Ǿ����� �׳� ���ξ �ȴ�.
	new�� ������ ��쿡�� delete�� ���ָ� �ȴ�.
	���� ������ �ѹ� ���ֺ��� ������ Clear �Լ��� ���. ���� ���� ���̴�.
*/
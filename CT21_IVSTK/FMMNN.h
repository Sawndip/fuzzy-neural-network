//
// FMMNN 클래스
// 만든 이 : 박광현 (아까이)
// 만든 시기 : 2002년 12월 7일
// 버전 1.3

// 주의점 : 기본 골격은 Original FMMNN을 따랐으나, 박스 크기 제한과 Membership Function은 GFMMNN을 따랐음.

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
	int m_nDimension; // 입력 벡터의 Dimension
	int m_nClass; // 구분하고자 하는 클래스 수
	float m_lfTheta, m_lfGamma; // theta, gamma
	
	float* m_pInput; // 입력 벡터
	float* m_pOutput; // 출력 벡터 = 각 클래스에 대한 멤버쉽 값

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
	
	MINMAX* GetBox(int nClass, int nNum) { return &m_pBox[nClass][nNum]; } // nClass : 클래스 번호(0부터 시작), nNum : 박스 번호(0부터 시작)

	void Training(int nClass, float* pInput);
	int Test(float* pInput);
	int Test(float* pInput, float* &pOutput);
	float* GetResultMembership() { return m_pOutput; } // 테스트 결과, 각 클래스에 대한 membership value 반환
	void LoadBox(FILE* fp);
	void SaveBox(FILE* fp);
	void LoadBox2(CString file);
	void SaveBox2(CString file);
};

#endif

/* 매뉴얼

	1. 초기화

	1) 멤버 변수로 선언된 경우
	이 때엔 파라미터를 바로 넣어주지 못하기 때문에 Init 함수를 사용해야 한다.

	예)
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

	2) 지역 변수로 선언하거나 new로 객체를 생성할 경우
	이 때엔 선언할 때 파라미터를 넣어주거나 Init으로 파라미터를 넣을 수 있다.
	단, 선언할 때 파라미터를 넣은 경우에는 Init을 사용하지 말 것 ! 메모리 낭비가 생긴다.

	예)
	CMyTest::MyFunction()
	{
		CFMMNN fmm(2, 3, 0.3, 4);
	}
	혹은
	CMyTest::MyFunction()
	{
		CFMMNN fmm;
		fmm.Init(2, 3, 0.3, 4);
	}
	혹은
	CMyTest::MyFunction()
	{
		CFMMNN* pFMM = new CFMMNN(2, 3, 0.3, 4);
	}
	혹은
	CMyTest::MyFunction()
	{
		CFMMNN* pFMM = new CFMMNN;
		pFMM->Init(2, 3, 0.3, 4);
	}

	2. 파라미터 변경
	파라미터 변경을 위해 Init을 사용할 수도 있다.
	SetTheta와 SetGamma 함수로 Theta와 Gamma를 각각 변경할 수도 있다.

	3. 파라미터 읽기
	초기화 때 넣어준 입력 벡터 차원수(dimension), 클래스 수를 알 수 있다.
	GetDimension과 GetClassNum 함수를 쓰자.

	현재 Theta값과 Gamma값을 알 수 있다.
	GetTheta와 GetGamma 함수를 쓰자.
	
	4. 학습
	학습시킬 입력 벡터와 해당 클래스 번호를 입력으로 넣는다.
	이때 입력 벡터의 dimension은 초기화 때 넣어준 dimension과 같아야 한다.
	입력 벡터의 각 컴포넌트 값은 0 ~ 1 (normalized된 값)이어야 한다.
	Training 함수를 쓰자.

	예)
	CMyTest::MyFunction()
	{
		CFMMNN fmm(2, 3, 0.3, 4);
		float in[2] = { 0.1, 0.2 };
		fmm.Training(0, in);
	}

	5. 테스트
	테스트할 입력 벡터를 입력으로 넣는다.
	이때 입력 벡터의 dimension은 초기화 때 넣어준 dimension과 같아야 한다.
	입력 벡터의 각 컴포넌트 값은 0 ~ 1 (normalized된 값)이어야 한다.
	Test 함수는 2가지가 있는데 하나는 결과 클래스 번호만 반환하고, 다른 하나는 각 클래스의 membership value도 함께 반환한다.
	
	예)
	CMyTest::MyFunction()
	{
		CFMMNN fmm(2, 3, 0.3, 4);
		float in[2] = { 0.1, 0.2 };
		fmm.Training(0, in);
		int nClass = fmm.Test(in); // nClass가 in을 테스트한 결과로 반환된 클래스 번호다.
								   // 즉, nClass가 0 이라면 in을 클래스 0으로 분류한 것인다.
		혹은
		float* out;
		int nClass = fmm.Test(in, out); // 이렇게 하면 nClass는 위의 경우와 같고,
										// 테스트 벡터 in의 각 클래스에 대한 소속 정도 (membership value)가 out으로 반환된다.
		printf("%lf %lf %lf", out[0], out[1], out[2]);
	}

	각 클래스에 대한 소속 정도는 GetResultMembership() 함수를 통해 얻을 수도 있다.
	GetResultMembership 함수를 쓰기 전에 Test 함수를 먼저 돌려야 할 것이다.
	그렇지 않으면 NULL을 반환 받거나 이전 테스트 결과를 반환 받게 된다.

	예)
	CMyTest::MyFunction()
	{
		...
		int nClass = fmm.Test(in);
		float* out = fmm.GetResultMembership();
	}

  6. 박스 정보를 얻자.
	클래스 번호가 nClass인 클래스의 하이퍼박스 수와 전체 하이퍼박스 수를 알 수 있다.
	GetBoxNum 함수와 GetTotalBoxNum 함수를 쓰자.

	클래스 번호가 nClass이고 박스 번호가 nNum인 하이퍼박스를 얻을 수 있다.
	GetBox 함수를 쓰자.

	예)
	CMyTest::MyFunction()
	{
		...
		int nBox = fmm.GetBox(0);
		int nTotalBox = fmm.GetTotalBoxNum();
		MINMAX* pBox = fmm.GetBox(0, 0);
	}

	7. 끝내기
	멤버 변수로 선언이 되었거나 지역 변수로 선언이 되었으면 그냥 놔두어도 된다.
	new로 선언한 경우에는 delete로 없애면 된다.
	궂이 강제로 한번 없애보고 싶으면 Clear 함수를 써라. 쓸데 없는 짓이다.
*/
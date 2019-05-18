#include "Rand32.h"
#include <random>
#include <ctime>

class pcg
{
public:
	using result_type = uint32_t;
	static constexpr result_type(min)() { return 0; }
	static constexpr result_type(max)() { return UINT32_MAX; }
	friend bool operator==(pcg const &, pcg const &);
	friend bool operator!=(pcg const &, pcg const &);

	pcg()
		: m_state(0x853c49e6748fea9bULL)
		, m_inc(0xda3e39cb94b95bdbULL)
	{}
	explicit pcg(std::random_device &rd)
	{
		seed(rd);
	}

	void seed(std::random_device &rd)
	{
		uint64_t s0 = uint64_t(rd()) << 31 | uint64_t(rd());
		uint64_t s1 = uint64_t(rd()) << 31 | uint64_t(rd());

		m_state = 0;
		m_inc = (s1 << 1) | 1;
		(void)operator()();
		m_state += s0;
		(void)operator()();
	}

	result_type operator()()
	{
		uint64_t oldstate = m_state;
		m_state = oldstate * 6364136223846793005ULL + m_inc;
		uint32_t xorshifted = uint32_t(((oldstate >> 18u) ^ oldstate) >> 27u);
		int rot = oldstate >> 59u;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}

	void discard(unsigned long long n)
	{
		for (unsigned long long i = 0; i < n; ++i)
			operator()();
	}

private:
	uint64_t m_state;
	uint64_t m_inc;
};

Rand32::Rand32()
{
	srand((unsigned int)time(nullptr));
	//unsigned long long int v2 = rand();
	double v2 = ((double)rand() / (double)RAND_MAX);
	Seed((unsigned int)(1170746341 * v2 - 755606699), 
		(unsigned int)(1170746341 * v2 - 755606699), 
		(unsigned int)(1170746341 * v2 - 755606699));
}

void Rand32::Seed(unsigned int s1, unsigned int s2, unsigned int s3)
{
	std::lock_guard<std::mutex> lock(m_lock);
	m_s1 = s1 | 0x100000;
	m_past_s1 = s1 | 0x100000;
	m_s2 = s2 | 0x1000;
	m_past_s2 = s2 | 0x1000;
	m_s3 = s3 | 0x10;
	m_past_s3 = s3 | 0x10;
}

Rand32 * Rand32::GetInstance()
{
	static Rand32* pInstance = new Rand32;
	return pInstance;
}

unsigned int Rand32::Random()
{
	std::lock_guard<std::mutex> lock(m_lock);
	static std::random_device rd;
	static pcg rand(rd);
	static std::uniform_int_distribution<> u(0, 895);
	/*unsigned int v3; // eax@1
	unsigned int v4; // ecx@1
	unsigned int v5; // edx@1
	unsigned int v6; // ebx@1
	unsigned int v7; // edi@1
	unsigned int v8; // ebx@1
	unsigned int v9; // eax@1
	auto v1 = this;
	v3 = v1->m_s1;
	v4 = v1->m_s2;
	v5 = v1->m_s3;
	v6 = v1->m_s1;
	v1->m_past_s1 = v1->m_s1;
	v7 = ((v3 & 0xFFFFFFFE) << 12) ^ ((v6 & 0x7FFC0 ^ (v3 >> 13)) >> 6);
	v1->m_past_s2 = v4;
	v8 = 16 * (v4 & 0xFFFFFFF8) ^ (((v4 >> 2) ^ v4 & 0x3F800000) >> 23);
	v1->m_past_s3 = v5;
	v9 = ((v5 & 0xFFFFFFF0) << 17) ^ (((v5 >> 3) ^ v5 & 0x1FFFFF00) >> 8);
	v1->m_s3 = v9;
	v1->m_s1 = v7;
	v1->m_s2 = v8;
	return  v7 ^ v8 ^ v9;*/
	return (u(rand) * u(rand) * u(rand));
}

unsigned int Rand32::Random(unsigned int nMin, unsigned int nMax)
{
	unsigned int nRndRange = (nMax - nMin);
	if (nRndRange == 0)
		return 0;
	return nMin + GetInstance()->Random() % (nMax - nMin);
}

std::vector<int> Rand32::GetRandomUniqueArray(int nStart, int nRange, int nCount)
{
	std::vector<int> aRet;
	aRet.resize(nCount);
	std::vector<int> aSample;
	for (int i = 0; i < nRange; ++i)
		aSample.push_back(nStart + i);
	int nRnd = 0;
	for (int i = 0; i < nCount; ++i)
	{
		nRnd = Random() % aSample.size();
		aRet[i] = aSample[nRnd];
		aSample.erase(aSample.begin() + nRnd);
	}
	return aRet;
}

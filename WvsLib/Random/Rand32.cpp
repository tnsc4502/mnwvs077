#include "Rand32.h"
#include <random>
#include <ctime>

class WvsPCG
{
public:
	using result_type = uint32_t;
	static constexpr result_type(min)() { return 0; }
	static constexpr result_type(max)() { return UINT32_MAX; }

	WvsPCG()
		: m_state(0x853c49e6748fea9bULL)
		, m_inc(0xda3e39cb94b95bdbULL)
	{}
	explicit WvsPCG(std::random_device &rd)
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
	/*double v2 = ((double)rand() / (double)RAND_MAX);
	Seed((unsigned int)(1170746341 * v2 - 755606699), 
		(unsigned int)(1170746341 * v2 - 755606699), 
		(unsigned int)(1170746341 * v2 - 755606699));*/
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

unsigned long long int Rand32::Random()
{
	std::lock_guard<std::mutex> lock(m_lock);
	static std::random_device rd;
	static WvsPCG rand(rd);
	static std::uniform_int_distribution<> u(0, 2006674111); //Fine-tuned number
	static std::uniform_int_distribution<> u2(0, 1721); //Fine-tuned number

	//Fine-tuned combination, more uniform than u^u^u
	return (u(rand) ^ u(rand) ^ u(rand)) *(u2(rand));
}

unsigned long long int Rand32::Random(unsigned int nMin, unsigned int nMax)
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
		nRnd = (int)(Random() % aSample.size());
		aRet[i] = aSample[nRnd];
		aSample.erase(aSample.begin() + nRnd);
	}
	return aRet;
}
